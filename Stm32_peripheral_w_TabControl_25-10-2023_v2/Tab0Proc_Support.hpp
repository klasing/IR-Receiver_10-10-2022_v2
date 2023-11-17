#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern Statusbar g_oStatusbar;
//extern HWND g_hWndDlgTab1;
//extern HWND g_hWndDlgTab2;

//****************************************************************************
//*                     global
//****************************************************************************
const FLOAT DEFAULT_T_LO = 15.;
const FLOAT DEFAULT_T_HI = 16.;
// Resource Aquisition Is Initialisation RAII 
FRAME g_oFrame = { SOH, 0, STX, { 0 }, ETX, ETB, EOT };
HANDLE g_hComm = INVALID_HANDLE_VALUE;
BOOL g_bContinueTxRx = FALSE;
HANDLE g_hThreadTxRx = INVALID_HANDLE_VALUE;
// no need to initialise
std::queue<tagFRAME> g_queue;
UCHAR g_chBuffer[BUFFER_MAX_SERIAL] = { '\0' };
UINT32 g_valCrc = 0;
UINT g_cTransmission = 0;
UINT g_cErrorCrc = 0;
CHAR g_chTextBuffer[LEN_MAX_TEXT_BUFFER] = { 0 };
//UINT16 g_old_rpm = 0;

//*****************************************************************************
//*                     prototype
//*****************************************************************************
DWORD               WINAPI TxRx(LPVOID lpVoid);
BOOL                set_date_time(const CHAR* pszDateTime);
BOOL                date_time_for_serial(CHAR* pszDateTime);
BOOL                connect();
BOOL                setRangeSensor();               // in Tab4Proc_Support.hpp

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
        OutputDebugString(L"CONNECT_SERIAL\n");
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
            // set date time on statusbar, by using the g_oFrame.payload as a buffer
            set_date_time(g_oFrame.payload);

            // start the communication by transferring the date and time
            // to the STM32
            g_oFrame.cmd = WR_DATE_TIME;
            g_queue.push(g_oFrame);

            // prepare for writing temperature range sensor
            g_oFrame.cmd = WR_RANGE_SENSOR;
            INT16 iTempLo = (INT16)(DEFAULT_T_LO / 0.0625) << 4;
            INT16 iTempHi = (INT16)(DEFAULT_T_HI / 0.0625) << 4;
            g_oFrame.payload[0] = (iTempLo & 0xFF00) >> 8;
            g_oFrame.payload[1] = (iTempLo & 0x00FF);
            g_oFrame.payload[2] = (iTempHi & 0xFF00) >> 8;
            g_oFrame.payload[3] = (iTempHi & 0x00FF);
            g_queue.push(g_oFrame);

            // prepare for no operation
            g_oFrame.cmd = NOP;
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

            // clear a possible previous "Can't connect"
            g_oStatusbar.setTextStatusbar(3, L"");
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
		// when g_hComm is INVALID_HANDLE_VALUE, no need to close the handle
		if (g_hComm == INVALID_HANDLE_VALUE) return (INT_PTR)TRUE;
		if (CloseHandle(g_hComm))
		{
			g_hComm = INVALID_HANDLE_VALUE;
		}
		// terminate thread
		g_bContinueTxRx = FALSE;
		g_hThreadTxRx = INVALID_HANDLE_VALUE;

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
//*                     receive
//****************************************************************************
BOOL receive(LPVOID lpVoid)
{
    OutputDebugString(L"receive()\n");
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
        // disregard and simply return with EXIT_FAILURE
        return EXIT_FAILURE;
    }
    else if (dwNofByteTransferred == LEN_FRAME + LEN_CRC)
    {
        // check crc
        // isolate received crc from g_chBuffer
        UINT32 rxValCrc = (g_chBuffer[LEN_FRAME + 0] << 24)
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
        // save the received payload into the frame
        for (int i = 0; i < LEN_MAX_ENTRY; i++)
        {
            g_oFrame.payload[i] = g_chBuffer[i + 4];
        }

        if (g_oFrame.cmd == WR_DATE_TIME)
        {
            if (g_oFrame.payload[0] == ACK)
            {
                OutputDebugString(L"ACK WR_DATE_TIME\n");
                g_oStatusbar.setTextStatusbar(3, L"RTC in STM32 is set");
                if (g_queue.size() > 1) g_queue.pop();
                return EXIT_SUCCESS;
            }
            if (g_oFrame.payload[0] == NAK)
            {
                OutputDebugString(L"NAK WR_DATE_TIME\n");
                return EXIT_FAILURE;
            }
        }
        if (g_oFrame.cmd == WR_RANGE_SENSOR)
        {
            if (g_oFrame.payload[0] == ACK)
            {
                OutputDebugString(L"ACK WR_RANGE_SENSOR\n");
                g_oStatusbar.setTextStatusbar(3, L"Temperature range is set");
                if (g_queue.size() > 1) g_queue.pop();
                setRangeSensor();
                return EXIT_SUCCESS;
            }
            if (g_oFrame.payload[0] == NAK)
            {
                OutputDebugString(L"NAK WR_STATE_SENSOR\n");
                return EXIT_FAILURE;
            }
        }
        if (g_oFrame.cmd == NOP)
        {
            if (g_oFrame.payload[0] == ACK)
            {
                OutputDebugString(L"ACK NOP\n");
                g_oStatusbar.setTextStatusbar(3, L"No operation");
                return EXIT_SUCCESS;
            }
            if (g_oFrame.payload[0] == NAK)
            {
                OutputDebugString(L"NAK NOP\n");
                return EXIT_FAILURE;
            }
        }
    }

    // command from STM32 is not recognized
    OutputDebugString(L"command from STM32 is not recognized\n");
    return EXIT_FAILURE;
}

