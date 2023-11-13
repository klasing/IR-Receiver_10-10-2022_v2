#pragma once

//****************************************************************************
//*                     onWmCommand_Tab3Proc
//****************************************************************************
INT_PTR onWmCommand_Tab3Proc(const HWND& hDlg
	, const WPARAM& wParam
)
{
	//static UCHAR chRelayState = 0;

	//switch (LOWORD(wParam))
	//{
	//case IDC_WR_RELAY_STATE:
	//{
	//	if (SendMessage(GetDlgItem(hDlg, IDC_RELAY1)
	//		, BM_GETCHECK
	//		, (WPARAM)0
	//		, (LPARAM)0
	//	)) chRelayState |= 0b00000001; else chRelayState &= 0b11111110;
	//	if (SendMessage(GetDlgItem(hDlg, IDC_RELAY2)
	//		, BM_GETCHECK
	//		, (WPARAM)0
	//		, (LPARAM)0
	//	)) chRelayState |= 0b00000010; else chRelayState &= 0b11111101;
	//	if (SendMessage(GetDlgItem(hDlg, IDC_RELAY3)
	//		, BM_GETCHECK
	//		, (WPARAM)0
	//		, (LPARAM)0
	//	)) chRelayState |= 0b00000100; else chRelayState &= 0b11111011;
	//	if (SendMessage(GetDlgItem(hDlg, IDC_RELAY4)
	//		, BM_GETCHECK
	//		, (WPARAM)0
	//		, (LPARAM)0
	//	)) chRelayState |= 0b00001000; else chRelayState &= 0b11110111;
	//	if (SendMessage(GetDlgItem(hDlg, IDC_RELAY5)
	//		, BM_GETCHECK
	//		, (WPARAM)0
	//		, (LPARAM)0
	//	)) chRelayState |= 0b00010000; else chRelayState &= 0b11101111;
	//	if (SendMessage(GetDlgItem(hDlg, IDC_RELAY6)
	//		, BM_GETCHECK
	//		, (WPARAM)0
	//		, (LPARAM)0
	//	)) chRelayState |= 0b00100000; else chRelayState &= 0b11011111;
	//	if (SendMessage(GetDlgItem(hDlg, IDC_RELAY7)
	//		, BM_GETCHECK
	//		, (WPARAM)0
	//		, (LPARAM)0
	//	)) chRelayState |= 0b01000000; else chRelayState &= 0b10111111;
	//	if (SendMessage(GetDlgItem(hDlg, IDC_RELAY8)
	//		, BM_GETCHECK
	//		, (WPARAM)0
	//		, (LPARAM)0
	//	)) chRelayState |= 0b10000000; else chRelayState &= 0b01111111;

	//	g_oFrame.cmd = WR_RELAY_STATE;
	//	g_oFrame.payload[0] = 1;
	//	g_oFrame.payload[1] = chRelayState;

	//	return (INT_PTR)TRUE;
	//} // eof IDC_WR_RELAY_STATE
	//} // eof switch

	return (INT_PTR)FALSE;
}
/*
#pragma once
//****************************************************************************
//*                     extern
//****************************************************************************
extern FRAME g_oFrame;

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
	case IDC_WR_RELAY_STATE:
	{
		if (SendMessage(GetDlgItem(hDlg, IDC_RELAY1)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0
		)) chRelayState |= 0b00000001; else chRelayState &= 0b11111110;
		if (SendMessage(GetDlgItem(hDlg, IDC_RELAY2)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0
		)) chRelayState |= 0b00000010; else chRelayState &= 0b11111101;
		if (SendMessage(GetDlgItem(hDlg, IDC_RELAY3)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0
		)) chRelayState |= 0b00000100; else chRelayState &= 0b11111011;
		if (SendMessage(GetDlgItem(hDlg, IDC_RELAY4)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0
		)) chRelayState |= 0b00001000; else chRelayState &= 0b11110111;
		if (SendMessage(GetDlgItem(hDlg, IDC_RELAY5)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0
		)) chRelayState |= 0b00010000; else chRelayState &= 0b11101111;
		if (SendMessage(GetDlgItem(hDlg, IDC_RELAY6)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0
		)) chRelayState |= 0b00100000; else chRelayState &= 0b11011111;
		if (SendMessage(GetDlgItem(hDlg, IDC_RELAY7)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0
		)) chRelayState |= 0b01000000; else chRelayState &= 0b10111111;
		if (SendMessage(GetDlgItem(hDlg, IDC_RELAY8)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0
		)) chRelayState |= 0b10000000; else chRelayState &= 0b01111111;

		g_oFrame.cmd = WR_RELAY_STATE;
		g_oFrame.payload[0] = 1;
		g_oFrame.payload[1] = chRelayState;

		return (INT_PTR)TRUE;
	} // eof IDC_WR_RELAY_STATE
	} // eof switch

	return (INT_PTR)FALSE;
}
*/