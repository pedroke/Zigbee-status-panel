#include <stdlib.h>
#include <stdio.h>
#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "nwk_util.h"
#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"
#include "zcl_statuspanel.h"
#include "bdb.h"
#include "OnBoard.h"
#include "bdb_interface.h"

#include "onboard.h"
#include "spi.h"
#include "epddriver.h"
#include "graphics.h"
#include "images.h"
#include "binary.h"

const uint32 REPORT_INTERVAL =   3600000; // 1 hour
const uint32 NO_SYNC_INTERVAL =  1800000; // 30 minutes
const uint32 REFRESH_DISPLAY_INTERVAL = 86400000; // 24 hours

/*********************************************************************
 * GLOBAL VARIABLES
 */
byte zclStatusPanel_TaskID;
extern bool requestNewTrustCenterLinkKey;
uint16 zclStatusPanelSeqNum = 0;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */
int16 zclStatusPanel_OutsideTemperature = 0;
int16 zclStatusPanel_InsideTemperature = 0;
uint8 zclStatusPanel_Warnings = 0x00;
uint8 zclStatusPanel_State = 0;

/*********************************************************************
 * LOCAL VARIABLES
 */
afAddrType_t zclStatusPanel_DstAddr;
devStates_t zclStatusPanel_NwkState = DEV_INIT;

uint8 zclStatusPanel_NoSync = 1;
int16 lastOutsideTemperature = 0;
int16 lastInsideTemperature = 0;
uint8 lastWarnings = 0;
uint8 lastState = 0;
uint8 lastSync = 0;
uint8 errorCode = 0;
uint8 batteryProblem = 0;
uint8 pressedFR = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void displayInit(void);
static void clearDisplay(void);
static void layoutBaseContent(void);
static void layoutTemperatures(void);
static void layoutWarnings(void);
static void refreshDisplay(void);
static uint8 isChangedContent(void);
static void sp_sendReport(void);
static void processDisplayedAttributes(void);
static void setErrorCode(void);
static void configureInterrupts(void);
static void configureUnusedPorts(void);

static void zclStatusPanel_ProcessCommissioningStatus(bdbCommissioningModeMsg_t *bdbCommissioningModeMsg);

// Functions to process ZCL Foundation incoming Command/Response messages
static void zclStatusPanel_ProcessIncomingMsg( zclIncomingMsg_t *msg );

static uint8 zclStatusPanel_ProcessInDefaultRspCmd( zclIncomingMsg_t *pInMsg );
static void zclStatusPanel_BatteryWarningCB( uint8 voltLevel);

/*********************************************************************
 * ZCL General Profile Callback table
 */
static zclGeneral_AppCallbacks_t zclStatusPanel_CmdCallbacks =
{
  NULL,                                    // Basic Cluster Reset command
  NULL,                                   // Identify Trigger Effect command
  NULL,                                   // On/Off cluster commands
  NULL,                                   // On/Off cluster enhanced command Off with Effect
  NULL,                                   // On/Off cluster enhanced command On with Recall Global Scene
  NULL,                                   // On/Off cluster enhanced command On with Timed Off
  NULL,                                  // RSSI Location command
  NULL                                   // RSSI Location Response command
};

/*********************************************************************
 * STATUSPANEL_TODO: Add other callback structures for any additional application specific 
 *       Clusters being used, see available callback structures below.
 *
 *       bdbTL_AppCallbacks_t 
 *       zclApplianceControl_AppCallbacks_t 
 *       zclApplianceEventsAlerts_AppCallbacks_t 
 *       zclApplianceStatistics_AppCallbacks_t 
 *       zclElectricalMeasurement_AppCallbacks_t 
 *       zclGeneral_AppCallbacks_t 
 *       zclGp_AppCallbacks_t 
 *       zclHVAC_AppCallbacks_t 
 *       zclLighting_AppCallbacks_t 
 *       zclMS_AppCallbacks_t 
 *       zclPollControl_AppCallbacks_t 
 *       zclPowerProfile_AppCallbacks_t 
 *       zclSS_AppCallbacks_t  
 *
 */

/*********************************************************************
 * @fn          zclStatusPanel_Init
 *
 * @brief       Initialization function for the zclGeneral layer.
 *
 * @param       none
 *
 * @return      none
 */