//****************************************************************************
//*                     transmit
//****************************************************************************
BOOL transmit(LPVOID lpVoid)
{
    OutputDebugString(L"transmit()\n");

    if (g_queue.empty()) return EXIT_FAILURE;

    FRAME oFrame = g_queue.front();

    if (oFrame.cmd == WR_DATE_TIME) OutputDebugString(L"transmit WR_DATE_TIME\n");
    if (oFrame.cmd == WR_RANGE_SENSOR) OutputDebugString(L"transmit WR_RANGE_SENSOR\n");
    if (oFrame.cmd == NOP) OutputDebugString(L"transmit NOP\n");

    // transfer frame to buffer
    g_chBuffer[0] = oFrame.soh;
    g_chBuffer[1] = (oFrame.cmd >> 8) & 0xFF;
    g_chBuffer[2] = oFrame.cmd & 0xFF;
    g_chBuffer[3] = oFrame.stx;
    for (int i = 0; i < LEN_MAX_ENTRY; i++)
    {
        g_chBuffer[i + 4] = oFrame.payload[i];
    }
    g_chBuffer[36] = oFrame.etx;
    g_chBuffer[37] = oFrame.etb;
    g_chBuffer[38] = oFrame.eot;

    // calculate crc and feed into g_chBuffer
    calcCrcEx((const UCHAR*)g_chBuffer, LEN_FRAME, g_valCrc);
    g_chBuffer[39] = (g_valCrc & 0xFF000000) >> 24;
    g_chBuffer[40] = (g_valCrc & 0x00FF0000) >> 16;
    g_chBuffer[41] = (g_valCrc & 0x0000FF00) >> 8;
    g_chBuffer[42] = (g_valCrc & 0x000000FF);

    // transmit
    DWORD dwNofByteTransferred = 0;
    WriteFile(g_hComm
        , &g_chBuffer
        , LEN_FRAME + LEN_CRC
        , &dwNofByteTransferred
        , NULL
    );

    // when dwNofByteTransferred is LEN_FRAME + LEN_CRC then the transmission
    // is completed
    if (dwNofByteTransferred == LEN_FRAME + LEN_CRC)
    {
        ++g_cTransmission;
        SendMessageA(GetDlgItem((HWND)lpVoid, IDC_NOF_TRANSMISSION)
            , WM_SETTEXT
            , (WPARAM)0
            , (LPARAM)std::to_string(g_cTransmission).c_str()
        );
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
        // size pszDateTime buffer is
        // LEN_DATE_TIME plus one place for null character
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
    if (!SetupComm(g_hComm, BUFFER_MAX_SERIAL, BUFFER_MAX_SERIAL))
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/*
#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern Statusbar g_oStatusbar;
extern HWND g_hWndDlgTab0; // Serial comm.
extern HWND g_hWndDlgTab1; // IR-remote
extern HWND g_hWndDlgTab2; // Fan control
extern HWND g_hWndDlgTab3; // Relay module
extern HWND g_hWndDlgTab4; // Temp sensor

//****************************************************************************
//*                     global
//****************************************************************************
const FLOAT g_DEFAULT_T_LO = 15.;
const FLOAT g_DEFAULT_T_HI = 16.;
// Resource Aquisition Is Initialisation RAII 
FRAME g_oFrame = { SOH, 0, STX, { 0 }, ETX, ETB, EOT };
HANDLE g_hComm = INVALID_HANDLE_VALUE;
BOOL g_bContinueTxRx = FALSE;
HANDLE g_hThreadTxRx = INVALID_HANDLE_VALUE;
extern UCHAR g_chBuffer[BUFFER_MAX_SERIAL] = { 0 };
UINT32 g_valCrc = 0;
UINT g_cTransmission = 0;
UINT g_cErrorCrc = 0;
CHAR g_chTextBuffer[16] = { 0 };
BOOL g_bReadFanState = TRUE;
FLOAT old_fRPM = 0.;
UCHAR g_lenPayload = 0;

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

        // start by setting default temp low / temp high
        g_oFrame.cmd = WR_TEMP_RANGE;
        // FLOAT is 32 bit is 4 byte
        // per register 2 FLOAT is 2 x 4 byte = 8 byte
        // 4 register: 4 x 8 byte = 32 byte
        g_lenPayload = 32;
        // transfer default setting for temp range into payload
        // sensor 1, 2, 3, 4
        g_oFrame.payload[0] = ((INT32)g_DEFAULT_T_LO & 0xFF000000) >> 24;
        g_oFrame.payload[1] = ((UINT32)g_DEFAULT_T_LO & 0x00FF0000) >> 16;
        g_oFrame.payload[2] = ((INT32)g_DEFAULT_T_HI & 0xFF000000) >> 24;
        g_oFrame.payload[3] = ((UINT32)g_DEFAULT_T_HI & 0x00FF0000) >> 16;

        return (INT_PTR)TRUE;
    } // eof CONNECT_SERIAL
    case DISCONNECT_SERIAL:
    {
        // kill timer
        KillTimer(g_hWndDlgTab0, IDT_POLLING_TIMER);
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
//*                     PollingTimerProc
//****************************************************************************
VOID PollingTimerProc(HWND hWnd // handle to window, in this case g_hWndDlgTab0
    , UINT uMsg                 // WM_TIMER message
    , UINT_PTR idTimer          // in this case IDT_POLLING_TIMER
    , DWORD dwTime)             // current system time
{
    if (uMsg == WM_TIMER)
    {
        //OutputDebugString(L"timer expired\n");
        // polling PWM and RPM from fan
        if (g_bReadFanState)
        {
            g_oFrame.cmd = RD_FAN_STATE;
            //OutputDebugString(L"RD_FAN_STATE is transmitted\n");
        }
        else
        {
            // polling temperature sensor
            g_oFrame.cmd = RD_REG_TEMP;
            //OutputDebugString(L"RD_REG_TEMP is transmitted\n");
        }
    }
}
//****************************************************************************
//*                     TxRx
//****************************************************************************
DWORD WINAPI TxRx(LPVOID lpVoid)
{
    // set polling timer
    SetTimer(g_hWndDlgTab0
        , IDT_POLLING_TIMER
        , 1000 // 1 s interval
        , (TIMERPROC)PollingTimerProc
    );
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
    // g_lenPayload holds the length of data to be stored in the payload
    // max payload is LEN_MAX_ENTRY = 32
    for (uint8_t i = 0; i < g_lenPayload; i++)
    {
        g_chBuffer[i + 4] = g_oFrame.payload[i];
    }
    // TODO: this must go
    //g_chBuffer[34] = '\0';
    g_chBuffer[36] = g_oFrame.etx;
    g_chBuffer[37] = g_oFrame.etb;
    g_chBuffer[38] = g_oFrame.eot;

    // calculate crc and feed into g_chBuffer
    calcCrcEx((const UCHAR*)g_chBuffer, LEN_FRAME, g_valCrc);
    g_chBuffer[39] = (g_valCrc & 0xFF000000) >> 24;
    g_chBuffer[40] = (g_valCrc & 0x00FF0000) >> 16;
    g_chBuffer[41] = (g_valCrc & 0x0000FF00) >> 8;
    g_chBuffer[42] = (g_valCrc & 0x000000FF);

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
    }

    if (g_oFrame.cmd == WR_FAN_STATE)
    {
        //OutputDebugString(L"WR_FAN_STATE is transmitted\n");
        return EXIT_SUCCESS;
    }
    if (g_oFrame.cmd == WR_RELAY_STATE)
    {
        //OutputDebugString(L"WR_RELAY_STATE is transmitted\n");
        return EXIT_SUCCESS;
    }
    if (g_oFrame.cmd == WR_TEMP_RANGE)
    {
        OutputDebugString(L"WR_TEMP_RANGE is transmitted\n");
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}
//****************************************************************************
//*                     bit12ToCelsius
//****************************************************************************
BOOL bit12ToCelsius(const WORD& wBit12
    , const UINT& resourceId
)
{
    INT16 tempVal = wBit12;
    // convert to 2's complement, since the temperature can be negative
    if (tempVal > 0x7FF)
    {
        tempVal |= 0xF000;
    }
    // convert to float temperature value in degrees Celcius
    FLOAT fTempCelsius = tempVal * .0625;
    // convert the temperature to a decimal format
    fTempCelsius *= 100;
    // TODO: check for temp below zero
    sprintf_s(g_chTextBuffer, 8, "%d.%02u"//"%u.%02u"
        , (INT)fTempCelsius / 100//(UINT)fTempCelsius / 100
        , (UINT)fTempCelsius % 100//(UINT)fTempCelsius % 100
    );
    // set temperature value into edittext control
    SendMessageA(GetDlgItem(g_hWndDlgTab4, resourceId)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );
    return EXIT_SUCCESS;
}
//****************************************************************************
//*                     isolateAlertBit
//****************************************************************************
BOOL isolateAlertBit(const WORD& wBit12
    , const UINT& resourceId
)
{
    (wBit12 & 2) ?
        SendMessage(GetDlgItem(g_hWndDlgTab4, resourceId)
            , BM_SETCHECK
            , (WPARAM)BST_CHECKED
            , (LPARAM)0
        ) :
        SendMessage(GetDlgItem(g_hWndDlgTab4, resourceId)
            , BM_SETCHECK
            , (WPARAM)BST_UNCHECKED
            , (LPARAM)0
        );
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
        UINT32 rxValCrc = (g_chBuffer[39] << 24)
            | (g_chBuffer[40] << 16)
            | (g_chBuffer[41] << 8)
            | (g_chBuffer[42]);

        g_chBuffer[4] = ACK;
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
            //do nothing while testing return EXIT_FAILURE;
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
            return EXIT_SUCCESS;
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
            // fan on via IR-remote
            if (g_oFrame.cmd == 0xEEF)
            {
                // description: pause/play | back/?
                SendMessage(GetDlgItem(g_hWndDlgTab2, IDC_FAN_ON)
                    , BM_SETCHECK
                    , (WPARAM)BST_CHECKED
                    , (LPARAM)0
                );
            }
            // fan off via IR-remote
            if (g_oFrame.cmd == 0xBEB)
            {
                // description: stop
                SendMessage(GetDlgItem(g_hWndDlgTab2, IDC_FAN_ON)
                    , BM_SETCHECK
                    , (WPARAM)BST_UNCHECKED
                    , (LPARAM)0
                );
            }
            return EXIT_SUCCESS;
        }
        if (g_oFrame.cmd == FAN_STATE_CHANGED)
        {
            //OutputDebugString(L"FAN_STATE_CHANGED is received\n");
            if ((BOOL)g_chBuffer[4])
            {
                SendMessage(GetDlgItem(g_hWndDlgTab2, IDC_FAN_ON)
                    , BM_SETCHECK
                    , (WPARAM)BST_UNCHECKED
                    , (LPARAM)0
                );
            }
            else
            {
                SendMessage(GetDlgItem(g_hWndDlgTab2, IDC_FAN_ON)
                    , BM_SETCHECK
                    , (WPARAM)BST_CHECKED
                    , (LPARAM)0
                );
            }
            g_oFrame.cmd = WR_DATE_TIME;
            return EXIT_SUCCESS;
        }
        // the WR_FAN_STATE acknowledge is received
        if (g_oFrame.cmd == WR_FAN_STATE && g_chBuffer[4] == ACK)
        {
            //OutputDebugString(L"WR_FAN_STATE acknowledge is received\n");
            g_oFrame.cmd = WR_DATE_TIME;
            return EXIT_SUCCESS;
        }
        // the WR_RELAY_STATE acknowledge is received
        if (g_oFrame.cmd == WR_RELAY_STATE && g_chBuffer[4] == ACK)
        {
            //OutputDebugString(L"WR_RELAY_STATE acknowledge is received\n");
            g_oFrame.cmd = WR_DATE_TIME;
            return EXIT_SUCCESS;
        }
        // the RD_FAN_STATE data is received
        if (g_oFrame.cmd == RD_FAN_STATE)
        {
            //OutputDebugString(L"RD_FAN_STATE data is received\n");
            // TODO: can't be done, if done the fan can't be turned on from
            // the laptop
            //// fan on/off
            //((BOOL)g_chBuffer[4]) ?
            //    SendMessage(GetDlgItem(g_hWndDlgTab2, IDC_FAN_ON)
            //        , BM_SETCHECK
            //        , (WPARAM)BST_UNCHECKED
            //        , (LPARAM)0
            //    ) :
            //    SendMessage(GetDlgItem(g_hWndDlgTab2, IDC_FAN_ON)
            //        , BM_SETCHECK
            //        , (WPARAM)BST_CHECKED
            //        , (LPARAM)0
            //    );            
            // PWM
            // value lies between 0 .. 99, adjust this value to 1 .. 100
            sprintf_s(g_chTextBuffer, 8, "%d", g_chBuffer[5] + 1);
            SendMessageA(GetDlgItem(g_hWndDlgTab2, IDC_PWM_FAN)
                , WM_SETTEXT
                , (WPARAM)0
                , (LPARAM)g_chTextBuffer
            );
            // RPM
            FLOAT fRPM = (FLOAT)(g_chBuffer[6] << 24) +
                (FLOAT)(g_chBuffer[7] << 16) +
                (FLOAT)(g_chBuffer[8] << 8) +
                (FLOAT)g_chBuffer[9];
            if (fRPM != 0.) old_fRPM = fRPM;
            sprintf_s(g_chTextBuffer, 16, "%d", (UINT)old_fRPM);
            SendMessageA(GetDlgItem(g_hWndDlgTab2, IDC_RPM_FAN)
                , WM_SETTEXT
                , (WPARAM)0
                , (LPARAM)g_chTextBuffer
            );
            g_oFrame.cmd = WR_DATE_TIME;
            // poll the next data item
            g_bReadFanState = FALSE;
            return EXIT_SUCCESS;
        }
        // the RD_REG_TEMP data is received
        // and also the other registers: REG_CNFG, REG_T_LO, REG_T_HI
        if (g_oFrame.cmd == RD_REG_TEMP)
        {
            //OutputDebugString(L"RD_REG_TEMP data is received\n");
            // temp sensor 1
            // combine the bytes for param 1
            bit12ToCelsius(((INT16)g_chBuffer[4] << 4) | (g_chBuffer[5] >> 4)
                , IDC_TEMP_SENSOR1
            );
            isolateAlertBit(((INT16)g_chBuffer[6] << 4) | (g_chBuffer[7] >> 4)
                , IDC_ALERT_SENSOR1
            );
            // temp low sensor 1
            bit12ToCelsius(((INT16)g_chBuffer[8] << 4) | (g_chBuffer[9] >> 4)
                , IDC_TEMP_LO_SENSOR1
            );
            // temp high sensor 1
            bit12ToCelsius(((INT16)g_chBuffer[10] << 4) | (g_chBuffer[11] >> 4)
                , IDC_TEMP_HI_SENSOR1
            );
            // temp sensor 2
            // combine the bytes for param 1
            bit12ToCelsius(((INT16)g_chBuffer[12] << 4) | (g_chBuffer[13] >> 4)
                , IDC_TEMP_SENSOR2
            );
            isolateAlertBit(((INT16)g_chBuffer[14] << 4) | (g_chBuffer[15] >> 4)
                , IDC_ALERT_SENSOR2
            );
            // temp low sensor 2
            bit12ToCelsius(((INT16)g_chBuffer[16] << 4) | (g_chBuffer[17] >> 4)
                , IDC_TEMP_LO_SENSOR2
            );
            // temp high sensor 2
            bit12ToCelsius(((INT16)g_chBuffer[18] << 4) | (g_chBuffer[19] >> 4)
                , IDC_TEMP_HI_SENSOR2
            );
            // temp sensor 3
            // combine the bytes for param 1
            bit12ToCelsius(((INT16)g_chBuffer[20] << 4) | (g_chBuffer[21] >> 4)
                , IDC_TEMP_SENSOR3
            );
            isolateAlertBit(((INT16)g_chBuffer[22] << 4) | (g_chBuffer[23] >> 4)
                , IDC_ALERT_SENSOR3
            );
            // temp low sensor 2
            bit12ToCelsius(((INT16)g_chBuffer[24] << 4) | (g_chBuffer[25] >> 4)
                , IDC_TEMP_LO_SENSOR3
            );
            // temp high sensor 2
            bit12ToCelsius(((INT16)g_chBuffer[26] << 4) | (g_chBuffer[27] >> 4)
                , IDC_TEMP_HI_SENSOR3
            );
            // temp sensor 4
            // combine the bytes for param 1
            bit12ToCelsius(((INT16)g_chBuffer[28] << 4) | (g_chBuffer[29] >> 4)
                , IDC_TEMP_SENSOR4
            );
            isolateAlertBit(((INT16)g_chBuffer[30] << 4) | (g_chBuffer[31] >> 4)
                , IDC_ALERT_SENSOR4
            );
            // temp low sensor 2
            bit12ToCelsius(((INT16)g_chBuffer[32] << 4) | (g_chBuffer[33] >> 4)
                , IDC_TEMP_LO_SENSOR4
            );
            // temp high sensor 2
            bit12ToCelsius(((INT16)g_chBuffer[34] << 4) | (g_chBuffer[35] >> 4)
                , IDC_TEMP_HI_SENSOR4
            );
            g_oFrame.cmd = WR_DATE_TIME;
            // poll the first data item
            g_bReadFanState = TRUE;
            return EXIT_SUCCESS;
        }
        // the WR_TEMP_RANGE acknowledge is received
        if (g_oFrame.cmd == WR_TEMP_RANGE && g_chBuffer[4] == ACK)
        {
            OutputDebugString(L"WR_TEMP_RANGE acknowledge is received\n");
            g_oFrame.cmd = WR_DATE_TIME;
            return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}
*/