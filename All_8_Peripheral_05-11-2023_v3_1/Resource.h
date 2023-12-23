// crc
#define INIT_CRC						0xFFFFFFFF
#define POLY_CRC						0x04C11DB7
// frame
#define SOH								1
#define STX								2
#define ETX								3
#define ETB								23
#define EOT								4
// characters used in g_oFrame.payload for acknowledgement
#define ACK								6
#define NAK								21
// serial communication
#define LEN_DATE_TIME					7
#define MAX_LEN_ENTRY					32	// max length payload
#define LEN_FRAME						43
#define LEN_CRC							4
#define OFFSET_PAYLOAD					8
#define MAX_LEN_TEXT_BUFFER				MAX_LEN_ENTRY
#define MAX_BUFFER_SERIAL				512
#define DELAY_TIME_SERIAL				25 // (ms) roundtrip is 50 ms
// control
// common
#define IDC_STATUSBAR					33000
#define IDC_TABCONTROL					33001
// resource
#define CONNECT_SERIAL					33100
#define DISCONNECT_SERIAL				33101
#define IDC_NOF_TRANSMISSION			33102
#define IDC_NOF_ERROR_CRC				33103

#define IDC_CODE						33104
#define IDC_DESCRIPTION					33105
#define IDC_RUWIDO_REMOTE				33106
#define IDC_YAMAHA_REMOTE				33107
// serial communication
#define WR_NOP							33699 // 0x000083A3 no operation
#define RD_IR_REMOTE					33610 // 0x0000834A read code and description from IR-remote

#define IDT_TIMER						33999

//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by All_8_Peripheral_05-11-2023_v3_1.rc

#define IDS_APP_TITLE			103

#define IDR_MAINFRAME			128
#define IDD_ALL8PERIPHERAL05112023V31_DIALOG	102
#define IDD_ABOUTBOX			103
#define IDM_ABOUT				104
#define IDM_EXIT				105
#define IDI_ALL8PERIPHERAL05112023V31			107
#define IDI_SMALL				108
#define IDC_ALL8PERIPHERAL05112023V31			109
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
