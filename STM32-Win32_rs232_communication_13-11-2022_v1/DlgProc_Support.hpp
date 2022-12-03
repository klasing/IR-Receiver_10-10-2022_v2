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

//*****************************************************************************
//*                     prototype
//*****************************************************************************
BOOL				connect(HANDLE& hComm);
DWORD WINAPI		TxRx(LPVOID lpVoid);
BOOL                transmit();
BOOL                receive(LPVOID lpVoid);

//*****************************************************************************
//*                     onWmInitDialog_DlgProc
//*****************************************************************************
BOOL onWmInitDialog_DlgProc(const HWND& hDlg
)
{
	// set edit control IDC_STATUS_CONNECT text
	SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT)
		, WM_SETTEXT
		, (WPARAM)0
		, (LPARAM)L"Disconnected"
	);

	// disable button DISCONNECT_SERIAL
	EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), FALSE);

	return EXIT_SUCCESS;
}

//*****************************************************************************
//*                     onWmCommand_DlgProc
//*****************************************************************************
INT_PTR onWmCommand_DlgProc(const HWND& hDlg
	, const WPARAM& wParam
	, HANDLE& hThreadTxRx
)
{
	switch (LOWORD(wParam))
	{
	case CONNECT_SERIAL:
	{
		if (connect(g_hComm) == EXIT_SUCCESS)
		{
			// set text edit control IDC_STATUS
			SendMessage(GetDlgItem(hDlg, IDC_STATUS_CONNECT)
				, WM_SETTEXT
				, (WPARAM)0
				, (LPARAM)L"Connected"
			);
			// enable/disable button
			EnableWindow(GetDlgItem(hDlg, CONNECT_SERIAL), FALSE);
			EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), TRUE);

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
//*                     TxRx
//****************************************************************************
DWORD WINAPI TxRx(LPVOID lpVoid)
{
	// infinite loop
	while (g_bContinueTxRx)
	{
		transmit();
		Sleep(DELAY_4HZ_SERIAL);
		receive(lpVoid);
		Sleep(DELAY_4HZ_SERIAL);
	}
	return 0;
}

//****************************************************************************
//*                     transmit
//****************************************************************************
BOOL transmit()
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

	// transmit frame
	DWORD dwNofByteTransferred = 0;
	WriteFile(g_hComm
		, &g_chBuffer
		, LEN_FRAME
		, &dwNofByteTransferred
		, NULL
	);

	return EXIT_SUCCESS;
}

//****************************************************************************
//*                     receive
//****************************************************************************
BOOL receive(LPVOID lpVoid)
{
	OutputDebugString(L"receiving\n");

	return EXIT_SUCCESS;
}


