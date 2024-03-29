#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern Statusbar g_oStatusbar;
extern HWND g_hWndDlgTab0;
extern BOOL g_bConnected;

//****************************************************************************
//*                     global
//****************************************************************************
HANDLE g_hComm = INVALID_HANDLE_VALUE;
// Resource Aquisition Is Initialisation (RAII)
FRAME g_oFrame = { SOH, 0, 0, STX, {'\0'}, ETX, ETB, EOT };
// no need to initialise
std::queue<tagFRAME> g_queue;
UINT8 g_chBuffer[MAX_BUFFER_SERIAL] = { '\0' };
UINT32 g_valCrc = 0;
UINT g_cTransmission = 0;
UINT g_cErrorCrc = 0;
BOOL g_bContinueTxRx = FALSE;
HANDLE g_hThreadTxRx = INVALID_HANDLE_VALUE;
BOOL g_bSwitch = TRUE;
BOOL g_bTransmit = FALSE;
CHAR g_chTextBuffer[MAX_LEN_TEXT_BUFFER] = { 0 };

//*****************************************************************************
//*                     prototype
//*****************************************************************************
BOOL                connect();
BOOL                date_time_for_serial(UCHAR* pszDateTime);
BOOL                set_date_time(const UCHAR* pszDateTime);
DWORD               WINAPI TxRx(LPVOID lpVoid);
void                TimerProc(HWND hWnd, UINT uint, UINT_PTR pUint, DWORD dword);
BOOL                transmit(LPVOID lpVoid);
BOOL                receive(LPVOID lpVoid);
BOOL                reorganize_queue();
BOOL                setIrRemote(const FRAME& oFrame);
BOOL                setStateFan(const FRAME& oFrame);
BOOL                setStateRelay(const FRAME& oFrame);
BOOL                setRangeSensor(const FRAME& oFrame);
BOOL                setTempSensor(const FRAME& oFrame);
BOOL                reorganize_queue();
BOOL                enableButtonStateFan();
BOOL                enableButtonStateRelay();
BOOL                enableButtonRangeSensor();

