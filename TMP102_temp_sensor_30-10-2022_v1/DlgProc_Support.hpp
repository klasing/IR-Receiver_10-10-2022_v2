#pragma once
//****************************************************************************
//*                     note
//*
//* TODO: adjust the delay when changing the conversion rate
//* TODO: modify the temperature conversion to degrees Celsius when changing 
//*       to 13-bit extended mode
//* TODO: see command from button TMP102_RESET
//*****************************************************************************
//*****************************************************************************
//*                     include
//*****************************************************************************
#include "framework.h"
//#include "TMP102_temp_sensor_30-10-2022_v1.h"

//****************************************************************************
//*                     typedef
//*
//* when writing to the device, the device will respond with OK in the payload
//* O=79 0x4F 0100 1111
//* K=75 0x4B 0100 1011
//* payload=0x4F4B
//****************************************************************************
typedef struct tagFRAME
{
	const CHAR soh;
	UINT16 cmnd;
	const CHAR stx;
	INT16 payload;
	const CHAR etx;
	const CHAR etb;
	const CHAR eot;
} FRAME, *PFRAME;

// bitfield CONFIGURATION ////////////////////////////////////////////////////
typedef struct tagCONFIGURATION
{
	UINT8 oneShot : 1;
	UINT8 resolution : 2;
	UINT8 faultQueue : 2;
	UINT8 polarity : 1;
	UINT8 modeThermostat : 1;
	UINT8 shutDown : 1;
	UINT8 conversionRate : 2;
	UINT8 alert : 1;
	UINT8 modeExtended : 1;
	UINT8 : 4;
	VOID setHiByte(const UINT8& byte)
	{
		oneShot = (byte & 0x80) >> 7;
		resolution = (byte & 0x60) >> 5;
		faultQueue = (byte & 0x18) >> 3;
		polarity = (byte & 0x04) >> 2;
		modeThermostat = (byte & 0x02) >> 1;
		shutDown = byte & 0x01;
	}
	UINT16 getHiByte()
	{
		UINT16 word = 0;
		word = (oneShot << 7)
			| (resolution << 5)
			| (faultQueue << 3)
			| (polarity << 2)
			| (modeThermostat << 1)
			| shutDown;
		return word;
	}
	VOID setLoByte(const UINT8& byte)
	{
		conversionRate = (byte & 0xC0) >> 6;
		alert = (byte & 0x20) >> 5;
		modeExtended = (byte & 0x10) >> 4;
	}
	UINT16 getLoByte()
	{
		UINT16 word = 0;
		word = (conversionRate << 6)
			| (alert << 5)
			| (modeExtended << 4);
		return word;
	}
	VOID setOneShot(const BOOL& b)
	{
		oneShot = (b) ? TRUE : FALSE;
	}
	BOOL getOneShot()
	{
		return oneShot;
	}
	// read only
	//VOID setResolution(const CHAR& byte)
	//{
	//	resolution = byte & 0x03;
	//}
	UINT8 getResolution()
	{
		return resolution;
	}
	VOID setFaultQueue(const UINT8& byte)
	{
		faultQueue = byte & 0x03;
	}
	UINT8 getFaultQueue()
	{
		return faultQueue;
	}
	VOID setPolarity(const BOOL& b)
	{
		polarity = (b) ? TRUE : FALSE;
	}
	BOOL getPolarity()
	{
		return polarity;
	}
	VOID setModeThermostat(const BOOL& b)
	{
		modeThermostat = (b) ? TRUE : FALSE;
	}
	BOOL getModeThermostat()
	{
		return modeThermostat;
	}
	VOID setShutDown(const BOOL& b)
	{
		shutDown = (b) ? TRUE : FALSE;
	}
	BOOL getShutDown()
	{
		return shutDown;
	}
	VOID setConversionRate(const UINT8& byte)
	{
		conversionRate = byte & 0x03;
	}
	UINT8 getConversionRate()
	{
		return conversionRate;
	}
	// read only
	//VOID setAlert(const BOOL& b)
	//{
	//	alert = (b) ? TRUE : FALSE;
	//}
	BOOL getAlert()
	{
		return alert;
	}
	VOID setModeExtended(const BOOL& b)
	{
		modeExtended = (b) ? TRUE : FALSE;
	}
	BOOL getModeExtended()
	{
		return modeExtended;
	}
} CONFIGURATION, *PCONFIGURATION;

// 12-bit bitfield TEMPERATURE ///////////////////////////////////////////////
typedef struct tagBIT12TEMP
{
	INT16 temp : 12;
	FLOAT fTempInClcs = 0.;
	FLOAT fTempInClcsTimes100 = 0.;
	CHAR chBufferTempInCelcius[LEN_TEMP_IN_CLCS] = { 0 };
	VOID setHiByte(INT8 byte)
	{
		// must be called before setLoByte is called
		temp = byte << 4;
	}
	VOID setLoByte(UINT8 byte)
	{
		temp |= byte >> 4;
	}
	// this member func will also set:
	// fTempInClcs, 
	// fTempInClcsTimes100, and
	// chBufferTempInCelcius
	CHAR* getTempInClcs_toStringA()
	{
		INT16 val = temp;
		if (val & 0x8000)
		{
			val = ~val;
			val += 1;
		}
		fTempInClcs = (FLOAT)(val * 0.0625);
		fTempInClcsTimes100 = fTempInClcs * 100;
		sprintf_s(chBufferTempInCelcius
			, LEN_TEMP_IN_CLCS
			, "%d.%02d"
			, ((UINT)fTempInClcsTimes100 / 100)
			, ((UINT)fTempInClcsTimes100 % 100)
		);
		return chBufferTempInCelcius;
	}
} BIT12TEMP, *PBIT12TEMP;

// 12-bit bitfield MEASURED TEMPERATURE //////////////////////////////////////
typedef struct tagBIT12MSRDTEMP : tagBIT12TEMP
{
	UINT8 alert : 1;
	UINT8 modeExtended : 1;
	VOID setAlert(UINT8 byte)
	{
		alert = (byte & 0x02) >> 1;
	}
	BOOL getAlert()
	{
		return alert;
	}
	VOID setModeExtended(UINT8 byte)
	{
		modeExtended = byte & 0x01;
	}
	BOOL getModeExtended()
	{
		return modeExtended;
	}
} BIT12MSRDTEMP, *PBIT12MSRDTEMP;

// 13-bit bitfield TEMPERATURE ///////////////////////////////////////////////

//****************************************************************************
//*                     extern
//****************************************************************************
extern HWND g_hDlg;

//****************************************************************************
//*                     global
//****************************************************************************
HANDLE g_hComm = INVALID_HANDLE_VALUE;
// Resource Aquisition Is Initialisation RAII 
FRAME g_oFrame = { SOH, 0, STX, 0, ETX, ETB, EOT };
BOOL g_bContinueTxRx = FALSE;
CHAR g_chBuffer[BUFFER_MAX_SERIAL] = { 0 };
//UINT8 g_alert_bit = 0;
UINT8 g_cReceive = 0;
//INT16 g_iTempLo = 0;
//INT16 g_iTempHi = 0;
//INT16 g_iTempLoTimes100 = 0;
//INT16 g_iTempHiTimes100 = 0;
//BOOL g_bModeThermostat = FALSE;
//BOOL g_bCheckedStateChbOneshot = FALSE;
//BOOL g_bCheckedStateChbShutdown = FALSE;
//BOOL g_bCheckedStateChbExtended = FALSE;
//BOOL g_bPolarity = FALSE;
//UINT8 g_hiByteCnfg = 0, g_loByteCnfg = 0;

CONFIGURATION g_oConfiguration{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };
BIT12TEMP g_oTempLo{ 0 };
BIT12TEMP g_oTempHi{ 0 };
// TODO: initializing is not yet under control
BIT12MSRDTEMP g_oMsrdTemp;
FRAME g_oFrameEx = { SOH, 0, STX, 0, ETX, ETB, EOT };
std::queue<tagFRAME> g_queue;

