#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern HWND g_hWndDlgTab3;
extern CHAR g_chTextBuffer[LEN_MAX_TEXT_BUFFER];

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
    static UCHAR chRelayState = 0;

    switch (LOWORD(wParam))
    {
    case BTN_STATE_RELAY:
    {
        OutputDebugString(L"BTN_STATE_RELAY\n");

        (SendMessage(GetDlgItem(hDlg, IDC_RELAY1)
            , BM_GETCHECK
            , (WPARAM)0
            , (LPARAM)0)) ?
            chRelayState |= 0b00000001 : chRelayState &= 0b11111110;
        (SendMessage(GetDlgItem(hDlg, IDC_RELAY2)
            , BM_GETCHECK
            , (WPARAM)0
            , (LPARAM)0)) ?
            chRelayState |= 0b00000010 : chRelayState &= 0b11111101;
        (SendMessage(GetDlgItem(hDlg, IDC_RELAY3)
            , BM_GETCHECK
            , (WPARAM)0
            , (LPARAM)0)) ?
            chRelayState |= 0b00000100 : chRelayState &= 0b11111011;
        (SendMessage(GetDlgItem(hDlg, IDC_RELAY4)
            , BM_GETCHECK
            , (WPARAM)0
            , (LPARAM)0)) ?
            chRelayState |= 0b00001000 : chRelayState &= 0b11110111;
        (SendMessage(GetDlgItem(hDlg, IDC_RELAY5)
            , BM_GETCHECK
            , (WPARAM)0
            , (LPARAM)0)) ?
            chRelayState |= 0b00010000 : chRelayState &= 0b11101111;
        (SendMessage(GetDlgItem(hDlg, IDC_RELAY6)
            , BM_GETCHECK
            , (WPARAM)0
            , (LPARAM)0)) ?
            chRelayState |= 0b00100000 : chRelayState &= 0b11011111;
        (SendMessage(GetDlgItem(hDlg, IDC_RELAY7)
            , BM_GETCHECK
            , (WPARAM)0
            , (LPARAM)0)) ?
            chRelayState |= 0b01000000 : chRelayState &= 0b10111111;
        (SendMessage(GetDlgItem(hDlg, IDC_RELAY8)
            , BM_GETCHECK
            , (WPARAM)0
            , (LPARAM)0)) ?
            chRelayState |= 0b10000000 : chRelayState &= 0b01111111;

        // kill timer
        KillTimer(g_hWndDlgTab0, IDT_TIMER);
        g_oFrame.cmd = WR_STATE_RELAY;
        g_oFrame.payload[0] = chRelayState;
        g_queue.push(g_oFrame);
        // set timer
        SetTimer(g_hWndDlgTab0
            , IDT_TIMER
            , DELAY_HALFHZ_SERIAL
            , (TIMERPROC)TimerProc
        );

        return (INT_PTR)TRUE;
    } // eof BTN_STATE_RELAY
    } // eof switch

    return (INT_PTR)FALSE;
}

//****************************************************************************
//*                     setStateRelay
//****************************************************************************
BOOL setStateRelay(const FRAME& oFrame)
{
    // IDC_RELAY1 is lsb
    UINT16 aResourceId[8] = { IDC_RELAY1
        , IDC_RELAY2
        , IDC_RELAY3
        , IDC_RELAY4
        , IDC_RELAY5
        , IDC_RELAY6
        , IDC_RELAY7
        , IDC_RELAY8
    };
    BYTE stateRelay = (BYTE)oFrame.payload[1];
    UINT8 mask = 0b00000001;
    for (int i = 0; i < 8; i++)
    {
        ((stateRelay & mask) == mask) ?
            SendMessage(GetDlgItem(g_hWndDlgTab3, aResourceId[i])
                , BM_SETCHECK
                , (WPARAM)BST_CHECKED
                , (LPARAM)0
            )
            :
            SendMessage(GetDlgItem(g_hWndDlgTab3, aResourceId[i])
                , BM_SETCHECK
                , (WPARAM)BST_UNCHECKED
                , (LPARAM)0
            );
        mask <<= 1;
    }

    return EXIT_SUCCESS;
}


