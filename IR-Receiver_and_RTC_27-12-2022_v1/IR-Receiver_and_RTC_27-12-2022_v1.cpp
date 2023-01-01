// IR-Receiver_and_RTC_27-12-2022_v1.cpp : Defines the entry point for the application.
//

//****************************************************************************
//*                     include
//****************************************************************************
#include "framework.h"
#include "IR-Receiver_and_RTC_27-12-2022_v1.h"

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

HWND g_hDlg = { 0 };

//****************************************************************************
//*                     prototype
//****************************************************************************
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
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
    // and tooltip
    INITCOMMONCONTROLSEX ic = { 0 };
    ic.dwSize = sizeof(INITCOMMONCONTROLSEX);
    ic.dwICC = ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES;
    BOOL bSuccessInit = InitCommonControlsEx(&ic);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_IRRECEIVERANDRTC27122022V1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IRRECEIVERANDRTC27122022V1));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IRRECEIVERANDRTC27122022V1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_IRRECEIVERANDRTC27122022V1);
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW
       , 15
       , 15
       , 370
       , 310
       , nullptr, nullptr, hInstance, nullptr);

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
    switch (uMsg)
    {
    case WM_COMMAND:
    {

        onWmCommand_DlgProc(hDlg
            , wParam
        );
        // this break is vital, otherwise a WM_COMMAND falls
        // through into the underlying message handler!
        break;
    } // eof WM_COMMAND
    } // eof switch
    
    return (INT_PTR)FALSE;
}
/*
// waste /////////////////////////////////////////////////////////////////////
//****************************************************************************
//*                     DlgProc
//****************************************************************************
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        onWmInitDialog_DlgProc(g_hInst
            , hDlg
        );

        return (INT_PTR)FALSE;
    } // eof WM_INITDIALOG
    case WM_SIZE:
    {
        onWmSize_DlgProc(hDlg
        );
        return (INT_PTR)TRUE;
    } // eof WM_SIZE
    case WM_COMMAND:
    {

        onWmCommand_DlgProc(hDlg
            , wParam
        );
        // this break is vital, otherwise a WM_COMMAND falls
        // through into the underlying message handler!
        break;
    } // eof WM_COMMAND
    } // eof switch

    return (INT_PTR)FALSE;
}
*/

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
