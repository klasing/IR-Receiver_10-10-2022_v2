#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern HWND g_hWndDlgTab4;
extern const FLOAT DEFAULT_T_LO;
extern const FLOAT DEFAULT_T_HI;
extern CHAR g_chTextBuffer[LEN_MAX_TEXT_BUFFER];

//****************************************************************************
//*                     onWmInitDialog_Tab4Proc
//****************************************************************************
BOOL onWmInitDialog_Tab4Proc(const HWND& hDlg)
{
	// set the focus on the first applicable control
	SendMessage(hDlg
		, WM_NEXTDLGCTL
		, (WPARAM)GetDlgItem(hDlg, IDC_TEMP_HI_SENSOR1)
		, (LPARAM)TRUE
	);

	return EXIT_SUCCESS;
}

//****************************************************************************
//*                     onWmCommand_Tab4Proc
//****************************************************************************
INT_PTR onWmCommand_Tab4Proc(const HWND& hDlg
	, const WPARAM& wParam
)
{
	switch (LOWORD(wParam))
	{
	} // eof switch

	return (INT_PTR)FALSE;
}
