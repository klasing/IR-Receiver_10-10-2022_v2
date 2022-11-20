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

		// after 10 receives the g_uSerialCommand value will change to the next,
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
			Sleep(125);
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