//****************************************************************************
//*                     onWmInitDialog_Tab0Proc
//****************************************************************************
BOOL onWmInitDialog_Tab0Proc(const HWND& hDlg)
{
    // disable button DISCONNECT_SERIAL
    EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), FALSE);

    // set the focus on the first applicable control, must be a func PostMessage()
    PostMessage(hDlg
        , WM_NEXTDLGCTL
        , (WPARAM)GetDlgItem(hDlg, CONNECT_SERIAL)
        , (LPARAM)TRUE
    );

    // register for WM_POWERBROADCAST
    DWORD dwFlags = DEVICE_NOTIFY_WINDOW_HANDLE;
    HPOWERNOTIFY hPwrNotify = RegisterPowerSettingNotification(hDlg
        , (LPCGUID)&GUID_SESSION_USER_PRESENCE
        , dwFlags
    );

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
        OutputDebugString(L"CONNECT_SERIAL\n");
        if (connect() == EXIT_SUCCESS)
        {
            // enable/disable button
            EnableWindow(GetDlgItem(hDlg, CONNECT_SERIAL), FALSE);
            EnableWindow(GetDlgItem(hDlg, DISCONNECT_SERIAL), TRUE);
            // set connect state
            g_oStatusbar.setTextStatusbar(0, L"STM32 connected");
            g_bConnected = TRUE;
            // set focus to control DISCONNECT_SERIAL
            PostMessage(hDlg
                , WM_NEXTDLGCTL
                , (WPARAM)GetDlgItem(hDlg, DISCONNECT_SERIAL)
                , (LPARAM)TRUE
            );
            // clear a possible previous "Can't connect"
            g_oStatusbar.setTextStatusbar(3, L"");
            // clear a possible previous crc error
            g_cErrorCrc = 0;
            SendMessage(GetDlgItem(hDlg, IDC_NOF_ERROR_CRC)
                , WM_SETTEXT
                , (WPARAM)0
                , (LPARAM)L""
            );
            // get the date and time for synchronising the real time clock (RTC)
            // in the STM32
            date_time_for_serial(g_oFrame.payload);
            // set date time on statusbar, by using the g_oFrame.payload as a buffer
            set_date_time(g_oFrame.payload);
            // start the communication
            // 1) write the date and time to the STM32
            g_oFrame.cmd = WR_DATE_TIME;
            g_queue.push(g_oFrame);
            // 2) read state of the fan
            g_oFrame.cmd = RD_STATE_FAN;
            g_queue.push(g_oFrame);
            // 3) read state of the relays
            g_oFrame.cmd = RD_STATE_RELAY;
            g_queue.push(g_oFrame);
            // 4) read temperature range of all sensors
            g_oFrame.cmd = RD_RANGE_SENSOR;
            g_queue.push(g_oFrame);

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
        OutputDebugString(L"DISCONNECT_SERIAL\n");

        // when g_hComm is INVALID_HANDLE_VALUE, there is no need to close the handle
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
        g_bConnected = FALSE;
        // clear connected date time, error, and last transaction
        g_oStatusbar.setTextStatusbar(1, L"");
        g_oStatusbar.setTextStatusbar(2, L"");
        g_oStatusbar.setTextStatusbar(3, L"");

        // reset the transmission counter
        g_cTransmission = 0;

        // set focus to control CONNECT_SERIAL
        PostMessage(hDlg
            , WM_NEXTDLGCTL
            , (WPARAM)GetDlgItem(hDlg, CONNECT_SERIAL)
            , (LPARAM)TRUE
        );
        // disable infinite loop
        g_bContinueTxRx = FALSE;
        // make empty queue
        g_queue = { };

        return (INT_PTR)TRUE;
    } // eof DISCONNECT_SERIAL
    } // eof switch
    
    return (INT_PTR)FALSE;
}


//****************************************************************************
//*                     onWmPowerBroadcast
//****************************************************************************
BOOL onWmPowerBroadcast(const HWND& hDlg
    , const WPARAM& wParam
    , const LPARAM& lParam
)
{
    if (wParam == PBT_POWERSETTINGCHANGE)
    {
        POWERBROADCAST_SETTING* pwrbc_setting =
            (POWERBROADCAST_SETTING*)lParam;
        // 2 = PowerUserInactive
        // Windows will fall into sleep mode
        if (pwrbc_setting->Data[0] == 2)
        {
            if (g_hComm != INVALID_HANDLE_VALUE)
            {
                // disconnect from STM32
                SendMessage(hDlg
                    , WM_COMMAND
                    , (WPARAM)DISCONNECT_SERIAL
                    , (LPARAM)0
                );
            }
        }
    }
    
    return EXIT_SUCCESS;
}
/*
    g_oStatusbar.setTextStatusbar(3, L"WM_POWERBROADCAST");
    if (wParam == PBT_POWERSETTINGCHANGE)
    {
        g_oStatusbar.setTextStatusbar(3, L"PBT_POWERSETTINGCHANGE");
        POWERBROADCAST_SETTING* pwrbc_setting =
            (POWERBROADCAST_SETTING*)lParam;
        // 0 = PowerUserPresent
        if (pwrbc_setting->Data[0] == 0)
        {
            g_oStatusbar.setTextStatusbar(3, L"PowerUserPresent");
        }
        // 2 = PowerUserInactive
        if (pwrbc_setting->Data[0] == 2)
        {
            g_oStatusbar.setTextStatusbar(2, L"PowerUserInactive");
            if (g_hComm != INVALID_HANDLE_VALUE)
            {
                SendMessage(hDlg
                    , WM_COMMAND
                    , (WPARAM)DISCONNECT_SERIAL
                    , (LPARAM)0
                );
            }
        }
    }
*/
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
    dcb.DCBlength = sizeof(DCB);
    dcb.BaudRate = 115200;
    dcb.fBinary = 1;
    dcb.fParity = 0;
    dcb.fOutxCtsFlow = 0;
    dcb.fOutxDsrFlow = 0;
    dcb.fDtrControl = 1;
    dcb.fDsrSensitivity = 0;
    dcb.fTXContinueOnXoff = 0;
    dcb.fOutX = 0;
    dcb.fInX = 0;
    dcb.fErrorChar = 0;
    dcb.fNull = 0;
    dcb.fRtsControl = 1;
    dcb.fAbortOnError = 0;
    dcb.fDummy2 = 0;
    dcb.wReserved = 0;
    dcb.ByteSize = 8;
    dcb.Parity = 0;
    dcb.StopBits = 0;
    dcb.XoffChar = 0;
    dcb.XoffChar = 0;
    dcb.ErrorChar = 24;
    dcb.EvtChar = 0;
    dcb.wReserved1 = 0;
    dcb.ByteSize = 8;
    dcb.StopBits = 0;
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
    if (!SetupComm(g_hComm, MAX_BUFFER_SERIAL, MAX_BUFFER_SERIAL))
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

