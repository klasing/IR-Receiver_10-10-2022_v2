#pragma once
//****************************************************************************
//*                     note
//*
//*****************************************************************************
//*****************************************************************************
//*                     include
//*****************************************************************************
#include "framework.h"

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
} FRAME, * PFRAME;

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
UINT8 g_alert_bit = 0;
UINT8 g_cReceive = 0;
INT16 g_iTempLo = 0;
INT16 g_iTempHi = 0;
INT16 g_iTempLoTimes100 = 0;
INT16 g_iTempHiTimes100 = 0;
BOOL g_bCheckedStateChbOneshot = FALSE;
BOOL g_bCheckedStateChbShutdown = FALSE;
BOOL g_bCheckedStateChbExtended = FALSE;
BOOL g_bPolarity = FALSE;

//*****************************************************************************
//*                     prototype
//*****************************************************************************
BOOL				connect(HANDLE& hComm);
DWORD WINAPI		TxRx(LPVOID lpVoid);
BOOL                transmit();
BOOL                receive(LPVOID lpVoid);
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

			// TEST start by checking receiving a temperature continuously
			// set g_oFrame into the default state
			//g_oFrame.cmnd = RD_REG_TEMP;
			//------------------------------------------------------------
			// set g_oFrame into the default state, i.d. capturing the
			// CONFIGURATION register
			// and trickling down towards continuously capturing the
			// TEMPERATURE register
			g_oFrame.cmnd = RD_REG_CNFG;

			// enable infinite loop
			g_bContinueTxRx = TRUE;

			// create thread to continuously transmit and receive
			DWORD dwThreadIdTxRx = 0;
			HANDLE hTreadTxRx = CreateThread(NULL
				, 0
				, TxRx
				, (LPVOID)hDlg
				, CREATE_SUSPENDED // wait until started
				, &dwThreadIdTxRx
			);

			// start thread exact on this command
			if (hTreadTxRx) ResumeThread(hTreadTxRx);
		}
		return (INT_PTR)TRUE;
	} // eof CONNECT_SERIAL
	case DISCONNECT_SERIAL:
	{
		// terminate thread
		g_bContinueTxRx = FALSE;

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

			// erase alert bit indicator
			brush = bkColorDlgBrush;
			InvalidateRect(hDlg, &rect, TRUE);
		}
		return (INT_PTR)TRUE;
	} // eof DISCONNECT_SERIAL
	case APPLY_SETTING:
	{
		// get setting for the configuration register
		getSetting(hDlg);
		// convert the value of IDC_T_LO_CLCS
		CHAR chBuffer[8] = { 0 };
		std::string str = "";
		FLOAT fTemp = 0.;
		SendMessageA(GetDlgItem(hDlg, IDC_T_LO_CLCS)
			, WM_GETTEXT
			, (WPARAM)8
			, (LPARAM)chBuffer
		);
		str = chBuffer;
		fTemp = std::stof(str);
		fTemp /= 0.0625;
		g_iTempLo = (INT16)fTemp << 4;
		// write iTempLo to STM32
		g_oFrame.cmnd = WR_REG_T_LO; // 33611
		//g_oFrame.payload = iTempLo;
		//g_val_t_lo = iTempLo >> 4;

		// convert the value of IDC_T_HI_CLCS
		SendMessageA(GetDlgItem(hDlg, IDC_T_HI_CLCS)
			, WM_GETTEXT
			, (WPARAM)8
			, (LPARAM)chBuffer
		);
		str = chBuffer;
		fTemp = std::stof(str);
		fTemp /= 0.0625;
		g_iTempHi = (INT16)fTemp << 4;
		// write iTempHi to STM32
		//g_oFrame.cmnd = WR_REG_T_HI; // 33612
		//g_oFrame.payload = iTempHi;
		//g_val_t_hi = iTempHi >> 4;

		return (INT_PTR)TRUE;
	} // eof APPLY_SETTING
	case IDC_CHB_ONESHOT:
	{
		if (g_bCheckedStateChbOneshot)
		{
			g_bCheckedStateChbOneshot = FALSE;
			SendMessage(GetDlgItem(hDlg, IDC_CHB_ONESHOT)
				, BM_SETCHECK
				, (WPARAM)BST_UNCHECKED
				, (LPARAM)0
			);
		}
		else
		{
			g_bCheckedStateChbOneshot = TRUE;
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
		if (g_bCheckedStateChbShutdown)
		{
			g_bCheckedStateChbShutdown = FALSE;
			SendMessage(GetDlgItem(hDlg, IDC_CHB_SHUTDOWN)
				, BM_SETCHECK
				, (WPARAM)BST_UNCHECKED
				, (LPARAM)0
			);
		}
		else
		{
			g_bCheckedStateChbShutdown = TRUE;
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
		if (g_bCheckedStateChbExtended)
		{
			g_bCheckedStateChbExtended = FALSE;
			SendMessage(GetDlgItem(hDlg, IDC_CHB_EXTENDED)
				, BM_SETCHECK
				, (WPARAM)BST_UNCHECKED
				, (LPARAM)0
			);
		}
		else
		{
			g_bCheckedStateChbExtended = TRUE;
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
				// 1) comparator mode and polarity active low: 
				// alert-bit is not active (high) under temp low, i.d. greenBrush
				// alert-bit is active (low) above temp low, i.d. redBrush
				brush = (!g_bPolarity & g_alert_bit) ? greenBrush : redBrush;
				// 2) in thermostat mode things will be slightly different!
				InvalidateRect(hDlg, &rect, TRUE);
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
	if (g_oFrame.cmnd == WR_REG_T_LO)
	{
		g_oFrame.payload = g_iTempLo;
	}

	if (g_oFrame.cmnd == WR_REG_T_HI)
	{
		g_oFrame.payload = g_iTempHi;
	}

	sprintf_s((CHAR*)g_chBuffer, (size_t)BUFFER_MAX_SERIAL, "%c%c%c%c%c%c%c%c%c"
		, g_oFrame.soh
		, (g_oFrame.cmnd >> 8) & 0xFF
		, (g_oFrame.cmnd & 0xFF)
		, g_oFrame.stx
		, (g_oFrame.payload >> 8) & 0xFF
		, (g_oFrame.payload & 0xFF)
		, g_oFrame.etx
		, g_oFrame.etb
		, g_oFrame.eot
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

	// write TEMPERATURE LOW /////////////////////////////////////
	if (g_oFrame.cmnd == WR_REG_T_LO)
	{
		++g_cReceive;
		if (g_cReceive == MAX_RETRY_SERIAL)
		{
			g_cReceive = 0;
			g_oFrame.cmnd = WR_REG_T_HI;
		}
	}

	// write TEMPERATURE HIGH ////////////////////////////////////
	if (g_oFrame.cmnd == WR_REG_T_HI)
	{
		++g_cReceive;
		if (g_cReceive == MAX_RETRY_SERIAL)
		{
			g_cReceive = 0;
			g_oFrame.cmnd = RD_REG_TEMP;
		}
	}

	// set CONFIGURATION /////////////////////////////////////////
	if (g_oFrame.cmnd == RD_REG_CNFG)
	{
		updateRegister(GetDlgItem(g_hDlg, IDC_LV_CONFIGURATION)
			, g_chBuffer[4]
			, g_chBuffer[5]
			, 12
		);

		updateSetting(g_hDlg
			, g_chBuffer[4]
			, g_chBuffer[5]
		);

		++g_cReceive;
		if (g_cReceive == MAX_RETRY_SERIAL)
		{
			g_cReceive = 0;
			g_oFrame.cmnd = RD_REG_T_LO;
		}
	}

	// set TEMPERATURE LOW ///////////////////////////////////////
	if (g_oFrame.cmnd == RD_REG_T_LO)
	{
		updateRegister(GetDlgItem(g_hDlg, IDC_LV_T_LO)
			, g_chBuffer[4]
			, g_chBuffer[5]
		);

		// do not use
		//val = ((INT16)g_chBufferReceive[0] << 4) | (g_chBufferReceive[1] >> 4);
		// use this correct statement
		INT16 val = ((INT8)g_chBuffer[4] << 4 | (UINT8)g_chBuffer[5] >> 4);
		// do not use
		//if (val > 0x7FF) val |= 0xF000;
		// use the traditional definition of a two'2 complement
		// logical not operator ! or bitwise operator ~ can be used
		if (val & 0x8000)
		{
			val = ~val;
			val += 1;
		}
		FLOAT temp_c = val * 0.0625;
		g_iTempLoTimes100 = temp_c *= 100;
		sprintf_s(chBufferTempInCelcius
			, BUFFER_MAX_SERIAL
			, "%d.%02d"
			, ((UINT)temp_c / 100)
			, ((UINT)temp_c % 100)
		);

		SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_LO_CLCS)
			, WM_SETTEXT
			, (WPARAM)0
			, (LPARAM)chBufferTempInCelcius
		);

		++g_cReceive;
		if (g_cReceive == MAX_RETRY_SERIAL)
		{
			g_cReceive = 0;
			g_oFrame.cmnd = RD_REG_T_HI;
		}
	}

	// set TEMPERATURE HIGH //////////////////////////////////////
	if (g_oFrame.cmnd == RD_REG_T_HI)
	{
		updateRegister(GetDlgItem(g_hDlg, IDC_LV_T_HI)
			, g_chBuffer[4]
			, g_chBuffer[5]
		);

		// do not use
		//val = ((INT16)g_chBufferReceive[0] << 4) | (g_chBufferReceive[1] >> 4);
		// use this correct statement
		INT16 val = ((INT8)g_chBuffer[4] << 4 | (UINT8)g_chBuffer[5] >> 4);
		// do not use
		//if (val > 0x7FF) val |= 0xF000;
		// use the traditional definition of a two'2 complement
		// logical not operator ! or bitwise operator ~ can be used
		if (val & 0x8000)
		{
			val = ~val;
			val += 1;
		}
		FLOAT temp_c = val * 0.0625;
		g_iTempHiTimes100 = temp_c *= 100;
		sprintf_s(chBufferTempInCelcius
			, BUFFER_MAX_SERIAL
			, "%d.%02d"
			, ((UINT)temp_c / 100)
			, ((UINT)temp_c % 100)
		);

		SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_HI_CLCS)
			, WM_SETTEXT
			, (WPARAM)0
			, (LPARAM)chBufferTempInCelcius
		);

		++g_cReceive;
		if (g_cReceive == MAX_RETRY_SERIAL)
		{
			g_cReceive = 0;
			g_oFrame.cmnd = RD_REG_TEMP;
		}
	}

	// set TEMPERATURE REGISTER //////////////////////////////////
	if (g_oFrame.cmnd == RD_REG_TEMP)
	{
		// update register
		updateRegister(GetDlgItem(g_hDlg, IDC_LV_TREG)
			, g_chBuffer[4]
			, g_chBuffer[5]
		);

		// isolate alert bit
		g_alert_bit = (g_chBuffer[5] & 0x02) >> 1;

		// do not use
		//val = ((INT16)g_chBufferReceive[0] << 4) | (g_chBufferReceive[1] >> 4);
		// use this correct statement
		INT16 val = ((INT8)g_chBuffer[4] << 4 | (UINT8)g_chBuffer[5] >> 4);
		// do not use
		//if (val > 0x7FF) val |= 0xF000;
		// use the traditional definition of a two'2 complement
		// logical not operator ! or bitwise operator ~ can be used
		if (val & 0x8000)
		{
			val = ~val;
			val += 1;
		}
		FLOAT temp_c = val * 0.0625;
		INT16 iTempTimes100 = temp_c *= 100;
		sprintf_s(chBufferTempInCelcius
			, BUFFER_MAX_SERIAL
			, "%d.%02d"
			, ((UINT)temp_c / 100)
			, ((UINT)temp_c % 100)
		);

		SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_CLCS)
			, WM_SETTEXT
			, (WPARAM)0
			, (LPARAM)chBufferTempInCelcius
		);

		if (iTempTimes100 < g_iTempLoTimes100)
		{
			SendMessageA(GetDlgItem((HWND)lpVoid, IDC_T_ALERT)
				, WM_SETTEXT
				, (WPARAM)0
				, (LPARAM)"too low"
			);
		}
		else if (iTempTimes100 > g_iTempHiTimes100)
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
	}

	return EXIT_SUCCESS;
}

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

