//****************************************************************************
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

HANDLE g_hComm = { 0 };

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL                connect_serial();
DWORD WINAPI        receive_serial(LPVOID lpVoid);
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
    static HWND hWndDlg;
    switch (message)
    {
    case WM_NCCREATE:
    {
		// create dialog
		hWndDlg = CreateDialog(g_hInst, L"DLGPROCWINDOW", hWnd, DlgProc);
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
		SetWindowPos(hWndDlg
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
    static HWND hWndLV_Cnfg = { 0 };
    static HWND hWndLV_T_lo = { 0 };
	static HWND hWndLV_T_hi = { 0 };
	static HWND hWndLV_Treg = { 0 };
	switch (uMsg)
    {
    case WM_INITDIALOG:
    {
		// set edit control IDC_STATUS_CONNECT_ text
		SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT_)
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

		return (INT_PTR)FALSE;
    }
    case WM_SIZE:
    {
		// CONFIGURATION
		MoveWindow(hWndLV_Cnfg
			, 20, 75
			, 336
			, 40
			, TRUE
		);
		// TEMPERATURE LOW
		MoveWindow(hWndLV_T_lo
			, 20, 140
			, 417
			, 40
			, TRUE
		);
		// TEMPERATURE HIGH
		MoveWindow(hWndLV_T_hi
			, 20, 205
			, 417
			, 40
			, TRUE
		);
		// TEMPERATURE REGISTER
		MoveWindow(hWndLV_Treg
			, 20, 270
			, 417
			, 40
			, TRUE
		);
		return (INT_PTR)TRUE;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case CONNECT_SERIAL:
        {
			// connect to serial
            if (connect_serial())
            {
                // set edit control IDC_STATUS_CONNECT_ text
                SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT_)
                    , WM_SETTEXT
                    , (WPARAM)0
                    , (LPARAM)L"Connected"
                );
                // enable/disable button
                EnableWindow(GetDlgItem(hDlg, CONNECT_SERIAL), FALSE);
                EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), TRUE);

				DWORD dwThreadId;
				HANDLE hThread_ = CreateThread(NULL
					, 0
					, receive_serial
					, (LPVOID)hDlg
					, 0
					, &dwThreadId
				);
            }
            return (INT_PTR)TRUE;
        } // eof CONNECT_SERIAL
        case DISCONNECT_SERIAL:
        {
			// disconnect from serial
			if (CloseHandle(g_hComm))
			{
                // set edit control IDC_STATUS_CONNECT_ text
				SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT_)
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
        } // eof switch
        break;
    } // eof WM_COMMAND
    } // eof switch
    return (INT_PTR)FALSE;
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

