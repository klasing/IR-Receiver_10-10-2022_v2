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
#define DELAY_TIME_SERIAL				250 // (ms) roundtrip is 1.2 s
/*#define DELAY_TIME_SERIAL				25 // (ms) roundtrip is 50 ms*/
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
#define BTN_YAMAHA_REMOTE				33108

#define IDC_FAN_ON						33109
#define IDC_FAN_ON_OFF					33110
#define IDC_PWM_FAN						33111
#define IDC_RPM_FAN						33112
#define BTN_STATE_FAN					33113 // button that applies fan state

#define IDC_RELAY1						33114
#define IDC_RELAY2						33115
#define IDC_RELAY3						33116
#define IDC_RELAY4						33117
#define IDC_RELAY5						33118
#define IDC_RELAY6						33119
#define IDC_RELAY7						33120
#define IDC_RELAY8						33121
#define BTN_STATE_RELAY					33122 // button that applies relay state

#define IDC_TEMP_SENSOR1				33123
#define IDC_TEMP_SENSOR2				33124
#define IDC_TEMP_SENSOR3				33125
#define IDC_TEMP_SENSOR4				33126
#define IDC_ALERT_SENSOR1				33127
#define IDC_ALERT_SENSOR2				33128
#define IDC_ALERT_SENSOR3				33129
#define IDC_ALERT_SENSOR4				33130
#define IDC_TEMP_HI_SENSOR1				33131
#define IDC_TEMP_HI_SENSOR2				33132
#define IDC_TEMP_HI_SENSOR3				33133
#define IDC_TEMP_HI_SENSOR4				33134
#define IDC_TEMP_LO_SENSOR1				33135
#define IDC_TEMP_LO_SENSOR2				33136
#define IDC_TEMP_LO_SENSOR3				33137
#define IDC_TEMP_LO_SENSOR4				33138
#define BTN_RANGE_SENSOR				33139 // button that applies temp range
// serial communication
#define WR_DATE_TIME					33600 // 0x00008340 write date and time to STM32, to initialize the RTC
#define WR_STATE_FAN					33601 // 0x00008341 write state fan
#define WR_STATE_RELAY					33602 // 0x00008342 write state of all relays to STM32
#define WR_RANGE_SENSOR					33603 // 0x00008343 write range sensor to all sensors
#define WR_YAMAHA_REMOTE				33604 // 0x00008344 write Yamaha IR-remote mode
#define WR_NOP							33609 // 0x000083A3 write no operation
#define RD_IR_REMOTE					33610 // 0x0000834A read code and description from IR-remote
#define RD_STATE_FAN					33611 // 0x0000834B read state fan
#define RD_STATE_RELAY					33612 // 0x0000834C read state to all relays
#define RD_RANGE_SENSOR					33613 // 0x0000834D read temperature range from all sensors
#define RD_TEMP_SENSOR					33614 // 0x0000834E read temperature and alert from all sensors

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
