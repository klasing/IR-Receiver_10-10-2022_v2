// STM32-Win32_rs232_communication_13-11-2022_v1.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "STM32-Win32_rs232_communication_13-11-2022_v1.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE g_hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HANDLE g_hComm = INVALID_HANDLE_VALUE;
BOOL g_bTransmit = TRUE;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI        transmit(LPVOID lpVoid);
DWORD WINAPI        receive(LPVOID lpVoid);
BOOL                connect();

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
    LoadStringW(hInstance, IDC_STM32WIN32RS232COMMUNICATION13112022V1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_STM32WIN32RS232COMMUNICATION13112022V1));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_STM32WIN32RS232COMMUNICATION13112022V1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_STM32WIN32RS232COMMUNICATION13112022V1);
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW
	   , 15
	   , 15
	   , 300
	   , 200
	   , nullptr, nullptr, hInstance, nullptr);

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
    static HWND hWndDlg = NULL;
    switch (message)
    {
    case WM_NCCREATE:
    {
        // create dialog
        hWndDlg = CreateDialog(g_hInst, L"DLGPROCWINDOW", hWnd, DlgProc);
        return DefWindowProc(hWnd, message, wParam, lParam);
    } // eof WM_NCCREATE
    case WM_SIZE:
    {
        RECT rect;
        GetClientRect(hWnd, &rect);
        // sset size dialog and show dialog
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
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        // set text edit control IDC_STATUS
        SendMessage(GetDlgItem(hDlg, IDC_STATUS)
            , WM_SETTEXT
            , (WPARAM)0
            , (LPARAM)L"Disconnected"
        );
        return (INT_PTR)FALSE;
    } // eof WM_INITDIALOG
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_CONNECT:
        {
            if (connect())
            {
                // set text edit control IDC_STATUS
                SendMessage(GetDlgItem(hDlg, IDC_STATUS)
                    , WM_SETTEXT
                    , (WPARAM)0
                    , (LPARAM)L"Connected"
                );
                // enable/disable button
                EnableWindow(GetDlgItem(hDlg, IDC_CONNECT), FALSE);
                EnableWindow(GetDlgItem(hDlg, IDC_DISCONNECT), TRUE);

                // create thread to transmit
                DWORD dwThreadIdTransmit = 0;
                HANDLE hTreadTransmit = CreateThread(NULL
                    , 0
                    , transmit
                    , (LPVOID)hDlg
                    , 0
                    , &dwThreadIdTransmit
                );

                // create thread to receive
                DWORD dwThreadIdReceive = 0;
                HANDLE hTreadReceive = CreateThread(NULL
                    , 0
                    , receive
                    , (LPVOID)hDlg
                    , 0
                    , &dwThreadIdReceive
                );
            }
            return (INT_PTR)TRUE;
        } // eof IDC_CONNECT
        case IDC_DISCONNECT:
        {
            if (g_hComm == INVALID_HANDLE_VALUE) return (INT_PTR)TRUE;
            if (CloseHandle(g_hComm))
            {
                g_hComm = INVALID_HANDLE_VALUE;

                // set text edit control IDC_STATUS
                SendMessage(GetDlgItem(hDlg, IDC_STATUS)
                    , WM_SETTEXT
                    , (WPARAM)0
                    , (LPARAM)L"Disconnected"
                );

                // enable/disable button
                EnableWindow(GetDlgItem(hDlg, IDC_CONNECT), TRUE);
                EnableWindow(GetDlgItem(hDlg, IDC_DISCONNECT), FALSE);
            }
            return (INT_PTR)TRUE;
        } // eof IDC_DISCONNECT
        } // eof switch
        return (INT_PTR)FALSE;
    }  // eof WM_COMMAND
    } // eof switch
    return (INT_PTR)FALSE;
}

//****************************************************************************
//*                     transmit
//****************************************************************************
DWORD WINAPI transmit(LPVOID lpVoid)
{
    CHAR chBuffer[BUFFER_MAX_SERIAL] = { 0 };
    sprintf_s(chBuffer, BUFFER_MAX_SERIAL, "33603");
    DWORD dwNofByteTransferred = 0;
    BOOL bResult = FALSE;
    // infinite loop
    while (TRUE)
    {
        if (g_bTransmit)
        {
            bResult = WriteFile(g_hComm
                , &chBuffer
                , 5
                , &dwNofByteTransferred
                , NULL
            );
            OutputDebugStringA(chBuffer);
            OutputDebugString(L" ");
            Sleep(125);
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
    CHAR chBuffer[BUFFER_MAX_SERIAL] = { 0 };
    DWORD dwNofByteTransferred = 0;
    BOOL bResult = FALSE;
    // infinite loop
    while (TRUE)
    {
        if (!g_bTransmit)
        {
            bResult = ReadFile(g_hComm
                , &chBuffer
                , 2//strlen(chBuffer)
                , &dwNofByteTransferred
                , NULL
            );
            OutputDebugStringA(chBuffer);
            OutputDebugString(L"\n");
            Sleep(125);
            g_bTransmit = TRUE;
        }
    }
    return 0;
}

//****************************************************************************
//*                     connect
//****************************************************************************
BOOL connect()
{
    // create file
    g_hComm = CreateFile(L"\\\\.\\COM3"
        , GENERIC_READ | GENERIC_WRITE
        , 0
        , NULL
        , OPEN_EXISTING
        , FILE_ATTRIBUTE_NORMAL // synchronous I/O
        , NULL
    );
    if (g_hComm == INVALID_HANDLE_VALUE)
    {
        return FALSE;
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
    if (!SetCommState(g_hComm, (LPDCB)&dcb))
    {
        return FALSE;
    }

    // set structure for the communication port timeout
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

    // set the communication port mask bit to capture event
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