Connect2SQLite g_oSqlite;
int g_rc = 0;

//*****************************************************************************
//*                     prototype
//*****************************************************************************
UINT16				clcsToBit12(const HWND& hWnd, BIT12TEMP& bit12Temp);
BOOL				connect(HANDLE& hComm);
DWORD WINAPI		TxRx(LPVOID lpVoid);
BOOL                transmit();
BOOL                receive(LPVOID lpVoid);
//FLOAT bit12toclcs(const INT8& hiByte
//	, const UINT8& loByte
//);
VOID updateListViewItemEx(const HWND& hWndLV
	, const INT& iItem
	, const INT& iSubItem
	, const PWCHAR& pszItem
);
BOOL getSetting(const HWND& hDlg);
BOOL updateSetting(const HWND& hDlg
	, const UINT8& hiByte
	, const UINT8& loByte
);
BOOL updateRegister(const HWND& hWndDlgItem
	, const UINT8 hiByte
	, const UINT8 loByte
	, const UINT8 nofBits = 16
);

//*****************************************************************************
//*                     onWmInitDialog_DlgProc
//*****************************************************************************
BOOL onWmInitDialog_DlgProc(const HINSTANCE& hInst
	, const HWND& hDlg
	, HWND& hWndLV_Cnfg
	, HWND& hWndLV_T_lo
	, HWND& hWndLV_T_hi
	, HWND& hWndLV_Treg
	, RECT& rect
	, HBRUSH& brush
	, const HBRUSH& bkColorDlgBrush
)
{
	// set edit control IDC_STATUS_CONNECT text
	SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT)
		, WM_SETTEXT
		, (WPARAM)0
		, (LPARAM)L"Disconnected"
	);

	// disable button DISCONNECT_SERIAL
	EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), FALSE);

	// create listview CONFIGURATION /////////////////////////////////////
	hWndLV_Cnfg = createListView(hInst, hDlg, IDC_LV_CONFIGURATION);
	// add column to listview
	addColumn(hInst
		, hWndLV_Cnfg
		, 12
		, IDS_LVCONFIG_COL0, IDS_LVCONFIG_COL1, IDS_LVCONFIG_COL2
		, IDS_LVCONFIG_COL3, IDS_LVCONFIG_COL4, IDS_LVCONFIG_COL5
		, IDS_LVCONFIG_COL6, IDS_LVCONFIG_COL7, IDS_LVCONFIG_COL8
		, IDS_LVCONFIG_COL9, IDS_LVCONFIG_COL10, IDS_LVCONFIG_COL11
	);
	// adjust column with to the header text width
	// and add one row with empty listview item
	for (uint8_t i = 0; i < 12; i++)
	{
		ListView_SetColumnWidth(hWndLV_Cnfg, i, LVSCW_AUTOSIZE_USEHEADER);
		createListViewItem(hWndLV_Cnfg
			, (PWCHAR)L" "
			, 0
			, i
		);
	}

	// create listview TEMPERATURE LOW ///////////////////////////////////
	hWndLV_T_lo = createListView(hInst, hDlg, IDC_LV_T_LO);
	// add column to listview
	addColumn(hInst
		, hWndLV_T_lo
		, 16
		, IDS_LVTEMP_COL0, IDS_LVTEMP_COL1, IDS_LVTEMP_COL2, IDS_LVTEMP_COL3
		, IDS_LVTEMP_COL4, IDS_LVTEMP_COL5, IDS_LVTEMP_COL6, IDS_LVTEMP_COL7
		, IDS_LVTEMP_COL0, IDS_LVTEMP_COL1, IDS_LVTEMP_COL2, IDS_LVTEMP_COL3
		, IDS_LVTEMP_COL4, IDS_LVTEMP_COL5, IDS_LVTEMP_COL6, IDS_LVTEMP_COL7
	);
	// adjust column with to the header text width
	// and add one row with empty listview item
	for (uint8_t i = 0; i < 16; i++)
	{
		ListView_SetColumnWidth(hWndLV_T_lo, i, LVSCW_AUTOSIZE_USEHEADER);
		createListViewItem(hWndLV_T_lo
			, (PWCHAR)L" "
			, 0
			, i
		);
	}

	// create listview TEMPERATURE HIGH //////////////////////////////////
	hWndLV_T_hi = createListView(hInst, hDlg, IDC_LV_T_HI);
	// add column to listview
	addColumn(hInst
		, hWndLV_T_hi
		, 16
		, IDS_LVTEMP_COL0, IDS_LVTEMP_COL1, IDS_LVTEMP_COL2, IDS_LVTEMP_COL3
		, IDS_LVTEMP_COL4, IDS_LVTEMP_COL5, IDS_LVTEMP_COL6, IDS_LVTEMP_COL7
		, IDS_LVTEMP_COL0, IDS_LVTEMP_COL1, IDS_LVTEMP_COL2, IDS_LVTEMP_COL3
		, IDS_LVTEMP_COL4, IDS_LVTEMP_COL5, IDS_LVTEMP_COL6, IDS_LVTEMP_COL7
	);
	// adjust column with to the header text width
	// and add one row with empty listview item
	for (uint8_t i = 0; i < 16; i++)
	{
		ListView_SetColumnWidth(hWndLV_T_hi, i, LVSCW_AUTOSIZE_USEHEADER);
		createListViewItem(hWndLV_T_hi
			, (PWCHAR)L" "
			, 0
			, i
		);
	}

	// create listview TEMPERATURE REGISTER //////////////////////////////
	hWndLV_Treg = createListView(hInst, hDlg, IDC_LV_TREG);
	// add column to listview
	addColumn(hInst
		, hWndLV_Treg
		, 16
		, IDS_LVTEMP_COL0, IDS_LVTEMP_COL1, IDS_LVTEMP_COL2, IDS_LVTEMP_COL3
		, IDS_LVTEMP_COL4, IDS_LVTEMP_COL5, IDS_LVTEMP_COL6, IDS_LVTEMP_COL7
		, IDS_LVTEMP_COL0, IDS_LVTEMP_COL1, IDS_LVTEMP_COL2, IDS_LVTEMP_COL3
		, IDS_LVTEMP_COL4, IDS_LVTEMP_COL5, IDS_LVTEMP_COL6, IDS_LVTEMP_COL7
	);
	// adjust column with to the header text width
	// and add one row with empty listview item
	for (uint8_t i = 0; i < 16; i++)
	{
		ListView_SetColumnWidth(hWndLV_Treg, i, LVSCW_AUTOSIZE_USEHEADER);
		createListViewItem(hWndLV_Treg
			, (PWCHAR)L" "
			, 0
			, i
		);
	}

	// populate combobox fault queue /////////////////////////////////////////
	SendMessage(GetDlgItem(hDlg, IDC_CB_FAULT_QUEUE)
		, CB_ADDSTRING
		, (WPARAM)0
		, (LPARAM)L"1"
	);
	SendMessage(GetDlgItem(hDlg, IDC_CB_FAULT_QUEUE)
		, CB_ADDSTRING
		, (WPARAM)0
		, (LPARAM)L"2"
	);
	SendMessage(GetDlgItem(hDlg, IDC_CB_FAULT_QUEUE)
		, CB_ADDSTRING
		, (WPARAM)0
		, (LPARAM)L"4"
	);
	SendMessage(GetDlgItem(hDlg, IDC_CB_FAULT_QUEUE)
		, CB_ADDSTRING
		, (WPARAM)0
		, (LPARAM)L"6"
	);
	// set the default value for the fault queue
	SendMessage(GetDlgItem(hDlg, IDC_CB_FAULT_QUEUE)
		, CB_SETCURSEL
		, (WPARAM)0
		, (LPARAM)0
	);

	// populate combobox polarity alert //////////////////////////////////////
	SendMessage(GetDlgItem(hDlg, IDC_CB_POLARITY_ALERT)
		, CB_ADDSTRING
		, (WPARAM)0
		, (LPARAM)L"Active low"
	);
	SendMessage(GetDlgItem(hDlg, IDC_CB_POLARITY_ALERT)
		, CB_ADDSTRING
		, (WPARAM)0
		, (LPARAM)L"Active high"
	);
	// set the default value for the fault queue
	SendMessage(GetDlgItem(hDlg, IDC_CB_POLARITY_ALERT)
		, CB_SETCURSEL
		, (WPARAM)0
		, (LPARAM)0
	);

	// populate combobox mode thermostat /////////////////////////////////////
	SendMessage(GetDlgItem(hDlg, IDC_CB_MODE_THERMOSTAT)
		, CB_ADDSTRING
		, (WPARAM)0
		, (LPARAM)L"Comparator"
	);
	SendMessage(GetDlgItem(hDlg, IDC_CB_MODE_THERMOSTAT)
		, CB_ADDSTRING
		, (WPARAM)0
		, (LPARAM)L"Interrupt"
	);
	// set the default value for the mode thermostat
	SendMessage(GetDlgItem(hDlg, IDC_CB_MODE_THERMOSTAT)
		, CB_SETCURSEL
		, (WPARAM)0
		, (LPARAM)0
	);

	// populate combobox rate conversion /////////////////////////////////////
	SendMessage(GetDlgItem(hDlg, IDC_CB_RATE_CONVERSION)
		, CB_ADDSTRING
		, (WPARAM)0
		, (LPARAM)L"0.25"
	);
	SendMessage(GetDlgItem(hDlg, IDC_CB_RATE_CONVERSION)
		, CB_ADDSTRING
		, (WPARAM)0
		, (LPARAM)L"1"
	);
	SendMessage(GetDlgItem(hDlg, IDC_CB_RATE_CONVERSION)
		, CB_ADDSTRING
		, (WPARAM)0
		, (LPARAM)L"4"
	);
	SendMessage(GetDlgItem(hDlg, IDC_CB_RATE_CONVERSION)
		, CB_ADDSTRING
		, (WPARAM)0
		, (LPARAM)L"8"
	);
	// set the default value for the rate conversion
	SendMessage(GetDlgItem(hDlg, IDC_CB_RATE_CONVERSION)
		, CB_SETCURSEL
		, (WPARAM)2
		, (LPARAM)0
	);

	// initialize rect
	rect.left = 450;
	rect.top = 327;
	rect.right = 503;
	rect.bottom = 337;

	// default brush color
	brush = bkColorDlgBrush;

	// open database
	g_rc = g_oSqlite.openDb("Resource\\tmp102.db");
	// if not exists: create table
	g_rc = g_oSqlite.createTable(IDR_MEASUREMENT, "measurement");
	g_rc = g_oSqlite.createTable(IDR_VALUE_MEASUREMENT, "value_measurement");

	// TEST create tuple in table measurement: name is measurement1
	std::initializer_list<std::string> list{ "measurement1" };
	// this will be done only once,
	// every second time the func insertTuple() will return 19 (g_rc = 19)
	// SQLite Error 19: 'UNIQUE constraint failed'
	g_rc = g_oSqlite.insertTuple(IDR_MEASUREMENT
		, "measurement"
		, list
	);

	return EXIT_SUCCESS;
}

