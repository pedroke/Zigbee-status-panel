#ifndef ZCL_STATUSPANEL_H
#define ZCL_STATUSPANEL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "zcl.h"

/*********************************************************************
 * CONSTANTS
 */
#define STATUSPANEL_ENDPOINT            8

// Application Events
#define STATUSPANEL_END_DEVICE_REJOIN_EVT                 0x0004  
#define STATUSPANEL_IDENTIFY_TIMEOUT_EVT		  0x0001
#define STATUSPANEL_REFRESH_DISPLAY_EVT                   0x0002 
#define STATUSPANEL_REPORTING_EVT        	          0x0008
#define STATUSPANEL_NO_SYNC_EVT        	                  0x0010
#define STATUSPANEL_FR_PRESSED_EVT                        0x0020
  

#define STATUSPANEL_END_DEVICE_REJOIN_DELAY 10000

/*********************************************************************
 * MACROS
 */
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * VARIABLES
 */

extern SimpleDescriptionFormat_t zclStatusPanel_SimpleDesc;

extern CONST zclCommandRec_t zclStatusPanel_Cmds[];

extern CONST uint8 zclCmdsArraySize;

// attribute list
extern CONST zclAttrRec_t zclStatusPanel_Attrs[];
extern CONST uint8 zclStatusPanel_NumAttributes;

// custom attributes
#define ATTRID_INSIDE_TEMPERATURE 41364
#define ATTRID_OUTSIDE_TEMPERATURE 41365
#define ATTRID_WARNINGS 41366

// Identify attributes
extern uint16 zclStatusPanel_IdentifyTime;
extern uint8  zclStatusPanel_IdentifyCommissionState;

extern int16 zclStatusPanel_OutsideTemperature;
extern int16 zclStatusPanel_InsideTemperature;
extern uint8 zclStatusPanel_Warnings;
extern uint8 zclStatusPanel_State;

/*********************************************************************
 * FUNCTIONS
 */

 /*
  * Initialization for the task
  */
extern void zclStatusPanel_Init( byte task_id );

/*
 *  Event Process for the task
 */
extern UINT16 zclStatusPanel_event_loop( byte task_id, UINT16 events );

extern void sp_sendReport(void);
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ZCL_STATUSPANEL_H */
