#pragma once

//****************************************************************************
//*                     onWmInitDialog_Tab2Proc
//****************************************************************************
BOOL onWmInitDialog_Tab2Proc(const HWND& hDlg)
{

    return EXIT_SUCCESS;
}

//****************************************************************************
//*                     onWmCommand_Tab2Proc
//****************************************************************************
INT_PTR onWmCommand_Tab2Proc(const HWND& hDlg
    , const WPARAM& wParam
)
{
    switch (LOWORD(wParam))
    {
	case BTN_STATE_FAN:
	{
        OutputDebugString(L"BTN_STATE_FAN\n");

		return (INT_PTR)TRUE;
	} // eof BTN_STATE_FAN
    } // eof switch

    return (INT_PTR)FALSE;
}