//*****************************************************************************
//*                     onWmSize_DlgProc
//*****************************************************************************
BOOL onWmSize_DlgProc(const HWND& hWndLV_Cnfg
	, const HWND& hWndLV_T_lo
	, const HWND& hWndLV_T_hi
	, const HWND& hWndLV_Treg
)
{
	// CONFIGURATION
	MoveWindow(hWndLV_Cnfg
		, 20, 90
		, 336
		, 40
		, TRUE
	);
	// TEMPERATURE LOW
	MoveWindow(hWndLV_T_lo
		, 20, 155
		, 417
		, 40
		, TRUE
	);
	// TEMPERATURE HIGH
	MoveWindow(hWndLV_T_hi
		, 20, 220
		, 417
		, 40
		, TRUE
	);
	// TEMPERATURE REGISTER
	MoveWindow(hWndLV_Treg
		, 20, 285
		, 417
		, 40
		, TRUE
	);

	return EXIT_SUCCESS;
}

//*****************************************************************************
//*                     onWmCommand_DlgProc
//*****************************************************************************
INT_PTR onWmCommand_DlgProc(const HWND& hDlg
	, const WPARAM& wParam
	, HANDLE& hThreadTxRx
	, RECT& rect
	, HBRUSH& brush
	, const HBRUSH& bkColorDlgBrush
	, const HBRUSH& redBrush
	, const HBRUSH& greenBrush
)
{
	switch (LOWORD(wParam))
	{
	case CONNECT_SERIAL:
	{
		if (connect(g_hComm) == EXIT_SUCCESS)
		{
			// set text edit control IDC_STATUS
			SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT)
				, WM_SETTEXT
				, (WPARAM)0
				, (LPARAM)L"Connected"
			);
			// enable/disable button
			EnableWindow(GetDlgItem(hDlg, CONNECT_SERIAL), FALSE);
			EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), TRUE);

			// enable lo/hi
			EnableWindow(GetDlgItem(hDlg, IDC_T_LO_CLCS), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_T_HI_CLCS), TRUE);

			// enable setting
			EnableWindow(GetDlgItem(hDlg, TMP102_RESET), TRUE);
			// hold disabled untill IDC_CHB_SHUTDOWN is checked
			//EnableWindow(GetDlgItem(hDlg, IDC_CHB_ONESHOT), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_CB_FAULT_QUEUE), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_CB_POLARITY_ALERT), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_CB_MODE_THERMOSTAT), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHB_SHUTDOWN), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_CB_RATE_CONVERSION), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHB_EXTENDED), TRUE);
			EnableWindow(GetDlgItem(hDlg, APPLY_SETTING), TRUE);
			
			g_oFrameEx.cmnd = RD_REG_CNFG;
			g_queue.push(g_oFrameEx);
			g_oFrameEx.cmnd = RD_REG_T_LO;
			g_queue.push(g_oFrameEx);
			g_oFrameEx.cmnd = RD_REG_T_HI;
			g_queue.push(g_oFrameEx);
			g_oFrameEx.cmnd = RD_REG_TEMP;
			g_queue.push(g_oFrameEx);

			// enable infinite loop
			g_bContinueTxRx = TRUE;

			// create thread to continuously transmit and receive
			DWORD dwThreadIdTxRx = 0;
			hThreadTxRx = CreateThread(NULL
				, 0
				, TxRx
				, (LPVOID)hDlg
				, CREATE_SUSPENDED // wait until started
				, &dwThreadIdTxRx
			);

			// start thread exact on this command
			if (hThreadTxRx) ResumeThread(hThreadTxRx);
		}
		return (INT_PTR)TRUE;
	} // eof CONNECT_SERIAL
	//case CONNECT_SERIAL:
	//{
	//	if (connect(g_hComm) == EXIT_SUCCESS)
	//	{
	//		// set text edit control IDC_STATUS
	//		SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT)
	//			, WM_SETTEXT
	//			, (WPARAM)0
	//			, (LPARAM)L"Connected"
	//		);
	//		// enable/disable button
	//		EnableWindow(GetDlgItem(hDlg, CONNECT_SERIAL), FALSE);
	//		EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), TRUE);

	//		// TEST start by checking receiving a temperature continuously
	//		// set g_oFrame into the default state
	//		//g_oFrame.cmnd = RD_REG_TEMP;
	//		//------------------------------------------------------------
	//		// set g_oFrame into the default state, i.d. capturing the
	//		// CONFIGURATION register
	//		// and trickling down towards continuously capturing the
	//		// TEMPERATURE register
	//		g_oFrame.cmnd = RD_REG_CNFG;

	//		// enable infinite loop
	//		g_bContinueTxRx = TRUE;

	//		// create thread to continuously transmit and receive
	//		DWORD dwThreadIdTxRx = 0;
	//		HANDLE hTreadTxRx = CreateThread(NULL
	//			, 0
	//			, TxRx
	//			, (LPVOID)hDlg
	//			, CREATE_SUSPENDED // wait until started
	//			, &dwThreadIdTxRx
	//		);

	//		// start thread exact on this command
	//		if (hTreadTxRx) ResumeThread(hTreadTxRx);
	//	}
	//	return (INT_PTR)TRUE;
	//} // eof CONNECT_SERIAL
	case DISCONNECT_SERIAL:
	{
		// terminate thread
		g_bContinueTxRx = FALSE;
		hThreadTxRx = INVALID_HANDLE_VALUE;

		if (g_hComm == INVALID_HANDLE_VALUE) return (INT_PTR)TRUE;

		if (CloseHandle(g_hComm))
		{
			g_hComm = INVALID_HANDLE_VALUE;

			// set text edit control IDC_STATUS
			SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT)
				, WM_SETTEXT
				, (WPARAM)0
				, (LPARAM)L"Disconnected"
			);

			// enable/disable button
			EnableWindow(GetDlgItem(hDlg, CONNECT_SERIAL), TRUE);
			EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), FALSE);

			// disable lo/hi
			EnableWindow(GetDlgItem(hDlg, IDC_T_LO_CLCS), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_T_HI_CLCS), FALSE);

			// disable setting
			EnableWindow(GetDlgItem(hDlg, TMP102_RESET), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHB_ONESHOT), FALSE);
			SendMessage(GetDlgItem(hDlg, IDC_RESOLUTION)
				, WM_SETTEXT
				, (WPARAM)0
				, (LPARAM)L"Read only"
			);
			EnableWindow(GetDlgItem(hDlg, IDC_CB_FAULT_QUEUE), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CB_POLARITY_ALERT), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CB_MODE_THERMOSTAT), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHB_SHUTDOWN), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CB_RATE_CONVERSION), FALSE);
			SendMessage(GetDlgItem(hDlg, IDC_ALERT)
				, WM_SETTEXT
				, (WPARAM)0
				, (LPARAM)L"Read only"
			);
			EnableWindow(GetDlgItem(hDlg, IDC_CHB_EXTENDED), FALSE);
			EnableWindow(GetDlgItem(hDlg, APPLY_SETTING), FALSE);

			// erase alert bit indicator
			brush = bkColorDlgBrush;
			InvalidateRect(hDlg, &rect, TRUE);
		}
		return (INT_PTR)TRUE;
	} // eof DISCONNECT_SERIAL
	case TMP102_RESET:
	{
		//TODO: TMP102_RESET implementation
		return (INT_PTR)TRUE;
	} // eof TMP102_RESET
	case APPLY_SETTING:
	{
		// g_bContinueTxRx is TRUE when connected
		// if not connected: return
		if (!g_bContinueTxRx) return (INT_PTR)TRUE;

		// kill thread
		
		// does not work
		// suspend thread
		//SuspendThread(hThreadTxRx);

		// empty queue
		g_queue = {};
		// push queue with write command, and finish with RD_REG_TEMP
		g_oFrameEx.cmnd = WR_REG_CNFG;
		g_oFrameEx.payload = g_oConfiguration.getHiByte() << 8;
		g_oFrameEx.payload |= g_oConfiguration.getLoByte();
		g_queue.push(g_oFrameEx);

		g_oFrameEx.cmnd = WR_REG_T_LO;
		g_oFrameEx.payload = clcsToBit12(GetDlgItem(hDlg, IDC_T_LO_CLCS)
			, g_oTempLo
		);
		g_queue.push(g_oFrameEx);

		g_oFrameEx.cmnd = WR_REG_T_HI;
		g_oFrameEx.payload = clcsToBit12(GetDlgItem(hDlg, IDC_T_HI_CLCS)
			, g_oTempHi
		);
		g_queue.push(g_oFrameEx);

		g_oFrameEx.cmnd = RD_REG_TEMP;
		g_queue.push(g_oFrameEx);

		// create thread to continuously transmit and receive
		
		// start thread exact on this command
		 
		// does not work
		// resume thread
		//ResumeThread(hThreadTxRx);

		return (INT_PTR)TRUE;
	} // eof APPLY_SETTING
		//// write the configuration to STM32
		//g_oFrame.cmnd = WR_REG_CNFG;
		//
		//// convert the value of IDC_T_LO_CLCS
		//CHAR chBuffer[8] = { 0 };
		//std::string str = "";
		//FLOAT fTemp = 0.;
		//SendMessageA(GetDlgItem(hDlg, IDC_T_LO_CLCS)
		//	, WM_GETTEXT
		//	, (WPARAM)8
		//	, (LPARAM)chBuffer
		//);
		//str = chBuffer;
		//fTemp = std::stof(str);
		//fTemp /= 0.0625;
		//g_iTempLo = (INT16)fTemp << 4;
		//// write iTempLo to STM32

		//// convert the value of IDC_T_HI_CLCS
		//SendMessageA(GetDlgItem(hDlg, IDC_T_HI_CLCS)
		//	, WM_GETTEXT
		//	, (WPARAM)8
		//	, (LPARAM)chBuffer
		//);
		//str = chBuffer;
		//fTemp = std::stof(str);
		//fTemp /= 0.0625;
		//g_iTempHi = (INT16)fTemp << 4;
		//// write iTempHi to STM32
	case IDC_CHB_ONESHOT:
	{
		if (g_oConfiguration.oneShot)
		//if (g_bCheckedStateChbOneshot)
		{
			g_oConfiguration.oneShot = FALSE;
			//g_bCheckedStateChbOneshot = FALSE;
			SendMessage(GetDlgItem(hDlg, IDC_CHB_ONESHOT)
				, BM_SETCHECK
				, (WPARAM)BST_UNCHECKED
				, (LPARAM)0
			);
		}
		else
		{
			g_oConfiguration.oneShot = TRUE;
			//g_bCheckedStateChbOneshot = TRUE;
			SendMessage(GetDlgItem(hDlg, IDC_CHB_ONESHOT)
				, BM_SETCHECK
				, (WPARAM)BST_CHECKED
				, (LPARAM)0
			);
		}
		return (INT_PTR)TRUE;
	} // eof IDC_CHB_ONESHOT
	case IDC_CHB_SHUTDOWN:
	{
		if (g_oConfiguration.shutDown)
		//if (g_bCheckedStateChbShutdown)
		{
			g_oConfiguration.shutDown = FALSE;
			//g_bCheckedStateChbShutdown = FALSE;
			SendMessage(GetDlgItem(hDlg, IDC_CHB_SHUTDOWN)
				, BM_SETCHECK
				, (WPARAM)BST_UNCHECKED
				, (LPARAM)0
			);
		}
		else
		{
			g_oConfiguration.shutDown = TRUE;
			//g_bCheckedStateChbShutdown = TRUE;
			SendMessage(GetDlgItem(hDlg, IDC_CHB_SHUTDOWN)
				, BM_SETCHECK
				, (WPARAM)BST_CHECKED
				, (LPARAM)0
			);
		}
		return (INT_PTR)TRUE;
	} // eof IDC_CHB_SHUTDOWN
	case IDC_CHB_EXTENDED:
	{
		if (g_oConfiguration.modeExtended)
		//if (g_bCheckedStateChbExtended)
		{
			g_oConfiguration.modeExtended = FALSE;
			//g_bCheckedStateChbExtended = FALSE;
			SendMessage(GetDlgItem(hDlg, IDC_CHB_EXTENDED)
				, BM_SETCHECK
				, (WPARAM)BST_UNCHECKED
				, (LPARAM)0
			);
		}
		else
		{
			g_oConfiguration.modeExtended = TRUE;
			//g_bCheckedStateChbExtended = TRUE;
			SendMessage(GetDlgItem(hDlg, IDC_CHB_EXTENDED)
				, BM_SETCHECK
				, (WPARAM)BST_CHECKED
				, (LPARAM)0
			);
		}
		return (INT_PTR)TRUE;
	} // eof IDC_CHB_EXTENDED
	case IDC_T_CLCS:
	{
		switch (HIWORD(wParam))
		{
		case EN_CHANGE:
		{
			if (g_bContinueTxRx)
			{
				// hThreadTxRx is running
				// comparator mode
				if (g_oConfiguration.polarity)
				{
					// active high
					brush = (g_oMsrdTemp.alert) ? redBrush : greenBrush;
				}
				else
				{
					// active low
					brush = (g_oMsrdTemp.alert) ? greenBrush : redBrush;
				}

				// thermostat mode
				// TODO

				// adjust color indicator
				InvalidateRect(hDlg, &rect, TRUE);

				// TEST insert value into database:
				// name table....: value_measurement
				// foreign key...: 1 (referencing measurement1)
			}
			return (INT_PTR)TRUE;
		} // eof EN_CHANGE
		} // eof switch
		return (INT_PTR)FALSE;
	} // eof IDC_T_CLCS
	} // eof switch
	return (INT_PTR)FALSE;
}