//****************************************************************************
//*                     getSetting
//****************************************************************************
BOOL getSetting(const HWND& hDlg)
{
	return EXIT_SUCCESS;
}

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
		g_bCheckedStateChbOneshot = TRUE;
	}
	else
	{
		SendMessage(GetDlgItem(hDlg, IDC_CHB_ONESHOT)
			, BM_SETCHECK
			, (WPARAM)BST_UNCHECKED
			, (LPARAM)0
		);
		g_bCheckedStateChbOneshot = FALSE;
	}
	// update IDC_RESOLUTION //////////////////////////////////////////////////
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
	// update IDC_CB_FAULT_QUEUE ////////////////////////////////////////////////
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
	g_bPolarity = bit_field;
	// update IDC_CB_MODE_THERMOSTAT /////////////////////////////////////////
	bit_field = (hiByte & 0x02) >> 1; // note precedence!
	SendMessage(GetDlgItem(hDlg, IDC_CB_MODE_THERMOSTAT)
		, CB_SETCURSEL
		, (WPARAM)bit_field
		, (LPARAM)0
	);
	// update IDC_CHB_SHUTDOWN ///////////////////////////////////////////////
	if (hiByte & 1)
	{
		SendMessage(GetDlgItem(hDlg, IDC_CHB_SHUTDOWN)
			, BM_SETCHECK
			, (WPARAM)BST_CHECKED
			, (LPARAM)0
		);
		g_bCheckedStateChbShutdown = TRUE;
	}
	else
	{
		SendMessage(GetDlgItem(hDlg, IDC_CHB_SHUTDOWN)
			, BM_SETCHECK
			, (WPARAM)BST_UNCHECKED
			, (LPARAM)0
		);
		g_bCheckedStateChbShutdown = FALSE;
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
	if (!g_bPolarity & !(loByte & 0x20))
	{
		// alert active
		wstrStateAlert = L"Active";
	}
	else
	{
		// alert not active
		wstrStateAlert = L"Not active";
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
		g_bCheckedStateChbExtended = TRUE;
	}
	else
	{
		SendMessage(GetDlgItem(hDlg, IDC_CHB_EXTENDED)
			, BM_SETCHECK
			, (WPARAM)BST_UNCHECKED
			, (LPARAM)0
		);
		g_bCheckedStateChbExtended = FALSE;
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
