#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern HWND g_hWndDlgTab2;
extern CHAR g_chTextBuffer[LEN_MAX_TEXT_BUFFER];

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


//****************************************************************************
//*                     setStateFan
//****************************************************************************
BOOL setStateFan(const FRAME& oFrame)
{
    // bFanOff is true, result in 0 (type int)
    // bFanOff is false, results in 1 (type int
    (((UINT8)oFrame.payload[1] >> 7) == 0) ?
        // fan is off
        SendMessage(GetDlgItem(g_hWndDlgTab2, IDC_FAN_ON)
            , BM_SETCHECK
            , (WPARAM)BST_UNCHECKED
            , (LPARAM)0
        )
        :
        // fan is on
        SendMessage(GetDlgItem(g_hWndDlgTab2, IDC_FAN_ON)
            , BM_SETCHECK
            , (WPARAM)BST_CHECKED
            , (LPARAM)0
        );
    // percentagePWM
    // value lies between 0 .. 99, adjust this value to 1 .. 100
    // add 1 to the value given in oFrame.payload[1]
    sprintf_s(g_chTextBuffer, LEN_MAX_TEXT_BUFFER, "%d", (oFrame.payload[1] & 0b01111111) + 1);
    SendMessageA(GetDlgItem(g_hWndDlgTab2, IDC_PWM_FAN)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );
    // RPM use memcpy!
    FLOAT fRPM;
    memcpy(&fRPM, &oFrame.payload[2], sizeof(FLOAT));
    sprintf_s(g_chTextBuffer, LEN_MAX_TEXT_BUFFER, "%.02f", fRPM);
    SendMessageA(GetDlgItem(g_hWndDlgTab2, IDC_RPM_FAN)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );

    return EXIT_SUCCESS;
}