//*****************************************************************************
//*                     onWmPaint_DlgProc
//*****************************************************************************
BOOL onWmPaint_DlgProc(const HWND& hDlg
	, const HBRUSH& brush
	, const RECT& rect
)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hDlg, &ps);
	SelectObject(hdc, brush);
	Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	EndPaint(hDlg, &ps);

	return EXIT_SUCCESS;
}

//****************************************************************************
//*                     clcsToBit12
//****************************************************************************
UINT16 clcsToBit12(const HWND& hWnd, BIT12TEMP& oBit12Temp)
{
	CHAR chBuffer[LEN_TEMP_IN_CLCS] = { 0 };
	std::string str = "";
	FLOAT fTemp = 0.;
	// get value from edittext control
	SendMessageA(hWnd
		, WM_GETTEXT
		, (WPARAM)8
		, (LPARAM)chBuffer
	);
	str = chBuffer;
	fTemp = std::stof(str);
	fTemp /= 0.0625;
	oBit12Temp.fTempInClcs = fTemp;
	oBit12Temp.fTempInClcsTimes100 = fTemp * 100;
	return (UINT16)fTemp << 4;
}

//****************************************************************************
//*                     connect
//****************************************************************************
BOOL connect(HANDLE& hComm)
{
	// create file
	hComm = CreateFile(L"\\\\.\\COM3"
		, GENERIC_READ | GENERIC_WRITE
		, 0
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		//, FILE_FLAG_OVERLAPPED
		, NULL
	);
	if (hComm == INVALID_HANDLE_VALUE)
	{
		return EXIT_FAILURE;
	}

	// set structure to initialize the communication port
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = 115200;
	dcb.fBinary = 1;
	dcb.fParity = 0;
	dcb.fOutxCtsFlow = 0;
	dcb.fOutxDsrFlow = 0;
	dcb.fDtrControl = 1;
	dcb.fDsrSensitivity = 0;
	dcb.fTXContinueOnXoff = 0;
	dcb.fOutX = 0;
	dcb.fInX = 0;
	dcb.fErrorChar = 0;
	dcb.fNull = 0;
	dcb.fRtsControl = 1;
	dcb.fAbortOnError = 0;
	dcb.fDummy2 = 0;
	dcb.wReserved = 0;
	dcb.ByteSize = 8;
	dcb.Parity = 0;
	dcb.StopBits = 0;
	dcb.XoffChar = 0;
	dcb.XoffChar = 0;
	dcb.ErrorChar = 24;
	dcb.EvtChar = 0;
	dcb.wReserved1 = 0;
	dcb.ByteSize = 8;
	dcb.StopBits = 0;
	// initialize the communication port
	if (!SetCommState(hComm, (LPDCB)&dcb))
	{
		return EXIT_FAILURE;
	}

	// set structure for the communication port timeout
	COMMTIMEOUTS commtimeouts;
	commtimeouts.ReadIntervalTimeout = MAXDWORD;
	commtimeouts.ReadTotalTimeoutMultiplier = 0;
	commtimeouts.ReadTotalTimeoutConstant = 0;
	commtimeouts.WriteTotalTimeoutMultiplier = 0;
	commtimeouts.WriteTotalTimeoutConstant = 0;
	// set communication port timeout
	if (!SetCommTimeouts(hComm, (LPCOMMTIMEOUTS)&commtimeouts))
	{
		return EXIT_FAILURE;
	}

	// set the communication port mask bit to capture event
	if (!SetCommMask(hComm, EV_TXEMPTY | EV_RXCHAR))
	{
		return EXIT_FAILURE;
	}

	// set in/out queue buffers
	if (!SetupComm(hComm, BUFFER_MAX_SERIAL, BUFFER_MAX_SERIAL))
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

//****************************************************************************
//*                     TxRx
//****************************************************************************
DWORD WINAPI TxRx(LPVOID lpVoid)
{
	// infinite loop
	while (g_bContinueTxRx)
	{
		transmit();
		Sleep(DELAY_4HZ_SERIAL);
		receive(lpVoid);
		Sleep(DELAY_4HZ_SERIAL);
	}
	return 0;
}

//****************************************************************************
//*                     transmit
//****************************************************************************
BOOL transmit()
{
	OutputDebugString(L"transmitting\n");

	FRAME oFrameEx = g_queue.front();

	sprintf_s((CHAR*)g_chBuffer, (size_t)BUFFER_MAX_SERIAL, "%c%c%c%c%c%c%c%c%c"
		, oFrameEx.soh
		, (oFrameEx.cmnd >> 8) & 0xFF
		, (oFrameEx.cmnd & 0xFF)
		, oFrameEx.stx
		, (oFrameEx.payload >> 8) & 0xFF
		, (oFrameEx.payload & 0xFF)
		, oFrameEx.etx
		, oFrameEx.etb
		, oFrameEx.eot
	);
	DWORD dwNofByteTransferred = 0;
	WriteFile(g_hComm
		, &g_chBuffer
		, LEN_FRAME
		, &dwNofByteTransferred
		, NULL
	);
	return EXIT_SUCCESS;
}
////****************************************************************************
////*                     transmit
////****************************************************************************
//BOOL transmit()
//{
//	OutputDebugString(L"transmitting\n");
//
//	if (g_oFrame.cmnd == WR_REG_CNFG)
//	{
//		g_oFrame.payload = (g_hiByteCnfg << 8) | g_loByteCnfg;
//	}
//
//	if (g_oFrame.cmnd == WR_REG_T_LO)
//	{
//		g_oFrame.payload = g_iTempLo;
//	}
//
//	if (g_oFrame.cmnd == WR_REG_T_HI)
//	{
//		g_oFrame.payload = g_iTempHi;
//	}
//
//	sprintf_s((CHAR*)g_chBuffer, (size_t)BUFFER_MAX_SERIAL, "%c%c%c%c%c%c%c%c%c"
//		, g_oFrame.soh
//		, (g_oFrame.cmnd >> 8) & 0xFF
//		, (g_oFrame.cmnd & 0xFF)
//		, g_oFrame.stx
//		, (g_oFrame.payload >> 8) & 0xFF
//		, (g_oFrame.payload & 0xFF)
//		, g_oFrame.etx
//		, g_oFrame.etb
//		, g_oFrame.eot
//	);
//	DWORD dwNofByteTransferred = 0;
//	WriteFile(g_hComm
//		, &g_chBuffer
//		, LEN_FRAME
//		, &dwNofByteTransferred
//		, NULL
//	);
//	return EXIT_SUCCESS;
//}

//****************************************************************************
//*                     receive
//****************************************************************************
BOOL receive(LPVOID lpVoid)
{
	OutputDebugString(L"receiving\n");
	CHAR chBufferTempInCelcius[BUFFER_MAX_SERIAL] = { 0 };
	DWORD dwNofByteTransferred = 0;
	ReadFile(g_hComm
		, &g_chBuffer
		, LEN_FRAME
		, &dwNofByteTransferred
		, NULL
	);

	++g_cReceive;
	if (g_cReceive < MAX_RETRY_SERIAL) return EXIT_SUCCESS;
	g_cReceive = 0;

	FRAME g_oFrameEx = g_queue.front();
	switch (g_oFrameEx.cmnd)
	{
	case RD_REG_CNFG:
	{
		g_oConfiguration.setHiByte(g_chBuffer[4]);
		g_oConfiguration.setLoByte(g_chBuffer[5]);

		updateRegister(GetDlgItem(g_hDlg, IDC_LV_CONFIGURATION)
			, g_chBuffer[4]
			, g_chBuffer[5]
			, 12
		);

		updateSetting(g_hDlg
			, g_chBuffer[4]
			, g_chBuffer[5]
		);

		break;
	} // eof RD_REG_CNFG
	case RD_REG_T_LO:
	{
		g_oTempLo.setHiByte(g_chBuffer[4]);
		g_oTempLo.setLoByte(g_chBuffer[5]);

		updateRegister(GetDlgItem(g_hDlg, IDC_LV_T_LO)
			, g_chBuffer[4]
			, g_chBuffer[5]
		);

		SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_LO_CLCS)
			, WM_SETTEXT
			, (WPARAM)0
			, (LPARAM)g_oTempLo.getTempInClcs_toStringA()
		);

		break;
	} // eof RD_REG_T_LO
	case RD_REG_T_HI:
	{
		g_oTempHi.setHiByte(g_chBuffer[4]);
		g_oTempHi.setLoByte(g_chBuffer[5]);

		updateRegister(GetDlgItem(g_hDlg, IDC_LV_T_HI)
			, g_chBuffer[4]
			, g_chBuffer[5]
		);

		SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_HI_CLCS)
			, WM_SETTEXT
			, (WPARAM)0
			, (LPARAM)g_oTempHi.getTempInClcs_toStringA()
		);

		break;
	} // eof RD_REG_T_HI
	case RD_REG_TEMP:
	{
		g_oMsrdTemp.setHiByte(g_chBuffer[4]);
		g_oMsrdTemp.setLoByte(g_chBuffer[5]);
		g_oMsrdTemp.setAlert(g_chBuffer[5]);
		g_oMsrdTemp.setModeExtended(g_chBuffer[5]);

		updateRegister(GetDlgItem(g_hDlg, IDC_LV_TREG)
			, g_chBuffer[4]
			, g_chBuffer[5]
		);

		SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_CLCS)
			, WM_SETTEXT
			, (WPARAM)0
			, (LPARAM)g_oMsrdTemp.getTempInClcs_toStringA()
		);

		std::wstring wstrStateAlert = L"";
		if (g_oConfiguration.polarity)
		{
			wstrStateAlert = (g_oMsrdTemp.alert) ? L"Active" : L"Not active";
		}
		else
		{
			wstrStateAlert = (g_oMsrdTemp.alert) ? L"Not active" : L"Active";
		}

		if (g_oMsrdTemp.fTempInClcsTimes100 < g_oTempLo.fTempInClcsTimes100)
		{
			SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_ALERT)
				, WM_SETTEXT
				, (WPARAM)0
				, (LPARAM)"too low"
			);
		}
		else if (g_oMsrdTemp.fTempInClcsTimes100 > g_oTempHi.fTempInClcsTimes100)
		{
			SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_ALERT)
				, WM_SETTEXT
				, (WPARAM)0
				, (LPARAM)"too high"
			);
		}
		else
		{
			SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_ALERT)
				, WM_SETTEXT
				, (WPARAM)0
				, (LPARAM)"in range"
			);
		}
		break;
	} // eof RD_REG_TEMP
	} // eof switch

	if (g_queue.size() > 1)
	{
		// last frame stays in the queue
		g_queue.pop();
	}

	return EXIT_SUCCESS;
}