//****************************************************************************
//*                     date_time_for_serial
//****************************************************************************
BOOL date_time_for_serial(UCHAR* pszDateTime)
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
    sprintf_s((char*)pszDateTime, (size_t)LEN_DATE_TIME + 2, "%c%c%c%c%c%c%c%c"
        // avoid conflict with ACK in STM32
        // place date/time in OFFSET_PAYLOAD + 1
        , ' ' // dummy
        // size pszDateTime buffer is
        // LEN_DATE_TIME plus one space for null character
        , ((t.tm_hour / 10) << 4) | (t.tm_hour % 10)
        , ((t.tm_min / 10) << 4) | (t.tm_min % 10)
        , ((t.tm_sec / 10) << 4) | (t.tm_sec % 10)
        , ((t.tm_mday / 10) << 4) | (t.tm_mday % 10)
        , ((t.tm_mon / 10) << 4) | (t.tm_mon % 10)
        , (((t.tm_year % 100) / 10) << 4) | ((t.tm_year % 100) % 10)
        , t.tm_wday
    );
    /*
    sprintf_s((char*)pszDateTime, (size_t)LEN_DATE_TIME + 1, "%c%c%c%c%c%c%c"
        // size pszDateTime buffer is
        // LEN_DATE_TIME plus one space for null character
        , ((t.tm_hour / 10) << 4) | (t.tm_hour % 10)
        , ((t.tm_min / 10) << 4) | (t.tm_min % 10)
        , ((t.tm_sec / 10) << 4) | (t.tm_sec % 10)
        , ((t.tm_mday / 10) << 4) | (t.tm_mday % 10)
        , ((t.tm_mon / 10) << 4) | (t.tm_mon % 10)
        , (((t.tm_year % 100) / 10) << 4) | ((t.tm_year % 100) % 10)
        , t.tm_wday
    );
    */

    return EXIT_SUCCESS;
}

//****************************************************************************
//*                     set_date_time
//****************************************************************************
BOOL set_date_time(const UCHAR* pszDateTime)
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
    switch (pszDateTime[5])
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
    /*
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
    */
    const UCHAR lenDateTime = 24;
    WCHAR wstrDateTime[lenDateTime] = { L'\0' };
    wsprintf(wstrDateTime, L"%s %d%d-%s-20%d%d %d%d:%d%d:%d%d"
        , wstrDow[pszDateTime[7]]
        , (pszDateTime[4] & 0xF0) >> 4
        , pszDateTime[4] & 0x0F
        , wstrMonth[idx_month]
        , (pszDateTime[6] & 0xF0) >> 4
        , pszDateTime[6] & 0x0F
        , (pszDateTime[1] & 0xF0) >> 4
        , pszDateTime[1] & 0x0F
        , (pszDateTime[2] & 0xF0) >> 4
        , pszDateTime[2] & 0x0F
        , (pszDateTime[3] & 0xF0) >> 4
        , pszDateTime[3] & 0x0F
    );
    /*
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
    */
    g_oStatusbar.setTextStatusbar(1, wstrDateTime);

    return EXIT_SUCCESS;
}

