#pragma once
//*****************************************************************************
//*                     include
//*****************************************************************************
#include "framework.h"

//*****************************************************************************
//*                     DoCreateStatusBar
//*****************************************************************************
HWND DoCreateStatusBar(const HINSTANCE& hinst
    , const HWND& hwndParent
    , const int& idStatus
    , const int& cParts
)
{
    HWND hwndStatus;
    RECT rcClient;
    HLOCAL hloc;
    PINT paParts;
    int i, nWidth;

    // Ensure that the common control DLL is loaded.
    InitCommonControls();

    // Create the status bar.
    hwndStatus = CreateWindowEx(
        0,                      // no extended styles
        STATUSCLASSNAME,        // name of status bar class
        (PCTSTR)NULL,           // no text when first created
        SBARS_SIZEGRIP |        // includes a sizing grip
        WS_CHILD | WS_VISIBLE,  // creates a visible child window
        0, 0, 0, 0,             // ignores size and position
        hwndParent,             // handle to parent window
        (HMENU)idStatus,        // child window identifier
        hinst,                  // handle to application instance
        NULL);                  // no window creation data

    // Get the coordinates of the parent window's client area.
    GetClientRect(hwndParent, &rcClient);

    // Allocate an array for holding the right edge coordinates.
    hloc = LocalAlloc(LHND, sizeof(int) * cParts);
    paParts = (PINT)LocalLock(hloc);

    // Calculate the right edge coordinate for each part, and
    // copy the coordinates to the array.
    nWidth = rcClient.right / cParts;
    int rightEdge = nWidth;
    for (i = 0; i < cParts; i++) {
        paParts[i] = rightEdge;
        rightEdge += nWidth;
    }

    // Tell the status bar to create the window parts.
    SendMessage(hwndStatus, SB_SETPARTS, (WPARAM)cParts, (LPARAM)
        paParts);

    // Free the array, and return.
    LocalUnlock(hloc);
    LocalFree(hloc);

    return hwndStatus;
}