//*****************************************************************************
//*
//*****************************************************************************
//FLOAT bit12toclcs(const INT8& hiByte
//	, const UINT8& loByte
//)
//{
//	// do not use
//	//val = ((INT16)g_chBufferReceive[0] << 4) | (g_chBufferReceive[1] >> 4);
//	// use this correct statement
//	INT16 val = ((INT8)g_chBuffer[4] << 4 | (UINT8)g_chBuffer[5] >> 4);
//	// do not use
//	//if (val > 0x7FF) val |= 0xF000;
//	// use the traditional definition of a two'2 complement
//	// logical not operator ! or bitwise operator ~ can be used
//	if (val & 0x8000)
//	{
//		val = ~val;
//		val += 1;
//	}
//	return val * 0.0625;
//}

////****************************************************************************
////*                     receive
////****************************************************************************
//BOOL receive(LPVOID lpVoid)
//{
//	OutputDebugString(L"receiving\n");
//	CHAR chBufferTempInCelcius[BUFFER_MAX_SERIAL] = { 0 };
//	DWORD dwNofByteTransferred = 0;
//	ReadFile(g_hComm
//		, &g_chBuffer
//		, LEN_FRAME
//		, &dwNofByteTransferred
//		, NULL
//	);
//
//	// write CONFIGURATION ///////////////////////////////////////
//	if (g_oFrame.cmnd == WR_REG_CNFG)
//	{
//		++g_cReceive;
//		if (g_cReceive == MAX_RETRY_SERIAL)
//		{
//			g_cReceive = 0;
//			g_oFrame.cmnd = WR_REG_T_LO;
//		}
//	}
//
//	// write TEMPERATURE LOW /////////////////////////////////////
//	if (g_oFrame.cmnd == WR_REG_T_LO)
//	{
//		++g_cReceive;
//		if (g_cReceive == MAX_RETRY_SERIAL)
//		{
//			g_cReceive = 0;
//			g_oFrame.cmnd = WR_REG_T_HI;
//		}
//	}
//
//	// write TEMPERATURE HIGH ////////////////////////////////////
//	if (g_oFrame.cmnd == WR_REG_T_HI)
//	{
//		++g_cReceive;
//		if (g_cReceive == MAX_RETRY_SERIAL)
//		{
//			g_cReceive = 0;
//			g_oFrame.cmnd = RD_REG_TEMP;
//		}
//	}
//
//	// set CONFIGURATION /////////////////////////////////////////
//	if (g_oFrame.cmnd == RD_REG_CNFG)
//	{
//		updateRegister(GetDlgItem(g_hDlg, IDC_LV_CONFIGURATION)
//			, g_chBuffer[4]
//			, g_chBuffer[5]
//			, 12
//		);
//
//		updateSetting(g_hDlg
//			, g_chBuffer[4]
//			, g_chBuffer[5]
//		);
//
//		++g_cReceive;
//		if (g_cReceive == MAX_RETRY_SERIAL)
//		{
//			g_cReceive = 0;
//			g_oFrame.cmnd = RD_REG_T_LO;
//		}
//	}
//
//	// set TEMPERATURE LOW ///////////////////////////////////////
//	if (g_oFrame.cmnd == RD_REG_T_LO)
//	{
//		updateRegister(GetDlgItem(g_hDlg, IDC_LV_T_LO)
//			, g_chBuffer[4]
//			, g_chBuffer[5]
//		);
//
//		// do not use
//		//val = ((INT16)g_chBufferReceive[0] << 4) | (g_chBufferReceive[1] >> 4);
//		// use this correct statement
//		INT16 val = ((INT8)g_chBuffer[4] << 4 | (UINT8)g_chBuffer[5] >> 4);
//		// do not use
//		//if (val > 0x7FF) val |= 0xF000;
//		// use the traditional definition of a two'2 complement
//		// logical not operator ! or bitwise operator ~ can be used
//		if (val & 0x8000)
//		{
//			val = ~val;
//			val += 1;
//		}
//		FLOAT temp_c = val * 0.0625;
//		g_iTempLoTimes100 = temp_c *= 100;
//		sprintf_s(chBufferTempInCelcius
//			, BUFFER_MAX_SERIAL
//			, "%d.%02d"
//			, ((UINT)temp_c / 100)
//			, ((UINT)temp_c % 100)
//		);
//
//		SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_LO_CLCS)
//			, WM_SETTEXT
//			, (WPARAM)0
//			, (LPARAM)chBufferTempInCelcius
//		);
//
//		++g_cReceive;
//		if (g_cReceive == MAX_RETRY_SERIAL)
//		{
//			g_cReceive = 0;
//			g_oFrame.cmnd = RD_REG_T_HI;
//		}
//	}
//
//	// set TEMPERATURE HIGH //////////////////////////////////////
//	if (g_oFrame.cmnd == RD_REG_T_HI)
//	{
//		updateRegister(GetDlgItem(g_hDlg, IDC_LV_T_HI)
//			, g_chBuffer[4]
//			, g_chBuffer[5]
//		);
//
//		// do not use
//		//val = ((INT16)g_chBufferReceive[0] << 4) | (g_chBufferReceive[1] >> 4);
//		// use this correct statement
//		INT16 val = ((INT8)g_chBuffer[4] << 4 | (UINT8)g_chBuffer[5] >> 4);
//		// do not use
//		//if (val > 0x7FF) val |= 0xF000;
//		// use the traditional definition of a two'2 complement
//		// logical not operator ! or bitwise operator ~ can be used
//		if (val & 0x8000)
//		{
//			val = ~val;
//			val += 1;
//		}
//		FLOAT temp_c = val * 0.0625;
//		g_iTempHiTimes100 = temp_c *= 100;
//		sprintf_s(chBufferTempInCelcius
//			, BUFFER_MAX_SERIAL
//			, "%d.%02d"
//			, ((UINT)temp_c / 100)
//			, ((UINT)temp_c % 100)
//		);
//
//		SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_HI_CLCS)
//			, WM_SETTEXT
//			, (WPARAM)0
//			, (LPARAM)chBufferTempInCelcius
//		);
//
//		++g_cReceive;
//		if (g_cReceive == MAX_RETRY_SERIAL)
//		{
//			g_cReceive = 0;
//			g_oFrame.cmnd = RD_REG_TEMP;
//		}
//	}
//
//	// set TEMPERATURE REGISTER //////////////////////////////////
//	if (g_oFrame.cmnd == RD_REG_TEMP)
//	{
//		// update register
//		updateRegister(GetDlgItem(g_hDlg, IDC_LV_TREG)
//			, g_chBuffer[4]
//			, g_chBuffer[5]
//		);
//
//		// isolate alert bit
//		g_alert_bit = (g_chBuffer[5] & 0x02) >> 1;
//
//		// do not use
//		//val = ((INT16)g_chBufferReceive[0] << 4) | (g_chBufferReceive[1] >> 4);
//		// use this correct statement
//		INT16 val = ((INT8)g_chBuffer[4] << 4 | (UINT8)g_chBuffer[5] >> 4);
//		// do not use
//		//if (val > 0x7FF) val |= 0xF000;
//		// use the traditional definition of a two'2 complement
//		// logical not operator ! or bitwise operator ~ can be used
//		if (val & 0x8000)
//		{
//			val = ~val;
//			val += 1;
//		}
//		FLOAT temp_c = val * 0.0625;
//		INT16 iTempTimes100 = temp_c *= 100;
//		sprintf_s(chBufferTempInCelcius
//			, BUFFER_MAX_SERIAL
//			, "%d.%02d"
//			, ((UINT)temp_c / 100)
//			, ((UINT)temp_c % 100)
//		);
//
//		SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_CLCS)
//			, WM_SETTEXT
//			, (WPARAM)0
//			, (LPARAM)chBufferTempInCelcius
//		);
//
//		std::wstring wstrStateAlert = L"";
//		if (g_bPolarity)
//		{
//			wstrStateAlert = (g_alert_bit) ? L"Active" : L"Not active";
//		}
//		else
//		{
//			wstrStateAlert = (g_alert_bit) ? L"Not active" : L"Active";
//		}
//		SendMessage(GetDlgItem((HWND)lpVoid, IDC_ALERT)
//			, WM_SETTEXT
//			, (WPARAM)0
//			, (LPARAM)wstrStateAlert.c_str()
//		);
//
//		if (iTempTimes100 < g_iTempLoTimes100)
//		{
//			SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_ALERT)
//				, WM_SETTEXT
//				, (WPARAM)0
//				, (LPARAM)"too low"
//			);
//		}
//		else if (iTempTimes100 > g_iTempHiTimes100)
//		{
//			SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_ALERT)
//				, WM_SETTEXT
//				, (WPARAM)0
//				, (LPARAM)"too high"
//			);
//		}
//		else
//		{
//			SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_ALERT)
//				, WM_SETTEXT
//				, (WPARAM)0
//				, (LPARAM)"in range"
//			);
//		}
//	}
//
//	return EXIT_SUCCESS;
//}

