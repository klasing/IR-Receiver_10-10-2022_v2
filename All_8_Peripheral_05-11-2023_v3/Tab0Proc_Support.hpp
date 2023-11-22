#pragma once

//****************************************************************************
//*                     global
//****************************************************************************
BOOL g_bContinueTxRx = FALSE;

//****************************************************************************
//*                     onWmInitDialog_Tab0Proc
//****************************************************************************
BOOL onWmInitDialog_Tab0Proc(const HWND& hDlg)
{
    // disable button DISCONNECT_SERIAL
    EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), FALSE);

	// set the focus on the first applicable control, must be a func PostMessage()
	PostMessage(hDlg
		, WM_NEXTDLGCTL
		, (WPARAM)GetDlgItem(hDlg, CONNECT_SERIAL)
		, (LPARAM)TRUE
	);

    return EXIT_SUCCESS;
}
