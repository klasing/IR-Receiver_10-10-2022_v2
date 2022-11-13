#define BUFFER_MAX_SERIAL		512
// control
#define IDC_STATUS_CONNECT_		33000
#define IDC_LV_CONFIGURATION	33001
#define IDC_LV_T_LO				33002
#define IDC_LV_T_HI				33003
#define IDC_LV_TREG				33004
#define IDC_CB_FAULT_QUEUE		33005
#define IDC_CB_POLARITY_ALERT	33006
#define IDC_CB_MODE_THERMOSTAT	33007
#define IDC_RCV_MESSAGE			33008
// message
#define CONNECT_SERIAL			33100
#define DISCONNECT_SERIAL		33101
#define APPLY_SETTING			33102
// string
#define IDS_LVCONFIG_COL0		33500
#define IDS_LVCONFIG_COL1		33501
#define IDS_LVCONFIG_COL2		33502
#define IDS_LVCONFIG_COL3		33503
#define IDS_LVCONFIG_COL4		33504
#define IDS_LVCONFIG_COL5		33505
#define IDS_LVCONFIG_COL6		33506
#define IDS_LVCONFIG_COL7		33507
#define IDS_LVCONFIG_COL8		33508
#define IDS_LVCONFIG_COL9		33509
#define IDS_LVCONFIG_COL10		33510
#define IDS_LVCONFIG_COL11		33511

#define IDS_LVTEMP_COL0			33512
#define IDS_LVTEMP_COL1			33513
#define IDS_LVTEMP_COL2			33514
#define IDS_LVTEMP_COL3			33515
#define IDS_LVTEMP_COL4			33516
#define IDS_LVTEMP_COL5			33517
#define IDS_LVTEMP_COL6			33518
#define IDS_LVTEMP_COL7			33519
// serial command
#define RD_REG_TEMP				33600 // address to approach two bytes with 12/13 bits data (with 13 bits data LSB=1 in second byte)
#define RD_REG_CNFG				33601 // address to approach two bytes with 12 bits data
#define RD_REG_T_LO				33602 // address to approach two bytes with 12/13 bits data
#define RD_REG_T_HI				33603 // address to approach two bytes with 12/13 bits data

// control
#define IDC_STATUS_CONNECT		33002
#define IDC_RCV_MSG				33003
#define IDC_RCV_MSG_MULTILINE	33004
#define IDC_TRM_MSG				33005
// message
#define START_RECEIVE			33102
#define SERIAL_DISCONNECTED		33103
#define START_TRANSMIT			33104
//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by TMP102_temp_sensor_30-10-2022_v1.rc

#define IDS_APP_TITLE			103

#define IDR_MAINFRAME			128
#define IDD_TMP102TEMPSENSOR30102022V1_DIALOG	102
#define IDD_ABOUTBOX			103
#define IDM_ABOUT				104
#define IDM_EXIT				105
#define IDI_TMP102TEMPSENSOR30102022V1			107
#define IDI_SMALL				108
#define IDC_TMP102TEMPSENSOR30102022V1			109
#define IDC_MYICON				2
#ifndef IDC_STATIC
#define IDC_STATIC				-1
#endif
// Next default values for new objects
//
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS

#define _APS_NO_MFC					130
#define _APS_NEXT_RESOURCE_VALUE	129
#define _APS_NEXT_COMMAND_VALUE		32771
#define _APS_NEXT_CONTROL_VALUE		1000
#define _APS_NEXT_SYMED_VALUE		110
#endif
#endif
