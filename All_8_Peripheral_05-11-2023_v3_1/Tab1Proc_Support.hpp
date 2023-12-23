#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern HWND g_hWndDlgTab1;
extern CHAR g_chTextBuffer[MAX_LEN_TEXT_BUFFER];

//****************************************************************************
//*                     onWmInitDialog_Tab1Proc
//****************************************************************************
BOOL onWmInitDialog_Tab1Proc(const HWND& hDlg)
{
    // check the control radiobutton IDC_RUWIDO_REMOTE
    // for the Ruwido remote
    SendMessage(GetDlgItem(hDlg, IDC_RUWIDO_REMOTE)
        , BM_SETCHECK
        , (WPARAM)BST_CHECKED
        , (LPARAM)0
    );

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
        , MAX_LEN_TEXT_BUFFER
        , "0x%X"
        , oFrame.functionCode
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
    // vendor remote
    if ((oFrame.payload[0] & 0x80) == 0)
    {
        // check the control radiobutton IDC_RUWIDO_REMOTE
        // for the Ruwido remote
        SendMessage(GetDlgItem(g_hWndDlgTab1, IDC_RUWIDO_REMOTE)
            , BM_SETCHECK
            , (WPARAM)BST_CHECKED
            , (LPARAM)0
        );
        SendMessage(GetDlgItem(g_hWndDlgTab1, IDC_YAMAHA_REMOTE)
            , BM_SETCHECK
            , (WPARAM)BST_UNCHECKED
            , (LPARAM)0
        );
    }
    else
    {
        SendMessage(GetDlgItem(g_hWndDlgTab1, IDC_RUWIDO_REMOTE)
            , BM_SETCHECK
            , (WPARAM)BST_UNCHECKED
            , (LPARAM)0
        );
        // check the control radiobutton IDC_YAMAHA_REMOTE
        // for the Yamaha remote
        SendMessage(GetDlgItem(g_hWndDlgTab1, IDC_YAMAHA_REMOTE)
            , BM_SETCHECK
            , (WPARAM)BST_CHECKED
            , (LPARAM)0
        );
    }

    return EXIT_SUCCESS;
}


