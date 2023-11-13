#pragma once

//****************************************************************************
//*                     onWmInitDialog_Tab2Proc
//****************************************************************************
BOOL onWmInitDialog_Tab2Proc(const HWND& hDlg)
{
    //// create an Up/Down control for the IDC_PWM_FAN edittext control
    //g_hWndUpDnCtlPwm = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING
    //    , UPDOWN_CLASS
    //    , NULL
    //    , WS_CHILDWINDOW | WS_VISIBLE
    //    | UDS_AUTOBUDDY | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK
    //    // this control is set alongside the last control that is
    //    // coded in the resource script
    //    , 0, 0, 0, 0
    //    , hDlg
    //    , NULL
    //    , g_hInst
    //    , NULL
    //);

    //// sets the controls direction and range 
    //SendMessage(g_hWndUpDnCtlPwm
    //    , UDM_SETRANGE
    //    , 0
    //    , MAKELPARAM(g_valMaxPwm, g_valMinPwm)
    //);

    //// clear the inital PWM value in the IDC_PWM_FAN edittext control
    //SendMessage(GetDlgItem(hDlg, IDC_PWM_FAN)
    //    , WM_SETTEXT
    //    , (WPARAM)0
    //    , (LPARAM)L""
    //);

    return EXIT_SUCCESS;
}

//****************************************************************************
//*                     onWmCommand_Tab2Proc
//****************************************************************************
INT_PTR onWmCommand_Tab2Proc(const HWND& hDlg
    , const WPARAM& wParam
)
{
    //switch (LOWORD(wParam))
    //{
    //case IDC_WR_FAN_STATE:
    //{
    //    g_oFrame.cmd = WR_FAN_STATE;
    //    g_oFrame.payload[0] = 1;
    //    (SendMessage(GetDlgItem(hDlg, IDC_FAN_ON)
    //        , BM_GETCHECK
    //        , (WPARAM)0
    //        , (LPARAM)0) == BST_UNCHECKED) ?
    //        g_oFrame.payload[1] = (CHAR)TRUE :
    //        g_oFrame.payload[1] = (CHAR)FALSE;
    //    return (INT_PTR)TRUE;
    //} // eof IDC_WR_FAN_STATE
    //} // eof switch
    return (INT_PTR)FALSE;
}
/*
#pragma once

//****************************************************************************
//*                     include
//****************************************************************************
#include "framework.h"
#include "Resource.h"

//****************************************************************************
//*                     extern
//****************************************************************************
extern HINSTANCE g_hInst;
extern FRAME g_oFrame;
extern HANDLE g_hThreadTxRx;

//****************************************************************************
//*                     global
//****************************************************************************
UINT g_valMinPwm = 1;
UINT g_valMaxPwm = 100;
HWND g_hWndUpDnCtlPwm = NULL;

//****************************************************************************
//*                     onWmInitDialog_Tab2Proc
//****************************************************************************
BOOL onWmInitDialog_Tab2Proc(const HWND& hDlg)
{
    // create an Up/Down control for the IDC_PWM_FAN edittext control
    g_hWndUpDnCtlPwm = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING
        , UPDOWN_CLASS
        , NULL
        , WS_CHILDWINDOW | WS_VISIBLE
        | UDS_AUTOBUDDY | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK
        // this control is set alongside the last control that is
        // coded in the resource script
        , 0, 0, 0, 0         
        , hDlg
        , NULL
        , g_hInst
        , NULL
    );

    // sets the controls direction and range 
    SendMessage(g_hWndUpDnCtlPwm
        , UDM_SETRANGE
        , 0
        , MAKELPARAM(g_valMaxPwm, g_valMinPwm)
    );

    // clear the inital PWM value in the IDC_PWM_FAN edittext control
    SendMessage(GetDlgItem(hDlg, IDC_PWM_FAN)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)L""
    );
    
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
    case IDC_WR_FAN_STATE:
    {
		g_oFrame.cmd = WR_FAN_STATE;
		g_oFrame.payload[0] = 1;
		(SendMessage(GetDlgItem(hDlg, IDC_FAN_ON)
			, BM_GETCHECK
			, (WPARAM)0
			, (LPARAM)0) == BST_UNCHECKED) ?
			g_oFrame.payload[1] = (CHAR)TRUE :
			g_oFrame.payload[1] = (CHAR)FALSE;
		return (INT_PTR)TRUE;
    } // eof IDC_WR_FAN_STATE
    } // eof switch
    return (INT_PTR)FALSE;
}
*/