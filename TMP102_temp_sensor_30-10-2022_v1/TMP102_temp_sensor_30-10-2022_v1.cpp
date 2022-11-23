//****************************************************************************
//*                     note
//*
//****************************************************************************

//****************************************************************************
//*                     include
//****************************************************************************
#include "framework.h"
#include "TMP102_temp_sensor_30-10-2022_v1.h"

//****************************************************************************
//*                     define
//****************************************************************************
#define MAX_LOADSTRING 100

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
//*                     global
//****************************************************************************
// Global Variables:
HINSTANCE g_hInst;								// current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND g_hDlg = { 0 };
HANDLE g_hComm = INVALID_HANDLE_VALUE;
// Resource Aquisition Is Initialisation RAII 
FRAME g_oFrame = { SOH, 0, STX, 0, ETX, ETB, EOT }; 
BOOL g_bContinueTxRx = FALSE;
CHAR g_chBuffer[BUFFER_MAX_SERIAL] = { 0 };
UINT8 g_alert_bit = 0;

//****************************************************************************
//*                     prototype
//****************************************************************************
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI        TxRx(LPVOID lpVoid);
BOOL                transmit();
BOOL                receive(LPVOID lpVoid);

//****************************************************************************
//*                     wWinMain
//****************************************************************************
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TMP102TEMPSENSOR30102022V1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TMP102TEMPSENSOR30102022V1));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (IsDialogMessage(g_hDlg, &msg))
		{
			continue;
		}
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//****************************************************************************
//*                     MyRegisterClass
//****************************************************************************
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TMP102TEMPSENSOR30102022V1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TMP102TEMPSENSOR30102022V1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//****************************************************************************
//*                     InitInstance
//****************************************************************************
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass
		, szTitle
		, WS_OVERLAPPEDWINDOW
		, 10
		, 10
		, 1080
		, 680
		, nullptr
		, nullptr
		, hInstance
		, nullptr
	);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//****************************************************************************
//*                     WndProc
//****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NCCREATE:
	{
		// create dialog
		g_hDlg = CreateDialog(g_hInst, L"DLGPROCWINDOW", hWnd, DlgProc);

		return DefWindowProc(hWnd, message, wParam, lParam);
	} // eof WM_NCCREATE
	case WM_CREATE:
	{
		return (INT_PTR)FALSE;
	} // eof WM_CREATE
	case WM_SIZE:
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		// set size dialog and show dialog
		SetWindowPos(g_hDlg
			, HWND_TOP
			, rect.left
			, rect.top
			, rect.right
			, rect.bottom
			, SWP_SHOWWINDOW
		);
		return (INT_PTR)TRUE;
	} // eof WM_SIZE
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	} // eof WM_COMMAND
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	} // eof WM_PAINT
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	} // eof switch
	return 0;
}

//****************************************************************************
//*                     DlgProc
//****************************************************************************
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hWndLV_Cnfg = { 0 };
	static HWND hWndLV_T_lo = { 0 };
	static HWND hWndLV_T_hi = { 0 };
	static HWND hWndLV_Treg = { 0 };
	static HBRUSH brush = { 0 };
	static HBRUSH redBrush = CreateSolidBrush(RGB(0xFF, 0, 0));
	static HBRUSH greenBrush = CreateSolidBrush(RGB(0, 0xFF, 0));
	static HBRUSH bkColorDlgBrush = CreateSolidBrush(RGB(0xF3, 0xF3, 0xF3));
	static RECT rect;
	switch (uMsg)
	{
	case WM_INITDIALOG:
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
		hWndLV_Cnfg = createListView(g_hInst, hDlg, IDC_LV_CONFIGURATION);
		// add column to listview
		addColumn(g_hInst
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
		hWndLV_T_lo = createListView(g_hInst, hDlg, IDC_LV_T_LO);
		// add column to listview
		addColumn(g_hInst
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
		hWndLV_T_hi = createListView(g_hInst, hDlg, IDC_LV_T_HI);
		// add column to listview
		addColumn(g_hInst
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
		hWndLV_Treg = createListView(g_hInst, hDlg, IDC_LV_TREG);
		// add column to listview
		addColumn(g_hInst
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

		// initialize rect
		rect.left = 450;
		rect.top = 330;
		rect.right = 503;
		rect.bottom = 340;

		// default brush color
		brush = bkColorDlgBrush;

		return (INT_PTR)FALSE;
	} // eof WM_INITDIALOG
	case WM_SIZE:
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
		return (INT_PTR)TRUE;
	} // eof WM_SIZE
	case WM_COMMAND:
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
				g_oFrame.cmnd = RD_REG_TEMP;
				//------------------------------------------------------------

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

				// TODO make sure temperature is preserved in IDC_T_CLCS
			}
			return (INT_PTR)TRUE;
		} // eof DISCONNECT_SERIAL
		case IDC_T_CLCS:
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE:
			{
				if (g_bContinueTxRx)
				{
					// hThreadTxRx is running
					brush = (g_alert_bit) ? redBrush : greenBrush;
					InvalidateRect(hDlg, &rect, TRUE);
				}
				return (INT_PTR)TRUE;
			} // eof EN_CHANGE
			} // eof switch
			return (INT_PTR)FALSE;
		} // eof IDC_T_CLCS
		} // eof switch
		return (INT_PTR)FALSE;
	} // eof WM_COMMAND
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hDlg, &ps);		
		SelectObject(hdc, brush);
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
		EndPaint(hDlg, &ps);
	} // eof WM_PAINT
	} // eof switch
	return (INT_PTR)FALSE;
}

