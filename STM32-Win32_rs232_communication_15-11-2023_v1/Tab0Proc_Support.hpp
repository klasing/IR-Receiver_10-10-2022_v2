#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern Statusbar g_oStatusbar;

//****************************************************************************
//*                     global
//****************************************************************************
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
//CHAR g_chTextBuffer[LEN_MAX_TEXT_BUFFER] = { 0 };

//*****************************************************************************
//*                     prototype
//*****************************************************************************
DWORD               WINAPI TxRx(LPVOID lpVoid);
BOOL                connect();

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

            // start the communication by transferring no operation
            // to the STM32
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

        for (int i = 0; i < g_queue.size(); i++) g_queue.pop();

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
        if (g_oFrame.cmd == NOP)
        {
            if (g_oFrame.payload[0] == ACK)
            {
                OutputDebugString(L"NOP ACK\n");
                g_oFrame.cmd = OTHER;
                g_queue.pop();
                g_queue.push(g_oFrame);
                return EXIT_SUCCESS;
            }
            if (g_oFrame.payload[0] == NAK)
            {
                OutputDebugString(L"NOP NAK\n");
                return EXIT_FAILURE;
            }
        }
        if (g_oFrame.cmd == OTHER)
        {
            if (g_oFrame.payload[0] == ACK)
            {
                OutputDebugString(L"OTHER ACK\n");
                g_oFrame.cmd = OTHER1;
                g_queue.pop();
                g_queue.push(g_oFrame);
                return EXIT_SUCCESS;
            }
            if (g_oFrame.payload[0] == NAK)
            {
                OutputDebugString(L"OTHER NAK\n");
                return EXIT_FAILURE;
            }
        }
        if (g_oFrame.cmd == OTHER1)
        {
            if (g_oFrame.payload[0] == ACK)
            {
                OutputDebugString(L"OTHER1 ACK\n");
                return EXIT_SUCCESS;
            }
            if (g_oFrame.payload[0] == NAK)
            {
                OutputDebugString(L"OTHER1 NAK\n");
                return EXIT_FAILURE;
            }
        }
    }

    return EXIT_SUCCESS;
}

//****************************************************************************
//*                     transmit
//****************************************************************************
BOOL transmit(LPVOID lpVoid)
{
    OutputDebugString(L"transmit()\n");

    if (g_queue.empty())
    {
       return EXIT_FAILURE;
    }

    FRAME oFrame = g_queue.front();

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