void zclStatusPanel_Init( byte task_id )
{
  zclStatusPanel_TaskID = task_id;
  requestNewTrustCenterLinkKey = FALSE;
  
  // This app is part of the Home Automation Profile
  bdb_RegisterSimpleDescriptor( &zclStatusPanel_SimpleDesc );

  // Register the ZCL General Cluster Library callback functions
  zclGeneral_RegisterCmdCallbacks( STATUSPANEL_ENDPOINT, &zclStatusPanel_CmdCallbacks );
  
  // TODO: Register other cluster command callbacks here

  // Register the application's attribute list
  zcl_registerAttrList( STATUSPANEL_ENDPOINT, zclStatusPanel_NumAttributes, zclStatusPanel_Attrs );

  // Register the Application to receive the unprocessed Foundation command/response messages
  zcl_registerForMsg( zclStatusPanel_TaskID );

  // Register low voltage NV memory protection application callback
  RegisterVoltageWarningCB( zclStatusPanel_BatteryWarningCB );

  bdb_RegisterCommissioningStatusCB( zclStatusPanel_ProcessCommissioningStatus );
  
  configureInterrupts();
  configureUnusedPorts();
  SPIInit();
  displayInit();
  refreshDisplay();
  
  bdb_StartCommissioning(BDB_COMMISSIONING_REJOIN_EXISTING_NETWORK_ON_STARTUP);
   
  osal_start_timerEx( zclStatusPanel_TaskID, STATUSPANEL_REFRESH_DISPLAY_EVT, REFRESH_DISPLAY_INTERVAL );
  osal_start_timerEx( zclStatusPanel_TaskID, STATUSPANEL_REPORTING_EVT, REPORT_INTERVAL );
  osal_start_timerEx( zclStatusPanel_TaskID, STATUSPANEL_NO_SYNC_EVT, NO_SYNC_INTERVAL );
}

void displayInit(void) {
  epdInit();
  clearDisplay();
  epdSleep();
}

void clearDisplay(void) {
  lastOutsideTemperature = 0;
  lastInsideTemperature = 0;
  lastWarnings = 0;
  lastState = 0;
  epdClearFrameMemoryBase();
  epdDisplayFrame();
}

void layoutBaseContent(void) {
  epdSetFrameMemoryXY(INDOOR_ICON, 100, 0, 24, 24);
  epdSetFrameMemoryXY(OUTDOOR_ICON, 100, 146, 24, 24);

  char degreesString[] = {'D', 'C', '\0'};
  printString(75, 96, degreesString);
  printString(75, 240, degreesString);
  
  epdSetFrameMemoryXY(HL, 34, 0, 8, 288);
  epdSetFrameMemoryXY(VL, 38, 138, 88, 8);  
}

void refreshDisplay(void) {
    epdInitPartial();
    
    layoutBaseContent();
    layoutTemperatures();
    layoutWarnings();
   
    epdDisplayFramePartial();
    epdSleep();
}

uint8 isChangedContent() {
 return (lastOutsideTemperature + 5 <= zclStatusPanel_OutsideTemperature) || 
        (lastOutsideTemperature - 5 >= zclStatusPanel_OutsideTemperature) ||
        (lastInsideTemperature != zclStatusPanel_InsideTemperature) || 
        (lastWarnings != zclStatusPanel_Warnings) || 
        (lastState != zclStatusPanel_State) ||
        (lastSync != zclStatusPanel_NoSync) ||
        (batteryProblem != 0);
}

void layoutTemperatures() {
  char tempString[] = {' ', ' ', ' ', '.', ' ' , '\0'};
  
  //INSIDE TEMP
  if(zclStatusPanel_NoSync) {
    tempString[0] = 'E';
  } else if(zclStatusPanel_InsideTemperature < 0) {
    tempString[0] = '-';
  }
  tempString[1] = abs(zclStatusPanel_InsideTemperature) / 100 % 10 + '0';
  if(tempString[1] == '0') {
    tempString[1] = ' ';
  }
  tempString[2] = abs(zclStatusPanel_InsideTemperature) / 10 % 10 + '0';
  tempString[4] = abs(zclStatusPanel_InsideTemperature) % 10 + '0';
  printString(40, 0, tempString);
  
  lastInsideTemperature = zclStatusPanel_InsideTemperature;

  // OUTSIDE TEMP
  if(zclStatusPanel_NoSync) {
    tempString[0] = 'E';
  } else if(zclStatusPanel_OutsideTemperature < 0) {
    tempString[0] = '-';
  }
 
  tempString[1] = abs(zclStatusPanel_OutsideTemperature) / 100 % 10 + '0';
  if(tempString[1] == '0') {
    tempString[1] = ' ';
  }
  tempString[2] = abs(zclStatusPanel_OutsideTemperature) / 10 % 10 + '0'; 
  tempString[4] = abs(zclStatusPanel_OutsideTemperature) % 10 + '0';
  printString(40, 154, tempString);
  
  lastOutsideTemperature = zclStatusPanel_OutsideTemperature;
}

