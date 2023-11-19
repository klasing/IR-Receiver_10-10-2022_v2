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

#define LEN_MAX_TEXT_BUFFER										32
// control
// common
#define IDC_STATUSBAR											33000
#define IDC_TABCONTROL											33001
// resource
#define CONNECT_SERIAL											33100
#define DISCONNECT_SERIAL										33101
#define IDC_NOF_TRANSMISSION									33102
#define IDC_NOF_ERROR_CRC										33103

#define IDC_CODE												33104
#define IDC_DESCRIPTION											33105

#define IDC_FAN_ON												33106
#define IDC_PWM_FAN												33107
#define IDC_RPM_FAN												33108
#define BTN_STATE_FAN											33109 // button that applies fan state

#define IDC_RELAY1												33110
#define IDC_RELAY2												33111
#define IDC_RELAY3												33112
#define IDC_RELAY4												33113
#define IDC_RELAY5												33114
#define IDC_RELAY6												33115
#define IDC_RELAY7												33116
#define IDC_RELAY8												33117
#define BTN_STATE_RELAY											33118 // button that applies relay state

#define IDC_TEMP_SENSOR1										33119
#define IDC_TEMP_SENSOR2										33120
#define IDC_TEMP_SENSOR3										33121
#define IDC_TEMP_SENSOR4										33122
#define IDC_ALERT_SENSOR1										33123
#define IDC_ALERT_SENSOR2										33124
#define IDC_ALERT_SENSOR3										33125
#define IDC_ALERT_SENSOR4										33126
#define IDC_TEMP_HI_SENSOR1										33127
#define IDC_TEMP_HI_SENSOR2										33128
#define IDC_TEMP_HI_SENSOR3										33129
#define IDC_TEMP_HI_SENSOR4										33130
#define IDC_TEMP_LO_SENSOR1										33131
#define IDC_TEMP_LO_SENSOR2										33132
#define IDC_TEMP_LO_SENSOR3										33133
#define IDC_TEMP_LO_SENSOR4										33134
#define BTN_RANGE_SENSOR										33135 // button that applies temp range
// serial communication
#define WR_DATE_TIME											33600 // write date and time to STM32, to initialize the RTC
#define WR_RANGE_SENSOR											33601 // write temperature range to all sensors
#define WR_STATE_FAN											33602 // write state fan (On/Off)
#define WR_STATE_RELAY											33603 // write state to all relays
#define NOP														33604 // no operation

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
