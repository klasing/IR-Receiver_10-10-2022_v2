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
	CHAR payload[LEN_MAX_ENTRY];
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
UCHAR g_chBuffer[BUFFER_MAX_SERIAL] = { 0 };
UINT32 g_valCrc = 0;
UINT g_cTransmission = 0;
UINT g_cErrorCrc = 0;
CHAR g_chTextBuffer[8] = { 0 };

std::vector<CHAR> v0 = { '0', ' ' };
std::vector<CHAR> v1 = { '1', ';', '@', '?', '!' };
std::vector<CHAR> v2 = { '2', 'a', 'b', 'c' };
std::vector<CHAR> v3 = { '3', 'd', 'e', 'f' };
std::vector<CHAR> v4 = { '4', '6', 'g', 'h', 'i', 'm', 'n', 'o' }; // 4 = 6
std::vector<CHAR> v5 = { '5', 'j', 'k', 'l' };
std::vector<CHAR> v6 = { '4', '6', 'g', 'h', 'i', 'm', 'n', 'o' }; // 6 = 4
std::vector<CHAR> v7 = { '7', 'p', 'q', 'r', 's' };
std::vector<CHAR> v8 = { '8', 't', 'u', 'v' };
std::vector<CHAR> v9 = { '9', 'w', 'x', 'y', 'z' };
std::pair<UINT16, std::vector<CHAR>> p0 = { 0xFAB, v0 };
std::pair<UINT16, std::vector<CHAR>> p1 = { 0xFFE, v1 };
std::pair<UINT16, std::vector<CHAR>> p2 = { 0xFFA, v2 };
std::pair<UINT16, std::vector<CHAR>> p3 = { 0xFFB, v3 };
std::pair<UINT16, std::vector<CHAR>> p4 = { 0xFEE, v4 };
std::pair<UINT16, std::vector<CHAR>> p5 = { 0xFEB, v5 };
std::pair<UINT16, std::vector<CHAR>> p6 = { 0xFEE, v6 };
std::pair<UINT16, std::vector<CHAR>> p7 = { 0xFEF, v7 };
std::pair<UINT16, std::vector<CHAR>> p8 = { 0xFBE, v8 };
std::pair<UINT16, std::vector<CHAR>> p9 = { 0xFBB, v9 };
std::vector<std::pair<UINT16, std::vector<CHAR>>> g_vKey =
{ p0, p1, p2, p3, p4, p5, p6, p7, p8, p9 };

//BOOL g_bChoose = FALSE;
//std::pair<UINT16, std::vector<CHAR>> g_p = { 0, { 0 } };
//UINT8 g_nvKey = 0, g_cvKey = 0;
UINT16 g_lastCommand = 0;
UINT8 g_nvKey = 0;
BOOL g_bRunningTimer = FALSE;

//*****************************************************************************
//*                     prototype
//*****************************************************************************
BOOL				connect(HANDLE& hComm);
DWORD WINAPI		Rx(LPVOID lpVoid);
BOOL                receive(LPVOID lpVoid);
BOOL				processCommand(LPVOID lpVoid);
BOOL				evaluate(CHAR& ch);
VOID				Timerproc(HWND, UINT, UINT_PTR, DWORD);
BOOL				evaluate(LPVOID lpVoid, CHAR& ch);
//BOOL				evaluate(UINT8& nvKey, CHAR& ch);
BOOL				appendTextToEditControlA(const HWND& hWndEditControl
						, const std::string str);
BOOL				eraseLastCharA(const HWND& hWndEditControl);

//*****************************************************************************
//*                     onWmInitDialog_DlgProc
//*****************************************************************************
BOOL onWmInitDialog_DlgProc(const HINSTANCE& hInst
	, const HWND& hDlg
	, HWND& hWndStatusbar
	//, BOOL& bChoose
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

	//// create a timer
	//SetTimer(hDlg
	//	, IDT_TIMER
	//	, 2000
	//	, (TIMERPROC)NULL
	//);
	//bChoose = TRUE;
	//SendMessage(GetDlgItem(hDlg, IDC_CHB_CHOOSE)
	//	, BM_SETCHECK
	//	, (WPARAM)BST_CHECKED
	//	, (LPARAM)0
	//);

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
//*                     onWmTimer_DlgProc
//****************************************************************************
BOOL onWmTimer_DlgProc(const HWND& hDlg)
{
	OutputDebugString(L"timer triggered\n");
	//g_bChoose = FALSE;
	SendMessage(GetDlgItem(hDlg, IDC_CHB_CHOOSE)
		, BM_SETCHECK
		, (WPARAM)BST_UNCHECKED
		, (LPARAM)0
	);
	KillTimer(hDlg, IDT_TIMER);

	return EXIT_SUCCESS;
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
		UINT32 rxValCrc = (g_chBuffer[38] << 24)
			| (g_chBuffer[39] << 16)
			| (g_chBuffer[40] << 8)
			| (g_chBuffer[41]);

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
			// transfer command from g_chBuffer to g_oFrame.cmnd
			g_oFrame.cmnd = (g_chBuffer[1] & 0x0F) << 8
				| g_chBuffer[2];
			// transfer payload from g_chBuffer to g_oFrame.payload
			for (UINT8 i = 0; i < LEN_MAX_ENTRY; i++)
			{
				g_oFrame.payload[i] = g_chBuffer[i + 4];
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
			processCommand(lpVoid);
		}
	}

	return EXIT_SUCCESS;
}

