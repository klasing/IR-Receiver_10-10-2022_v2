#pragma once
//****************************************************************************
//*                     note
//*
//*****************************************************************************
//*****************************************************************************
//*                     include
//*****************************************************************************
#include "framework.h"
//*****************************************************************************
//*                     extern
//*****************************************************************************
extern HWND g_hDlg;
extern COLORREF g_crInRange;
extern COLORREF g_crOutRange;
extern COLORREF g_crT_Lo_Clcs;
extern COLORREF g_crT_Hi_Clcs;
//*****************************************************************************
//*                     prototype
//*****************************************************************************
BOOL connect(HANDLE& hComm);
DWORD WINAPI transmit(LPVOID lpVoid);
DWORD WINAPI receive(LPVOID lpVoid);
//*****************************************************************************
//*                     onWmInitDialog_DlgProc
//*****************************************************************************
BOOL onWmInitDialog_DlgProc(const HINSTANCE& hInst
	, const HWND& hDlg
	, HWND& hWndLV_Cnfg
	, HWND& hWndLV_T_lo
	, HWND& hWndLV_T_hi
	, HWND& hWndLV_Treg
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
INT_PTR CALLBACK onWmCommand_DlgProc(const HINSTANCE& hInst
	, const HWND& hDlg
	, const WPARAM& wParam
	, HANDLE& hComm
	, HANDLE& hThreadTransmit
	, HANDLE& hThreadReceive
	, BOOL& bCheckedStateChbOneshot
	, BOOL& bCheckedStateChbShutdown
	, BOOL& bCheckedStateChbExtended
)
{
	switch (LOWORD(wParam))
	{
	case CONNECT_SERIAL:
	{
		if (connect(hComm) == EXIT_SUCCESS)
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

			// create thread to transmit
			DWORD dwThreadIdTransmit = 0;
			hThreadTransmit = CreateThread(NULL
				, 0
				, transmit
				, (LPVOID)hDlg
				, 0
				, &dwThreadIdTransmit
			);

			// create thread to receive
			DWORD dwThreadIdReceive = 0;
			hThreadReceive = CreateThread(NULL
				, 0
				, receive
				, (LPVOID)hDlg
				, 0
				, &dwThreadIdReceive
			);
		}
		return (INT_PTR)TRUE;
	} // eof CONNECT_SERIAL
	case DISCONNECT_SERIAL:
	{
		if (hComm == INVALID_HANDLE_VALUE) return (INT_PTR)TRUE;
		if (CloseHandle(hComm))
		{
			BOOL bResult = FALSE;
			DWORD dwExitCode = 0;
			bResult = TerminateThread(hThreadTransmit, 100);
			bResult = GetExitCodeThread(hThreadTransmit, &dwExitCode);
			hThreadTransmit = INVALID_HANDLE_VALUE;
			bResult = TerminateThread(hThreadReceive, 100);
			bResult = GetExitCodeThread(hThreadReceive, &dwExitCode);
			hThreadReceive = INVALID_HANDLE_VALUE;

			hComm = INVALID_HANDLE_VALUE;

			// set text edit control IDC_STATUS_CONNECT
			SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT)
				, WM_SETTEXT
				, (WPARAM)0
				, (LPARAM)L"Disconnected"
			);

			// enable/disable button
			EnableWindow(GetDlgItem(hDlg, CONNECT_SERIAL), TRUE);
			EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), FALSE);
		}
		return (INT_PTR)TRUE;
	} // eof DISCONNECT_SERIAL
	case IDC_CHB_ONESHOT:
	{
		if (bCheckedStateChbOneshot)
		{
			SendMessage(GetDlgItem(hDlg, IDC_CHB_ONESHOT)
				, BM_SETCHECK
				, (WPARAM)BST_UNCHECKED
				, (LPARAM)0
			);
			bCheckedStateChbOneshot = FALSE;
		}
		else
		{
			SendMessage(GetDlgItem(hDlg, IDC_CHB_ONESHOT)
				, BM_SETCHECK
				, (WPARAM)BST_CHECKED
				, (LPARAM)0
			);
			bCheckedStateChbOneshot = TRUE;
		}
		return (INT_PTR)TRUE;
	} // eof IDC_CHB_ONESHOT
	case IDC_CHB_SHUTDOWN:
	{
		if (bCheckedStateChbShutdown)
		{
			SendMessage(GetDlgItem(hDlg, IDC_CHB_SHUTDOWN)
				, BM_SETCHECK
				, (WPARAM)BST_UNCHECKED
				, (LPARAM)0
			);
			bCheckedStateChbShutdown = FALSE;
		}
		else
		{
			SendMessage(GetDlgItem(hDlg, IDC_CHB_SHUTDOWN)
				, BM_SETCHECK
				, (WPARAM)BST_CHECKED
				, (LPARAM)0
			);
			bCheckedStateChbShutdown = TRUE;
		}
		return (INT_PTR)TRUE;
	} // eof IDC_CHB_SHUTDOWN
	case IDC_CHB_EXTENDED:
	{
		if (bCheckedStateChbExtended)
		{
			SendMessage(GetDlgItem(hDlg, IDC_CHB_EXTENDED)
				, BM_SETCHECK
				, (WPARAM)BST_UNCHECKED
				, (LPARAM)0
			);
			bCheckedStateChbExtended = FALSE;
		}
		else
		{
			SendMessage(GetDlgItem(hDlg, IDC_CHB_EXTENDED)
				, BM_SETCHECK
				, (WPARAM)BST_CHECKED
				, (LPARAM)0
			);
			bCheckedStateChbExtended = TRUE;
		}
		return (INT_PTR)TRUE;
	} // eof IDC_CHB_EXTENDED
	case APPLY_SETTING:
	{
		return (INT_PTR)TRUE;
	} // eof APPLY_SETTING
	} // eof switch
	
	return (INT_PTR)FALSE;
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
//*                     updateRegister
//****************************************************************************
BOOL updateRegister(const HWND& hWndDlgItem
	, const UINT8& hiByte
	, const UINT8& loByte
	, const UINT8 nofBits = 16
)
{
	UINT8 mask = 128;
	for (UINT8 i = 0; i < 8; i++)
	{
		if (hiByte & mask)
		{
			updateListViewItem(hWndDlgItem
				, 0
				, i
				, (PWCHAR)L"1"
			);
		}
		else
		{
			updateListViewItem(hWndDlgItem
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
			updateListViewItem(hWndDlgItem
				, 0
				, i + 8
				, (PWCHAR)L"1"
			);
		}
		else
		{
			updateListViewItem(hWndDlgItem
				, 0
				, i + 8
				, (PWCHAR)L"0"
			);
		}
		mask >>= 1;
	}
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
		//TODO bCheckedStateChbOneshot is not updated accordingly
	}
	else
	{
		SendMessage(GetDlgItem(hDlg, IDC_CHB_ONESHOT)
			, BM_SETCHECK
			, (WPARAM)BST_UNCHECKED
			, (LPARAM)0
		);
		//TODO bCheckedStateChbOneshot is not updated accordingly
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
		//TODO bCheckedStateChbShutdown is not updated accordingly
	}
	else
	{
		SendMessage(GetDlgItem(hDlg, IDC_CHB_SHUTDOWN)
			, BM_SETCHECK
			, (WPARAM)BST_UNCHECKED
			, (LPARAM)0
		);
		//TODO bCheckedStateChbShutdown is not updated accordingly
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
	if (loByte & 0x20)
	{
		// alert active
		wstrStateAlert = L"Active";
		// test changing background color in edittext
		//g_crT_Lo_Clcs = g_crOutRange;
		//SendMessage(hDlg
		//	, WM_CTLCOLOREDIT
		//	, (WPARAM)GetDC(hDlg)
		//	, (LPARAM)GetDlgItem(hDlg, IDC_T_LO_CLCS)
		//);
		//g_crT_Hi_Clcs = g_crOutRange;
		//SendMessage(hDlg
		//	, WM_CTLCOLOREDIT
		//	, (WPARAM)GetDC(hDlg)
		//	, (LPARAM)GetDlgItem(hDlg, IDC_T_HI_CLCS)
		//);
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
		//TODO bCheckedStateChbExtended is not updated accordingly
	}
	else
	{
		SendMessage(GetDlgItem(hDlg, IDC_CHB_EXTENDED)
			, BM_SETCHECK
			, (WPARAM)BST_UNCHECKED
			, (LPARAM)0
		);
		//TODO bCheckedStateChbExtended is not updated accordingly
	}

	return EXIT_SUCCESS;
}
//VOID updateBkColor(const HWND& hDlg
//	, const UINT16& val_lo
//	, const UINT16& val_hi
//	, const UINT16& val
//)
//{
//	if (val < val_lo)
//	{
//		g_crT_Lo_Clcs = g_crOutRange;
//		SendMessage(hDlg
//			, WM_CTLCOLOREDIT
//			, (WPARAM)GetDC(hDlg)
//			, (LPARAM)GetDlgItem(hDlg, IDC_T_LO_CLCS)
//		);
//		g_crT_Hi_Clcs = g_crInRange;
//		SendMessage(hDlg
//			, WM_CTLCOLOREDIT
//			, (WPARAM)GetDC(hDlg)
//			, (LPARAM)GetDlgItem(hDlg, IDC_T_HI_CLCS)
//		);
//	}
//	else if (val > val_hi)
//	{
//		g_crT_Lo_Clcs = g_crInRange;
//		SendMessage(hDlg
//			, WM_CTLCOLOREDIT
//			, (WPARAM)GetDC(hDlg)
//			, (LPARAM)GetDlgItem(hDlg, IDC_T_LO_CLCS)
//		);
//		g_crT_Hi_Clcs = g_crOutRange;
//		SendMessage(hDlg
//			, WM_CTLCOLOREDIT
//			, (WPARAM)GetDC(hDlg)
//			, (LPARAM)GetDlgItem(hDlg, IDC_T_HI_CLCS)
//		);
//	}
//	else
//	{
//		g_crT_Lo_Clcs = g_crInRange;
//		SendMessage(hDlg
//			, WM_CTLCOLOREDIT
//			, (WPARAM)GetDC(hDlg)
//			, (LPARAM)GetDlgItem(hDlg, IDC_T_LO_CLCS)
//		);
//		g_crT_Hi_Clcs = g_crInRange;
//		SendMessage(hDlg
//			, WM_CTLCOLOREDIT
//			, (WPARAM)GetDC(hDlg)
//			, (LPARAM)GetDlgItem(hDlg, IDC_T_HI_CLCS)
//		);
//	}
	//g_crT_Lo_Clcs = g_crOutRange;
	//SendMessage(hDlg
	//	, WM_CTLCOLOREDIT
	//	, (WPARAM)GetDC(hDlg)
	//	, (LPARAM)GetDlgItem(hDlg, IDC_T_LO_CLCS)
	//);
	//g_crT_Hi_Clcs = g_crOutRange;
	//SendMessage(hDlg
	//	, WM_CTLCOLOREDIT
	//	, (WPARAM)GetDC(hDlg)
	//	, (LPARAM)GetDlgItem(hDlg, IDC_T_HI_CLCS)
	//);
//}

