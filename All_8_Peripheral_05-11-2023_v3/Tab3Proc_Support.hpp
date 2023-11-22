#pragma once

//****************************************************************************
//*                     onWmInitDialog_Tab3Proc
//****************************************************************************
BOOL onWmInitDialog_Tab3Proc(const HWND& hDlg)
{

    return EXIT_SUCCESS;
}

//****************************************************************************
//*                     onWmCommand_Tab3Proc
//****************************************************************************
INT_PTR onWmCommand_Tab3Proc(const HWND& hDlg
    , const WPARAM& wParam
)
{
    switch (LOWORD(wParam))
    {
    case BTN_STATE_RELAY:
    {
        OutputDebugString(L"BTN_STATE_RELAY\n");

        return (INT_PTR)TRUE;
    } // eof BTN_STATE_RELAY
    } // eof switch

    return (INT_PTR)FALSE;
}

