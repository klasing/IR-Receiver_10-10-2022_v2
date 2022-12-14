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
	INT16 payload;
	const CHAR etx;
	const CHAR etb;
	const CHAR eot;
} FRAME, * PFRAME;

//****************************************************************************
//*                     global
//****************************************************************************
HANDLE g_hComm = INVALID_HANDLE_VALUE;

// Resource Aquisition Is Initialisation RAII 
FRAME g_oFrame = { SOH, 0, STX, 0, ETX, ETB, EOT };

BOOL g_bContinueTxRx = FALSE;
CHAR g_chBuffer[BUFFER_MAX_SERIAL] = { 0 };
std::queue<tagFRAME> g_queue;
UINT32 g_valCrc = 0;
UINT g_cTransmission = 0;
UINT g_cErrorCrc = 0;
CHAR g_chTextBuffer[8] = { 0 };

//*****************************************************************************
//*                     prototype
//*****************************************************************************
BOOL				connect(HANDLE& hComm);
DWORD WINAPI		TxRx(LPVOID lpVoid);
BOOL                transmit(LPVOID lpVoid);
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
	, HANDLE& hThreadTxRx
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

			
			// calculate crc from 9 byte (2 x 32 bit + 8 bit = 72 bit)
			calcCrcEx(g_chBuffer, 9, g_valCrc);

			// place a frame into the queue
			g_queue.push(g_oFrame);

			// enable infinite loop
			g_bContinueTxRx = TRUE;

			// create thread to continuously transmit and receive
			DWORD dwThreadIdTxRx = 0;
			hThreadTxRx = CreateThread(NULL
				, 0
				, TxRx
				, (LPVOID)hDlg
				, CREATE_SUSPENDED // wait until started
				, &dwThreadIdTxRx
			);

			// start thread exact on this command
			if (hThreadTxRx) ResumeThread(hThreadTxRx);
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
		g_bContinueTxRx = FALSE;
		hThreadTxRx = INVALID_HANDLE_VALUE;

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
	case RESTART_SERIAL:
	{
		// reset g_cTransmission
		g_cTransmission = 0;
		// clear edittext IDC_NOF_TRANSMISSION
		SendMessageA(GetDlgItem(hDlg, IDC_NOF_TRANSMISSION)
			, WM_SETTEXT
			, (WPARAM)0
			, (LPARAM)""
		);
		// clear edittext IDC_NOF_ERROR_CRC
		SendMessageA(GetDlgItem(hDlg, IDC_NOF_ERROR_CRC)
			, WM_SETTEXT
			, (WPARAM)0
			, (LPARAM)""
		);
		// send message DISCONNECT_SERIAL
		SendMessage(hDlg
			, WM_COMMAND
			, (WPARAM)DISCONNECT_SERIAL
			, (LPARAM)0
		);
		// hold for two second
		Sleep(2000);
		// send message CONNECT_SERIAL
		SendMessage(hDlg
			, WM_COMMAND
			, (WPARAM)CONNECT_SERIAL
			, (LPARAM)0
		);
		return (INT_PTR)TRUE;
	} // eof RESTART_SERIAL
	default:
	{
		return (INT_PTR)FALSE;
	}
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
//*                     TxRx
//****************************************************************************
DWORD WINAPI TxRx(LPVOID lpVoid)
{
	// infinite loop
	while (g_bContinueTxRx)
	{
		++g_cTransmission;
		SendMessageA(GetDlgItem((HWND)lpVoid, IDC_NOF_TRANSMISSION)
			, WM_SETTEXT
			, (WPARAM)0
			, (LPARAM)std::to_string(g_cTransmission).c_str()
		);
		transmit(lpVoid);
		Sleep(DELAY_4HZ_SERIAL);
		receive(lpVoid);
		Sleep(DELAY_4HZ_SERIAL);
	}
	return 0;
}

//****************************************************************************
//*                     transmit
//****************************************************************************
BOOL transmit(LPVOID lpVoid)
{
	OutputDebugString(L"transmitting\n");

	FRAME oFrameEx = g_queue.front();

	sprintf_s((CHAR*)g_chBuffer, (size_t)BUFFER_MAX_SERIAL, "%c%c%c%c%c%c%c%c%c"
		, oFrameEx.soh
		, (oFrameEx.cmnd >> 8) & 0xFF
		, (oFrameEx.cmnd & 0xFF)
		, oFrameEx.stx
		, (oFrameEx.payload >> 8) & 0xFF
		, (oFrameEx.payload & 0xFF)
		, oFrameEx.etx
		, oFrameEx.etb
		, oFrameEx.eot
	);

	// calculate crc and feed into chBuffer
	calcCrcEx(g_chBuffer, 9, g_valCrc);
	g_chBuffer[9] = (g_valCrc & 0xFF000000) >> 24;
	g_chBuffer[10] = (g_valCrc & 0x00FF0000) >> 16;
	g_chBuffer[11] = (g_valCrc & 0x0000FF00) >> 8;
	g_chBuffer[12] = (g_valCrc & 0x000000FF);
	g_chBuffer[13] = '\0';


	// transmit frame
	DWORD dwNofByteTransferred = 0;
	WriteFile(g_hComm
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

	return EXIT_SUCCESS;
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

	// isolate received crc from g_chBuffer
	// the g_buffer char must be cast to UCHAR
	UINT32 rxValCrc = ((UCHAR)g_chBuffer[9] << 24)
		| ((UCHAR)g_chBuffer[10] << 16)
		| ((UCHAR)g_chBuffer[11] << 8)
		| ((UCHAR)g_chBuffer[12]);
	
	// calculate crc
	calcCrcEx(g_chBuffer, 9, g_valCrc);

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

	return EXIT_SUCCESS;
}