//****************************************************************************
//*                     updateListViewItemEx
//*
//* use this func instead of the func updateListViewItem() from the static lib
//* this func avoids too much OutputDebugString()
//****************************************************************************
VOID updateListViewItemEx(const HWND& hWndLV
	, const INT& iItem
	, const INT& iSubItem
	, const PWCHAR& pszItem
)
{
	LVITEM lvi = { 0 };
	UINT cchItem = wcslen(pszItem);
	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = iSubItem;
	lvi.cchTextMax = cchItem;
	lvi.pszText = pszItem;
	// get the column width
	size_t iColumnWidth = SendMessage(hWndLV
		, LVM_GETCOLUMNWIDTH
		, (WPARAM)iSubItem
		, (LPARAM)0
	);
	// set width column for (sub)item
	size_t iItemLength = wcslen(pszItem) * 8 + 10;
	if (iItemLength > iColumnWidth)
	{
		// adjust the column width for the length of the (sub)item
		SendMessage(hWndLV
			, LVM_SETCOLUMNWIDTH
			, (WPARAM)iSubItem
			, (LPARAM)iItemLength
		);
	}
	BOOL bSuccess;
	// set (sub)item
	bSuccess = SendMessage(hWndLV
		, LVM_SETITEMTEXT
		, (WPARAM)iItem
		, (LPARAM)&lvi
	);
}

