#pragma once
//*****************************************************************************
//*                     include
//*****************************************************************************
#include "framework.h"

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
