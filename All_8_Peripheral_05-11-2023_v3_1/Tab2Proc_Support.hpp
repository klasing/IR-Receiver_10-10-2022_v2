#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern Statusbar g_oStatusbar;
extern HWND g_hWndDlgTab2;
extern CHAR g_chTextBuffer[MAX_LEN_TEXT_BUFFER];

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

        // disable button BTN_STATE_FAN, until the transaction
        // has finished
        EnableWindow(GetDlgItem(hDlg, BTN_STATE_FAN), FALSE);
        // clear for a new statusbar message
        g_oStatusbar.setTextStatusbar(3, L"");

        // SOLVED by introducing a new control EDITTEXT IDC_FAN_ON_OFF
        //        where the state will be shown
        // TODO: there will be a confusion when STM32 sets the old state
        //       in the checkbox while the user has altered the state
        // kill timer
        KillTimer(g_hWndDlgTab0, IDT_TIMER);
        g_oFrame.cmd = WR_STATE_FAN;
        g_oFrame.payload[0] = (SendMessage(GetDlgItem(hDlg, IDC_FAN_ON)
            , BM_GETCHECK
            , (WPARAM)0
            , (LPARAM)0)) ?
            1 : 0;
        g_queue.push(g_oFrame);
        // set timer
        SetTimer(g_hWndDlgTab0
            , IDT_TIMER
            , DELAY_TIME_SERIAL
            , (TIMERPROC)TimerProc
        );

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
    // bFanOff is true, results in 0 (type int)
	// bFanOff is false, results in 1 (type int)
    (((UINT8)oFrame.payload[1] >> 7) == 0) ?
        // fan is off
        SendMessage(GetDlgItem(g_hWndDlgTab2, IDC_FAN_ON_OFF)
            , WM_SETTEXT
            , (WPARAM)0
            , (LPARAM)L"OFF"
        )
        :
        // fan is on
        SendMessage(GetDlgItem(g_hWndDlgTab2, IDC_FAN_ON_OFF)
            , WM_SETTEXT
            , (WPARAM)0
            , (LPARAM)L"ON"
        );
    /*
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
    */
    // percentagePWM
    // value lies between 0 .. 99, adjust this value to 1 .. 100
    // add 1 to the value given in oFrame.payload[1]
    sprintf_s(g_chTextBuffer, MAX_LEN_TEXT_BUFFER, "%d", (oFrame.payload[1] & 0b01111111) + 1);
    SendMessageA(GetDlgItem(g_hWndDlgTab2, IDC_PWM_FAN)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );
    // RPM use memcpy!
    FLOAT fRPM;
    memcpy(&fRPM, &oFrame.payload[2], sizeof(FLOAT));
    sprintf_s(g_chTextBuffer, MAX_LEN_TEXT_BUFFER, "%.02f", fRPM);
    SendMessageA(GetDlgItem(g_hWndDlgTab2, IDC_RPM_FAN)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );

    return EXIT_SUCCESS;
}

//****************************************************************************
//*                     enableButtonStateFan
//****************************************************************************
BOOL enableButtonStateFan()
{
    // enable button BTN_STATE_FAN, now the transaction
    // has been finished
    EnableWindow(GetDlgItem(g_hWndDlgTab2, BTN_STATE_FAN), TRUE);
    // set statusbar message
    g_oStatusbar.setTextStatusbar(3, L"state fan is set");

    return EXIT_SUCCESS;
}