//****************************************************************************
//*                     TxRx
//****************************************************************************
DWORD WINAPI TxRx(LPVOID lpVoid)
{
    // set timer
    SetTimer(g_hWndDlgTab0
        , IDT_TIMER
        , DELAY_TIME_SERIAL
        , (TIMERPROC)TimerProc
    );

    // infinite loop, does nothing
    while (g_bContinueTxRx);

    // kill timer
    KillTimer(g_hWndDlgTab0, IDT_TIMER);

    return 0;
}

//****************************************************************************
//*                     TimerProc
//****************************************************************************
void TimerProc(HWND hWnd
    , UINT uMsg
    , UINT_PTR uint_ptr
    , DWORD dword
)
{
    if (uMsg == WM_TIMER)
    {
        /*OutputDebugString(L"timer elapsed WM_TIMER\n");*/
        // toggle between transmit and receive
        if (g_bSwitch) g_bTransmit = !g_bTransmit;
        if (g_bTransmit) transmit(g_hWndDlgTab0); else receive(g_hWndDlgTab0);
    }
}

//****************************************************************************
//*                     transmit
//****************************************************************************
BOOL transmit(LPVOID lpVoid)
{
    /*OutputDebugString(L"transmit()\n");*/

    if (g_queue.empty()) return EXIT_FAILURE;
    FRAME oFrame = g_queue.front();

    if (oFrame.cmd == WR_DATE_TIME) OutputDebugString(L"transmit WR_DATE_TIME\n");
    /*if (oFrame.cmd == WR_STATE_FAN) OutputDebugString(L"transmit WR_STATE_FAN\n");*/
    /*if (oFrame.cmd == WR_STATE_RELAY) OutputDebugString(L"transmit WR_STATE_RELAY\n");*/
    /*if (oFrame.cmd == WR_RANGE_SENSOR) OutputDebugString(L"transmit WR_RANGE_SENSOR\n");*/
    /*if (oFrame.cmd == WR_YAMAHA_REMOTE) OutputDebugString(L"transmit WR_YAMAHA_REMOTE\n");*/
    if (oFrame.cmd == WR_NOP) OutputDebugString(L"transmit WR_NOP\n");
    /*if (oFrame.cmd == RD_IR_REMOTE) OutputDebugString(L"transmit RD_IR_REMOTE\n");*/
    if (oFrame.cmd == RD_STATE_FAN) OutputDebugString(L"transmit RD_STATE_FAN\n");
    if (oFrame.cmd == RD_STATE_RELAY) OutputDebugString(L"transmit RD_STATE_RELAY\n");
    if (oFrame.cmd == RD_RANGE_SENSOR) OutputDebugString(L"transmit RD_RANGE_SENSOR\n");
    /*if (oFrame.cmd == RD_TEMP_SENSOR) OutputDebugString(L"transmit RD_TEMP_SENSOR\n");*/

    // transfer frame to buffer
    g_chBuffer[0] = oFrame.soh;
    g_chBuffer[1] = (oFrame.cmd & 0xFF00) >> 8;
    g_chBuffer[2] = (oFrame.cmd & 0x00FF);

    g_chBuffer[3] = (oFrame.functionCode & 0xFF000000) >> 24;
    g_chBuffer[4] = (oFrame.functionCode & 0x00FF0000) >> 16;
    g_chBuffer[5] = (oFrame.functionCode & 0x0000FF00) >> 8;
    g_chBuffer[6] = (oFrame.functionCode & 0x000000FF);

    g_chBuffer[7] = oFrame.stx;
    for (int i = 0; i < MAX_LEN_ENTRY; i++)
    {
        g_chBuffer[OFFSET_PAYLOAD + i] = oFrame.payload[i];
    }
    g_chBuffer[40] = oFrame.etx;
    g_chBuffer[41] = oFrame.etb;
    g_chBuffer[42] = oFrame.eot;

    // calculate crc and feed into g_chBuffer
    calcCrcEx((const UCHAR*)g_chBuffer, LEN_FRAME, g_valCrc);
    g_chBuffer[LEN_FRAME + 0] = (g_valCrc & 0xFF000000) >> 24;
    g_chBuffer[LEN_FRAME + 1] = (g_valCrc & 0x00FF0000) >> 16;
    g_chBuffer[LEN_FRAME + 2] = (g_valCrc & 0x0000FF00) >> 8;
    g_chBuffer[LEN_FRAME + 3] = (g_valCrc & 0x000000FF);

    // transmit
    DWORD dwNofByteTransferred = 0;
    WriteFile(g_hComm
        , &g_chBuffer
        , LEN_FRAME + LEN_CRC
        , &dwNofByteTransferred
        , NULL
    );

    // when dwNofByteTransferred is LEN_FRAME + LEN_CRC
    // then the transmission is completed
    if (dwNofByteTransferred == LEN_FRAME + LEN_CRC)
    {
        ++g_cTransmission;
        SendMessageA(GetDlgItem((HWND)lpVoid, IDC_NOF_TRANSMISSION)
            , WM_SETTEXT
            , (WPARAM)0
            , (LPARAM)std::to_string(g_cTransmission).c_str()
        );

        // debug message
        sprintf_s(g_chTextBuffer
            , MAX_LEN_TEXT_BUFFER
            , "%02d byte written\n"
            , dwNofByteTransferred
        );
        OutputDebugStringA(g_chTextBuffer);
    }

    return EXIT_SUCCESS;
}

