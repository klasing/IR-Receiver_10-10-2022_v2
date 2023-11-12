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
		g_oFrame.payload[0] = 0;

		return (INT_PTR)TRUE;
	} // eof IDC_WR_TEMP_RANGE
	} // eof switch

	return (INT_PTR)FALSE;
}

