#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern FRAME g_oFrameTx;
extern std::queue<tagFRAME> g_queue;
extern HANDLE g_hThreadTxRx;

//****************************************************************************
//*                     onWmInitDialog_Tab3Proc
//****************************************************************************
BOOL onWmInitDialog_Tab3Proc(const HWND& hDlg)
{
	// set the focus on the first applicable control
	SendMessage(hDlg
		, WM_NEXTDLGCTL
		, (WPARAM)GetDlgItem(hDlg, IDC_RELAY1)
		, (LPARAM)TRUE
	);

	return EXIT_SUCCESS;
}

//****************************************************************************
//*                     onWmCommand_Tab3Proc
//****************************************************************************
INT_PTR onWmCommand_Tab3Proc(const HWND& hDlg
	, const WPARAM& wParam
)
{
	static UCHAR chRelayState = 0;

	switch (LOWORD(wParam))
	{
	case BTN_STATE_RELAY:
	{
		(SendMessage(GetDlgItem(hDlg, IDC_RELAY1)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0)) ?
			chRelayState |= 0b00000001 : chRelayState &= 0b11111110;
		(SendMessage(GetDlgItem(hDlg, IDC_RELAY2)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0)) ?
			chRelayState |= 0b00000010 : chRelayState &= 0b11111101;
		(SendMessage(GetDlgItem(hDlg, IDC_RELAY3)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0)) ?
			chRelayState |= 0b00000100 : chRelayState &= 0b11111011;
		(SendMessage(GetDlgItem(hDlg, IDC_RELAY4)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0)) ?
			chRelayState |= 0b00001000 : chRelayState &= 0b11110111;
		(SendMessage(GetDlgItem(hDlg, IDC_RELAY5)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0)) ?
			chRelayState |= 0b00010000 : chRelayState &= 0b11101111;
		(SendMessage(GetDlgItem(hDlg, IDC_RELAY6)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0)) ?
			chRelayState |= 0b00100000 : chRelayState &= 0b11011111;
		(SendMessage(GetDlgItem(hDlg, IDC_RELAY7)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0)) ?
			chRelayState |= 0b01000000 : chRelayState &= 0b10111111;
		(SendMessage(GetDlgItem(hDlg, IDC_RELAY8)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0)) ?
			chRelayState |= 0b10000000 : chRelayState &= 0b01111111;

		g_oFrameTx.cmd = WR_STATE_RELAY;
		g_oFrameTx.payload[0] = chRelayState;
		clear_queue_and_add_one_cmd();

		return (INT_PTR)TRUE;
	} // eof IDC_WR_RELAY_STATE
	} // eof switch

	return (INT_PTR)FALSE;
}