//****************************************************************************
//*                     receive
//****************************************************************************
BOOL receive(LPVOID lpVoid)
{
    /*OutputDebugString(L"receive()\n");*/

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
        OutputDebugString(L"no bytes received from STM32\n");
        g_oStatusbar.setTextStatusbar(2, L"no bytes received");

        // disregard and simply return with EXIT_FAILURE
        return EXIT_FAILURE;
    }
    else if (dwNofByteTransferred == LEN_FRAME + LEN_CRC)
    {
        // no error, clear possible message
        g_oStatusbar.setTextStatusbar(2, L"");

        // check crc
        // isolate received crc from g_chBuffer
        UINT32 rxValCrc = 0;
		rxValCrc = (g_chBuffer[LEN_FRAME + 0] << 24)
			| (g_chBuffer[LEN_FRAME + 1] << 16)
			| (g_chBuffer[LEN_FRAME + 2] << 8)
			| (g_chBuffer[LEN_FRAME + 3]);
        // calculate crc
        calcCrcEx(g_chBuffer, LEN_FRAME, g_valCrc);

        // compare received CRC with calculated CRC
        if (rxValCrc != g_valCrc)
        {
            OutputDebugString(L"crc error\n");
            // crc error
            ++g_cErrorCrc;
            SendMessageA(GetDlgItem((HWND)lpVoid, IDC_NOF_ERROR_CRC)
                , WM_SETTEXT
                , (WPARAM)0
                , (LPARAM)std::to_string(g_cErrorCrc).c_str()
            );
            // do nothing, simply return failed
            return EXIT_FAILURE;
        }

        // no crc error
        // save the received command into the frame
        g_oFrame.cmd = g_chBuffer[1] << 8 | g_chBuffer[2];

        // save the received function code into the frame
        g_oFrame.functionCode = g_chBuffer[3] << 24
            | g_chBuffer[4] << 16
            | g_chBuffer[5] << 8
            | g_chBuffer[6];

        // save the received payload into the frame
        for (int i = 0; i < MAX_LEN_ENTRY; i++)
        {
            g_oFrame.payload[i] = g_chBuffer[OFFSET_PAYLOAD + i];
        }

        // debug message
        sprintf_s(g_chTextBuffer
            , MAX_LEN_TEXT_BUFFER
            , "%02d byte read\n"
            , dwNofByteTransferred
        );

        switch (g_oFrame.cmd)
        {
        case (WR_DATE_TIME):
        {
            if (g_chBuffer[OFFSET_PAYLOAD] == ACK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"ACK WR_DATE_TIME\n");
                if (g_queue.size() > 0) g_queue.pop();
                return EXIT_SUCCESS;
            }
            if (g_chBuffer[OFFSET_PAYLOAD] == NAK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"NAK WR_DATE_TIME\n");
                return EXIT_FAILURE;
            }
            break;
        } // eof WR_DATE_TIME
        case (WR_STATE_FAN):
        {
            if (g_chBuffer[OFFSET_PAYLOAD] == ACK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"ACK WR_STATE_FAN\n");
                enableButtonStateFan();
                if (g_queue.size() > 0) g_queue.pop();
                if (g_queue.size() == 0) reorganize_queue();
                return EXIT_SUCCESS;
            }
            if (g_chBuffer[OFFSET_PAYLOAD] == NAK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"NAK WR_STATE_FAN\n");
                return EXIT_FAILURE;
            }
            break;
        } // eof WR_STATE_FAN
        case (WR_STATE_RELAY):
        {
            if (g_chBuffer[OFFSET_PAYLOAD] == ACK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"ACK WR_STATE_RELAY\n");
                enableButtonStateRelay();
                if (g_queue.size() > 0) g_queue.pop();
                if (g_queue.size() == 0) reorganize_queue();
                return EXIT_SUCCESS;
            }
            if (g_chBuffer[OFFSET_PAYLOAD] == NAK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"NAK WR_STATE_RELAY\n");
                return EXIT_FAILURE;
            }
            break;
        } // eof WR_STATE_RELAY
        case (WR_RANGE_SENSOR):
        {
            if (g_chBuffer[OFFSET_PAYLOAD] == ACK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"ACK WR_RANGE_SENSOR\n");
                enableButtonRangeSensor();
                if (g_queue.size() > 0) g_queue.pop();
                if (g_queue.size() == 0) reorganize_queue();
                return EXIT_SUCCESS;
            }
            if (g_chBuffer[OFFSET_PAYLOAD] == NAK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"NAK WR_RANGE_SENSOR\n");
                return EXIT_FAILURE;
            }
            break;
        } // eof WR_RANGE_SENSOR
        case (WR_YAMAHA_REMOTE): // 33604
        {
            if ((g_chBuffer[OFFSET_PAYLOAD] & 0b01111111) == ACK)
            {
                OutputDebugString(L"receive ACK WR_YAMAHA_REMOTE\n");
                OutputDebugStringA(g_chTextBuffer);
                setIrRemote(g_oFrame);
                if (g_queue.size() > 0) g_queue.pop();
                if (g_queue.size() == 0) reorganize_queue();
                return EXIT_SUCCESS;
            }
            if (g_chBuffer[OFFSET_PAYLOAD] == NAK)
            {
                OutputDebugString(L"receive NAK WR_YAMAHA_REMOTE\n");
                OutputDebugStringA(g_chTextBuffer);
                return EXIT_FAILURE;
            }

            break;
        } // eof WR_YAMAHA_REMOTE
        case (WR_NOP): // 33699
        {
            if (g_chBuffer[OFFSET_PAYLOAD] == ACK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"receive ACK WR_NOP\n");
                if (g_queue.size() > 0) g_queue.pop();
                if (g_queue.size() == 0) reorganize_queue();
                return EXIT_SUCCESS;
            }
            if (g_chBuffer[OFFSET_PAYLOAD] == NAK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"receive NAK WR_NOP\n");
                return EXIT_FAILURE;
            }

            break;
        } // eof WR_NOP
        case (RD_IR_REMOTE): // 33610
        {
            if ((g_chBuffer[OFFSET_PAYLOAD] & 0b01111111) == ACK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"receive ACK RD_IR_REMOTE\n");
                setIrRemote(g_oFrame);
                if (g_queue.size() > 0) g_queue.pop();
                if (g_queue.size() == 0) reorganize_queue();
                return EXIT_SUCCESS;
            }
            if (g_chBuffer[OFFSET_PAYLOAD] == NAK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"receive NAK RD_IR_REMOTE\n");
                return EXIT_FAILURE;
            }
            break;
        } // eof RD_IR_REMOTE
        case (RD_STATE_FAN):
        {
            if (g_chBuffer[OFFSET_PAYLOAD] == ACK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"ACK RD_STATE_FAN\n");
                setStateFan(g_oFrame);
                if (g_queue.size() > 0) g_queue.pop();
                if (g_queue.size() == 0) reorganize_queue();
                return EXIT_SUCCESS;
            }
            if (g_chBuffer[OFFSET_PAYLOAD] == NAK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"NAK RD_STATE_FAN\n");
                return EXIT_FAILURE;
            }
            break;
        } // eof RD_STATE_FAN
        case (RD_STATE_RELAY):
        {
            if (g_chBuffer[OFFSET_PAYLOAD] == ACK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"ACK RD_STATE_RELAY\n");
                setStateRelay(g_oFrame);
                if (g_queue.size() > 0) g_queue.pop();
                if (g_queue.size() == 0) reorganize_queue();
                return EXIT_SUCCESS;
            }
            if (g_chBuffer[OFFSET_PAYLOAD] == NAK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"NAK RD_STATE_RELAY\n");
                return EXIT_FAILURE;
            }
            break;
        } // eof RD_STATE_RELAY
        case (RD_RANGE_SENSOR):
        {
            if (g_chBuffer[OFFSET_PAYLOAD] == ACK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"ACK RD_RANGE_SENSOR\n");
                setRangeSensor(g_oFrame);
                if (g_queue.size() > 0) g_queue.pop();
                if (g_queue.size() == 0) reorganize_queue();
                return EXIT_SUCCESS;
            }
            if (g_chBuffer[OFFSET_PAYLOAD] == NAK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"NAK RD_RANGE_SENSOR\n");
                return EXIT_FAILURE;
            }
            break;
        } // eof RD_RANGE_SENSOR
        case (RD_TEMP_SENSOR):
        {
            if (g_chBuffer[OFFSET_PAYLOAD] == ACK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"ACK RD_TEMP_SENSOR\n");
                setTempSensor(g_oFrame);
                if (g_queue.size() > 0) g_queue.pop();
                if (g_queue.size() == 0) reorganize_queue();
                return EXIT_SUCCESS;
            }
            if (g_chBuffer[OFFSET_PAYLOAD] == NAK)
            {
                OutputDebugStringA(g_chTextBuffer);
                OutputDebugString(L"NAK RD_TEMP_SENSOR\n");
                return EXIT_FAILURE;
            }
            break;
        } // eof RD_TEMP_SENSOR
        } // eof switch
    }

    return EXIT_SUCCESS;
}

//****************************************************************************
//*                     reorganize_queue
//****************************************************************************
BOOL reorganize_queue()
{
    g_oFrame.cmd = RD_IR_REMOTE;
    g_queue.push(g_oFrame);
    g_oFrame.cmd = RD_STATE_FAN;
    g_queue.push(g_oFrame);
    g_oFrame.cmd = RD_STATE_RELAY;
    g_queue.push(g_oFrame);
    g_oFrame.cmd = RD_TEMP_SENSOR;
    g_queue.push(g_oFrame);
    g_oFrame.cmd = WR_NOP;
    g_queue.push(g_oFrame);
 
    return EXIT_SUCCESS;
}