//****************************************************************************
//*                     processCommand
//****************************************************************************
BOOL processCommand(LPVOID lpVoid)
{
	CHAR ch = '\0';
	if (evaluate(lpVoid, ch) == EXIT_SUCCESS)
	{
		// the character is a numeric/alphanumeric character, so place it
		// in the edit control IDC_EDT
		std::string str = "";
		str.push_back(ch);
		appendTextToEditControlA(GetDlgItem((HWND)lpVoid, IDC_EDT)
			, str
		);
		g_lastCommand = g_oFrame.cmnd;
		// start a timer
		SetTimer((HWND)lpVoid
			, IDT_TIMER
			, 2000
			, (TIMERPROC)Timerproc
		);
		g_bRunningTimer = TRUE;
		// check checkbox control IDC_CHB_CHOOSE
		SendMessage(GetDlgItem((HWND)lpVoid, IDC_CHB_CHOOSE)
			, BM_SETCHECK
			, (WPARAM)BST_CHECKED
			, (LPARAM)0
		);
	}
	return EXIT_SUCCESS;
}

//****************************************************************************
//*                     evaluate
//****************************************************************************
BOOL evaluate(LPVOID lpVoid, CHAR& ch)
{
	std::pair<UINT16, std::vector<CHAR>> p = { 0, { 0 } };
	auto ite = g_vKey.begin();
	for (; ite != g_vKey.end(); ++ite)
	{
		p = *ite;
		if (g_oFrame.cmnd == std::get<UINT16>(p))
		{
			break;
		}
	}
	if (ite == g_vKey.end())
	{
		// key not found
		return EXIT_FAILURE;
	}
	// key found
	std::vector<CHAR> v = std::get<std::vector<CHAR>>(p);
	if (g_oFrame.cmnd == g_lastCommand)
	{
		if (g_bRunningTimer)
		{
			eraseLastCharA(GetDlgItem((HWND)lpVoid, IDC_EDT));
			g_nvKey = ((++g_nvKey) % v.size());
			// assign character
			ch = v.at(g_nvKey);
		}
		else
		{
			g_nvKey = 0;
			ch = v.at(g_nvKey);
		}
	}
	else
	{
		g_nvKey = 0;
		ch = v.at(g_nvKey);
	}
	return EXIT_SUCCESS;
}

//	CHAR ch = '\0';
//	if (evaluate(ch) == EXIT_SUCCESS)
//	{
//		if (g_lastCommand == 0)
//		{
//			// the character is a numeric/alphanumeric character, so place it
//			// in the edit control IDC_EDT
//			std::string str = "";
//			str.push_back(ch);
//			appendTextToEditControlA(GetDlgItem((HWND)lpVoid, IDC_EDT)
//				, str
//			);
//			// start a timer
//			SetTimer((HWND)lpVoid
//				, IDT_TIMER
//				, 2000
//				, (TIMERPROC)Timerproc
//			);
//			g_bRunningTimer = TRUE;
//			// check checkbox control IDC_CHB_CHOOSE
//			SendMessage(GetDlgItem((HWND)lpVoid, IDC_CHB_CHOOSE)
//				, BM_SETCHECK
//				, (WPARAM)BST_CHECKED
//				, (LPARAM)0
//			);
//			g_lastCommand = g_oFrame.cmnd;
//		}
//		else
//		{
//
//		}
//		//if (g_oFrame.cmnd == g_lastCommand)
//		//{
//		//	if (g_bRunningTimer)
//		//	{
//		//		// increase the index, with a circular buffer style,
//		//		// to point to a following character option
//		//		g_nvKey = ((g_nvKey + 1) % g_cvKey);
//		//		// get the following character option
//		//		CHAR ch = std::get<std::vector<CHAR>>(g_p).at(g_nvKey);
//		//		// replace the last character in the edit control IDC_EDT
//		//		// with the following character option
//		//		str.clear();
//		//		str.push_back(0x08);
//		//		str.push_back(ch);
//		//		appendTextToEditControlA(GetDlgItem((HWND)lpVoid, IDC_EDT)
//		//			, str
//		//		);
//		//		// hold the last received character
//		//						
//		//		// re-time the timer by coalescing the timer
//		//		SetCoalescableTimer((HWND)lpVoid
//		//			, IDT_TIMER
//		//			, 2000
//		//			, (TIMERPROC)Timerproc
//		//			, 0
//		//		);
//		//	}
//		//	else
//		//	{
//		//		// place a following character in the edit control IDC_EDT
//
//		//		// reset g_lastCommand to zero
//		//	}
//		//}
//	}
//
//	return EXIT_SUCCESS;
//}

