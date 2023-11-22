#pragma once

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

