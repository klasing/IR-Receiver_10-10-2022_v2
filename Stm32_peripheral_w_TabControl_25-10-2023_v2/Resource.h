// crc
#define INIT_CRC												0xFFFFFFFF
#define POLY_CRC												0x04C11DB7
// frame
#define SOH														1
#define STX														2
#define ETX														3
#define ETB														23
#define EOT														4
// characters used in g_oFrame.payload for acknowledgement
#define ACK														6
#define NAK														21
// serial communication
#define LEN_MAX_ENTRY											32	// max length payload
#define BUFFER_MAX_SERIAL										512
#define LEN_DATE_TIME											7
#define DELAY_4HZ_SERIAL										125 // 2 * 125 = 250, is 4 times per second, is 4 Hz
#define LEN_FRAME												39
#define LEN_CRC													4
// control
// common
#define IDC_STATUSBAR											33000
#define IDC_TABCONTROL											33001
// resource
#define CONNECT_SERIAL											33100
#define DISCONNECT_SERIAL										33101
#define IDC_NOF_TRANSMISSION									33102
#define IDC_NOF_ERROR_CRC										33103
// serial communication
#define WR_DATE_TIME											33600 // write date and time to STM32, to initialize the RTC
#define WR_TEMP_RANGE_SENSOR									33601 // write temperature range to all sensors
#define NOP														33602 // no operation

//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by Stm32_peripheral_w_TabControl_25-10-2023_v2.rc

#define IDS_APP_TITLE			103

#define IDR_MAINFRAME			128
#define IDD_STM32PERIPHERALWTABCONTROL25102023V2_DIALOG	102
#define IDD_ABOUTBOX			103
#define IDM_ABOUT				104
#define IDM_EXIT				105
#define IDI_STM32PERIPHERALWTABCONTROL25102023V2			107
#define IDI_SMALL				108
#define IDC_STM32PERIPHERALWTABCONTROL25102023V2			109
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
