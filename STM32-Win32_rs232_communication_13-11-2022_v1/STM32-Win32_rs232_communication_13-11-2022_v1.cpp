// STM32-Win32_rs232_communication_13-11-2022_v1.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "STM32-Win32_rs232_communication_13-11-2022_v1.h"

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
} FRAME, *PFRAME;

// Global Variables:
HINSTANCE g_hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND g_hDlg = { 0 };
//HANDLE g_hComm = INVALID_HANDLE_VALUE;
//BOOL g_bTransmit = TRUE;
volatile BOOL g_bContinueTxRx = TRUE;
volatile UINT16 g_iCommand = RD_REG_0;
volatile BOOL g_bRead = TRUE;
volatile FRAME g_oFrame = { SOH, 0, STX, 0, ETX, ETB, EOT }; // RAII
volatile CHAR g_chBuffer[BUFFER_MAX_SERIAL] = { 0 };

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL                connect(HANDLE& hComm);
DWORD WINAPI        TxRx(LPVOID lpVoid);
BOOL                transmit();
BOOL                receive(LPVOID lpVoid);
//DWORD WINAPI        transmit(LPVOID lpVoid);
//DWORD WINAPI        receive(LPVOID lpVoid);

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
	   , 370
	   , 300
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
    switch (message)
    {
    case WM_NCCREATE:
    {
        // create dialog
        g_hDlg = CreateDialog(g_hInst, L"DLGPROCWINDOW", hWnd, DlgProc);
        return DefWindowProc(hWnd, message, wParam, lParam);
    } // eof WM_NCCREATE
    case WM_SIZE:
    {
        RECT rect;
        GetClientRect(hWnd, &rect);
        // sset size dialog and show dialog
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
    static HANDLE hComm = INVALID_HANDLE_VALUE;
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        // set text edit control IDC_STATUS
        SendMessage(GetDlgItem(hDlg, IDC_STATUS_SERIAL)
            , WM_SETTEXT
            , (WPARAM)0
            , (LPARAM)L"Disconnected"
        );
        // populate combobox IDC_CB_REGISTER
        SendMessage(GetDlgItem(hDlg, IDC_CB_REGISTER)
            , CB_ADDSTRING
            , (WPARAM)0
            , (LPARAM)L"0"
        );
        SendMessage(GetDlgItem(hDlg, IDC_CB_REGISTER)
            , CB_ADDSTRING
            , (WPARAM)0
            , (LPARAM)L"1"
        );
        SendMessage(GetDlgItem(hDlg, IDC_CB_REGISTER)
            , CB_ADDSTRING
            , (WPARAM)0
            , (LPARAM)L"2"
        );
        // set the default value for the IDC_CB_REGISTER
        SendMessage(GetDlgItem(hDlg, IDC_CB_REGISTER)
            , CB_SETCURSEL
            , (WPARAM)0
            , (LPARAM)0
        );
        // check IDC_RB_READ
        CheckRadioButton(hDlg
            , IDC_RB_WRITE
            , IDC_RB_READ
            , IDC_RB_READ
        );
        // disable edit control IDC_TRANSMIT
        EnableWindow(GetDlgItem(hDlg, IDC_TRANSMIT), FALSE);
        return (INT_PTR)FALSE;
    } // eof WM_INITDIALOG
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_CONNECT:
        {
            //if (connect(hComm) == EXIT_SUCCESS)
            //{
                g_bContinueTxRx = TRUE;

                // set text edit control IDC_STATUS
                SendMessage(GetDlgItem(hDlg, IDC_STATUS_SERIAL)
                    , WM_SETTEXT
                    , (WPARAM)0
                    , (LPARAM)L"Connected"
                );
                // enable/disable button
                EnableWindow(GetDlgItem(hDlg, IDC_CONNECT), FALSE);
                EnableWindow(GetDlgItem(hDlg, IDC_DISCONNECT), TRUE);

                // set g_oFrame into the default state
                g_oFrame.cmnd = 33600;

				// create/start thread to continuously transmit and receive
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
                //// create thread to transmit
                //DWORD dwThreadIdTransmit = 0;
                //HANDLE hTreadTransmit = CreateThread(NULL
                //    , 0
                //    , transmit
                //    , (LPVOID)hDlg
                //    , 0
                //    , &dwThreadIdTransmit
                //);

                //// create thread to receive
                //DWORD dwThreadIdReceive = 0;
                //HANDLE hTreadReceive = CreateThread(NULL
                //    , 0
                //    , receive
                //    , (LPVOID)hDlg
                //    , 0
                //    , &dwThreadIdReceive
                //);
            //}
            return (INT_PTR)TRUE;
        } // eof IDC_CONNECT
        case IDC_RB_WRITE:
        {
            g_bRead = FALSE;
            // enable edit control IDC_TRANSMIT
            EnableWindow(GetDlgItem(hDlg, IDC_TRANSMIT), TRUE);
            return (INT_PTR)TRUE;
        } // IDC_RB_WRITE;
        case IDC_RB_READ:
        {
            g_bRead = TRUE;
            // disable edit control IDC_TRANSMIT
            EnableWindow(GetDlgItem(hDlg, IDC_TRANSMIT), FALSE);
            return (INT_PTR)TRUE;
        } // IDC_RB_READ;
        case IDC_DISCONNECT:
        {
            //if (hComm == INVALID_HANDLE_VALUE) return (INT_PTR)TRUE;
            if (CloseHandle(hComm))
            {
                g_bContinueTxRx = FALSE;

                hComm = INVALID_HANDLE_VALUE;

                // set text edit control IDC_STATUS
                SendMessage(GetDlgItem(hDlg, IDC_STATUS_SERIAL)
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
        , FILE_ATTRIBUTE_NORMAL // synchronous I/O
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
        Sleep(DELAY_DFLT_SERIAL);
        receive(lpVoid);
        Sleep(DELAY_DFLT_SERIAL);
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
    return EXIT_SUCCESS;
}

//****************************************************************************
//*                     receive
//****************************************************************************
BOOL receive(LPVOID lpVoid)
{
    OutputDebugString(L"receiving\n");
    g_oFrame.cmnd = (g_chBuffer[1] << 8) | (g_chBuffer[2]);
    g_oFrame.payload  = (g_chBuffer[4] << 8) | (g_chBuffer[5]);
    std::string str = std::to_string(g_oFrame.payload);
    SendMessageA(GetDlgItem((HWND)lpVoid, IDC_RECEIVE)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)str.c_str()
    );
    return EXIT_SUCCESS;
}
//****************************************************************************
//*                     transmit
//****************************************************************************
//DWORD WINAPI transmit(LPVOID lpVoid)
//{
//    CHAR chBuffer[BUFFER_MAX_SERIAL] = { 0 };
//    sprintf_s(chBuffer, BUFFER_MAX_SERIAL, "33603");
//    DWORD dwNofByteTransferred = 0;
//    BOOL bResult = FALSE;
//    // infinite loop
//    while (TRUE)
//    {
//        if (g_bTransmit)
//        {
//            bResult = WriteFile(g_hComm
//                , &chBuffer
//                , 5
//                , &dwNofByteTransferred
//                , NULL
//            );
//            OutputDebugStringA(chBuffer);
//            OutputDebugString(L" ");
//            Sleep(125);
//            g_bTransmit = FALSE;
//        }
//    }
//    return 0;
//}

//****************************************************************************
//*                     receive
//****************************************************************************
//DWORD WINAPI receive(LPVOID lpVoid)
//{
//    CHAR chBuffer[BUFFER_MAX_SERIAL] = { 0 };
//    DWORD dwNofByteTransferred = 0;
//    BOOL bResult = FALSE;
//    // infinite loop
//    while (TRUE)
//    {
//        if (!g_bTransmit)
//        {
//            bResult = ReadFile(g_hComm
//                , &chBuffer
//                , 2//strlen(chBuffer)
//                , &dwNofByteTransferred
//                , NULL
//            );
//            OutputDebugStringA(chBuffer);
//            OutputDebugString(L"\n");
//            Sleep(125);
//            g_bTransmit = TRUE;
//        }
//    }
//    return 0;
//}

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
