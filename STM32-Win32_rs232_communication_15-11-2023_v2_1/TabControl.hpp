#pragma once
//*****************************************************************************
//*                     include
//*****************************************************************************
#include "framework.h"

//****************************************************************************
//*                     extern
//****************************************************************************
extern BOOL g_bContinueTxRx;

//*****************************************************************************
//*                     TabControl
//*****************************************************************************
class  TabControl
{
	static const UINT MAX_TAB = 16;
	static const DWORD OFFSET_TABHDR = 25;
	static const DWORD HEIGHT_STATUSBAR = 20;
	HWND hWndTab = NULL;
	UINT nofDlg = 0;	// can't be greater than the constant MAX_TAB
	UINT iPage = 0;		// holds the index of the window belonging to the active tab-item
public:
	HWND hWndDlg[MAX_TAB] = { 0 };
	//************************************************************************
	//*                  TabControl
	//************************************************************************
	TabControl()
	{
	}
	//************************************************************************
	//*                 createTabControl
	//************************************************************************
	BOOL createTabControl(const HINSTANCE& hInst
        , const HWND& hWnd
        , const UINT& resourceId
        , const int nofProc
        , ...
    )
	{
		// create tab control
		hWndTab = CreateWindowEx(0
			, WC_TABCONTROL
			, L""
			, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE
			, 0, 0, 0, 0
			, hWnd
			, (HMENU)resourceId
			, hInst
			, NULL
		);
		// set the font of the tabs to a more typical system GUI font
		SendMessage(hWndTab
			, WM_SETFONT
			, (WPARAM)GetStockObject(DEFAULT_GUI_FONT)
			, (LPARAM)0
		);
		TCITEM tie{ 0 };
		tie.mask = TCIF_TEXT;
		va_list args;
		va_start(args, nofProc);
		for (int i = 0; i < nofProc; i++)
		{
			// add tab
			tie.pszText = (LPWSTR)L"";
			TabCtrl_InsertItem(hWndTab, i, &tie);
			std::wstring wstrTabWindow = L"TAB[n]PROCWINDOW";
			wstrTabWindow.replace(wstrTabWindow.find(L"[n]"), 3, std::to_wstring(i));
			// create dialog for tab-item
			hWndDlg[i] = CreateDialog(hInst
				, wstrTabWindow.c_str()
				, hWnd
				, va_arg(args, const DLGPROC)
			);
		}
		va_end(args);
		// when this variable changes the static const named: cTabItem
		// in the module: StatusBar.hpp must be updated accordingly
		nofDlg = sizeof(hWndDlg) / sizeof(hWndDlg[0]);
		return EXIT_SUCCESS;
	}
	//****************************************************************************
	//*                     setItem
	//****************************************************************************
	BOOL setItem(const UINT& idx
		, const PWCHAR& pszText
	)
	{
		TCITEM tci{ 0 };
		tci.mask = LVIF_TEXT;
		tci.pszText = pszText;
		SendMessage(hWndTab
			, TCM_SETITEM
			, (WPARAM)idx
			, (LPARAM)&tci
		);
		return EXIT_SUCCESS;
	}
	//************************************************************************
	//*                    showTabItems
	//*
	//* lParam: parent window size
	//************************************************************************
	VOID showTabItems(const LPARAM& lParam
	)
	{
		OutputDebugString(L"showTabItems() TabControl.cpp -Proj: Common_Control -Sln: StaticLib\n");
		// on the rights side of he window is a irritating black vertical line
		// add two to the x-value to get rid of this line
		int x = GET_X_LPARAM(lParam) + 2;
		int y = GET_Y_LPARAM(lParam) - OFFSET_TABHDR - HEIGHT_STATUSBAR;
		// show the tab items
		SetWindowPos(hWndTab
			, HWND_TOP
			, 0, 0
			, x
			, y
			, SWP_SHOWWINDOW
		);
		for (unsigned i = 0; i < nofDlg; i++)
		{
			if (i == iPage)
			{
				// show the window belonging to the active tab-item
				// the y-value (OFFSET_TABHDR) is essential, it prevents hiding the tab control text
				SetWindowPos(hWndDlg[i]
					, HWND_TOP
					, 0, OFFSET_TABHDR
					, x
					, y
					, SWP_SHOWWINDOW
				);
			}
			else
			{
				// hide the window(s) belonging to the nonactive tab-item(s)
				// the y-value (OFFSET_TABHDR) is essential, it prevents hiding the tab control text
				SetWindowPos(hWndDlg[i]
					, HWND_TOP
					, 0, OFFSET_TABHDR
					, x
					, y
					, SWP_HIDEWINDOW
				);
			}
		}
	}
	//************************************************************************
	//*                 onWmNotify
	//*
	//* lParam: pointer to NMHDR
	//*
	//************************************************************************
	INT_PTR onWmNotify(const LPARAM& lParam)
	{
		LPNMHDR lpNmHdr = (LPNMHDR)lParam;
		switch (lpNmHdr->code)
		{
		case TCN_SELCHANGING:
		{
			// return FALSE to allow the selection to change
			return FALSE;
		} // eof TCN_SELCHANGING
		case TCN_SELCHANGE:
		{
			// respond to a tab item change
			iPage = TabCtrl_GetCurSel(hWndTab);
			for (unsigned i = 0; i < nofDlg; i++)
			{
				if (i == iPage)
				{
					// show the window belonging to the active tab item
					ShowWindow(hWndDlg[i], SW_SHOW);
					// set the focus on the window belonging to the active tab item
					switch (i)
					{
					case 0:
						if (g_bContinueTxRx)
							// set the focus on the first applicable control, must be a func PostMessage()
							PostMessage(hWndDlg[i]
								, WM_NEXTDLGCTL
								, (WPARAM)GetDlgItem(hWndDlg[i], DISCONNECT_SERIAL)
								, (LPARAM)TRUE
							);
						else
							// set the focus on the first applicable control, must be a func PostMessage()
							PostMessage(hWndDlg[i]
								, WM_NEXTDLGCTL
								, (WPARAM)GetDlgItem(hWndDlg[i], CONNECT_SERIAL)
								, (LPARAM)TRUE
							);
						break;
					/*
					case 1:
						break;
					case 2:
						// set the focus on the first applicable control, must be a func PostMessage()
						PostMessage(hWndDlg[i]
							, WM_NEXTDLGCTL
							, (WPARAM)GetDlgItem(hWndDlg[i], IDC_FAN_ON)
							, (LPARAM)TRUE
						);
						break;
					case 3:
						// set the focus on the first applicable control, must be a func PostMessage()
						PostMessage(hWndDlg[i]
							, WM_NEXTDLGCTL
							, (WPARAM)GetDlgItem(hWndDlg[i], IDC_RELAY1)
							, (LPARAM)TRUE
						);
						break;
					case 4:
						// set the focus on the first applicable control, must be a func PostMessage()
						PostMessage(hWndDlg[i]
							, WM_NEXTDLGCTL
							, (WPARAM)GetDlgItem(hWndDlg[i], IDC_TEMP_HI_SENSOR1)
							, (LPARAM)TRUE
						);
						break;
					*/
					} // eof switch
				}
				else
				{
					// hide the window(s) belonging to the nonactive tab-item(s)
					ShowWindow(hWndDlg[i], SW_HIDE);
				}
			}
			// when switching between pages on a tab control
			// a control may loose its focus
			// by hitting the tab-key the focus will be restored
			break;
		} // eof TCN_SELCHANGE
		case NM_CLICK:
		{
			break;
		} // eof NM_CLICK
		} // eof switch
		// the return value is ignored
		return (INT_PTR)TRUE;
	}
private:
};
