// STM32-Win32_rs232_communication_15-11-2023_v2_1.cpp : Defines the entry point for the application.
//

//****************************************************************************
//*                     include
//****************************************************************************
#include "framework.h"
#include "STM32-Win32_rs232_communication_15-11-2023_v2_1.h"

//****************************************************************************
//*                     define
//****************************************************************************
#define MAX_LOADSTRING 100

//****************************************************************************
//*                     global
//****************************************************************************
// Global Variables:
HINSTANCE g_hInst;                              // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

Statusbar g_oStatusbar;
TabControl g_oTabControl;

HWND g_hWndDlgTab0 = NULL;

// when receiving a message WM_SIZE in the WndProc
// the connected state must be known to correctly
// restore the statusbar
BOOL g_bConnected = FALSE;

//****************************************************************************
//*                     prototype
//****************************************************************************
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Tab0Proc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
    // load common controls: tab, listview, header, toolbar, statusbar,
    // tooltip, and Up/Down control
    INITCOMMONCONTROLSEX ic = { 0 };
    ic.dwSize = sizeof(INITCOMMONCONTROLSEX);
    ic.dwICC = ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES | ICC_UPDOWN_CLASS;
    BOOL bSuccessInit = InitCommonControlsEx(&ic);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_STM32WIN32RS232COMMUNICATION15112023V21, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_STM32WIN32RS232COMMUNICATION15112023V21));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (IsDialogMessage(g_hWndDlgTab0, &msg))
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

//****************************************************************************
//*                     MyRegisterClass
//****************************************************************************
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_STM32WIN32RS232COMMUNICATION15112023V21));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_STM32WIN32RS232COMMUNICATION15112023V21);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
       , 10//CW_USEDEFAULT
       , 10//0
       , 800//CW_USEDEFAULT
       , 600//0
       , nullptr, nullptr, hInstance, nullptr
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
        // create statusbar
        g_oStatusbar.createStatusbar(g_hInst
            , hWnd
            , 4
        );
        // create tab control
        g_oTabControl.createTabControl(g_hInst
            , hWnd
            , IDC_TABCONTROL
            , 1
            , Tab0Proc
        );
        g_oTabControl.setItem(0, (PWCHAR)L"Serial comm.");
        g_hWndDlgTab0 = g_oTabControl.hWndDlg[0];

        return DefWindowProc(hWnd, message, wParam, lParam);
    } // eof WM_NCCREATE
    case WM_SIZE:
    {
        RECT rect;
        GetClientRect(hWnd, &rect);
        // set statusbar
        g_oStatusbar.setStatusbar(hWnd);
        // set intial connect state, 
        // the state must be kept during a Minimize/Resize operation
        (g_bConnected) ?
            g_oStatusbar.setTextStatusbar(0, L"STM32 connected")
            :
            g_oStatusbar.setTextStatusbar(0, L"STM32 disconnected");
        // set teb control
        g_oTabControl.showTabItems(lParam);

        return (INT_PTR)TRUE;
    } // eof WM_SIZE
    case WM_NOTIFY:
    {
        if (((LPNMHDR)lParam)->idFrom == IDC_TABCONTROL)
        {
            return g_oTabControl.onWmNotify(lParam);
        }
        return (INT_PTR)FALSE;
    } // eof WM_NOTIFY
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

//*****************************************************************************
//*                     Tab0Proc
//*
//* Serial comm.
//*
//*****************************************************************************
INT_PTR CALLBACK Tab0Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        onWmInitDialog_Tab0Proc(hDlg);

        return (INT_PTR)FALSE;
    } // eof WM_INITDIALOG
    case WM_COMMAND:
    {
        return onWmCommand_Tab0Proc(hDlg
            , wParam
        );
    } // eof WM_COMMAND
    case WM_POWERBROADCAST:
    {
        onWmPowerBroadcast(hDlg
            , wParam
            , lParam
        );

        return (INT_PTR)TRUE;
    } // eof WM_POWERBROADCAST
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
