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


