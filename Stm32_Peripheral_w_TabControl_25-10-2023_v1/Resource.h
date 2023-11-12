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
#define LEN_MAX_ENTRY											32//31
#define LEN_FRAME												39//38//9
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
#define IDC_RELAY1												33010
#define IDC_RELAY2												33011
#define IDC_RELAY3												33012
#define IDC_RELAY4												33013
#define IDC_RELAY5												33014
#define IDC_RELAY6												33015
#define IDC_RELAY7												33016
#define IDC_RELAY8												33017
#define IDC_WR_RELAY_STATE										33018
#define IDC_TEMP_SENSOR1										33019
#define IDC_TEMP_SENSOR2										33020
#define IDC_TEMP_SENSOR3										33021
#define IDC_TEMP_SENSOR4										33022
#define IDC_ALERT_SENSOR1										33023
#define IDC_ALERT_SENSOR2										33024
#define IDC_ALERT_SENSOR3										33025
#define IDC_ALERT_SENSOR4										33026
#define IDC_TEMP_HI_SENSOR1										33627
#define IDC_TEMP_HI_SENSOR2										33628
#define IDC_TEMP_HI_SENSOR3										33629
#define IDC_TEMP_HI_SENSOR4										33630
#define IDC_TEMP_LO_SENSOR1										33631
#define IDC_TEMP_LO_SENSOR2										33632
#define IDC_TEMP_LO_SENSOR3										33633
#define IDC_TEMP_LO_SENSOR4										33634
#define IDC_WR_TEMP_RANGE										33635
// message
#define CONNECT_SERIAL											33100
#define DISCONNECT_SERIAL										33101
// timer
#define IDT_POLLING_TIMER										33200
// command serial communication
// write command
#define WR_DATE_TIME											33621 // write date and time to STM32, to initialize the RTC
#define FAN_STATE_CHANGED										33622 // receive fan state from STM32
#define WR_FAN_STATE											33623 // write fan state to STM32
#define WR_RELAY_STATE											33624 // write relay state to STM32
#define WR_TEMP_RANGE											33625 // write temp range to STM32
// read command
#define RD_FAN_STATE											33626
#define RD_REG_TEMP												33627

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