////
////  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
////
////  PURPOSE: Processes messages for the main window.
////
////  WM_COMMAND  - process the application menu
////  WM_PAINT    - Paint the main window
////  WM_DESTROY  - post a quit message and return
////
////
////****************************************************************************
////*                     WndProc
////****************************************************************************
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    //static HWND hWndDlg;
//
//    switch (message)
//    {
//    case WM_NCCREATE:
//    {
//        // create dialog
//        g_hWndDlg = /*hWndDlg = */CreateDialog(g_hInst, L"DLGPROCWINDOW", hWnd, DlgProc);
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    } // eof WM_NCCREATE
//    case WM_CREATE:
//    {
//
//		return 0;
//    } // eof WM_CREATE
//    case WM_SIZE:
//    {
//        RECT rect;
//        GetClientRect(hWnd, &rect);
//        // set size dialog and show dialog
//        SetWindowPos(g_hWndDlg/*hWndDlg*/
//            , HWND_TOP
//            , rect.left
//            , rect.top
//            , rect.right
//            , rect.bottom
//            , SWP_SHOWWINDOW
//        );
//        //SendMessage(hWndDlg, WM_COMMAND, (WPARAM)START_RECEIVE, (LPARAM)0);
//        return (INT_PTR)TRUE;
//    } // eof WM_SIZE
//    case WM_COMMAND:
//    {
//        int wmId = LOWORD(wParam);
//        // Parse the menu selections:
//        switch (wmId)
//        {
//        case IDM_ABOUT:
//            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
//            break;
//        case IDM_EXIT:
//            DestroyWindow(hWnd);
//            break;
//        default:
//            return DefWindowProc(hWnd, message, wParam, lParam);
//        }
//    }
//    break;
//    case WM_PAINT:
//    {
//        PAINTSTRUCT ps;
//        HDC hdc = BeginPaint(hWnd, &ps);
//        // TODO: Add any drawing code that uses hdc here...
//        EndPaint(hWnd, &ps);
//    }
//    break;
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        break;
//    default:
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    }
//    return 0;
//}
////****************************************************************************
////*                     DlgProc
////****************************************************************************
//INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//    static HWND hWndLV = { 0 };
//    static HWND hWndLV_T_LO = { 0 };
//    static INT* pcmnd;
//    switch (uMsg)
//    {
//    case WM_INITDIALOG:
//    {
//        // set edit control IDC_STATUS_CONNECT_ text
//        SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT_)
//            , WM_SETTEXT
//            , (WPARAM)0
//            , (LPARAM)L"Disconnected"
//        );
//        // disable button DISCONNECT_SERIAL
//        EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), FALSE);
//        // create listview
//        hWndLV = createListView(g_hInst, hDlg, IDC_LV_CONFIGURATION);
//        // add column to listview
//        addColumn(g_hInst
//            , hWndLV
//            , 12
//            , IDS_LVCONFIG_COL0, IDS_LVCONFIG_COL1, IDS_LVCONFIG_COL2
//            , IDS_LVCONFIG_COL3, IDS_LVCONFIG_COL4, IDS_LVCONFIG_COL5
//            , IDS_LVCONFIG_COL6, IDS_LVCONFIG_COL7, IDS_LVCONFIG_COL8
//            , IDS_LVCONFIG_COL9, IDS_LVCONFIG_COL10, IDS_LVCONFIG_COL11
//        );
//        // adjust column with to the header text width
//        for (uint8_t i = 0; i < 12; i++)
//        {
//            ListView_SetColumnWidth(hWndLV, i, LVSCW_AUTOSIZE_USEHEADER);
//            createListViewItem(hWndLV
//                , (PWCHAR)L" "
//                , 0
//                , i
//            );
//        }
//        // create listview
//        hWndLV_T_LO = createListView(g_hInst, hDlg, IDC_LV_T_LO);
//        // add column to listview
//        addColumn(g_hInst
//            , hWndLV_T_LO
//            , 16
//            , IDS_LVTEMP_COL0, IDS_LVTEMP_COL1, IDS_LVTEMP_COL2, IDS_LVTEMP_COL3
//            , IDS_LVTEMP_COL4, IDS_LVTEMP_COL5, IDS_LVTEMP_COL6, IDS_LVTEMP_COL7
//            , IDS_LVTEMP_COL0, IDS_LVTEMP_COL1, IDS_LVTEMP_COL2, IDS_LVTEMP_COL3
//            , IDS_LVTEMP_COL4, IDS_LVTEMP_COL5, IDS_LVTEMP_COL6, IDS_LVTEMP_COL7
//        );
//        // adjust column with to the header text width
//        // and create an empty listview item
//        for (uint8_t i = 0; i < 16; i++)
//        {
//            ListView_SetColumnWidth(hWndLV_T_LO, i, LVSCW_AUTOSIZE_USEHEADER);
//        }
//        return (INT_PTR)FALSE;
//    }
//    case WM_SIZE:
//    {
//        MoveWindow(hWndLV
//            , 20, 75
//            , 336
//            , 40
//            , TRUE
//        );
//        MoveWindow(hWndLV_T_LO
//            , 20, 140
//            , 417
//            , 40
//            , TRUE
//        );
//        return (INT_PTR)TRUE;
//    }
//    case WM_COMMAND:
//    {
//        switch (LOWORD(wParam))
//        {
//        case CONNECT_SERIAL:
//        {
//            // connect to serial
//            if (connect_serial())
//            {
//                HANDLE hThread = { 0 };
//                DWORD dwThreadId = 0;
//                // set edit control IDC_STATUS_CONNECT_ text
//                SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT_)
//                    , WM_SETTEXT
//                    , (WPARAM)0
//                    , (LPARAM)L"Connected"
//                );
//                // enable/disable button
//                EnableWindow(GetDlgItem(hDlg, CONNECT_SERIAL), FALSE);
//                EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), TRUE);
//                // send message to read the configuration register
//                pcmnd = new INT(RD_REG_CNFG);
//                
//                hThread = CreateThread(NULL
//                    , 0
//                    , send_serial
//                    , (LPVOID)pcmnd
//                    , 0
//                    , &dwThreadId
//                );
//                // pcmnd can't be thrown away because the thread 
//                // is using it, this results in a minor memory leakage
//                // this thread dies after a transmission
//
//                // start receiving, has an infinite loop
//                DWORD dwThreadId_;
//                HANDLE hThread_ = CreateThread(NULL
//                    , 0
//                    , receive_serial
//                    , (LPVOID)pcmnd
//                    , 0
//                    , &dwThreadId
//                );
//                // send message to read the temperature low register
//                pcmnd = new INT(RD_REG_T_LO);
//
//                send_serial(pcmnd);
//                receive_serial(pcmnd);
//                //hThread = CreateThread(NULL
//                //    , 0
//                //    , send_serial
//                //    , (LPVOID)pcmnd
//                //    , 0
//                //    , &dwThreadId
//                //);
//            }
//
//            return (INT_PTR)TRUE;
//        } // eof CONNECT_SERIAL
//        case DISCONNECT_SERIAL:
//        {
//            // disconnect from serial
//            if (CloseHandle(g_hComm))
//            {
//                // set edit control IDC_STATUS_CONNECT_ text
//                SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT_)
//                    , WM_SETTEXT
//                    , (WPARAM)0
//                    , (LPARAM)L"Disconnected"
//                );
//                // enable/disable button
//                EnableWindow(GetDlgItem(hDlg, CONNECT_SERIAL), TRUE);
//                EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), FALSE);
//            }
//            return (INT_PTR)TRUE;
//        } // eof DISCONNECT_SERIAL
//   //     case START_RECEIVE:
//   //     {
//			//// create file for RS-232 communication port
//			//g_hComm = CreateFile(L"\\\\.\\COM3"
//			//	, GENERIC_READ | GENERIC_WRITE
//			//	, 0
//			//	, NULL
//			//	, OPEN_EXISTING
//			//	, FILE_FLAG_OVERLAPPED
//			//	, NULL
//			//);
//			//// set structure to initialize communication port
//			//DCB dcb;
//			//dcb.DCBlength = sizeof(DCB);
//			//dcb.BaudRate = 115200;
//			//dcb.fBinary = 1;
//			//dcb.fParity = 0;
//			//dcb.fOutxCtsFlow = 0;
//			//dcb.fOutxDsrFlow = 0;
//			//dcb.fDtrControl = 1;
//			//dcb.fDsrSensitivity = 0;
//			//dcb.fTXContinueOnXoff = 0;
//			//dcb.fOutX = 0;
//			//dcb.fInX = 0;
//			//dcb.fErrorChar = 0;
//			//dcb.fNull = 0;
//			//dcb.fRtsControl = 1;
//			//dcb.fAbortOnError = 0;
//			//dcb.fDummy2 = 0;
//			//dcb.wReserved = 0;
//			//dcb.ByteSize = 8;
//			//dcb.Parity = 0;
//			//dcb.StopBits = 0;
//			//dcb.XoffChar = 0;
//			//dcb.XoffChar = 0;
//			//dcb.ErrorChar = 24;
//			//dcb.EvtChar = 0;
//			//dcb.wReserved1 = 0;
//			//dcb.ByteSize = 8;
//			//dcb.StopBits = 0;
//			//// initialize communication port
//			//SetCommState(g_hComm, (LPDCB)&dcb);
//			//// set structure for communication port timeout
//			//COMMTIMEOUTS commtimeouts;
//			//commtimeouts.ReadIntervalTimeout = MAXDWORD;
//			//commtimeouts.ReadTotalTimeoutMultiplier = 0;
//			//commtimeouts.ReadTotalTimeoutConstant = 0;
//			//commtimeouts.WriteTotalTimeoutMultiplier = 0;
//			//commtimeouts.WriteTotalTimeoutConstant = 0;
//			//// set communication port timeout
//			//SetCommTimeouts(g_hComm, (LPCOMMTIMEOUTS)&commtimeouts);
//			//// set communication port mask bit to capture event
//			//SetCommMask(g_hComm, EV_RXCHAR);
//   //         SetupComm(g_hComm, BUFFER_MAX_SERIAL, 0);
//			//// create thread and pass a handle of the dialog to the thread func
//			//LPVOID lpParam = hDlg;
//			//DWORD dwThreadId;
//			//HANDLE hThread = CreateThread(NULL
//			//	, 0
//			//	, RS232ThreadFunc
//			//	, lpParam
//			//	, 0
//			//	, &dwThreadId
//			//);
//   //         Button_Enable(GetDlgItem(hDlg, START_RECEIVE), FALSE);
//   //         return (INT_PTR)TRUE;
//   //     }
//        case SERIAL_DISCONNECTED:
//        {
//            Button_Enable(GetDlgItem(hDlg, START_RECEIVE), TRUE);
//            return (INT_PTR)TRUE;
//        } // eof SERIAL_DISCONNECTED
//        //case START_TRANSMIT:
//        //{
//        //    // get content from edit control IDC_STATUS_CONNECT
//        //    CHAR chBuffer[13] = { 0 };
//        //    SendMessageA(GetDlgItem(hDlg, IDC_STATUS_CONNECT)
//        //        , WM_GETTEXT
//        //        , (WPARAM)13
//        //        , (LPARAM)chBuffer
//        //    );
//        //    // if IDC_STATUS_CONNECT contains 'Disconnected' return
//        //    if (strcmp(chBuffer, "Disconnected") == 0)
//        //    {
//        //        return (INT_PTR)TRUE;
//        //    }
//        //    // create thread and pass a handle of the dialog to the thread func
//        //    LPVOID lpParam = hDlg;
//        //    DWORD dwThreadId;
//        //    HANDLE hThread = CreateThread(NULL
//        //        , 0
//        //        , RS232ThreadFunc4Transmit
//        //        , lpParam
//        //        , 0
//        //        , &dwThreadId
//        //    );
//        //    return (INT_PTR)TRUE;
//        //} // eof START_TRANSMIT
//        } // eof switch
//        break;
//    } // eof WM_COMMAND
//    case RD_REG_CNFG:
//    {
//        CHAR* pChar = (CHAR*)lParam;
//        CHAR chBuffer[2] = { 0 };
//        chBuffer[0] = *(pChar + 0);
//        chBuffer[1] = *(pChar + 1);
//        BYTE mask = 128;
//        for (int i = 0; i < 8; i++)
//        {
//            if (chBuffer[0] & mask)
//            {
//                //// must be changed into an update listview item
//                //createListViewItem(hWndLV
//                //    , (PWCHAR)L"1"
//                //    , 0
//                //    , i
//                //);
//                updateListViewItem(hWndLV
//                    , 0
//                    , i
//                    , (PWCHAR)L"1"
//                );
//            }
//            else
//            {
//                //createListViewItem(hWndLV
//                //    , (PWCHAR)L"0"
//                //    , 0
//                //    , i
//                //);
//                updateListViewItem(hWndLV
//                    , 0
//                    , i
//                    , (PWCHAR)L"0"
//                );
//            }
//            mask >>= 1;
//        }
//        mask = 128;
//        for (int i = 0; i < 4; i++)
//        {
//            if (chBuffer[1] & mask)
//            {
//                //createListViewItem(hWndLV
//                //    , (PWCHAR)L"1"
//                //    , 0
//                //    , i + 8
//                //);
//                updateListViewItem(hWndLV
//                    , 0
//                    , i + 8
//                    , (PWCHAR)L"1"
//                );
//            }
//            else
//            {
//                //createListViewItem(hWndLV
//                //    , (PWCHAR)L"0"
//                //    , 0
//                //    , i + 8
//                //);
//                updateListViewItem(hWndLV
//                    , 0
//                    , i + 8
//                    , (PWCHAR)L"0"
//                );
//            }
//            mask >>= 1;
//        }
//        break;
//    } // eof RD_REG_CNFG
//    case RD_REG_T_LO:
//    {
//        CHAR* pChar = (CHAR*)lParam;
//        CHAR chBuffer[2] = { 0 };
//        chBuffer[0] = *(pChar + 0);
//        chBuffer[1] = *(pChar + 1);
//    } // eof RD_REG_T_LO
//    } // eof switch
//    return (INT_PTR)FALSE;
//}
////****************************************************************************
////*                     connect_serial
////****************************************************************************
//BOOL connect_serial()
//{
//    // create file for RS-232 communication port
//    g_hComm = CreateFile(L"\\\\.\\COM3"
//        , GENERIC_READ | GENERIC_WRITE
//        , 0
//        , NULL
//        , OPEN_EXISTING
//        , FILE_FLAG_OVERLAPPED
//        , NULL
//    );
//    if (g_hComm == INVALID_HANDLE_VALUE)
//    {
//        return FALSE;
//    }
//
//    // set structure to initialize communication port
//    DCB dcb;
//    dcb.DCBlength = sizeof(DCB);
//    dcb.BaudRate = 115200;
//    dcb.fBinary = 1;
//    dcb.fParity = 0;
//    dcb.fOutxCtsFlow = 0;
//    dcb.fOutxDsrFlow = 0;
//    dcb.fDtrControl = 1;
//    dcb.fDsrSensitivity = 0;
//    dcb.fTXContinueOnXoff = 0;
//    dcb.fOutX = 0;
//    dcb.fInX = 0;
//    dcb.fErrorChar = 0;
//    dcb.fNull = 0;
//    dcb.fRtsControl = 1;
//    dcb.fAbortOnError = 0;
//    dcb.fDummy2 = 0;
//    dcb.wReserved = 0;
//    dcb.ByteSize = 8;
//    dcb.Parity = 0;
//    dcb.StopBits = 0;
//    dcb.XoffChar = 0;
//    dcb.XoffChar = 0;
//    dcb.ErrorChar = 24;
//    dcb.EvtChar = 0;
//    dcb.wReserved1 = 0;
//    dcb.ByteSize = 8;
//    dcb.StopBits = 0;
//    // initialize communication port
//    if (!SetCommState(g_hComm, (LPDCB)&dcb))
//    {
//        return FALSE;
//    }
//
//    // set structure for communication port timeout
//    COMMTIMEOUTS commtimeouts;
//    commtimeouts.ReadIntervalTimeout = MAXDWORD;
//    commtimeouts.ReadTotalTimeoutMultiplier = 0;
//    commtimeouts.ReadTotalTimeoutConstant = 0;
//    commtimeouts.WriteTotalTimeoutMultiplier = 0;
//    commtimeouts.WriteTotalTimeoutConstant = 0;
//    // set communication port timeout
//    if (!SetCommTimeouts(g_hComm, (LPCOMMTIMEOUTS)&commtimeouts))
//    {
//        return FALSE;
//    }
//
//    // set communication port mask bit to capture event
//    if (!SetCommMask(g_hComm, EV_RXCHAR))
//    {
//        return FALSE;
//    }
//
//    // set in/out queue buffers
//    if (!SetupComm(g_hComm, BUFFER_MAX_SERIAL, BUFFER_MAX_SERIAL))
//    {
//        return FALSE;
//    }
//
//    return TRUE;
//}
////****************************************************************************
////*                     send_serial
////****************************************************************************
//DWORD WINAPI send_serial(LPVOID pcmnd_)
//{
//    // must be a two stage step
//    INT_PTR int_ptr = reinterpret_cast<INT_PTR>(pcmnd_);
//    INT* pcmnd = reinterpret_cast<INT*>(int_ptr);
//    CHAR chCmnd[6] = { 0 };
//    _itoa_s(*pcmnd, chCmnd, 6, 10);
//
//    DWORD dwReturnCode = 0;
//
//    OVERLAPPED overlapped = { 0 };
//    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//    if (overlapped.hEvent == NULL) dwReturnCode = 1;
//
//    DWORD dwNofByteTransferred = 0;
//    BOOL bResult = FALSE;
//    DWORD dwResult = 0;
//    CHAR chBuffer[BUFFER_MAX_SERIAL] = { 0 };
//
//    // infinite loop
//    while (TRUE)
//    {
//        // create a string for transmission
//        // STX ddddd ETX EOT
//        // where:
//        // ddddd is a five digit command
//        // in fact the transmission string has no spaces, i.d. 0x02ddddd0x030x04
//        sprintf_s(chBuffer, BUFFER_MAX_SERIAL, "%d%s%d%d", 0x02, chCmnd, 0x03, 0x04);
//        // issue a write
//        bResult = WriteFile(g_hComm, &chBuffer, strlen(chBuffer), &dwNofByteTransferred, &overlapped);
//        if (bResult)
//        {
//            // func WriteFile() completed immediately
//            // dwReturnCode = 0;
//            //if (overlapped.hEvent != NULL) CloseHandle(overlapped.hEvent);
//            //return dwReturnCode;
//        }
//        else
//        {
//            if (GetLastError() != ERROR_IO_PENDING)
//            {
//                // func WriteFile() failed, but isn't delayed
//                dwReturnCode = 1;
//                if (overlapped.hEvent != NULL) CloseHandle(overlapped.hEvent);
//                return dwReturnCode;
//            }
//            else
//            {
//                // func WriteFile() is pending
//                dwResult = WaitForSingleObject(overlapped.hEvent, INFINITE);
//                switch (dwResult)
//                {
//                case WAIT_OBJECT_0:
//                {
//                    if (!GetOverlappedResult(g_hComm, &overlapped, &dwNofByteTransferred, FALSE))
//                    {
//                        dwReturnCode = 2;
//                        if (overlapped.hEvent != NULL) CloseHandle(overlapped.hEvent);
//                        return dwReturnCode;
//                    }
//                    else
//                    {
//                        // write operation completed successfully
//                        //dwReturnCode = 0;
//                        //if (overlapped.hEvent != NULL) CloseHandle(overlapped.hEvent);
//                        //return dwReturnCode;
//                    }
//                    break;
//                } // eof WAIT_OBJECT_0
//                default:
//                {
//                    // an error occurred in func WaitForSingleObject()
//                    dwReturnCode = 3;
//                    if (overlapped.hEvent != NULL) CloseHandle(overlapped.hEvent);
//                    return dwReturnCode;
//                } // eof default
//                } // eof switch
//            }
//        }
//    }
//
//    if (overlapped.hEvent != NULL) CloseHandle(overlapped.hEvent);
//    return dwReturnCode;
//}
////****************************************************************************
////*                     receive_serial
////****************************************************************************
//DWORD WINAPI receive_serial(LPVOID pcmnd_)
//{
//	// must be a two stage step
//	INT_PTR int_ptr = reinterpret_cast<INT_PTR>(pcmnd_);
//	INT* pcmnd = reinterpret_cast<INT*>(int_ptr);
//
//    DWORD dwEvtMask = EV_RXCHAR;
//    OVERLAPPED overlapped = { 0 };
//    overlapped.hEvent = CreateEvent(NULL
//        , TRUE
//        , FALSE
//        , NULL
//    );
//    if (overlapped.hEvent == NULL) return 1;
//
//    DWORD dwReturnCode = 0;
//    DWORD dwNofByteTransferred = 0;
//    BOOL bResult = FALSE;
//    DWORD dwBytesRead = 0;
//    DWORD64 dwTotalBytesRead = 0;
//    CHAR chBuffer[BUFFER_MAX_SERIAL] = { 0 };
//
//    // infinite loop
//    while (TRUE)
//    {
//        WaitCommEvent(g_hComm, (LPDWORD)&dwEvtMask, &overlapped);
//        WaitForSingleObject(overlapped.hEvent, INFINITE);
//        GetOverlappedResult(g_hComm, &overlapped, &dwNofByteTransferred, FALSE);
//        if (dwEvtMask & EV_RXCHAR)
//        {
//            // create an overlapped structure for reading from file
//            OVERLAPPED overlapped_ = { 0 };
//            overlapped_.Offset = dwTotalBytesRead & 0xFFFFFFFF;
//            overlapped_.OffsetHigh = (DWORD)Int64ShrlMod32(dwTotalBytesRead, 31);
//            overlapped_.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//            // read input buffer into char-buffer
//            bResult = ReadFile(g_hComm, &chBuffer, BUFFER_MAX_SERIAL, &dwBytesRead, &overlapped_);
//            if (bResult)
//            {
//				// switch according to the receive state the appropriate message
//				switch (*pcmnd)
//				{
//				case 33601:
//				{
//					SendMessage(g_hWndDlg
//						, RD_REG_CNFG
//						, (WPARAM)0
//						, (LPARAM)chBuffer
//					);
//                    break;
//				} // eof 33601
//                case 33602:
//                {
//                    SendMessage(g_hWndDlg
//                        , RD_REG_T_LO
//                        , (WPARAM)0
//                        , (LPARAM)chBuffer
//                    );
//                    break;
//                } // eof 33602
//                default:
//					break;
//				} // eof switch
//            }
//            if (overlapped_.hEvent != NULL) CloseHandle(overlapped_.hEvent);
//        }
//    }
//
//    if (overlapped.hEvent != NULL) CloseHandle(overlapped.hEvent);
//    return dwReturnCode;
//}
////    // must be a two stage step
////    INT_PTR int_ptr = reinterpret_cast<INT_PTR>(pcmnd_);
////    INT* pcmnd = reinterpret_cast<INT*>(int_ptr);
////
////    BOOL bWaitingOnRead = FALSE;
////    DWORD dwReturnCode = 0;
////
////    OVERLAPPED overlapped = { 0 };
////    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
////    if (overlapped.hEvent == NULL) dwReturnCode = 1;
////
////    DWORD dwNofByteTransferred = 0;
////    CHAR chBuffer[BUFFER_MAX_SERIAL] = { 0 };
////    if (!bWaitingOnRead)
////    {
////        // issue a read operation
////        if (!ReadFile(g_hComm, &chBuffer, BUFFER_MAX_SERIAL, &dwNofByteTransferred, &overlapped))
////        {
////            // read not delayed?
////            if (GetLastError() != ERROR_IO_PENDING)
////            {
////                // error in communication
////                dwReturnCode = 2;
////            }
////            else
////            {
////                bWaitingOnRead = TRUE;
////            }
////        }
////        else
////        {
////            // read completed immediately
////        }
////    }
////
////    if (overlapped.hEvent != NULL) CloseHandle(overlapped.hEvent);
////
////    // switch according to the receive state the appropriate message
////    switch (*pcmnd)
////    {
////    case 33601:
////    {
////        SendMessage(g_hWndDlg
////            , RD_REG_CNFG
////            , (WPARAM)0
////            , (LPARAM)0
////        );
////        break;
////    }
////    default:
////        break;
////    } // eof switch
////
////    return dwReturnCode;
////}
////****************************************************************************
////*                     RS232ThreadFunc
////****************************************************************************
//DWORD WINAPI RS232ThreadFunc(LPVOID lpParam)
//{
//	HWND hWndDlg = (HWND)lpParam;
//	HWND hWndEditMultiLine = GetDlgItem(hWndDlg, IDC_RCV_MSG_MULTILINE);
//    HWND hWndStatus = GetDlgItem(hWndDlg, IDC_STATUS_CONNECT);
//    HWND hWndEdit = GetDlgItem(hWndDlg, IDC_RCV_MSG);  // edit control for receive
//    DWORD dwEvtMask = EV_RXCHAR;
//	OVERLAPPED overlapped = { 0 };
//	overlapped.hEvent = CreateEvent(NULL
//		, TRUE
//		, FALSE
//		, NULL
//	);
//	if (overlapped.hEvent == NULL) return 1;
//	DWORD dwNofByteTransferred = 0;
//    BOOL bResult = FALSE;
//	DWORD dwBytesRead = 0;
//	DWORD64 dwTotalBytesRead = 0;
//    CHAR chBuffer[BUFFER_MAX_SERIAL] = { 0 };
//    std::string str = "";
//	DCB dcb = { 0 };
//    if (GetCommState(g_hComm, &dcb))
//    {
//		SendMessage(GetDlgItem(hWndDlg, IDC_STATUS_CONNECT)
//			, WM_SETTEXT
//			, (WPARAM)0
//			, (LPARAM)L"Connected"
//		);
//    }
//    else
//    {
//        SendMessage(GetDlgItem(hWndDlg, IDC_STATUS_CONNECT)
//            , WM_SETTEXT
//            , (WPARAM)0
//            , (LPARAM)L"Disconnected"
//        );
//        SendMessage(hWndDlg, WM_COMMAND, (WPARAM)SERIAL_DISCONNECTED, (LPARAM)0);
//        return 1;
//    }
//	// infinite loop
//    while (TRUE)
//    {
//        WaitCommEvent(g_hComm, (LPDWORD)&dwEvtMask, &overlapped);
//        WaitForSingleObject(overlapped.hEvent, INFINITE);
//        GetOverlappedResult(g_hComm, &overlapped, &dwNofByteTransferred, FALSE);
//        if (dwEvtMask & EV_RXCHAR)
//        {
//            // create an overlapped structure for reading from file
//            OVERLAPPED overlapped_ = { 0 };
//            overlapped_.Offset = dwTotalBytesRead & 0xFFFFFFFF;
//            overlapped_.OffsetHigh = (DWORD)Int64ShrlMod32(dwTotalBytesRead, 31);
//            overlapped_.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//            // read input buffer into char-buffer
//            bResult = ReadFile(g_hComm, &chBuffer, BUFFER_MAX_SERIAL, &dwBytesRead, &overlapped_);
//            if (bResult)
//            {
//                if (dwBytesRead < BUFFER_MAX_SERIAL - 1) chBuffer[dwBytesRead] = '\0';
//                str = chBuffer;
//                // append string to edit control in dialog window
//                appendTextToEditControlA(hWndEditMultiLine, str.c_str());
//                // set string to edit control in dialog window
//                setTextToEditControlA(hWndEdit, str);
//                dwTotalBytesRead += dwBytesRead;
//            }
//            if (overlapped_.hEvent != NULL) CloseHandle(overlapped_.hEvent);
//        }
//        if (!GetCommState(g_hComm, &dcb))
//        {
//            SendMessage(hWndStatus
//                , WM_SETTEXT
//                , (WPARAM)0
//                , (LPARAM)L"Disconnected"
//            );
//            SendMessage(hWndDlg, WM_COMMAND, (WPARAM)SERIAL_DISCONNECTED, (LPARAM)0);
//            return 1;
//        }
//    }
//    OutputDebugString(L"RS232ThreadFunc finished\n");
//    if (overlapped.hEvent != NULL) CloseHandle(overlapped.hEvent);
//    SendMessage(hWndDlg, WM_COMMAND, (WPARAM)SERIAL_DISCONNECTED, (LPARAM)0);
//    return 0;
//}
////****************************************************************************
////*                     setTextToEditControlA
////****************************************************************************
//VOID setTextToEditControlA(const HWND hWndEdit, std::string str)
//{
//    std::string str_ = "";
//    for (std::string::iterator iter = str.begin(); iter != str.end(); ++iter)
//    {
//        if (*iter == '\n')
//        {
//            // skip character
//            continue;
//        }
//        if (*iter == '\r')
//        {
//            // only show a completed partial string
//            if (str_.length() == 7)
//            {
//                // send partial string to edit control
//                SendMessageA(hWndEdit
//                    , WM_SETTEXT
//                    , (WPARAM)0
//                    , (LPARAM)str_.c_str()
//                );
//                // continue collecting new partial string
//                str_ = "";
//            }
//        }
//        else
//        {
//            // build partial string
//            str_.append(1, *iter);
//        }
//    }
//}
//////****************************************************************************
//////*                     RS232ThreadFunc4Transmit
//////****************************************************************************
////DWORD WINAPI RS232ThreadFunc4Transmit(LPVOID lpParam)
////{
////    HWND hWndDlg = (HWND)lpParam;
////    HWND hWndEditMultiLine = GetDlgItem(hWndDlg, IDC_RCV_MSG_MULTILINE);
////    HWND hWndStatus = GetDlgItem(hWndDlg, IDC_STATUS_CONNECT);
////    HWND hWndEdit = GetDlgItem(hWndDlg, IDC_TRM_MSG); // edit control for transmit
////    // TODO check for disconnect
////
////    OVERLAPPED overlapped_ = { 0 };
////    overlapped_.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
////    if (overlapped_.hEvent == NULL) return 1;
////    DWORD dwNofByteTransferred = 0;
////    BOOL bResult = FALSE;
////    DWORD dwResult = 0;
////    CHAR chBuffer[BUFFER_MAX_SERIAL] = { 0 };
////    // get content of edit control IDC_TRM_MSG
////    SendMessageA(hWndEdit
////        , WM_GETTEXT
////        , (WPARAM)BUFFER_MAX_SERIAL
////        , (LPARAM)chBuffer
////    );
////    // issue a write
////    bResult = WriteFile(g_hComm, &chBuffer, strlen(chBuffer), &dwNofByteTransferred, &overlapped_);
////    if (bResult)
////    {
////        // func WriteFile() completed immediately
////        OutputDebugString(L"func WriteFile() completed immediately\n");
////    }
////    else
////    {
////        if (GetLastError() != ERROR_IO_PENDING)
////        {
////            // func WriteFile() failed, but isn't delayed
////            OutputDebugString(L"func WriteFile() failed, but isn't delayed\n");
////            return 1;
////        }
////        else
////        {
////            // func WriteFile() is pending
////            dwResult = WaitForSingleObject(overlapped_.hEvent, INFINITE);
////            switch (dwResult)
////            {
////            case WAIT_OBJECT_0:
////            {
////                if (!GetOverlappedResult(g_hComm, &overlapped_, &dwNofByteTransferred, FALSE))
////                {
////                    return 1;
////                }
////                else
////                {
////                    // write operation completed successfully
////                    OutputDebugString(L"write operation completed successfully\n");
////                }
////                break;
////            } // eof WAIT_OBJECT_0 
////            default:
////            {
////                // an error occurred in func WaitForSingleObject()
////                OutputDebugString(L"an error occurred in func WaitForSingleObject()\n");
////                return 1;
////            } // eof default
////            } // eof switch
////        }
////    }
////    return 0;
////}
//// Message handler for about box.
