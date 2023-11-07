#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern Statusbar g_oStatusbar;
extern HWND g_hWndDlgTab1;
extern HWND g_hWndDlgTab2;

//****************************************************************************
//*                     global
//****************************************************************************
// Resource Aquisition Is Initialisation RAII 
FRAME g_oFrame = { SOH, 0, STX, { '\0' }, ETX, ETB, EOT };
HANDLE g_hComm = INVALID_HANDLE_VALUE;
BOOL g_bContinueTxRx = FALSE;
HANDLE g_hThreadTxRx = INVALID_HANDLE_VALUE;
extern UCHAR g_chBuffer[BUFFER_MAX_SERIAL] = { 0 };
UINT32 g_valCrc = 0;
UINT g_cTransmission = 0;
UINT g_cErrorCrc = 0;
CHAR g_chTextBuffer[8] = { 0 };

//*****************************************************************************
//*                     prototype
//*****************************************************************************
BOOL                date_time_for_serial(CHAR* pszDateTime);
BOOL                set_date_time(const CHAR* pszDateTime);
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
            set_date_time(g_oFrame.payload);
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
        // clear connected date time and rtc is set message
        g_oStatusbar.setTextStatusbar(1, L"");
        g_oStatusbar.setTextStatusbar(3, L"");
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
    sprintf_s(pszDateTime, (size_t)LEN_DATE_TIME + 2, "%c%c%c%c%c%c%c%c"
        // the first character in the payload holds the length of the payload
        // in this case lenPayload (1) + LEN_DATE_TIME (7) + null character (1)
        , LEN_DATE_TIME
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
BOOL set_date_time(const CHAR* pszDateTime)
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
    // take into account that the first character in the payload is the length of the payload
    // so every index is incremented with 1
    switch (pszDateTime[4 + 1])
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
        idx_month = pszDateTime[4 + 1];
    }
    const UCHAR lenDateTime = 24;
    WCHAR wstrDateTime[lenDateTime] = { L'\0' };
    wsprintf(wstrDateTime, L"%s %d%d-%s-20%d%d %d%d:%d%d:%d%d"
        , wstrDow[pszDateTime[6 + 1]]
        , (pszDateTime[3 + 1] & 0xF0) >> 4
        , pszDateTime[3 + 1] & 0x0F
        , wstrMonth[idx_month]
        , (pszDateTime[5 + 1] & 0xF0) >> 4
        , pszDateTime[5 + 1] & 0x0F
        , (pszDateTime[0 + 1] & 0xF0) >> 4
        , pszDateTime[0 + 1] & 0x0F
        , (pszDateTime[1 + 1] & 0xF0) >> 4
        , pszDateTime[1 + 1] & 0x0F
        , (pszDateTime[2 + 1] & 0xF0) >> 4
        , pszDateTime[2 + 1] & 0x0F
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
    // bring over the frame into a buffer
    g_chBuffer[0] = g_oFrame.soh;
    g_chBuffer[1] = (g_oFrame.cmd >> 8) & 0xFF;
    g_chBuffer[2] = (g_oFrame.cmd & 0xFF);
    g_chBuffer[3] = g_oFrame.stx;
    // first character in the payload holds the length of the payload
    // this makes payloads with variable lengths possible
    // max payload is LEN_MAX_ENTRY = 31
    UCHAR lenPayload = g_oFrame.payload[0];
    for (uint8_t i = 0; i < lenPayload; i++)
    {
        g_chBuffer[i + 4] = g_oFrame.payload[i + 1];
    }
    g_chBuffer[34] = '\0';
    g_chBuffer[35] = g_oFrame.etx;
    g_chBuffer[36] = g_oFrame.etb;
    g_chBuffer[37] = g_oFrame.eot;

    // calculate crc and feed into g_chBuffer
    calcCrcEx((const UCHAR*)g_chBuffer, LEN_FRAME, g_valCrc);
    g_chBuffer[38] = (g_valCrc & 0xFF000000) >> 24;
    g_chBuffer[39] = (g_valCrc & 0x00FF0000) >> 16;
    g_chBuffer[40] = (g_valCrc & 0x0000FF00) >> 8;
    g_chBuffer[41] = (g_valCrc & 0x000000FF);

    // transmit
    DWORD dwNofByteTransferred = 0;
    WriteFile(g_hComm
        , &g_chBuffer
        , LEN_FRAME + LEN_CRC
        , &dwNofByteTransferred
        , NULL
    );

    // if dwNofByteTransferred is LEN_FRAME + LEN_CRC then the transmission
    // is completed
    if (dwNofByteTransferred == LEN_FRAME + LEN_CRC)
    {
        ++g_cTransmission;
        SendMessageA(GetDlgItem((HWND)lpVoid, IDC_NOF_TRANSMISSION)
            , WM_SETTEXT
            , (WPARAM)0
            , (LPARAM)std::to_string(g_cTransmission).c_str()
        );

        if (g_oFrame.cmd == WR_FAN_STATE)
        {
            OutputDebugString(L"fan state is transmitted\n");
            // avoid repeatedly transmission of fan state
            g_oFrame.cmd = WR_DATE_TIME;
        }
    }

    return EXIT_SUCCESS;
}
//****************************************************************************
//*                     receive
//****************************************************************************
BOOL receive(LPVOID lpVoid)
{
    // receive
    DWORD dwNofByteTransferred = 0;
    ReadFile(g_hComm
        , &g_chBuffer
        , LEN_FRAME + LEN_CRC
        , &dwNofByteTransferred
        , NULL
    );

    // connection is lost
    if (dwNofByteTransferred == 0)
    {
        // disregard and simply return with EXIT_FAILURE
        return EXIT_FAILURE;
    }
    else if (dwNofByteTransferred == LEN_FRAME + LEN_CRC)
    {
        // check crc
        // isolate received crc from g_chBuffer
        UINT32 rxValCrc = (g_chBuffer[38] << 24)
            | (g_chBuffer[39] << 16)
            | (g_chBuffer[40] << 8)
            | (g_chBuffer[41]);

        // calculate crc
        calcCrcEx(g_chBuffer, LEN_FRAME, g_valCrc);

        // compare received CRC with calculated CRC
        if (rxValCrc != g_valCrc)
        {
            // crc error
            ++g_cErrorCrc;
            SendMessageA(GetDlgItem((HWND)lpVoid, IDC_NOF_ERROR_CRC)
                , WM_SETTEXT
                , (WPARAM)0
                , (LPARAM)std::to_string(g_cErrorCrc).c_str()
            );
            return EXIT_FAILURE;
        }

        // no crc error
        // transfer command from g_chBuffer to g_oFrame.cmd
        g_oFrame.cmd = g_chBuffer[1] << 8
            | g_chBuffer[2];

        if (g_oFrame.cmd == WR_DATE_TIME)
        {
            // finish setting the RTC in the STM32
            // receive ACK: the laptop will continuesly send ACK
            // until a ir-remote (on the mcu) value is received
            if (g_chBuffer[4] == ACK)
            {
                g_oStatusbar.setTextStatusbar(3, L"RTC in STM32 is set");
                g_chBuffer[4] = ACK;
            }
        }

        if (g_oFrame.cmd == FAN_STATE_CHANGED)
        {
            // test
            ((BOOL)g_chBuffer[4]) ? OutputDebugString(L"fan off\n") :
                OutputDebugString(L"fan on\n");
            // g_chBuffer[4] is bFanOff = TRUE, when fan is off
            ((BOOL)g_chBuffer[4]) ?
				SendMessage(GetDlgItem(g_hWndDlgTab2, IDC_FAN_ON)
					, BM_SETCHECK
					, (WPARAM)BST_UNCHECKED
					, (LPARAM)0
				) :
				SendMessage(GetDlgItem(g_hWndDlgTab2, IDC_FAN_ON)
					, BM_SETCHECK
					, (WPARAM)BST_CHECKED
					, (LPARAM)0
				);
        }

        // the IR-receiver will return a g_oFrame.cmd value between
        // 0xABF and 0xFFF
        if (g_oFrame.cmd >= 0xABF && g_oFrame.cmd <= 0xFFF)
        {
            // transfer payload from g_chBuffer to g_oFrame.payload
            for (UINT8 i = 0; i < LEN_MAX_ENTRY; i++)
            {
                g_oFrame.payload[i] = g_chBuffer[i + 4];
            }
            // set g_oFrame.cmnd in edittext IDC_CODE
            sprintf_s(g_chTextBuffer, 8, "0x%X", g_oFrame.cmd);
            SendMessageA(GetDlgItem(g_hWndDlgTab1, IDC_CODE)
                , WM_SETTEXT
                , (WPARAM)0
                , (LPARAM)g_chTextBuffer
            );
            // set g_oFrame.payload in edittext IDC_DESCRIPTION
            SendMessageA(GetDlgItem(g_hWndDlgTab1, IDC_DESCRIPTION)
                , WM_SETTEXT
                , (WPARAM)0
                , (LPARAM)g_oFrame.payload
            );
            if (g_oFrame.cmd == 0xEEF)
            {
                // description: pause/play | back/?
                SendMessage(GetDlgItem(g_hWndDlgTab2, IDC_FAN_ON)
                    , BM_SETCHECK
                    , (WPARAM)BST_CHECKED
                    , (LPARAM)0
                );
            }
            if (g_oFrame.cmd == 0xBEB)
            {
                // description: stop
                SendMessage(GetDlgItem(g_hWndDlgTab2, IDC_FAN_ON)
                    , BM_SETCHECK
                    , (WPARAM)BST_UNCHECKED
                    , (LPARAM)0
                );
            }
        }
    }
    
    return EXIT_SUCCESS;
}