////****************************************************************************
////*                     getSetting
////****************************************************************************
//BOOL getSetting(const HWND& hDlg)
//{
//	// return when not connected
//	if (!g_bContinueTxRx) return EXIT_FAILURE;
//
//	UINT8 bit_field = 0;
//	g_hiByteCnfg = g_loByteCnfg = 0;
//
//	// get IDC_CHB_ONESHOT ///////////////////////////////////////////////////
//	if (g_bCheckedStateChbOneshot) g_hiByteCnfg |= 0x80;
//
//	// get IDC_CB_FAULT_QUEUE ////////////////////////////////////////////////
//	bit_field = SendMessage(GetDlgItem(hDlg, IDC_CB_FAULT_QUEUE)
//		, CB_GETCURSEL
//		, (WPARAM)0
//		, (LPARAM)0
//	);
//	g_hiByteCnfg |= (bit_field << 3);
//
//	// get IDC_CB_POLARITY_ALERT /////////////////////////////////////////////
//	if (g_bPolarity) g_hiByteCnfg |= 0x04;
//
//	// get IDC_CB_MODE_THERMOSTAT ////////////////////////////////////////////
//	bit_field = SendMessage(GetDlgItem(hDlg, IDC_CB_MODE_THERMOSTAT)
//		, CB_GETCURSEL
//		, (WPARAM)0
//		, (LPARAM)0
//	);
//	g_hiByteCnfg |= (bit_field << 1);
//
//	// get IDC_CHB_SHUTDOWN //////////////////////////////////////////////////
//	if (g_bCheckedStateChbShutdown) g_hiByteCnfg |= 0x01;
//
//	// get IDC_CB_RATE_CONVERSION ////////////////////////////////////////////
//	bit_field = SendMessage(GetDlgItem(hDlg, IDC_CB_RATE_CONVERSION)
//		, CB_GETCURSEL
//		, (WPARAM)0
//		, (LPARAM)0
//	);
//	g_loByteCnfg |= (bit_field << 6);
//
//	// get IDC_CHB_EXTENDED //////////////////////////////////////////////////
//	if (g_bCheckedStateChbExtended) g_loByteCnfg |= 0x10;
//
//	return EXIT_SUCCESS;
//}

