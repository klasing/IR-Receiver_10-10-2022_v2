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

//****************************************************************************
//*                     global
//****************************************************************************
UINT g_valMinPwm = 0;
UINT g_valMaxPwm = 99;
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
