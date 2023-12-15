#pragma once

//****************************************************************************
//*                     global
//****************************************************************************
HANDLE g_hComm = INVALID_HANDLE_VALUE;
// Resource Aquisition Is Initialisation (RAII)
FRAME g_oFrame = { SOH, 0, 0, STX, { '\0' }, ETX, ETB, EOT };

BOOL g_bContinueTxRx = FALSE;

//*****************************************************************************
//*                     prototype
//*****************************************************************************
BOOL                connect();

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

        return (INT_PTR)TRUE;
    } // eof CONNECT_SERIAL
    case DISCONNECT_SERIAL:
    {
        OutputDebugString(L"DISCONNECT_SERIAL\n");

        return (INT_PTR)TRUE;
    } // eof DISCONNECT_SERIAL
    } // eof switch
    
    return (INT_PTR)FALSE;
}
/*
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
        // reset error condition and the transmission counter
        g_bIsSetError = FALSE;
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
    if (!SetupComm(g_hComm, BUFFER_MAX_SERIAL, BUFFER_MAX_SERIAL))
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

