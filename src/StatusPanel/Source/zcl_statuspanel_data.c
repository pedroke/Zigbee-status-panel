#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDConfig.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"

/* STATUSPANEL_TODO: Include any of the header files below to access specific cluster data
#include "zcl_poll_control.h"
#include "zcl_electrical_measurement.h"
#include "zcl_diagnostic.h"
#include "zcl_meter_identification.h"
#include "zcl_appliance_identification.h"
#include "zcl_appliance_events_alerts.h"
#include "zcl_power_profile.h"
#include "zcl_appliance_control.h"
#include "zcl_appliance_statistics.h"
#include "zcl_hvac.h"
*/

#include "zcl_statuspanel.h"

/*********************************************************************
 * CONSTANTS
 */

#define STATUSPANEL_DEVICE_VERSION     0
#define STATUSPANEL_FLAGS              0

#define STATUSPANEL_HWVERSION          1
#define STATUSPANEL_ZCLVERSION         1

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Global attributes
const uint16 zclStatusPanel_clusterRevision_all = 0x0001; 

// Basic Cluster
const uint8 zclStatusPanel_HWRevision = STATUSPANEL_HWVERSION;
const uint8 zclStatusPanel_ZCLVersion = STATUSPANEL_ZCLVERSION;
const uint8 zclStatusPanel_ManufacturerName[] = { 13 , 'O','p','e','n','S','m','a','r','t','H','o','m','e' };
const uint8 zclStatusPanel_ModelId[] = { 5, 'Z','i','g','S','P' };
const uint8 zclStatusPanel_DateCode[] = { 16, '2','0','2','2','0','3','0','1',' ',' ',' ',' ',' ',' ',' ',' ' };
const uint8 zclStatusPanel_PowerSource = POWER_SOURCE_BATTERY;

uint8 zclStatusPanel_LocationDescription[17] = { 16, ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' };
uint8 zclStatusPanel_PhysicalEnvironment = 0;
uint8 zclStatusPanel_DeviceEnable = DEVICE_ENABLED;

// Identify Cluster
uint16 zclStatusPanel_IdentifyTime;

/* STATUSPANEL_TODO: declare attribute variables here. If its value can change,
 * initialize it in zclStatusPanel_ResetAttributesToDefaultValues. If its
 * value will not change, initialize it here.
 */

/*********************************************************************
 * ATTRIBUTE DEFINITIONS - Uses REAL cluster IDs
 */
CONST zclAttrRec_t zclStatusPanel_Attrs[] =
{
  // *** General Basic Cluster Attributes ***
  {
    ZCL_CLUSTER_ID_GEN_BASIC,             // Cluster IDs - defined in the foundation (ie. zcl.h)
    {  // Attribute record
      ATTRID_BASIC_HW_VERSION,            // Attribute ID - Found in Cluster Library header (ie. zcl_general.h)
      ZCL_DATATYPE_UINT8,                 // Data Type - found in zcl.h
      ACCESS_CONTROL_READ,                // Variable access control - found in zcl.h
      (void *)&zclStatusPanel_HWRevision  // Pointer to attribute variable
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_ZCL_VERSION,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ,
      (void *)&zclStatusPanel_ZCLVersion
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_MANUFACTURER_NAME,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclStatusPanel_ManufacturerName
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_MODEL_ID,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclStatusPanel_ModelId
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_DATE_CODE,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclStatusPanel_DateCode
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_POWER_SOURCE,
      ZCL_DATATYPE_ENUM8,
      ACCESS_CONTROL_READ,
      (void *)&zclStatusPanel_PowerSource
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_LOCATION_DESC,
      ZCL_DATATYPE_CHAR_STR,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)zclStatusPanel_LocationDescription
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_PHYSICAL_ENV,
      ZCL_DATATYPE_ENUM8,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclStatusPanel_PhysicalEnvironment
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_BASIC_DEVICE_ENABLED,
      ZCL_DATATYPE_BOOLEAN,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclStatusPanel_DeviceEnable
    }
  },{
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_INSIDE_TEMPERATURE,
      ZCL_DATATYPE_INT16,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclStatusPanel_InsideTemperature
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_OUTSIDE_TEMPERATURE,
      ZCL_DATATYPE_INT16,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclStatusPanel_OutsideTemperature
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    { // Attribute record
      ATTRID_WARNINGS,
      ZCL_DATATYPE_UINT8,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclStatusPanel_Warnings
    }
  },
#ifdef ZCL_IDENTIFY
  // *** Identify Cluster Attribute ***
  {
    ZCL_CLUSTER_ID_GEN_IDENTIFY,
    { // Attribute record
      ATTRID_IDENTIFY_TIME,
      ZCL_DATATYPE_UINT16,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclStatusPanel_IdentifyTime
    }
  },
#endif
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    {  // Attribute record
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void *)&zclStatusPanel_clusterRevision_all
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_IDENTIFY,
    {  // Attribute record
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void *)&zclStatusPanel_clusterRevision_all
    }
  },
};

uint8 CONST zclStatusPanel_NumAttributes = ( sizeof(zclStatusPanel_Attrs) / sizeof(zclStatusPanel_Attrs[0]) );

/*********************************************************************
 * SIMPLE DESCRIPTOR
 */
// This is the Cluster ID List and should be filled with Application
// specific cluster IDs.
const cId_t zclStatusPanel_InClusterList[] =
{
  ZCL_CLUSTER_ID_GEN_BASIC,
  ZCL_CLUSTER_ID_GEN_IDENTIFY,
  
  // STATUSPANEL_TODO: Add application specific Input Clusters Here. 
  //       See zcl.h for Cluster ID definitions
  
};
#define ZCLSTATUSPANEL_MAX_INCLUSTERS   (sizeof(zclStatusPanel_InClusterList) / sizeof(zclStatusPanel_InClusterList[0]))


const cId_t zclStatusPanel_OutClusterList[] =
{
  ZCL_CLUSTER_ID_GEN_BASIC,
  
  // STATUSPANEL_TODO: Add application specific Output Clusters Here. 
  //       See zcl.h for Cluster ID definitions
};
#define ZCLSTATUSPANEL_MAX_OUTCLUSTERS  (sizeof(zclStatusPanel_OutClusterList) / sizeof(zclStatusPanel_OutClusterList[0]))


SimpleDescriptionFormat_t zclStatusPanel_SimpleDesc =
{
  STATUSPANEL_ENDPOINT,                  //  int Endpoint;
  ZCL_HA_PROFILE_ID,                     //  uint16 AppProfId;
  // STATUSPANEL_TODO: Replace ZCL_HA_DEVICEID_ON_OFF_LIGHT with application specific device ID
  ZCL_HA_DEVICEID_ON_OFF_LIGHT,          //  uint16 AppDeviceId; 
  STATUSPANEL_DEVICE_VERSION,            //  int   AppDevVer:4;
  STATUSPANEL_FLAGS,                     //  int   AppFlags:4;
  ZCLSTATUSPANEL_MAX_INCLUSTERS,         //  byte  AppNumInClusters;
  (cId_t *)zclStatusPanel_InClusterList, //  byte *pAppInClusterList;
  ZCLSTATUSPANEL_MAX_OUTCLUSTERS,        //  byte  AppNumInClusters;
  (cId_t *)zclStatusPanel_OutClusterList //  byte *pAppInClusterList;
};

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

