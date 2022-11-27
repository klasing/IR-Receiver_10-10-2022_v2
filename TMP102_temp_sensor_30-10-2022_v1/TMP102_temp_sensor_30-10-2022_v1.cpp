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
//*
//* reset the TMP102, second byte is:
//* 0000 0110
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

/*
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
*/

//****************************************************************************
//*                     global
//****************************************************************************
// Global Variables:
HINSTANCE g_hInst;								// current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND g_hDlg = { 0 };
/*
HANDLE g_hComm = INVALID_HANDLE_VALUE;
// Resource Aquisition Is Initialisation RAII 
FRAME g_oFrame = { SOH, 0, STX, 0, ETX, ETB, EOT }; 
BOOL g_bContinueTxRx = FALSE;
CHAR g_chBuffer[BUFFER_MAX_SERIAL] = { 0 };
UINT8 g_alert_bit = 0;
*/

//****************************************************************************
//*                     prototype
//****************************************************************************
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/*
BOOL				connect();
DWORD WINAPI        TxRx(LPVOID lpVoid);
BOOL                transmit();
BOOL                receive(LPVOID lpVoid);
*/

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
		onWmInitDialog_DlgProc(g_hInst
			, hDlg
			, hWndLV_Cnfg
			, hWndLV_T_lo
			, hWndLV_T_hi
			, hWndLV_Treg
			, rect
			, brush
			, bkColorDlgBrush
		);

		return (INT_PTR)FALSE;
	} // eof WM_INITDIALOG
	case WM_SIZE:
	{
		onWmSize_DlgProc(hWndLV_Cnfg
			, hWndLV_T_lo
			, hWndLV_T_hi
			, hWndLV_Treg
		);

		return (INT_PTR)TRUE;
	} // eof WM_SIZE
	case WM_COMMAND:
	{
		onWmCommand_DlgProc(hDlg
			, wParam
			, rect
			, brush
			, bkColorDlgBrush
			, redBrush
			, greenBrush
		);
	} // eof WM_COMMAND
	case WM_PAINT:
	{
		onWmPaint_DlgProc(hDlg, brush, rect);

		return (INT_PTR)FALSE;
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

