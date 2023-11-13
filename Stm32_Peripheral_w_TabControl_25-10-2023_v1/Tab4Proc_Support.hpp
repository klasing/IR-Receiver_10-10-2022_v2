#pragma once

//****************************************************************************
//*                     onWmCommand_Tab4Proc
//****************************************************************************
INT_PTR onWmCommand_Tab4Proc(const HWND& hDlg
	, const WPARAM& wParam
)
{
	//switch (LOWORD(wParam))
	//{
	//case IDC_WR_TEMP_RANGE:
	//{

	//	g_oFrame.cmd = WR_TEMP_RANGE;
	//	// 4 (reg) x 2 (value) x 4 (byte per value: 32 bit) = 32
	//	g_lenPayload = 32;

	//	return (INT_PTR)TRUE;
	//} // eof IDC_WR_TEMP_RANGE
	//} // eof switch

	return (INT_PTR)FALSE;
}
/*
#pragma once

//****************************************************************************
//*                     onWmCommand_Tab4Proc
//****************************************************************************
INT_PTR onWmCommand_Tab4Proc(const HWND& hDlg
	, const WPARAM& wParam
)
{
	switch (LOWORD(wParam))
	{
	case IDC_WR_TEMP_RANGE:
	{

		g_oFrame.cmd = WR_TEMP_RANGE;
		// 4 (reg) x 2 (value) x 4 (byte per value: 32 bit) = 32
		g_lenPayload = 32;

		return (INT_PTR)TRUE;
	} // eof IDC_WR_TEMP_RANGE
	} // eof switch

	return (INT_PTR)FALSE;
}
*/