void layoutWarnings() {
    if(zclStatusPanel_Warnings & 1) {
      epdSetFrameMemoryXY(WINDOW_ICON, 5, 10, 24, 24);
    } else {
      epdClearFrameMemoryXY(5, 10, 24, 24);
    }
    if(zclStatusPanel_Warnings & 2) {
      epdSetFrameMemoryXY(DROP_ICON, 5, 40, 24, 24);
    } else {
      epdClearFrameMemoryXY(5, 40, 24, 24);
    }
    if(zclStatusPanel_Warnings & 4) {
      epdSetFrameMemoryXY(FIRE_ICON, 5, 70, 24, 24);
    } else {
      epdClearFrameMemoryXY(5, 70, 24, 24);
    }
    if(zclStatusPanel_Warnings & 8) {
      epdSetFrameMemoryXY(TAP_ICON, 5, 100, 24, 24);
    } else {
      epdClearFrameMemoryXY(5, 100, 24, 24);
    }
    lastWarnings = zclStatusPanel_Warnings;
    
    if(zclStatusPanel_NoSync) {
      epdSetFrameMemoryXY(SYNC_ICON, 5, 130, 24, 24);
    } else {
      epdClearFrameMemoryXY(5, 130, 24, 24);
    }
    lastSync = zclStatusPanel_NoSync;
    
    if(batteryProblem) {
      epdSetFrameMemoryXY(LOBAT_ICON, 5, 160, 24, 24);
    } else {
      epdClearFrameMemoryXY(5, 160, 24, 24);
    }
    
    if(zclStatusPanel_State == 6) {
      epdSetFrameMemoryXY(ONLINE_ICON, 5, 270, 24, 24);
    } else {
      epdClearFrameMemoryXY(5, 270, 24, 24);
    }    
    lastState = zclStatusPanel_State;
}

void processDisplayedAttributes(void) {
  if(isChangedContent()) {
      refreshDisplay();
  }
}

void sp_sendReport(void)
{
  // send report
  zclReportCmd_t *pReportCmd;
  
  uint8 NUM_ATTRS = 3;
  pReportCmd = osal_mem_alloc( sizeof(zclReportCmd_t) + ( NUM_ATTRS * sizeof(zclReport_t) ) );
  if ( pReportCmd != NULL )
  {
    pReportCmd->numAttr = NUM_ATTRS;
    
    pReportCmd->attrList[0].attrID = ATTRID_INSIDE_TEMPERATURE;
    pReportCmd->attrList[0].dataType = ZCL_DATATYPE_INT16;
    pReportCmd->attrList[0].attrData = (void *)(&zclStatusPanel_InsideTemperature);
    
    pReportCmd->attrList[1].attrID = ATTRID_OUTSIDE_TEMPERATURE;
    pReportCmd->attrList[1].dataType = ZCL_DATATYPE_INT16;
    pReportCmd->attrList[1].attrData = (void *)(&zclStatusPanel_OutsideTemperature);

    pReportCmd->attrList[2].attrID = ATTRID_WARNINGS;
    pReportCmd->attrList[2].dataType = ZCL_DATATYPE_UINT8;
    pReportCmd->attrList[2].attrData = (void *)(&zclStatusPanel_Warnings);
            
    zclStatusPanel_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
    zclStatusPanel_DstAddr.addr.shortAddr = 0;
    zclStatusPanel_DstAddr.endPoint=1;
    
    zcl_SendReportCmd(STATUSPANEL_ENDPOINT, &zclStatusPanel_DstAddr, ZCL_CLUSTER_ID_GEN_BASIC, pReportCmd, ZCL_FRAME_CLIENT_SERVER_DIR, false, zclStatusPanelSeqNum++ );
  }
  
  osal_mem_free( pReportCmd );
}

