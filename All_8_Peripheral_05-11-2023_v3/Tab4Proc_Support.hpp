#pragma once

//****************************************************************************
//*                     onWmInitDialog_Tab4Proc
//****************************************************************************
BOOL onWmInitDialog_Tab4Proc(const HWND& hDlg)
{

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
    case BTN_RANGE_SENSOR:
    {
        OutputDebugString(L"BTN_RANGE_SENSOR\n");

        return (INT_PTR)TRUE;
    } // eof BTN_RANGE_SENSOR
    } // eof switch

    return (INT_PTR)FALSE;
}
