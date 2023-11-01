#pragma once
//*****************************************************************************
//*                     include
//*****************************************************************************
#include "framework.h"

//*****************************************************************************
//*                     TabControl
//*****************************************************************************
class  TabControl
{
	static const UINT MAX_TAB = 16;
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
		int y = GET_Y_LPARAM(lParam);
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
				SetWindowPos(hWndDlg[i]
					, HWND_TOP
					, 0, 0
					, x
					, y
					, SWP_SHOWWINDOW
				);
			}
			else
			{
				// hide the window(s) belonging to the nonactive tab-item(s)
				SetWindowPos(hWndDlg[i]
					, HWND_TOP
					, 0, 0
					, x
					, y
					, SWP_HIDEWINDOW
				);
			}
		}
	}
private:
};