/*********************************************************************
 * @fn          zclStatusPanel_event_loop
 *
 * @brief       Event Loop Processor for zclGeneral.
 *
 * @param       none
 *
 * @return      none
 */
uint16 zclStatusPanel_event_loop( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;

  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    while ( (MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( zclStatusPanel_TaskID )) )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZCL_INCOMING_MSG:
          // Incoming ZCL Foundation command/response messages
          zclStatusPanel_ProcessIncomingMsg( (zclIncomingMsg_t *)MSGpkt );
          break;

        case ZDO_STATE_CHANGE:
          zclStatusPanel_NwkState = (devStates_t)(MSGpkt->hdr.status);
          zclStatusPanel_State = (uint8)zclStatusPanel_NwkState;
          processDisplayedAttributes();

          // now on the network
          if ( (zclStatusPanel_NwkState == DEV_ZB_COORD) ||
               (zclStatusPanel_NwkState == DEV_ROUTER)   ||
               (zclStatusPanel_NwkState == DEV_END_DEVICE) )
          {
            //TODO
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }
  
  if ( events & STATUSPANEL_REPORTING_EVT ) {
    sp_sendReport();
    OnBoard_CheckVoltage();
    osal_start_timerEx( zclStatusPanel_TaskID, STATUSPANEL_REPORTING_EVT, REPORT_INTERVAL );
    
    return ( events ^ STATUSPANEL_REPORTING_EVT );
  }
  
  if ( events & STATUSPANEL_REFRESH_DISPLAY_EVT ) {
    displayInit();
    refreshDisplay();
  
    osal_start_timerEx( zclStatusPanel_TaskID, STATUSPANEL_REFRESH_DISPLAY_EVT, REFRESH_DISPLAY_INTERVAL );

    return ( events ^ STATUSPANEL_REFRESH_DISPLAY_EVT );
  }

  if ( events & STATUSPANEL_NO_SYNC_EVT ) {
    zclStatusPanel_NoSync = 1;
    zclStatusPanel_Warnings = 0;
    setErrorCode();
    
    osal_start_timerEx( zclStatusPanel_TaskID, STATUSPANEL_NO_SYNC_EVT, NO_SYNC_INTERVAL );
    
    return ( events ^ STATUSPANEL_NO_SYNC_EVT );
  }
  
  if(events & STATUSPANEL_FR_PRESSED_EVT) {
    // factory reset button pressed
    bdb_resetLocalAction();
    
    return ( events ^ STATUSPANEL_FR_PRESSED_EVT );
  }
  
  
  if ( events & STATUSPANEL_END_DEVICE_REJOIN_EVT )
  {
    bdb_ZedAttemptRecoverNwk();
    return ( events ^ STATUSPANEL_END_DEVICE_REJOIN_EVT );
  }

  // Discard unknown events
  return 0;
}

void setErrorCode(void) {
  zclStatusPanel_InsideTemperature = errorCode;
  zclStatusPanel_OutsideTemperature = zclStatusPanel_State;
  processDisplayedAttributes();
}

void configureUnusedPorts(void) {
}

void configureInterrupts(void) {
  P0SEL &= B11111101;  // P0_1 GPIO
  P0DIR &= B11111101; // P0_1 input
  P0INP &= B11111101; // pullup P0_1
  
  //Port P0_1 enabled for falling edge interrupt
  
  PICTL |= B00000001; // set falling edge on port 0
  P0IEN |= B00000010;// enable interrupt on pin P0_1
  IEN1  |= B00100000; // enable P0 interrupt
  EA = 1; // enable interrupts in general
  P0IFG = 0; // clear interrupts flags
}

HAL_ISR_FUNCTION( halKeyPort0Isr, P0INT_VECTOR )
{
  HAL_ENTER_ISR();

  if (P0IFG & B00000010)
  {
    if(!pressedFR) {
      pressedFR = 1;
      osal_set_event(zclStatusPanel_TaskID, STATUSPANEL_FR_PRESSED_EVT);
    }
  }
  // clear interrupt flags
  P0IFG = 0;
  P0IF = 0;

  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}

