// TMP102_temp_sensor_30-10-2022_v1.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TMP102_temp_sensor_30-10-2022_v1.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE g_hInst;                                // current instance
HANDLE g_hComm;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI RS232ThreadFunc(LPVOID lpParam);
VOID clear_buffer(const DWORD dwBytesRead, CHAR* pchBuffer);
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
       , 440
       , 460
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
        SendMessage(hWndDlg, WM_COMMAND, (WPARAM)START_RECEIVE, (LPARAM)0);
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
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        return (INT_PTR)FALSE;
    }
    case WM_SIZE:
    {
        return (INT_PTR)TRUE;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case START_RECEIVE:
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
            SetCommState(g_hComm, (LPDCB)&dcb);
            // set structure for communication port timeout
            COMMTIMEOUTS commtimeouts;
            commtimeouts.ReadIntervalTimeout = MAXDWORD;
            commtimeouts.ReadTotalTimeoutMultiplier = 0;
            commtimeouts.ReadTotalTimeoutConstant = 0;
            commtimeouts.WriteTotalTimeoutMultiplier = 0;
            commtimeouts.WriteTotalTimeoutConstant = 0;
            // set communication port timeout
            SetCommTimeouts(g_hComm, (LPCOMMTIMEOUTS)&commtimeouts);
            // set communication port mask bit to capture event
            SetCommMask(g_hComm, EV_RXCHAR);
            // create thread and pass a handle of the dialog to the thread func
            LPVOID lpParam = hDlg;
            DWORD dwThreadId;
            HANDLE hThread = CreateThread(NULL
                , 0
                , RS232ThreadFunc
                , lpParam
                , 0
                , &dwThreadId
            );
            return (INT_PTR)TRUE;
        }
        case SERIAL_DISCONNECTED:
        {
            SendMessage(hDlg, WM_COMMAND, (WPARAM)START_RECEIVE, (LPARAM)0);
            return (INT_PTR)TRUE;
        } // eof SERIAL_DISCONNECTED
        } // eof switch
        break;
    }
    } // eof switch
    return (INT_PTR)FALSE;
}
//****************************************************************************
//*                     RS232ThreadFunc
//****************************************************************************
DWORD WINAPI RS232ThreadFunc(LPVOID lpParam)
{
	HWND hWndDlg = (HWND)lpParam;
	HWND hWndEdit = GetDlgItem(hWndDlg, IDC_RCV_MSG);
    DWORD dwEvtMask = EV_RXCHAR;
	OVERLAPPED g_overlapped = { 0 };
	g_overlapped.hEvent = CreateEvent(NULL
		, TRUE
		, FALSE
		, NULL
	);
	g_overlapped.Internal = 0;
	g_overlapped.InternalHigh = 0;
	g_overlapped.Offset = 0;
	g_overlapped.OffsetHigh = 0;
    BOOL bContinue = TRUE;
	BOOL bResult = FALSE;
	CHAR chBuffer[BUFFER_MAX] = { 0 };
    DWORD dwBytesRead = 0;
	DWORD64 dwTotalBytesRead = 0;
    std::string str = "";
    //DWORD modemStatus = 0;
    while (bContinue)
    {
        DCB dcb = { 0 };
        if (!GetCommState(g_hComm, &dcb))
        {
            OutputDebugString(L"serial disconnected\n");
            // check if the disconnected state is already indicated in the dialog item
            PWCHAR pszTextDlgItem[13];
            SendMessage(GetDlgItem(hWndDlg, IDC_STATUS_CONNECT)
                , WM_GETTEXT
                , (WPARAM)13
                , (LPARAM)pszTextDlgItem
            );
#pragma warning(disable : 6054; once)
            if (wcslen((const wchar_t*)pszTextDlgItem) <= 9)
            {

                SendMessage(GetDlgItem(hWndDlg, IDC_STATUS_CONNECT)
                    , WM_SETTEXT
                    , (WPARAM)0
                    , (LPARAM)L"Disconnected"
                );
            }
            SendMessage(hWndDlg, WM_COMMAND, (WPARAM)SERIAL_DISCONNECTED, (LPARAM)0);
            return 1;
        }
        else
        {
            SendMessage(GetDlgItem(hWndDlg, IDC_STATUS_CONNECT)
                , WM_SETTEXT
                , (WPARAM)0
                , (LPARAM)L"Connected"
            );
        }
        WaitCommEvent(g_hComm, (LPDWORD)&dwEvtMask, &g_overlapped);
        if (g_overlapped.hEvent == 0) break;
		WaitForSingleObject(g_overlapped.hEvent, INFINITE);
        if (dwEvtMask & EV_RXCHAR)
        {
            OutputDebugString(L"char received\n");
			// create an overlapped structure for reading one line from file
			OVERLAPPED overlapped = { 0 };
			overlapped.Offset = dwTotalBytesRead & 0xFFFFFFFF;
			overlapped.OffsetHigh = Int64ShrlMod32(dwTotalBytesRead, 31);
			overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			// read input buffer into char-buffer
			bResult = ReadFile(g_hComm, &chBuffer, BUFFER_MAX, &dwBytesRead, &overlapped);
			if (dwBytesRead < BUFFER_MAX - 1)
			{
				chBuffer[dwBytesRead] = '\0';
				str = chBuffer;
				// append string to edit control in dialog window
				appendTextToEditControlA(hWndEdit, str.c_str());
			}
			dwTotalBytesRead += dwBytesRead;
        }
   }
    return 0;
}
//    HWND hWndDlg = (HWND)lpParam;
//    HWND hWndEdit = GetDlgItem(hWndDlg, IDC_RCV_MSG);
//    DWORD dwEvtMask = EV_RXCHAR | EV_PERR | EV_ERR | EV_RLSD;
//    BOOL bResult = FALSE;
//    CHAR chBuffer[BUFFER_MAX] = { 0 };
//    DWORD dwBytesRead = 0;
//    DWORD64 dwTotalBytesRead = 0;
//    std::string str = "";
//    // create overlapped structure for WaitCommEvent func
//    OVERLAPPED g_overlapped = { 0 };
//    g_overlapped.hEvent = CreateEvent(NULL
//        , TRUE
//        , FALSE
//        , NULL
//    );
//    g_overlapped.Internal = 0;
//    g_overlapped.InternalHigh = 0;
//    g_overlapped.Offset = 0;
//    g_overlapped.OffsetHigh = 0;
//    BOOL bContinue = TRUE;
//    while (bContinue)
//    {
//		// wait for a transmission from the micro controller
//		while (WaitCommEvent(g_hComm, (LPDWORD)&dwEvtMask, &g_overlapped))//NULL))//))
//		{
//			// clear buffer
//			clear_buffer(dwBytesRead, chBuffer);
//			// a character has been received
//			if (dwEvtMask & EV_RXCHAR)
//			{
//				// create an overlapped structure for reading one line from file
//				OVERLAPPED overlapped = { 0 };
//				overlapped.Offset = dwTotalBytesRead & 0xFFFFFFFF;
//				overlapped.OffsetHigh = Int64ShrlMod32(dwTotalBytesRead, 31);
//				overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//				// read input buffer into char-buffer
//				bResult = ReadFile(g_hComm, &chBuffer, BUFFER_MAX, &dwBytesRead, &overlapped);
//				if (dwBytesRead < BUFFER_MAX - 1)
//				{
//					chBuffer[dwBytesRead] = '\0';
//					str = chBuffer;
//					// append string to edit control in dialog window
//					appendTextToEditControlA(hWndEdit, str.c_str());
//				}
//				dwTotalBytesRead += dwBytesRead;
//			}
//			else if (dwEvtMask & EV_PERR)
//			{
//				str = "parity error\r\n";
//				// append string to edit control in dialog window
//				appendTextToEditControlA(hWndEdit, str.c_str());
//			}
//			else if (dwEvtMask & EV_ERR)
//			{
//				str = "line status error: CE_FRAME | CE_OVERRUN | CE_RXPARITY\r\n";
//				// append string to edit control in dialog window
//				appendTextToEditControlA(hWndEdit, str.c_str());
//			}
//			else if (dwEvtMask & EV_RLSD)
//			{
//				OutputDebugString(L"bla\n");
//			}
//			else
//			{
//				DWORD dwRet = GetLastError();
//				if (dwRet == ERROR_IO_PENDING)
//				{
//					str = "I/O is pending\r\n";
//					// append string to edit control in dialog window
//					appendTextToEditControlA(hWndEdit, str.c_str());
//				}
//				else
//				{
//					str = "Wait failed with error: " + std::to_string(GetLastError());
//					// append string to edit control in dialog window
//					appendTextToEditControlA(hWndEdit, str.c_str());
//					return 1;
//				}
//			}
//		}
//		std::string strstr = "Wait failed with error: " + std::to_string(GetLastError()) + "\n";
//		OutputDebugStringA(strstr.c_str());
//		ResetEvent(g_overlapped.hEvent);
//	}
//    return 0;
//}
VOID clear_buffer(const DWORD dwBytesRead, CHAR* pchBuffer)
{
    if (dwBytesRead == 0)
    {
        return;
    }
    for (uint8_t i = 0; i < BUFFER_MAX; i++)
    {
        pchBuffer[i] = '\0';
    }
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
