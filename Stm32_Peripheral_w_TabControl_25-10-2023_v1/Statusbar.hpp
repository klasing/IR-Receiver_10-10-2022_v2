#pragma once
//*****************************************************************************
//*                     include
//*****************************************************************************
#include "framework.h"

//*****************************************************************************
//*                     Statusbar
//*****************************************************************************
class Statusbar
{
	typedef struct tagPART
	{
		std::wstring wstrTextPart;
	} PART, * PPART;

	// maximum nof part
	static const UINT8 PART_MAX = 8;
	UINT8 nPart = 0;
	PPART pPart[PART_MAX] = { 0 };
	HWND hWndStatusbar = NULL;
public:
	//************************************************************************
	//*                 Statusbar
	//************************************************************************
	Statusbar()
	{
		for (unsigned i = 0; i < PART_MAX; i++)
		{
			pPart[i] = new PART;
			pPart[i]->wstrTextPart = L"";
		}
	}
	//************************************************************************
	//*                 createStatusbar
	//************************************************************************
	BOOL createStatusbar(const HINSTANCE& hInst
		, const HWND& hWnd
		, const UINT8& nPart
	)
	{
		// actual nof part for this application
		this->nPart = nPart;

		// create a statusbar
		hWndStatusbar = CreateWindowEx(0
			, STATUSCLASSNAME
			, (PCTSTR)NULL
			, SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE
			, 0, 0, 0, 0
			, hWnd
			, (HMENU)IDC_STATUSBAR
			, hInst
			, NULL
		);

		return EXIT_SUCCESS;
	}
	//************************************************************************
	//*                 setStatusbar
	//************************************************************************
	BOOL setStatusbar(const HWND& hWnd)
	{
		// get the client coordinates of the parent window's client area
		RECT rectClient;
		GetClientRect(hWnd, &rectClient);

		// allocate an array for holding the right edge coordinates
		HLOCAL hloc = LocalAlloc(LHND, sizeof(int) * nPart);
		PINT paParts = (PINT)LocalLock(hloc);
		// calculate the right edge coordinates for each part,
		int nWidth = rectClient.right / nPart;
		int rightEdge = nWidth;
		// copy the coordinates to the array
		for (int i = 0; i < nPart; i++)
		{
			paParts[i] = rightEdge;
			rightEdge += nWidth;
		}
		// tell the statusbar to create the window parts
		SendMessage(hWndStatusbar
			, SB_SETPARTS
			, (WPARAM)nPart
			, (LPARAM)paParts
		);
		// free the array
		LocalUnlock(hloc);
		LocalFree(hloc);

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
	//************************************************************************
	//*                 setTextStatusbar
	//************************************************************************
	BOOL setTextStatusbar(const UINT8& part
		, const std::wstring& wstr
	)
	{
		SendMessage(hWndStatusbar
			, SB_SETTEXT
			, (WPARAM)part
			, (LPARAM)wstr.c_str()
		);

		return EXIT_SUCCESS;
	}
private:
};