//****************************************************************************
//*                     updateSetting
//****************************************************************************
BOOL updateSetting(const HWND& hDlg
	, const UINT8& hiByte
	, const UINT8& loByte
)
{
	UINT8 bit_field = 0;
	// update IDC_CHB_ONESHOT ////////////////////////////////////////////////
	if (hiByte & 128)
	{
		SendMessage(GetDlgItem(hDlg, IDC_CHB_ONESHOT)
			, BM_SETCHECK
			, (WPARAM)BST_CHECKED
			, (LPARAM)0
		);
		g_oConfiguration.oneShot = TRUE;
		//g_bCheckedStateChbOneshot = TRUE;
	}
	else
	{
		SendMessage(GetDlgItem(hDlg, IDC_CHB_ONESHOT)
			, BM_SETCHECK
			, (WPARAM)BST_UNCHECKED
			, (LPARAM)0
		);
		g_oConfiguration.oneShot = FALSE;
		//g_bCheckedStateChbOneshot = FALSE;
	}
	// update IDC_RESOLUTION /////////////////////////////////////////////////
	std::wstring wstrResolution = L"";
	bit_field = (hiByte & 0x60) >> 5; // note precedence!
	switch (bit_field)
	{
	case 0:
	case 1:
	case 2:
	{
		wstrResolution = L"not specified";
		break;
	} // eof 0 | 1 | 2
	case 3:
	{
		wstrResolution = L"12 bit-resolution";
		break;
	} // eof 3
	} // eof switch
	SendMessage(GetDlgItem(hDlg, IDC_RESOLUTION)
		, WM_SETTEXT
		, (WPARAM)0
		, (LPARAM)wstrResolution.c_str()
	);
	// update IDC_CB_FAULT_QUEUE /////////////////////////////////////////////
	bit_field = (hiByte & 0x18) >> 3; // note precedence!
	// 00 - 1
	// 01 - 2
	// 10 - 4
	// 11 - 6
	SendMessage(GetDlgItem(hDlg, IDC_CB_FAULT_QUEUE)
		, CB_SETCURSEL
		, (WPARAM)bit_field
		, (LPARAM)0
	);
	// update IDC_CB_POLARITY_ALERT //////////////////////////////////////////
	bit_field = (hiByte & 0x04) >> 2; // note precedence!
	SendMessage(GetDlgItem(hDlg, IDC_CB_POLARITY_ALERT)
		, CB_SETCURSEL
		, (WPARAM)bit_field
		, (LPARAM)0
	);
	g_oConfiguration.polarity = bit_field;
	//g_bPolarity = bit_field;
	// update IDC_CB_MODE_THERMOSTAT /////////////////////////////////////////
	bit_field = (hiByte & 0x02) >> 1; // note precedence!
	SendMessage(GetDlgItem(hDlg, IDC_CB_MODE_THERMOSTAT)
		, CB_SETCURSEL
		, (WPARAM)bit_field
		, (LPARAM)0
	);
	g_oConfiguration.modeThermostat = bit_field;
	//g_bModeThermostat = bit_field;
	// update IDC_CHB_SHUTDOWN ///////////////////////////////////////////////
	if (hiByte & 1)
	{
		SendMessage(GetDlgItem(hDlg, IDC_CHB_SHUTDOWN)
			, BM_SETCHECK
			, (WPARAM)BST_CHECKED
			, (LPARAM)0
		);
		g_oConfiguration.shutDown = TRUE;
		//g_bCheckedStateChbShutdown = TRUE;
	}
	else
	{
		SendMessage(GetDlgItem(hDlg, IDC_CHB_SHUTDOWN)
			, BM_SETCHECK
			, (WPARAM)BST_UNCHECKED
			, (LPARAM)0
		);
		g_oConfiguration.shutDown = FALSE;
		//g_bCheckedStateChbShutdown = FALSE;
	}
	// update IDC_CB_RATE_CONVERSION /////////////////////////////////////////
	// 00 - 0.25 Hz
	// 01 - 1    Hz
	// 10 - 4    Hz
	// 11 - 8    Hz
	bit_field = (loByte & 0xC0) >> 6; // note precedence!
	SendMessage(GetDlgItem(hDlg, IDC_CB_RATE_CONVERSION)
		, CB_SETCURSEL
		, (WPARAM)bit_field
		, (LPARAM)0
	);
	// update IDC_ALERT //////////////////////////////////////////////////////
	std::wstring wstrStateAlert = L"";
	if (g_oConfiguration.polarity)
	//if (g_bPolarity)
	{
		wstrStateAlert = (loByte & 0x20) ? L"Active" : L"Not active";
	}
	else
	{
		wstrStateAlert = (loByte & 0x20) ? L"Not active" : L"Active";
	}
	SendMessage(GetDlgItem(hDlg, IDC_ALERT)
		, WM_SETTEXT
		, (WPARAM)0
		, (LPARAM)wstrStateAlert.c_str()
	);
	// update IDC_CHB_EXTENDED ///////////////////////////////////////////////
	if (loByte & 0x10)
	{
		SendMessage(GetDlgItem(hDlg, IDC_CHB_EXTENDED)
			, BM_SETCHECK
			, (WPARAM)BST_CHECKED
			, (LPARAM)0
		);
		g_oConfiguration.modeExtended = TRUE;
		//g_bCheckedStateChbExtended = TRUE;
	}
	else
	{
		SendMessage(GetDlgItem(hDlg, IDC_CHB_EXTENDED)
			, BM_SETCHECK
			, (WPARAM)BST_UNCHECKED
			, (LPARAM)0
		);
		g_oConfiguration.modeExtended = FALSE;
		//g_bCheckedStateChbExtended = FALSE;
	}

	return EXIT_SUCCESS;
}

//****************************************************************************
//*                     updateRegister
//****************************************************************************
BOOL updateRegister(const HWND& hWndDlgItem
	, const UINT8 hiByte
	, const UINT8 loByte
	, const UINT8 nofBits // has a default value nofBits = 16
)
{
	UINT8 mask = 128;
	for (UINT8 i = 0; i < 8; i++)
	{
		if (hiByte & mask)
		{
			updateListViewItemEx(hWndDlgItem
				, 0
				, i
				, (PWCHAR)L"1"
			);
		}
		else
		{
			updateListViewItemEx(hWndDlgItem
				, 0
				, i
				, (PWCHAR)L"0"
			);
		}
		mask >>= 1;
	}
	UINT8 limit = (nofBits == 16) ? 8 : 4;
	mask = 128;
	for (UINT8 i = 0; i < limit; i++)
	{
		if (loByte & mask)
		{
			updateListViewItemEx(hWndDlgItem
				, 0
				, i + 8
				, (PWCHAR)L"1"
			);
		}
		else
		{
			updateListViewItemEx(hWndDlgItem
				, 0
				, i + 8
				, (PWCHAR)L"0"
			);
		}
		mask >>= 1;
	}
	return EXIT_SUCCESS;
}