/*********************************************************************
 * @fn      zclStatusPanel_ProcessCommissioningStatus
 *
 * @brief   Callback in which the status of the commissioning process are reported
 *
 * @param   bdbCommissioningModeMsg - Context message of the status of a commissioning process
 *
 * @return  none
 */
static void zclStatusPanel_ProcessCommissioningStatus(bdbCommissioningModeMsg_t *bdbCommissioningModeMsg)
{
  switch(bdbCommissioningModeMsg->bdbCommissioningMode)
  {
    case BDB_COMMISSIONING_NWK_STEERING:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS)
      {
        errorCode = 0;
        //YOUR JOB:
        //We are on the nwk, what now?
      }
      else
      {
        errorCode = 20 + bdbCommissioningModeMsg->bdbCommissioningStatus;
        //bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING);
        //See the possible errors for nwk steering procedure
        //No suitable networks found
        //Want to try other channels?
        //try with bdb_setChannelAttribute
      }
    break;
    case BDB_COMMISSIONING_INITIALIZATION:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS) {
        errorCode = 0;
      } else if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_NO_NETWORK) {
        errorCode = 10;
        bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING);
      } else {
         errorCode = 10 + bdbCommissioningModeMsg->bdbCommissioningStatus;
      }
    break;    
    case BDB_COMMISSIONING_PARENT_LOST:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_NETWORK_RESTORED)
      {
        //We did recover from losing parent
        errorCode = 0;
      }
      else
      {
        //Parent not found, attempt to rejoin again after a fixed delay
        errorCode = 30 + bdbCommissioningModeMsg->bdbCommissioningStatus;
        osal_start_timerEx(zclStatusPanel_TaskID, STATUSPANEL_END_DEVICE_REJOIN_EVT, STATUSPANEL_END_DEVICE_REJOIN_DELAY);
      }
    break;
  }
}

/*********************************************************************
 * @fn      zclSampleApp_BatteryWarningCB
 *
 * @brief   Called to handle battery-low situation.
 *
 * @param   voltLevel - level of severity
 *
 * @return  none
 */
void zclStatusPanel_BatteryWarningCB( uint8 voltLevel )
{
  if ( voltLevel == VOLT_LEVEL_CAUTIOUS )
  {
    batteryProblem = 1;
  }
  else if ( voltLevel == VOLT_LEVEL_BAD )
  {
    batteryProblem = 1;
    displayInit();
    osal_stop_timerEx( zclStatusPanel_TaskID, STATUSPANEL_NO_SYNC_EVT);
    osal_stop_timerEx( zclStatusPanel_TaskID, STATUSPANEL_REFRESH_DISPLAY_EVT);
    osal_stop_timerEx( zclStatusPanel_TaskID, STATUSPANEL_REPORTING_EVT);
    NLME_SetPollRate(0);
  }
}

/*********************************************************************
 * @fn      zclStatusPanel_ProcessIncomingMsg
 *
 * @brief   Process ZCL Foundation incoming message
 *
 * @param   pInMsg - pointer to the received message
 *
 * @return  none
 */
static void zclStatusPanel_ProcessIncomingMsg( zclIncomingMsg_t *pInMsg )
{
  switch ( pInMsg->zclHdr.commandID )
  {
    case ZCL_CMD_DEFAULT_RSP:
      zclStatusPanel_ProcessInDefaultRspCmd( pInMsg );
      break;
    case ZCL_CMD_WRITE:
      zclStatusPanel_NoSync = 0;
      errorCode = 0;
      processDisplayedAttributes();
      osal_stop_timerEx( zclStatusPanel_TaskID, STATUSPANEL_NO_SYNC_EVT);
      osal_start_timerEx( zclStatusPanel_TaskID, STATUSPANEL_NO_SYNC_EVT, NO_SYNC_INTERVAL );
      break;
    default:
      break;
  }

  if ( pInMsg->attrCmd )
    osal_mem_free( pInMsg->attrCmd );
}

/*********************************************************************
 * @fn      zclStatusPanel_ProcessInDefaultRspCmd
 *
 * @brief   Process the "Profile" Default Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclStatusPanel_ProcessInDefaultRspCmd( zclIncomingMsg_t *pInMsg )
{
  // zclDefaultRspCmd_t *defaultRspCmd = (zclDefaultRspCmd_t *)pInMsg->attrCmd;

  // Device is notified of the Default Response command.
  (void)pInMsg;

  return ( TRUE );
}
