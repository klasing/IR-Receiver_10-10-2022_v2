#pragma once
//*****************************************************************************
//*                     include
//*****************************************************************************
#include "framework.h"

//****************************************************************************
//*                     typedef
//****************************************************************************
typedef struct tagFRAME
{
	const CHAR soh;
	UINT16 cmnd;
	const CHAR stx;
	CHAR payload[LEN_ENTRY_MAX];
	const CHAR etx;
	const CHAR etb;
	const CHAR eot;
} FRAME, * PFRAME;

//****************************************************************************
//*                     global
//****************************************************************************
HANDLE g_hComm = INVALID_HANDLE_VALUE;

// Resource Aquisition Is Initialisation RAII 
FRAME g_oFrame = { SOH, 0, STX, { '\0' }, ETX, ETB, EOT };

BOOL g_bContinueRx = FALSE;
CHAR g_chBuffer[BUFFER_MAX_SERIAL] = { 0 };
UINT32 g_valCrc = 0;
UINT g_cTransmission = 0;
UINT g_cErrorCrc = 0;
CHAR g_chTextBuffer[8] = { 0 };

//*****************************************************************************
//*                     prototype
//*****************************************************************************
BOOL				connect(HANDLE& hComm);
DWORD WINAPI		Rx(LPVOID lpVoid);
BOOL                receive(LPVOID lpVoid);

//*****************************************************************************
//*                     onWmInitDialog_DlgProc
//*****************************************************************************
BOOL onWmInitDialog_DlgProc(const HINSTANCE& hInst
	, const HWND& hDlg
	, HWND& hWndStatusbar
)
{
	// set edit control IDC_STATUS_CONNECT text
	SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT)
		, WM_SETTEXT
		, (WPARAM)0
		, (LPARAM)L"Disconnected"
	);

	// disable button DISCONNECT_SERIAL, RESTART_SERIAL
	EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), FALSE);
	EnableWindow(GetDlgItem(hDlg, RESTART_SERIAL), FALSE);

	hWndStatusbar = DoCreateStatusBar(hInst
		, hDlg
		, IDC_STATUSBAR
		, 1
	);

	return EXIT_SUCCESS;
}

//*****************************************************************************
//*                     onWmSize_DlgProc
//*****************************************************************************
BOOL onWmSize_DlgProc(const HWND& hDlg
	, const HWND& hWndStatusbar
)
{
	// get the client coordinates of the parent window's client area
	RECT rectClient;
	GetClientRect(hDlg, &rectClient);
	// set parts on status bar
	int nParts[1] = { rectClient.right };
	SendMessage(hWndStatusbar, SB_SETPARTS, 1, (LPARAM)&nParts);
	// give the statusbar a theoretical size
	MoveWindow(hWndStatusbar
		, rectClient.left
		, rectClient.top
		, rectClient.right
		, rectClient.bottom
		, TRUE
	);
	ShowWindow(hWndStatusbar
		, SW_SHOWNORMAL
	);

	return EXIT_SUCCESS;
}

