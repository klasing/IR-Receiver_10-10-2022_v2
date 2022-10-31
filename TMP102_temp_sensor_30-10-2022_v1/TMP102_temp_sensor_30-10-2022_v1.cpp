// TMP102_temp_sensor_30-10-2022_v1.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TMP102_temp_sensor_30-10-2022_v1.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE g_hInst;                                // current instance
HANDLE g_hComm = { 0 };
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI        RS232ThreadFunc(LPVOID lpParam);
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
            Sleep(1000);
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
	OVERLAPPED overlapped = { 0 };
	overlapped.hEvent = CreateEvent(NULL
		, TRUE
		, FALSE
		, NULL
	);
	overlapped.Internal = 0;
	overlapped.InternalHigh = 0;
	overlapped.Offset = 0;
	overlapped.OffsetHigh = 0;
	BOOL bResult = FALSE;
	CHAR chBuffer[BUFFER_MAX] = { 0 };
    DWORD dwBytesRead = 0;
	DWORD64 dwTotalBytesRead = 0;
    std::string str = "";
    while (TRUE)
    {
        DCB dcb = { 0 };
        if (!GetCommState(g_hComm, &dcb))
        {
            // check if the disconnected state is already indicated in the dialog item
            PWCHAR pszTextDlgItem[13] = { 0 };
            SendMessage(GetDlgItem(hWndDlg, IDC_STATUS_CONNECT)
                , WM_GETTEXT
                , (WPARAM)13
                , (LPARAM)pszTextDlgItem
            );
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
        WaitCommEvent(g_hComm, (LPDWORD)&dwEvtMask, &overlapped);
        if (overlapped.hEvent == 0) break;
		WaitForSingleObject(overlapped.hEvent, INFINITE);
        if (dwEvtMask & EV_RXCHAR)
        {
			// create an overlapped structure for reading from file
			OVERLAPPED overlapped_ = { 0 };
			overlapped_.Offset = dwTotalBytesRead & 0xFFFFFFFF;
			overlapped_.OffsetHigh = Int64ShrlMod32(dwTotalBytesRead, 31);
			overlapped_.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			// read input buffer into char-buffer
			bResult = ReadFile(g_hComm, &chBuffer, BUFFER_MAX, &dwBytesRead, &overlapped_);
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
