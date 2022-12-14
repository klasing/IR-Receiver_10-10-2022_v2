// crc
#define INIT_CRC				0xFFFFFFFF
#define POLY_CRC				0x04C11DB7

// frame
#define SOH						1
#define STX						2
#define ETX						3
#define ETB						23
#define EOT						4

// serial communication
#define LEN_FRAME				9
#define LEN_CRC					4
#define DELAY_4HZ_SERIAL		125
#define BUFFER_MAX_SERIAL		512

// control
#define IDC_STATUS_CONNECT		33000
#define IDC_NOF_TRANSMISSION	33001
#define IDC_NOF_ERROR_CRC		33002
#define IDC_STATUSBAR			33003

// message
#define CONNECT_SERIAL			33100
#define DISCONNECT_SERIAL		33101
#define RESTART_SERIAL			33102
#define SET_TEXT_STATUSBAR		33103

//#define SOH						1
//#define STX						2
//#define ETX						3
//#define ETB						23
//#define EOT						4
//
//#define BUFFER_MAX_SERIAL		512
//#define DELAY_DFLT_SERIAL		125
//#define LEN_FRAME				9
//
//// control
//#define IDC_CONNECT				33000
//#define IDC_DISCONNECT			33001
//#define IDC_STATUS_SERIAL		33002
//#define IDC_CB_REGISTER			33603
//#define IDC_RB_WRITE			33604
//#define IDC_RB_READ				33605
//#define IDC_TRANSMIT			33606
//#define IDC_RECEIVE				33607
//
//// serial command
//// read commands
//#define RD_REG_0   				33600
//#define RD_REG_1   				33601
//#define RD_REG_2   				33602
//// write commands
//#define WR_REG_0   				33610
//#define WR_REG_1    			33611
//#define WR_REG_2   				33612
#define ERROR_CRC_STM32				33690	// STM32 sends this command when it detected a crc error

//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by STM32-Win32_rs232_communication_13-11-2022_v1.rc

#define IDS_APP_TITLE			103

#define IDR_MAINFRAME			128
#define IDD_STM32WIN32RS232COMMUNICATION13112022V1_DIALOG	102
#define IDD_ABOUTBOX			103
#define IDM_ABOUT				104
#define IDM_EXIT				105
#define IDI_STM32WIN32RS232COMMUNICATION13112022V1			107
#define IDI_SMALL				108
#define IDC_STM32WIN32RS232COMMUNICATION13112022V1			109
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
