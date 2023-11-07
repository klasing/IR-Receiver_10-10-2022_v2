// crc
#define INIT_CRC												0xFFFFFFFF
#define POLY_CRC												0x04C11DB7
// frame
#define SOH														1
#define STX														2
#define ETX														3
#define ETB														23
#define EOT														4
#define ACK														6
#define NAK														21
// serial communication
#define LEN_DATE_TIME											7
#define LEN_MAX_ENTRY											31
#define LEN_FRAME												38
#define DELAY_4HZ_SERIAL										125 // 2 * 125 = 250, is 4 times per second, is 4 Hz
#define BUFFER_MAX_SERIAL										512
#define LEN_CRC													4
// control
#define IDC_STATUSBAR											33000
#define IDC_TABCONTROL											33001
#define IDC_NOF_TRANSMISSION									33002
#define IDC_NOF_ERROR_CRC										33003
#define IDC_CODE												33004
#define IDC_DESCRIPTION											33005
#define IDC_FAN_ON												33006
#define IDC_PWM_FAN												33007
#define IDC_RPM_FAN												33008
#define IDC_WR_FAN_STATE										33009
// message
#define CONNECT_SERIAL											33100
#define DISCONNECT_SERIAL										33101
// command serial communication
// write command
#define WR_DATE_TIME											33621 // write date and time to STM32, to initialize the RTC
#define FAN_STATE_CHANGED										33622 // receive fan state from STM32
#define WR_FAN_STATE											33623 // transmit fan state to STM32
//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by Stm32_Peripheral_w_TabControl_25-10-2023_v1.rc

#define IDS_APP_TITLE			103

#define IDR_MAINFRAME			128
#define IDD_STM32PERIPHERALWTABCONTROL25102023V1_DIALOG	102
#define IDD_ABOUTBOX			103
#define IDM_ABOUT				104
#define IDM_EXIT				105
#define IDI_STM32PERIPHERALWTABCONTROL25102023V1			107
#define IDI_SMALL				108
#define IDC_STM32PERIPHERALWTABCONTROL25102023V1			109
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