//*****************************************************************************
//*                     onWmCommand_DlgProc
//*****************************************************************************
INT_PTR onWmCommand_DlgProc(const HWND& hDlg
	, const WPARAM& wParam
	, HANDLE& hThreadRx
	, const HWND& hWndStatusbar
)
{
	switch (LOWORD(wParam))
	{
	case CONNECT_SERIAL:
	{
		if (connect(g_hComm) == EXIT_SUCCESS)
		{
			SendMessage(hWndStatusbar
				, SB_SETTEXT
				, (WPARAM)0 | SBT_POPOUT // not clear what SBT_POPOUT means
				, (LPARAM)L""
			);
			// set text edit control IDC_STATUS
			SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT)
				, WM_SETTEXT
				, (WPARAM)0
				, (LPARAM)L"Connected"
			);
			// enable/disable button
			EnableWindow(GetDlgItem(hDlg, CONNECT_SERIAL), FALSE);
			EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), TRUE);
			EnableWindow(GetDlgItem(hDlg, RESTART_SERIAL), TRUE);

			// enable infinite loop
			g_bContinueRx = TRUE;

			// create thread to continuously transmit and receive
			DWORD dwThreadIdTxRx = 0;
			hThreadRx = CreateThread(NULL
				, 0
				, Rx
				, (LPVOID)hDlg
				, CREATE_SUSPENDED // wait until started
				, &dwThreadIdTxRx
			);

			// start thread exact on this command
			if (hThreadRx) ResumeThread(hThreadRx);
		}
		else
		{
			// can't connect with STM32, probably the connect cable is loose
			SendMessage(hWndStatusbar
				, SB_SETTEXT
				, (WPARAM)0 | SBT_POPOUT // not clear what SBT_POPOUT means
				, (LPARAM)L"can't connect"
			);
		}
		return (INT_PTR)TRUE;
	} // eof CONNECT_SERIAL
	case DISCONNECT_SERIAL:
	{
		// terminate thread
		g_bContinueRx = FALSE;
		hThreadRx = INVALID_HANDLE_VALUE;

		if (g_hComm == INVALID_HANDLE_VALUE) return (INT_PTR)TRUE;

		if (CloseHandle(g_hComm))
		{
			g_hComm = INVALID_HANDLE_VALUE;

			// set text edit control IDC_STATUS
			SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT)
				, WM_SETTEXT
				, (WPARAM)0
				, (LPARAM)L"Disconnected"
			);

			// enable/disable button
			EnableWindow(GetDlgItem(hDlg, CONNECT_SERIAL), TRUE);
			EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), FALSE);
			EnableWindow(GetDlgItem(hDlg, RESTART_SERIAL), FALSE);
		}
		return (INT_PTR)TRUE;
	} // eof DISCONNECT_SERIAL
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
		, FILE_ATTRIBUTE_NORMAL
		//, FILE_FLAG_OVERLAPPED
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
//*                     Rx
//****************************************************************************
DWORD WINAPI Rx(LPVOID lpVoid)
{
	// infinite loop
	while (g_bContinueRx)
	{
		++g_cTransmission;
		SendMessageA(GetDlgItem((HWND)lpVoid, IDC_NOF_TRANSMISSION)
			, WM_SETTEXT
			, (WPARAM)0
			, (LPARAM)std::to_string(g_cTransmission).c_str()
		);
		receive(lpVoid);
		Sleep(2 * DELAY_4HZ_SERIAL);
	}
	return 0;
}

//****************************************************************************
//*                     receive
//****************************************************************************
BOOL receive(LPVOID lpVoid)
{
	OutputDebugString(L"receiving\n");
	DWORD dwNofByteTransferred = 0;
	ReadFile(g_hComm
		, &g_chBuffer
		, LEN_FRAME + LEN_CRC
		, &dwNofByteTransferred
		, NULL
	);

	// connection is lost
	if (dwNofByteTransferred == 0)
	{
		// disregard and simply return with EXIT_FAILURE
		return EXIT_FAILURE;
		// this gives too much interruption
		//// kill thread
		//g_bContinueTxRx = FALSE;

		//SendMessage((HWND)lpVoid
		//	, SET_TEXT_STATUSBAR
		//	, (WPARAM)0
		//	, (LPARAM)L"connection is lost"
		//);
	}
	else if (dwNofByteTransferred == LEN_FRAME + LEN_CRC)
	{
		// isolate received crc from g_chBuffer
		// the g_buffer char must be cast to UCHAR
		UINT32 rxValCrc = ((UCHAR)g_chBuffer[40] << 24)
			| ((UCHAR)g_chBuffer[41] << 16)
			| ((UCHAR)g_chBuffer[42] << 8)
			| ((UCHAR)g_chBuffer[43]);

		// calculate crc
		calcCrcEx(g_chBuffer, LEN_FRAME, g_valCrc);

		// compare received CRC with calculated CRC
		if (rxValCrc != g_valCrc)
		{
			// crc error
			OutputDebugString(L"CRC ERROR *****************************************************\n");
			++g_cErrorCrc;
			SendMessageA(GetDlgItem((HWND)lpVoid, IDC_NOF_ERROR_CRC)
				, WM_SETTEXT
				, (WPARAM)0
				, (LPARAM)std::to_string(g_cErrorCrc).c_str()
			);
		}
		else
		{
			// set content g_chBuffer into frame
			g_oFrame.cmnd = (g_chBuffer[1] << 24)
				| (g_chBuffer[2] << 16)
				| (g_chBuffer[3] << 8)
				| (g_chBuffer[4]);
			for (UINT8 i = 0; i < LEN_ENTRY_MAX; i++)
			{
				g_oFrame.payload[i] = g_chBuffer[i + 6];
			}
			// set g_oFrame.cmnd in edittext IDC_CODE
			sprintf_s(g_chTextBuffer, 8, "0x%X", g_oFrame.cmnd);
			SendMessageA(GetDlgItem((HWND)lpVoid, IDC_CODE)
				, WM_SETTEXT
				, (WPARAM)0
				, (LPARAM)g_chTextBuffer
			);
			// set g_oFrame.payload in edittext IDC_DESCRIPTION
			SendMessageA(GetDlgItem((HWND)lpVoid, IDC_DESCRIPTION)
				, WM_SETTEXT
				, (WPARAM)0
				, (LPARAM)g_oFrame.payload
			);
		}
	}

	return EXIT_SUCCESS;
}
