#pragma once

//****************************************************************************
//*                     global
//****************************************************************************
// Resource Aquisition Is Initialisation RAII 
FRAME g_oFrame = { SOH, 0, STX, { '\0' }, ETX, ETB, EOT };
HANDLE g_hComm = INVALID_HANDLE_VALUE;
BOOL g_bContinueTxRx = FALSE;
HANDLE g_hThreadTxRx = INVALID_HANDLE_VALUE;

//*****************************************************************************
//*                     prototype
//*****************************************************************************
BOOL                date_time_for_serial(CHAR* pszDateTime);
BOOL                set_date_time(const CHAR* pszDateTime, Statusbar& g_oStatusbar);
BOOL                connect();
DWORD WINAPI	    TxRx(LPVOID lpVoid);
BOOL				transmit(LPVOID lpVoid);
BOOL                receive(LPVOID lpVoid);

//****************************************************************************
//*                     onWmInitDialog_Tab0Proc
//****************************************************************************
BOOL onWmInitDialog_Tab0Proc(const HWND& hDlg)
{
    // disable button DISCONNECT_SERIAL
    EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), FALSE);
    return EXIT_SUCCESS;
}
//****************************************************************************
//*                     onWmCommand_Tab0Proc
//****************************************************************************
INT_PTR onWmCommand_Tab0Proc(const HWND& hDlg
    , const WPARAM& wParam
    , Statusbar& g_oStatusbar
)
{
    switch (LOWORD(wParam))
    {
    case CONNECT_SERIAL:
    {
        if (connect() == EXIT_SUCCESS)
        {
            // enable/disable button
            EnableWindow(GetDlgItem(hDlg, CONNECT_SERIAL), FALSE);
            EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), TRUE);
            // set connect state
            g_oStatusbar.setTextStatusbar(0, L"STM32 connected");
            // get the date and time for synchronising the real time clock (RTC)
            // in the STM32
            date_time_for_serial(g_oFrame.payload);
            // set date time on statusbar
            set_date_time(g_oFrame.payload, g_oStatusbar);
            g_oFrame.cmd = WR_DATE_TIME;

            // enable infinite loop
            g_bContinueTxRx = TRUE;

            // create thread to continuously transmit and receive
            DWORD dwThreadIdTxRx = 0;
            g_hThreadTxRx = CreateThread(NULL
                , 0
                , TxRx
                , (LPVOID)hDlg
                , CREATE_SUSPENDED // wait until started
                , &dwThreadIdTxRx
            );

            // start thread exact on this command
            if (g_hThreadTxRx) ResumeThread(g_hThreadTxRx);
        }
        else
        {
            g_oStatusbar.setTextStatusbar(3, L"Can't connect");
        }
        return (INT_PTR)TRUE;
    } // eof CONNECT_SERIAL
    case DISCONNECT_SERIAL:
    {
        // terminate thread
        g_bContinueTxRx = FALSE;
        g_hThreadTxRx = INVALID_HANDLE_VALUE;
        // when g_hComm is INVALID_HANDLE_VALUE, no need to close the handle
        if (g_hComm == INVALID_HANDLE_VALUE) return (INT_PTR)TRUE;
        if (CloseHandle(g_hComm))
        {
            g_hComm = INVALID_HANDLE_VALUE;
        }
        // enable/disable button
        EnableWindow(GetDlgItem(hDlg, CONNECT_SERIAL), TRUE);
        EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), FALSE);
        // set connect state
        g_oStatusbar.setTextStatusbar(0, L"STM32 disconnected");
        // clear connected date time
        g_oStatusbar.setTextStatusbar(1, L"");
        return (INT_PTR)TRUE;
    } // eof DISCONNECT_SERIAL
    } // eof switch
    return (INT_PTR)FALSE;
}
//****************************************************************************
//*                     date_time_for_serial
//****************************************************************************
BOOL date_time_for_serial(CHAR* pszDateTime)
{
    // use ctime to get date and time
    time_t tt;
    time(&tt);
    tm t;
    localtime_s(&t, &tt);
    // coded in binary code decimal (BCD)
    // hour.....: t.tm_hour
    // minute...: t.tm_min
    // second...: t.tm_sec
    // day......: t.tm_mday
    // month....: (t.tm_mon + 1)
    // year.....: (t.tm_year % 100)
    sprintf_s(pszDateTime, (size_t)LEN_DATE_TIME + 1, "%c%c%c%c%c%c%c"
        , ((t.tm_hour / 10) << 4) | (t.tm_hour % 10)
        , ((t.tm_min / 10) << 4) | (t.tm_min % 10)
        , ((t.tm_sec / 10) << 4) | (t.tm_sec % 10)
        , ((t.tm_mday / 10) << 4) | (t.tm_mday % 10)
        , ((t.tm_mon / 10) << 4) | (t.tm_mon % 10)
        , (((t.tm_year % 100) / 10) << 4) | ((t.tm_year % 100) % 10)
        , t.tm_wday
    );

    return EXIT_SUCCESS;
}
//****************************************************************************
//*                     set_date_time
//****************************************************************************
BOOL set_date_time(const CHAR* pszDateTime, Statusbar& g_oStatusbar)
{
    const WCHAR wstrDow[7][3] = { { L"zo" }
        , { L"ma" }
        , { L"di" }
        , { L"wo" }
        , { L"do" }
        , { L"vr" }
        , { L"za" }
    };
    const WCHAR wstrMonth[12][4] = { { L"jan" }
        , { L"feb" }
        , { L"mrt" }
        , { L"apr" }
        , { L"mei" }
        , { L"jun" }
        , { L"jul" }
        , { L"aug" }
        , { L"sep" }
        , { L"okt" }
        , { L"nov" }
        , { L"dec" }
    };
    UCHAR idx_month = 0;
    switch (pszDateTime[4])
    {
    case 0b00010000: // (BCD format)
        // November
        idx_month = 10;
        break;
    case 0b00010001: // (BCD format)
        // Decomber
        idx_month = 11;
        break;
    default: // (0 - 9 in BCD format)
        // January up to October
        idx_month = pszDateTime[4];
    }
    const UCHAR lenDateTime = 24;
    WCHAR wstrDateTime[lenDateTime] = { L'\0' };
    wsprintf(wstrDateTime, L"%s %d%d-%s-20%d%d %d%d:%d%d:%d%d"
        , wstrDow[pszDateTime[6]]
        , (pszDateTime[3] & 0xF0) >> 4
        , pszDateTime[3] & 0x0F
        , wstrMonth[idx_month]
        , (pszDateTime[5] & 0xF0) >> 4
        , pszDateTime[5] & 0x0F
        , (pszDateTime[0] & 0xF0) >> 4
        , pszDateTime[0] & 0x0F
        , (pszDateTime[1] & 0xF0) >> 4
        , pszDateTime[1] & 0x0F
        , (pszDateTime[2] & 0xF0) >> 4
        , pszDateTime[2] & 0x0F
    );
    g_oStatusbar.setTextStatusbar(1, wstrDateTime);

    return EXIT_SUCCESS;
}
//****************************************************************************
//*                     connect
//****************************************************************************
BOOL connect()
{
    // create file
    g_hComm = CreateFile(L"\\\\.\\COM3"
        , GENERIC_READ | GENERIC_WRITE
        , 0
        , NULL
        , OPEN_EXISTING
        , FILE_ATTRIBUTE_NORMAL
        , NULL
    );
    if (g_hComm == INVALID_HANDLE_VALUE)
    {
        return EXIT_FAILURE;
    }

    // set structure to initialize the communication port
    DCB dcb;
    dcb.DCBlength           = sizeof(DCB);
    dcb.BaudRate            = 115200;
    dcb.fBinary             = 1;
    dcb.fParity             = 0;
    dcb.fOutxCtsFlow        = 0;
    dcb.fOutxDsrFlow        = 0;
    dcb.fDtrControl         = 1;
    dcb.fDsrSensitivity     = 0;
    dcb.fTXContinueOnXoff   = 0;
    dcb.fOutX               = 0;
    dcb.fInX                = 0;
    dcb.fErrorChar          = 0;
    dcb.fNull               = 0;
    dcb.fRtsControl         = 1;
    dcb.fAbortOnError       = 0;
    dcb.fDummy2             = 0;
    dcb.wReserved           = 0;
    dcb.ByteSize            = 8;
    dcb.Parity              = 0;
    dcb.StopBits            = 0;
    dcb.XoffChar            = 0;
    dcb.XoffChar            = 0;
    dcb.ErrorChar           = 24;
    dcb.EvtChar             = 0;
    dcb.wReserved1          = 0;
    dcb.ByteSize            = 8;
    dcb.StopBits            = 0;
    // initialize the communication port
    if (!SetCommState(g_hComm, (LPDCB)&dcb))
    {
        return EXIT_FAILURE;
    }

    // set structure for the communication port timeout
    COMMTIMEOUTS commtimeouts;
    commtimeouts.ReadIntervalTimeout = MAXDWORD;
    commtimeouts.ReadTotalTimeoutMultiplier = 0;
    commtimeouts.ReadTotalTimeoutConstant = 0;
    commtimeouts.WriteTotalTimeoutMultiplier = 0;
    commtimeouts.WriteTotalTimeoutConstant = 0;
    // set communication port timeout
    if (!SetCommTimeouts(g_hComm, (LPCOMMTIMEOUTS)&commtimeouts))
    {
        return EXIT_FAILURE;
    }

    // set the communication port mask bit to capture event
    if (!SetCommMask(g_hComm, EV_TXEMPTY | EV_RXCHAR))
    {
        return EXIT_FAILURE;
    }

    // set in/out queue buffers
    if (!SetupComm(g_hComm, BUFFER_MAX_SERIAL, BUFFER_MAX_SERIAL))
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
//****************************************************************************
//*                     TxRx
//****************************************************************************
DWORD WINAPI TxRx(LPVOID lpVoid)
{
    // infinite loop
    while (g_bContinueTxRx)
    {
        transmit(lpVoid);
        Sleep(DELAY_4HZ_SERIAL);
        receive(lpVoid);
        Sleep(DELAY_4HZ_SERIAL);
    }

    return 0;
}
//****************************************************************************
//*                     transmit
//****************************************************************************
BOOL transmit(LPVOID lpVoid)
{
    OutputDebugString(L"transmit\n");
    return EXIT_SUCCESS;
}
//****************************************************************************
//*                     receive
//****************************************************************************
BOOL receive(LPVOID lpVoid)
{
    OutputDebugString(L"receive\n");
    return EXIT_SUCCESS;
}

