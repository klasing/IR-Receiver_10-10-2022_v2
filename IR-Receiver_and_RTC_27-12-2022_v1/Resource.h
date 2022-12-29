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
#define LEN_MAX_ENTRY			31
#define LEN_FRAME				38
#define LEN_CRC					4
#define DELAY_SERIAL_COMM		125
#define BUFFER_MAX_SERIAL		512

// control
#define IDC_STATUS_CONNECT		33000
#define IDC_NOF_TRANSMISSION	33001
#define IDC_NOF_ERROR_CRC		33002
#define IDC_CODE				33003
#define IDC_DESCRIPTION			33004
#define IDC_CHB_CHOOSE			33005
#define IDC_EDT					33006
#define IDC_STATUSBAR			33007

// message
#define CONNECT_SERIAL			33100
#define DISCONNECT_SERIAL		33101
#define RESTART_SERIAL			33102

// timer
#define IDT_TIMER				33200

//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by IR-Receiver_and_RTC_27-12-2022_v1.rc

#define IDS_APP_TITLE			103

#define IDR_MAINFRAME			128
#define IDD_IRRECEIVERANDRTC27122022V1_DIALOG	102
#define IDD_ABOUTBOX			103
#define IDM_ABOUT				104
#define IDM_EXIT				105
#define IDI_IRRECEIVERANDRTC27122022V1			107
#define IDI_SMALL				108
#define IDC_IRRECEIVERANDRTC27122022V1			109
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