// Message handler for about box.
//****************************************************************************
//*                     About
//****************************************************************************
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
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
//*                     updateListViewItemEx
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
//*                     updateRegister
//****************************************************************************
BOOL updateRegister(const HWND& hWndDlgItem
	, const UINT8 hiByte
	, const UINT8 loByte
	, const UINT8 nofBits = 16
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

	// update register
	updateRegister(GetDlgItem(g_hDlg, IDC_LV_TREG)
		, g_chBuffer[4]
		, g_chBuffer[5]
	);

	// isolate alert bit
	g_alert_bit = (g_chBuffer[5] & 2) >> 1;

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
	temp_c *= 100;
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
	return EXIT_SUCCESS;
}

/*
* waste
//****************************************************************************
//*                     note
//*
//* investigate Rx/Tx with a STM32 UART at:
//* https://controllerstech.com/uart-receive-in-stm32/
//* https://www.youtube.com/watch?v=l5msGz2tZP0
//* three methods are discussed:
//* 1) poll
//* 2) interrupt
//* 3) dma
//* for now I will focus on method two (interrupt)
//* this C++ code is written following the text at:
//* https://www.tetraedre.ch/advanced/files/msdn_serial.htm
//****************************************************************************
// TMP102_temp_sensor_30-10-2022_v1.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TMP102_temp_sensor_30-10-2022_v1.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE g_hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND g_hDlg = { 0 };
HANDLE g_hComm = INVALID_HANDLE_VALUE;
UINT16 g_uSerialCommand = 0;
CHAR g_chBufferTransmit[BUFFER_MAX_SERIAL] = { 0 };
CHAR g_chBufferReceive[BUFFER_MAX_SERIAL] = { 0 };
BOOL g_bTransmit = TRUE;
//COLORREF g_crInRange = 0x0060DD60;
//COLORREF g_crOutRange = 0x008080FF;
//COLORREF g_crT_Lo_Clcs = g_crInRange;
//COLORREF g_crT_Hi_Clcs = g_crInRange;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
//TODO removeBOOL                connect_serial();
//TODO removeDWORD WINAPI        receive_serial(LPVOID lpVoid);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TMP102TEMPSENSOR30102022V1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TMP102TEMPSENSOR30102022V1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
		if (IsDialogMessage(g_hDlg, &msg))
		{
			continue;
		}
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TMP102TEMPSENSOR30102022V1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TMP102TEMPSENSOR30102022V1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   g_hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass
       , szTitle
       , WS_OVERLAPPEDWINDOW
       , 10
       , 10
       , 1080
       , 680
       , nullptr
       , nullptr
       , hInstance
       , nullptr
   );

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
//****************************************************************************
//*                     WndProc
//****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_NCCREATE:
    {
		// create dialog
		g_hDlg = CreateDialog(g_hInst, L"DLGPROCWINDOW", hWnd, DlgProc);
        return DefWindowProc(hWnd, message, wParam, lParam);
    } // eof WM_NCCREATE
    case WM_CREATE:
    {
        return 0;
    } // eof WM_CREATE
    case WM_SIZE:
    {
		RECT rect;
		GetClientRect(hWnd, &rect);
		// set size dialog and show dialog
		SetWindowPos(g_hDlg
			, HWND_TOP
			, rect.left
			, rect.top
			, rect.right
			, rect.bottom
			, SWP_SHOWWINDOW
		);
        return (INT_PTR)TRUE;
    } // eof WM_SIZE
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//****************************************************************************
//*                     DlgProc
//****************************************************************************
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HANDLE hThreadTransmit = INVALID_HANDLE_VALUE;
	static HANDLE hThreadReceive = INVALID_HANDLE_VALUE;
	static HWND hWndLV_Cnfg = { 0 };
    static HWND hWndLV_T_lo = { 0 };
	static HWND hWndLV_T_hi = { 0 };
	static HWND hWndLV_Treg = { 0 };
	static BOOL bCheckedStateChbOneshot = FALSE;
	static BOOL bCheckedStateChbShutdown = FALSE;
	static BOOL bCheckedStateChbExtended = FALSE;
	//static COLORREF crInRange = 0x0060DD60;
	//static COLORREF crOutRange = 0x008080FF;
	//static COLORREF crT_Lo_Clcs = crInRange;
	//static COLORREF crT_Hi_Clcs = crInRange;

	switch (uMsg)
    {
    case WM_INITDIALOG:
    {
		onWmInitDialog_DlgProc(g_hInst
			, hDlg
			, hWndLV_Cnfg
			, hWndLV_T_lo
			, hWndLV_T_hi
			, hWndLV_Treg
		);

		// send the command to fetch the value of the configuration register
		g_uSerialCommand = RD_REG_CNFG;
		sprintf_s(g_chBufferTransmit, BUFFER_MAX_SERIAL, "%d", g_uSerialCommand);

		// after MAX_RETRY_SERIAL receives the g_uSerialCommand value will change to the next,
		// ultimately the g_uSerialCommand value will be set to RD_REG_TEMP
		// and the actual temperature will be received
		
		// send the command to fetch the value of the temp-lo register
		//g_uSerialCommand = RD_REG_T_LO;
		//sprintf_s(g_chBufferTransmit, BUFFER_MAX_SERIAL, "%d", g_uSerialCommand);

		// send the command to fetch the value of the temp-hi register
		//g_uSerialCommand = RD_REG_T_HI;
		//sprintf_s(g_chBufferTransmit, BUFFER_MAX_SERIAL, "%d", g_uSerialCommand);

		// send the command to fetch the value of the temperature register
		//g_uSerialCommand = RD_REG_TEMP;
		//sprintf_s(g_chBufferTransmit, BUFFER_MAX_SERIAL, "%d", g_uSerialCommand);

		return (INT_PTR)FALSE;
    }
    case WM_SIZE:
    {
		onWmSize_DlgProc(hWndLV_Cnfg
			, hWndLV_T_lo
			, hWndLV_T_hi
			, hWndLV_Treg
		);

		return (INT_PTR)TRUE;
    }
	case WM_COMMAND:
	{
		onWmCommand_DlgProc(g_hInst
			, hDlg
			, wParam
			, g_hComm
			, hThreadTransmit
			, hThreadReceive
			, bCheckedStateChbOneshot
			, bCheckedStateChbShutdown
			, bCheckedStateChbExtended
		);
	} // eof WM_COMMAND
	//case WM_CTLCOLOREDIT:
	//{
	//	if ((HWND)lParam == GetDlgItem(hDlg, IDC_T_LO_CLCS))
	//	{
	//		SetBkColor((HDC)wParam, g_crT_Lo_Clcs);
	//		SetDCBrushColor((HDC)wParam, g_crT_Lo_Clcs);
	//		return (LRESULT)GetStockObject(DC_BRUSH);
	//	}
	//	if ((HWND)lParam == GetDlgItem(hDlg, IDC_T_HI_CLCS))
	//	{
	//		SetBkColor((HDC)wParam, g_crT_Hi_Clcs);
	//		SetDCBrushColor((HDC)wParam, g_crT_Hi_Clcs);
	//		return (LRESULT)GetStockObject(DC_BRUSH);
	//	}
	//	return (INT_PTR)FALSE;
	//} // eof WM_CTLCOLOREDIT
	} // eof switch
	return (INT_PTR)FALSE;
}

//****************************************************************************
//*                     transmit
//****************************************************************************
DWORD WINAPI transmit(LPVOID lpVoid)
{
	DWORD dwNofByteTransferred = 0;
	BOOL bResult = FALSE;
	// infinite loop
	while (TRUE)
	{
		if (g_bTransmit)
		{
			bResult = WriteFile(g_hComm
				, &g_chBufferTransmit
				, 5
				, &dwNofByteTransferred
				, NULL
			);
			OutputDebugStringA(g_chBufferTransmit);
			OutputDebugString(L" ");
			Sleep(DELAY_4Hz);
			g_bTransmit = FALSE;
		}
	}
	return 0;
}

//****************************************************************************
//*                     receive
//****************************************************************************
DWORD WINAPI receive(LPVOID lpVoid)
{
	CHAR chBufTempInCelcius[BUFFER_MAX_SERIAL] = { 0 };
	DWORD dwNofByteTransferred = 0;
	BOOL bResult = FALSE;
	UINT8 cReceive = 0;
	UINT16 val = 0, val_lo = 0, val_hi = 0;
	FLOAT temp_c = 0.;
	// infinite loop
	while (TRUE)
	{
		if (!g_bTransmit)
		{
			bResult = ReadFile(g_hComm
				, &g_chBufferReceive
				, 2
				, &dwNofByteTransferred
				, NULL
			);
			OutputDebugStringA(g_chBufferReceive);
			OutputDebugString(L"\n");
			if (dwNofByteTransferred == 2)
			{
				// set CONFIGURATION /////////////////////////////////////////
				if (g_uSerialCommand == RD_REG_CNFG)
				{
					updateRegister(GetDlgItem(g_hDlg, IDC_LV_CONFIGURATION)
						, g_chBufferReceive[0]
						, g_chBufferReceive[1]
						, 12
					);
					updateSetting(g_hDlg
						, g_chBufferReceive[0]
						, g_chBufferReceive[1]
					);
					++cReceive;
					if (cReceive == MAX_RETRY_SERIAL)
					{
						cReceive = 0;
						// send the command to fetch the value of the temp-lo register
						g_uSerialCommand = RD_REG_T_LO;
						sprintf_s(g_chBufferTransmit, BUFFER_MAX_SERIAL, "%d", g_uSerialCommand);
					}
				}
				// do not use
				//val = ((INT16)g_chBufferReceive[0] << 4) | (g_chBufferReceive[1] >> 4);
				// use this correct statement
				val = ((INT8)g_chBufferReceive[0] << 4 | (UINT8)g_chBufferReceive[1] >> 4);
				// do not use
				//if (val > 0x7FF) val |= 0xF000;
				// use the traditional definition of a two'2 complement
				// logical not operator ! or bitwise operator ~ can be used
				if (val & 0x8000)
				{
					val = ~val;
					val += 1;
				}
				temp_c = val * 0.0625;
				temp_c *= 100;
				sprintf_s(chBufTempInCelcius
					, BUFFER_MAX_SERIAL
					, "%d.%02d"
					, ((UINT)temp_c / 100)
					, ((UINT)temp_c % 100)
				);
				// set TEMPERATURE LOW ///////////////////////////////////////
				if (g_uSerialCommand == RD_REG_T_LO)
				{
					updateRegister(GetDlgItem(g_hDlg, IDC_LV_T_LO)
						, g_chBufferReceive[0]
						, g_chBufferReceive[1]
					);
					SendMessageA(GetDlgItem(g_hDlg, IDC_T_LO_CLCS)
						, WM_SETTEXT
						, (WPARAM)0
						, (LPARAM)chBufTempInCelcius
					);
					++cReceive;
					if (cReceive == MAX_RETRY_SERIAL)
					{
						val_lo = val;
						cReceive = 0;
						// send the command to fetch the value of the temp-lo register
						g_uSerialCommand = RD_REG_T_HI;
						sprintf_s(g_chBufferTransmit, BUFFER_MAX_SERIAL, "%d", g_uSerialCommand);
					}
				}
				// set TEMPERATURE HIGH //////////////////////////////////////
				if (g_uSerialCommand == RD_REG_T_HI)
				{
					updateRegister(GetDlgItem(g_hDlg, IDC_LV_T_HI)
						, g_chBufferReceive[0]
						, g_chBufferReceive[1]
					);
					SendMessageA(GetDlgItem(g_hDlg, IDC_T_HI_CLCS)
						, WM_SETTEXT
						, (WPARAM)0
						, (LPARAM)chBufTempInCelcius
					);
					++cReceive;
					if (cReceive == MAX_RETRY_SERIAL)
					{
						val_hi = val;
						cReceive = 0;
						// send the command to fetch the value of the temp-lo register
						g_uSerialCommand = RD_REG_TEMP;
						sprintf_s(g_chBufferTransmit, BUFFER_MAX_SERIAL, "%d", g_uSerialCommand);
					}
				}
				// set TEMPERATURE REGISTER //////////////////////////////////
				if (g_uSerialCommand == RD_REG_TEMP)
				{
					updateRegister(GetDlgItem(g_hDlg, IDC_LV_TREG)
						, g_chBufferReceive[0]
						, g_chBufferReceive[1]
					);
					SendMessageA(GetDlgItem(g_hDlg, IDC_T_CLCS)
						, WM_SETTEXT
						, (WPARAM)0
						, (LPARAM)chBufTempInCelcius
					);
					updateTAlert(g_hDlg, val_lo, val_hi, val);
				}
				//updateBkColor(g_hDlg, val_lo, val_hi, val);
			}
			Sleep(DELAY_4Hz);
			g_bTransmit = TRUE;
		}
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
*/
/*
//****************************************************************************
//*                     connect_serial
//****************************************************************************
BOOL connect_serial()
{
	// create file for RS-232 communication port
	g_hComm = CreateFile(L"\\\\.\\COM3"
		, GENERIC_READ | GENERIC_WRITE
		, 0
		, NULL
		, OPEN_EXISTING
		, FILE_FLAG_OVERLAPPED
		, NULL
	);
	if (g_hComm == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	// set structure to initialize communication port
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
	// initialize communication port
	if (!SetCommState(g_hComm, (LPDCB)&dcb))
	{
		return FALSE;
	}

	// set structure for communication port timeout
	COMMTIMEOUTS commtimeouts;
	commtimeouts.ReadIntervalTimeout = MAXDWORD;
	commtimeouts.ReadTotalTimeoutMultiplier = 0;
	commtimeouts.ReadTotalTimeoutConstant = 0;
	commtimeouts.WriteTotalTimeoutMultiplier = 0;
	commtimeouts.WriteTotalTimeoutConstant = 0;
	// set communication port timeout
	if (!SetCommTimeouts(g_hComm, (LPCOMMTIMEOUTS)&commtimeouts))
	{
		return FALSE;
	}

	// set communication port mask bit to capture event
	if (!SetCommMask(g_hComm, EV_TXEMPTY | EV_RXCHAR))
	{
		return FALSE;
	}

	// set in/out queue buffers
	if (!SetupComm(g_hComm, BUFFER_MAX_SERIAL, BUFFER_MAX_SERIAL))
	{
		return FALSE;
	}

	return TRUE;
}

//****************************************************************************
//*                     receive_serial
//****************************************************************************
DWORD WINAPI receive_serial(LPVOID lpVoid)
{
	HWND hWndRcvMessage = GetDlgItem((HWND)lpVoid, IDC_RCV_MESSAGE);

	DWORD dwEvtMask = 0;
	OVERLAPPED overlapped = { 0 };
	overlapped.hEvent = CreateEvent(NULL
		, TRUE
		, FALSE
		, NULL
	);
	if (overlapped.hEvent == NULL) return 1;

	DWORD dwReturnCode = 0;
	DWORD dwNofByteTransferred = 0;
	BOOL bResult = FALSE;
	DWORD dwBytesRead = 0;
	DWORD64 dwTotalBytesRead = 0;
	CHAR chBuffer[BUFFER_MAX_SERIAL] = { 0 };

	// infinite loop
	while (TRUE)
	{
		WaitCommEvent(g_hComm, (LPDWORD)&dwEvtMask, &overlapped);
		WaitForSingleObject(overlapped.hEvent, INFINITE);
		GetOverlappedResult(g_hComm, &overlapped, &dwNofByteTransferred, FALSE);
		if (dwEvtMask & EV_RXCHAR)
		{
			// create an overlapped structure for reading from file
			OVERLAPPED overlapped_ = { 0 };
			overlapped_.Offset = dwTotalBytesRead & 0xFFFFFFFF;
			overlapped_.OffsetHigh =
				(DWORD)Int64ShrlMod32(dwTotalBytesRead, 31);
			overlapped_.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			// read input buffer into char-buffer
			bResult = ReadFile(g_hComm, &chBuffer, BUFFER_MAX_SERIAL, &dwBytesRead, &overlapped_);
			if (bResult)
			{
				dwTotalBytesRead += dwBytesRead;
				std::string str = chBuffer;
				std::string sub = "";
				for (std::string::iterator ite = str.begin(); ite != str.end(); ++ite)
				{
					if (sub.length() >= 7)
					{
						SendMessageA(hWndRcvMessage
							, WM_SETTEXT
							, (WPARAM)0
							, (LPARAM)sub.c_str()
						);
					}
					if (*ite == '\r')
					{
						sub = "";
					}
					if (*ite != '\n')
					{
						sub += *ite;
					}
				}
			}
		}
	}
	return 0;
}
*/