//****************************************************************************
//*                     Timerproc
//****************************************************************************
VOID Timerproc(HWND hDlg
	, UINT uint			// 275??
	, UINT_PTR uint_ptr	// matches IDT_TIMER 33200
	, DWORD dw			// variable value??
)
{
	OutputDebugString(L"Timerproc\n");
	// kill the timer
	KillTimer(hDlg, uint_ptr);
	g_bRunningTimer = FALSE;
	// uncheck checkbox control IDC_CHB_CHOOSE
	SendMessage(GetDlgItem(hDlg, IDC_CHB_CHOOSE)
		, BM_SETCHECK
		, (WPARAM)BST_UNCHECKED
		, (LPARAM)0
	);
}

//****************************************************************************
//*                     evaluate
//****************************************************************************
//BOOL evaluate(CHAR& ch)
//{
//	//std::pair<UINT16, std::vector<CHAR>> p = { 0, { 0 } };
//	auto ite = g_vKey.begin();
//	for (; ite != g_vKey.end(); ++ite)
//	{
//		g_p = *ite;
//		if (g_oFrame.cmnd == std::get<UINT16>(g_p))
//		{
//			break;
//		}
//	}
//	if (ite == g_vKey.end())
//	{
//		// key not found
//		return EXIT_FAILURE;
//	}
//	// key found
//	std::vector<CHAR> v = std::get<std::vector<CHAR>>(g_p);
//	// get size vKey
//	g_cvKey = v.size();
//	// assign character
//	ch = v.at(0);
//	return EXIT_SUCCESS;
//}

//****************************************************************************
//*                     evaluate
//****************************************************************************
//BOOL evaluate(UINT8& nvKey, CHAR& ch)
//{
//	std::pair<UINT16, std::vector<CHAR>> p = { 0, { 0 } };
//	auto ite = g_vKey.begin();
//	for (; ite != g_vKey.end(); ++ite)
//	{
//		p = *ite;
//		//std::pair<UINT16, std::vector<CHAR>> p = *ite;
//		if (g_oFrame.cmnd == std::get<UINT16>(p))
//		{
//			break;
//		}
//	}
//	if (ite == g_vKey.end())
//	{
//		// key not found
//		return EXIT_FAILURE;
//	}
//	// key found, assign character
//	std::vector<CHAR> v = std::get<std::vector<CHAR>>(p);
//	nvKey = v.size();
//	ch = v.at(g_idxvKey);
//	return EXIT_SUCCESS;
//}

//****************************************************************************
//*                     appendTextToEditControlA
//****************************************************************************
BOOL appendTextToEditControlA(const HWND& hWndEditControl
	, const std::string str
)
{
	if (hWndEditControl == NULL) return EXIT_FAILURE;
	INT i = GetWindowTextLength(hWndEditControl);
	// set the selection at the end of the present text
	SendMessage(hWndEditControl
		, EM_SETSEL
		, (WPARAM)i
		, (LPARAM)i
	);
	// replace the selection with the to be appended text
	SendMessageA(hWndEditControl
		, EM_REPLACESEL
		, (WPARAM)0
		, (LPARAM)str.c_str()
	);

	return EXIT_SUCCESS;
}

//****************************************************************************
//*                     eraseLastCharA
//****************************************************************************
BOOL eraseLastCharA(const HWND & hWndEditControl)
{
	if (hWndEditControl == NULL) return EXIT_FAILURE;
	INT i = GetWindowTextLength(hWndEditControl);
	if (i == 0) return EXIT_FAILURE;
	// select the last char in the present text
	SendMessageA(hWndEditControl
		, EM_SETSEL
		, (WPARAM)i - 1
		, (LPARAM)i
	);
	// replace the selection with an empty string, i.d. erasing the last char 
	SendMessageA(hWndEditControl
		, EM_REPLACESEL
		, (WPARAM)0
		, (LPARAM)""
	);

	return EXIT_SUCCESS;

}


			// evaluate g_oFrame.cmnd for choosing numeric/alphanumeric character
			//UINT8 nvKey = 0;
			//CHAR ch;
			//if (evaluate(nvKey, ch) == EXIT_SUCCESS)
			//{
			//	// the character is a numeric/alphanumeric character, so place it
			//	// in the edit control IDC_EDT
			//	std::string str = "";
			//	str.push_back(ch);
			//	appendTextToEditControlA(GetDlgItem((HWND)lpVoid, IDC_EDT)
			//		, str
			//	);
			//	// create a timer
			//	SetTimer((HWND)lpVoid
			//		, IDT_TIMER
			//		, 2000
			//		, (TIMERPROC)NULL
			//	);
			//	g_bChoose = TRUE;
			//	SendMessage(GetDlgItem((HWND)lpVoid, IDC_CHB_CHOOSE)
			//		, BM_SETCHECK
			//		, (WPARAM)BST_CHECKED
			//		, (LPARAM)0
			//	);
			//}

