#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern HWND g_hWndDlgTab1;
extern CHAR g_chTextBuffer[LEN_MAX_TEXT_BUFFER];

//****************************************************************************
//*                     onWmInitDialog_Tab1Proc
//****************************************************************************
BOOL onWmInitDialog_Tab1Proc(const HWND& hDlg)
{

    return EXIT_SUCCESS;
}

//****************************************************************************
//*                     onWmCommand_Tab1Proc
//****************************************************************************
INT_PTR onWmCommand_Tab1Proc(const HWND& hDlg
    , const WPARAM& wParam
)
{
    /*
    switch (LOWORD(wParam))
    {
    } // eof switch
    */

    return (INT_PTR)FALSE;
}

//****************************************************************************
//*                     setIrRemote
//****************************************************************************
BOOL setIrRemote(const FRAME& oFrame)
{
    // code
    sprintf_s(g_chTextBuffer
        , LEN_MAX_TEXT_BUFFER
        , "0x%X"
        , (oFrame.cmd & 0xF00) | (oFrame.cmd & 0x0FF)
    );
    SendMessageA(GetDlgItem(g_hWndDlgTab1, IDC_CODE)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );
    // description
    SendMessageA(GetDlgItem(g_hWndDlgTab1, IDC_DESCRIPTION)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)&oFrame.payload[1]
    );

    return EXIT_SUCCESS;
}


