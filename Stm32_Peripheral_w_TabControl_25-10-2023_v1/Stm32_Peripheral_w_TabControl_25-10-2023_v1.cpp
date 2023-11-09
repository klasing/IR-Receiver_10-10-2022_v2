// Stm32_Peripheral_w_TabControl_25-10-2023_v1.cpp : Defines the entry point for the application.
//

//****************************************************************************
//*                     include
//****************************************************************************
#include "framework.h"
#include "Stm32_Peripheral_w_TabControl_25-10-2023_v1.h"

//****************************************************************************
//*                     define
//****************************************************************************
#define MAX_LOADSTRING 100

//****************************************************************************
//*                     global
//****************************************************************************
// Global Variables:
HINSTANCE g_hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

Statusbar g_oStatusbar;
TabControl g_oTabControl;


HWND g_hWndDlgTab0 = NULL;
HWND g_hWndDlgTab1 = NULL;
HWND g_hWndDlgTab2 = NULL;
HWND g_hWndDlgTab3 = NULL;
HWND g_hWndDlgTab4 = NULL;

//****************************************************************************
//*                     prototype
//****************************************************************************
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Tab0Proc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Tab1Proc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Tab2Proc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Tab3Proc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Tab4Proc(HWND, UINT, WPARAM, LPARAM);
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
    LoadStringW(hInstance, IDC_STM32PERIPHERALWTABCONTROL25102023V1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_STM32PERIPHERALWTABCONTROL25102023V1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (IsDialogMessage(g_hWndDlgTab0, &msg) ||
            IsDialogMessage(g_hWndDlgTab1, &msg) ||
            IsDialogMessage(g_hWndDlgTab2, &msg) ||
            IsDialogMessage(g_hWndDlgTab3, &msg) ||
            IsDialogMessage(g_hWndDlgTab4, &msg))
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_STM32PERIPHERALWTABCONTROL25102023V1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_STM32PERIPHERALWTABCONTROL25102023V1);
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
            , 5
            , Tab0Proc, Tab1Proc, Tab2Proc, Tab3Proc, Tab4Proc
        );
        g_oTabControl.setItem(0, (PWCHAR)L"Serial comm.");
        g_hWndDlgTab0 = g_oTabControl.hWndDlg[0];
        g_oTabControl.setItem(1, (PWCHAR)L"IR remote");
        g_hWndDlgTab1 = g_oTabControl.hWndDlg[1];
        g_oTabControl.setItem(2, (PWCHAR)L"Fan control");
        g_hWndDlgTab2 = g_oTabControl.hWndDlg[2];
        g_oTabControl.setItem(3, (PWCHAR)L"Relay module");
        g_hWndDlgTab3 = g_oTabControl.hWndDlg[3];
        g_oTabControl.setItem(4, (PWCHAR)L"Temp sensor");
        g_hWndDlgTab4 = g_oTabControl.hWndDlg[4];

        return DefWindowProc(hWnd, message, wParam, lParam);
    } // eof WM_NCCREATE
    case WM_SIZE:
    {
        RECT rect;
        GetClientRect(hWnd, &rect);
        // set statusbar
        g_oStatusbar.setStatusbar(hWnd);
        // set intial connect state
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
    }  // eof switch
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
    } // eof switch
    return (INT_PTR)FALSE;
}

//*****************************************************************************
//*                     Tab1Proc
//*
//* IR remote
//*
//*****************************************************************************
INT_PTR CALLBACK Tab1Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return (INT_PTR)FALSE;
}

//*****************************************************************************
//*                     Tab2Proc
//*
//* Fan control
//*
//*****************************************************************************
INT_PTR CALLBACK Tab2Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        onWmInitDialog_Tab2Proc(hDlg);

        return (INT_PTR)FALSE;
    } // eof WM_INITDIALOG
    case WM_COMMAND:
    {
        return onWmCommand_Tab2Proc(hDlg
            , wParam
        );
    } // eof WM_COMMAND
    } // eof switch

    return (INT_PTR)FALSE;
}

//*****************************************************************************
//*                     Tab3Proc
//*
//* Relay module
//*
//*****************************************************************************
INT_PTR CALLBACK Tab3Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        return (INT_PTR)FALSE;
    } // eof WM_INITDIALOG
    case WM_COMMAND:
    {
        return onWmCommand_Tab3Proc(hDlg
            , wParam
        );
    } // eof WM_COMMAND
    } // eof switch

    return (INT_PTR)FALSE;
}

//*****************************************************************************
//*                     Tab4Proc
//*
//* Temp sensor
//*
//*****************************************************************************
INT_PTR CALLBACK Tab4Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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
