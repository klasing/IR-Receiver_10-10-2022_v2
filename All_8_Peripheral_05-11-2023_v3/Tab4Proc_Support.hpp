#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern HWND g_hWndDlgTab4;
extern CHAR g_chTextBuffer[LEN_MAX_TEXT_BUFFER];
extern FRAME g_oFrame;

//****************************************************************************
//*                     prototype
//****************************************************************************
BOOL clcsToBit(const HWND& hDlg
    , const UINT16 aResourceId[8]
    , CHAR payload[]
);

//****************************************************************************
//*                     onWmInitDialog_Tab4Proc
//****************************************************************************
BOOL onWmInitDialog_Tab4Proc(const HWND& hDlg)
{

    return EXIT_SUCCESS;
}

//****************************************************************************
//*                     onWmCommand_Tab4Proc
//****************************************************************************
INT_PTR onWmCommand_Tab4Proc(const HWND& hDlg
    , const WPARAM& wParam
)
{
    static const UINT16 aResourceId[8] = { IDC_TEMP_LO_SENSOR1
        , IDC_TEMP_HI_SENSOR1
        , IDC_TEMP_LO_SENSOR2
        , IDC_TEMP_HI_SENSOR2
        , IDC_TEMP_LO_SENSOR3
        , IDC_TEMP_HI_SENSOR3
        , IDC_TEMP_LO_SENSOR4
        , IDC_TEMP_HI_SENSOR4
    };

    switch (LOWORD(wParam))
    {
    case BTN_RANGE_SENSOR:
    {
        OutputDebugString(L"BTN_RANGE_SENSOR\n");
        FLOAT fTemp = 0.;
        UINT16 iTemp;

        // 1.L) low range temp sensor
        SendMessageA(GetDlgItem(hDlg, IDC_TEMP_LO_SENSOR1)
            , WM_GETTEXT
            , (LPARAM)LEN_MAX_TEXT_BUFFER
            , (WPARAM)g_chTextBuffer
        );
        // try to convert string to float
        try
        {
            fTemp = std::stof(g_chTextBuffer);
        }
        catch (const std::exception e)
        {
            // no transmission
            return (INT_PTR)TRUE;
        }
        // conversion succeeded
        fTemp /= 0.0625;
        iTemp = (UINT16)fTemp;
        g_oFrame.payload[0] = (iTemp & 0x0FF0) >> 4;
        g_oFrame.payload[1] = (iTemp & 0x000F) << 4;
        /*
        iTemp = (INT16)fTemp;
        // no 2's complement calculation
        g_oFrame.payload[0] = (iTemp & 0x0FF0) >> 4;
        // shift left 4 to get 12-bit left justified
        g_oFrame.payload[1] = (iTemp & 0x000F) << 4;
        */

        // 1.H) high range temp sensor
        SendMessageA(GetDlgItem(hDlg, IDC_TEMP_HI_SENSOR1)
            , WM_GETTEXT
            , (LPARAM)LEN_MAX_TEXT_BUFFER
            , (WPARAM)g_chTextBuffer
        );
        // try to convert string to float
        try
        {
            fTemp = std::stof(g_chTextBuffer);
        }
        catch (const std::exception e)
        {
            // no transmission
            return (INT_PTR)TRUE;
        }
        // conversion succeeded
        fTemp /= 0.0625;
        iTemp = (UINT16)fTemp;
        g_oFrame.payload[2] = (iTemp & 0x0FF0) >> 4;
        g_oFrame.payload[3] = (iTemp & 0x000F) << 4;
        /*
        iTemp = (INT16)fTemp;
        // no 2's complement calculation
        g_oFrame.payload[2] = (iTemp & 0x0FF0) >> 4;
        // shift left 4 to get 12-bit left justified
        g_oFrame.payload[3] = (iTemp & 0x000F) << 4;
        */

        /*
        // 2.L) low range temp sensor
        SendMessageA(GetDlgItem(hDlg, IDC_TEMP_LO_SENSOR2)
            , WM_GETTEXT
            , (LPARAM)LEN_MAX_TEXT_BUFFER
            , (WPARAM)g_chTextBuffer
        );
        // try to convert string to float
        try
        {
            fTemp = std::stof(g_chTextBuffer);
        }
        catch (const std::exception e)
        {
            // no transmission
            return (INT_PTR)TRUE;
        }
        // conversion succeeded
        fTemp /= 0.0625;
        iTemp = (INT16)fTemp;
        // no 2's complement calculation
        g_oFrame.payload[4] = (iTemp & 0x0FF0) >> 4;
        // shift left 4 to get 12-bit left justified
        g_oFrame.payload[5] = (iTemp & 0x000F) << 4;

        // 2.H) high range temp sensor
        SendMessageA(GetDlgItem(hDlg, IDC_TEMP_HI_SENSOR2)
            , WM_GETTEXT
            , (LPARAM)LEN_MAX_TEXT_BUFFER
            , (WPARAM)g_chTextBuffer
        );
        // try to convert string to float
        try
        {
            fTemp = std::stof(g_chTextBuffer);
        }
        catch (const std::exception e)
        {
            // no transmission
            return (INT_PTR)TRUE;
        }
        // conversion succeeded
        fTemp /= 0.0625;
        iTemp = (INT16)fTemp;
        // no 2's complement calculation
        g_oFrame.payload[6] = (iTemp & 0x0FF0) >> 4;
        // shift left 4 to get 12-bit left justified
        g_oFrame.payload[7] = (iTemp & 0x000F) << 4;

        // 3.L) low range temp sensor
        SendMessageA(GetDlgItem(hDlg, IDC_TEMP_LO_SENSOR3)
            , WM_GETTEXT
            , (LPARAM)LEN_MAX_TEXT_BUFFER
            , (WPARAM)g_chTextBuffer
        );
        // try to convert string to float
        try
        {
            fTemp = std::stof(g_chTextBuffer);
        }
        catch (const std::exception e)
        {
            // no transmission
            return (INT_PTR)TRUE;
        }
        // conversion succeeded
        fTemp /= 0.0625;
        iTemp = (INT16)fTemp;
        // no 2's complement calculation
        g_oFrame.payload[8] = (iTemp & 0x0FF0) >> 4;
        // shift left 4 to get 12-bit left justified
        g_oFrame.payload[9] = (iTemp & 0x000F) << 4;

        // 3.H) high range temp sensor
        SendMessageA(GetDlgItem(hDlg, IDC_TEMP_HI_SENSOR3)
            , WM_GETTEXT
            , (LPARAM)LEN_MAX_TEXT_BUFFER
            , (WPARAM)g_chTextBuffer
        );
        // try to convert string to float
        try
        {
            fTemp = std::stof(g_chTextBuffer);
        }
        catch (const std::exception e)
        {
            // no transmission
            return (INT_PTR)TRUE;
        }
        // conversion succeeded
        fTemp /= 0.0625;
        iTemp = (INT16)fTemp;
        // no 2's complement calculation
        g_oFrame.payload[10] = (iTemp & 0x0FF0) >> 4;
        // shift left 4 to get 12-bit left justified
        g_oFrame.payload[11] = (iTemp & 0x000F) << 4;

        // 4.L) low range temp sensor
        SendMessageA(GetDlgItem(hDlg, IDC_TEMP_LO_SENSOR4)
            , WM_GETTEXT
            , (LPARAM)LEN_MAX_TEXT_BUFFER
            , (WPARAM)g_chTextBuffer
        );
        // try to convert string to float
        try
        {
            fTemp = std::stof(g_chTextBuffer);
        }
        catch (const std::exception e)
        {
            // no transmission
            return (INT_PTR)TRUE;
        }
        // conversion succeeded
        fTemp /= 0.0625;
        iTemp = (INT16)fTemp;
        // no 2's complement calculation
        g_oFrame.payload[12] = (iTemp & 0x0FF0) >> 4;
        // shift left 4 to get 12-bit left justified
        g_oFrame.payload[13] = (iTemp & 0x000F) << 4;

        // 4.H) high range temp sensor
        SendMessageA(GetDlgItem(hDlg, IDC_TEMP_HI_SENSOR4)
            , WM_GETTEXT
            , (LPARAM)LEN_MAX_TEXT_BUFFER
            , (WPARAM)g_chTextBuffer
        );
        // try to convert string to float
        try
        {
            fTemp = std::stof(g_chTextBuffer);
        }
        catch (const std::exception e)
        {
            // no transmission
            return (INT_PTR)TRUE;
        }
        // conversion succeeded
        fTemp /= 0.0625;
        iTemp = (INT16)fTemp;
        // no 2's complement calculation
        g_oFrame.payload[14] = (iTemp & 0x0FF0) >> 4;
        // shift left 4 to get 12-bit left justified
        g_oFrame.payload[15] = (iTemp & 0x000F) << 4;
        */

        // kill timer
        KillTimer(g_hWndDlgTab0, IDT_TIMER);
        g_oFrame.cmd = WR_RANGE_SENSOR;
        g_queue.push(g_oFrame);
        // set timer
        SetTimer(g_hWndDlgTab0
            , IDT_TIMER
            , DELAY_HALFHZ_SERIAL
            , (TIMERPROC)TimerProc
        );

        return (INT_PTR)TRUE;
    } // eof BTN_RANGE_SENSOR
    } // eof switch

    return (INT_PTR)FALSE;
}

//****************************************************************************
//*                     setRangeSensor
//****************************************************************************
BOOL setRangeSensor(const FRAME& oFrame)
{
    INT16 val;
    FLOAT fTempInCelsius;
    FLOAT fTempInCelsiusTimes100;

    // sensor 1
    val = oFrame.payload[1] << 4 | oFrame.payload[2] >> 4;
    if (val & 0x8000)
    {
        val = ~val;
        val += 1;
    }
    fTempInCelsius = (FLOAT)(val * 0.0625);
    fTempInCelsiusTimes100 = fTempInCelsius * 100.;
    sprintf_s(g_chTextBuffer
        , LEN_MAX_TEXT_BUFFER
        , "%d.%02d"
        , (INT)fTempInCelsiusTimes100 / 100
        , (UINT)fTempInCelsiusTimes100 % 100
    );
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_LO_SENSOR1)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );

    val = oFrame.payload[3] << 4 | oFrame.payload[4] >> 4;
    if (val & 0x8000)
    {
        val = ~val;
        val += 1;
    }
    fTempInCelsius = (FLOAT)(val * 0.0625);
    fTempInCelsiusTimes100 = fTempInCelsius * 100.;
    sprintf_s(g_chTextBuffer
        , LEN_MAX_TEXT_BUFFER
        , "%d.%02d"
        , (INT)fTempInCelsiusTimes100 / 100
        , (UINT)fTempInCelsiusTimes100 % 100
    );
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_HI_SENSOR1)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );

    /*
    // sensor 2
    val = oFrame.payload[5] << 4 | oFrame.payload[6] >> 4;
    if (val & 0x8000)
    {
        val = ~val;
        val += 1;
    }
    fTempInCelsius = (FLOAT)(val * 0.0625);
    fTempInCelsiusTimes100 = fTempInCelsius * 100.;
    sprintf_s(g_chTextBuffer
        , LEN_MAX_TEXT_BUFFER
        , "%d.%02d"
        , (INT)fTempInCelsiusTimes100 / 100
        , (UINT)fTempInCelsiusTimes100 % 100
    );
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_LO_SENSOR2)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );

    val = oFrame.payload[7] << 4 | oFrame.payload[8] >> 4;
    if (val & 0x8000)
    {
        val = ~val;
        val += 1;
    }
    fTempInCelsius = (FLOAT)(val * 0.0625);
    fTempInCelsiusTimes100 = fTempInCelsius * 100.;
    sprintf_s(g_chTextBuffer
        , LEN_MAX_TEXT_BUFFER
        , "%d.%02d"
        , (INT)fTempInCelsiusTimes100 / 100
        , (UINT)fTempInCelsiusTimes100 % 100
    );
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_HI_SENSOR2)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );

    // sensor 3
    val = oFrame.payload[9] << 4 | oFrame.payload[10] >> 4;
    if (val & 0x8000)
    {
        val = ~val;
        val += 1;
    }
    fTempInCelsius = (FLOAT)(val * 0.0625);
    fTempInCelsiusTimes100 = fTempInCelsius * 100.;
    sprintf_s(g_chTextBuffer
        , LEN_MAX_TEXT_BUFFER
        , "%d.%02d"
        , (INT)fTempInCelsiusTimes100 / 100
        , (UINT)fTempInCelsiusTimes100 % 100
    );
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_LO_SENSOR3)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );

    val = oFrame.payload[11] << 4 | oFrame.payload[12] >> 4;
    if (val & 0x8000)
    {
        val = ~val;
        val += 1;
    }
    fTempInCelsius = (FLOAT)(val * 0.0625);
    fTempInCelsiusTimes100 = fTempInCelsius * 100.;
    sprintf_s(g_chTextBuffer
        , LEN_MAX_TEXT_BUFFER
        , "%d.%02d"
        , (INT)fTempInCelsiusTimes100 / 100
        , (UINT)fTempInCelsiusTimes100 % 100
    );
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_HI_SENSOR3)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );

    // sensor 4
    val = oFrame.payload[13] << 4 | oFrame.payload[14] >> 4;
    if (val & 0x8000)
    {
        val = ~val;
        val += 1;
    }
    fTempInCelsius = (FLOAT)(val * 0.0625);
    fTempInCelsiusTimes100 = fTempInCelsius * 100.;
    sprintf_s(g_chTextBuffer
        , LEN_MAX_TEXT_BUFFER
        , "%d.%02d"
        , (INT)fTempInCelsiusTimes100 / 100
        , (UINT)fTempInCelsiusTimes100 % 100
    );
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_LO_SENSOR4)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );

    val = oFrame.payload[15] << 4 | oFrame.payload[16] >> 4;
    if (val & 0x8000)
    {
        val = ~val;
        val += 1;
    }
    fTempInCelsius = (FLOAT)(val * 0.0625);
    fTempInCelsiusTimes100 = fTempInCelsius * 100.;
    sprintf_s(g_chTextBuffer
        , LEN_MAX_TEXT_BUFFER
        , "%d.%02d"
        , (INT)fTempInCelsiusTimes100 / 100
        , (UINT)fTempInCelsiusTimes100 % 100
    );
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_HI_SENSOR4)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );
    */

    return EXIT_SUCCESS;
}

//****************************************************************************
//*                     setTempSensor
//****************************************************************************
BOOL setTempSensor(const FRAME& oFrame)
{
    INT16 val;
    FLOAT fTempInCelsius;
    FLOAT fTempInCelsiusTimes100;

    // sensor 1
    // the INT8/UINT8 casting is IMPORTANT!!!
    val = ((INT8)oFrame.payload[1] << 4) | ((UINT8)oFrame.payload[2] >> 4);
    if (val & 0x8000)
    {
        val = ~val;
        val += 1;
    }
    fTempInCelsius = (FLOAT)(val * 0.0625);
    fTempInCelsiusTimes100 = fTempInCelsius * 100.;
    sprintf_s(g_chTextBuffer
        , LEN_MAX_TEXT_BUFFER
        , "%d.%02d"
        , (INT)fTempInCelsiusTimes100 / 100
        , (UINT)fTempInCelsiusTimes100 % 100
    );
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_SENSOR1)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );
    // alert bit
	((oFrame.payload[2] & 2) == 2) ?
		SendMessage(GetDlgItem(g_hWndDlgTab4, IDC_ALERT_SENSOR1)
			, BM_SETCHECK
			, (WPARAM)BST_CHECKED
			, (LPARAM)0
		)
		:
		SendMessage(GetDlgItem(g_hWndDlgTab4, IDC_ALERT_SENSOR1)
			, BM_SETCHECK
			, (WPARAM)BST_UNCHECKED
			, (LPARAM)0
		);

    /*
    // sensor 2
    // the INT8/UINT8 casting is IMPORTANT!!!
    val = ((INT8)oFrame.payload[3] << 4) | ((UINT8)oFrame.payload[4] >> 4);
    if (val & 0x8000)
    {
        val = ~val;
        val += 1;
    }
    fTempInCelsius = (FLOAT)(val * 0.0625);
    fTempInCelsiusTimes100 = fTempInCelsius * 100.;
    sprintf_s(g_chTextBuffer
        , LEN_MAX_TEXT_BUFFER
        , "%d.%02d"
        , (INT)fTempInCelsiusTimes100 / 100
        , (UINT)fTempInCelsiusTimes100 % 100
    );
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_SENSOR2)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );    
    // alert bit
    ((oFrame.payload[4] & 2) == 2) ?
        SendMessage(GetDlgItem(g_hWndDlgTab4, IDC_ALERT_SENSOR2)
            , BM_SETCHECK
            , (WPARAM)BST_CHECKED
            , (LPARAM)0
        )
        :
        SendMessage(GetDlgItem(g_hWndDlgTab4, IDC_ALERT_SENSOR2)
            , BM_SETCHECK
            , (WPARAM)BST_UNCHECKED
            , (LPARAM)0
        );    

    // sensor 3
    // the INT8/UINT8 casting is IMPORTANT!!!
    val = ((INT8)oFrame.payload[5] << 4) | ((UINT8)oFrame.payload[6] >> 4);
    if (val & 0x8000)
    {
        val = ~val;
        val += 1;
    }
    fTempInCelsius = (FLOAT)(val * 0.0625);
    fTempInCelsiusTimes100 = fTempInCelsius * 100.;
    sprintf_s(g_chTextBuffer
        , LEN_MAX_TEXT_BUFFER
        , "%d.%02d"
        , (INT)fTempInCelsiusTimes100 / 100
        , (UINT)fTempInCelsiusTimes100 % 100
    );
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_SENSOR3)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );    
    // alert bit
    ((oFrame.payload[6] & 2) == 2) ?
        SendMessage(GetDlgItem(g_hWndDlgTab4, IDC_ALERT_SENSOR3)
            , BM_SETCHECK
            , (WPARAM)BST_CHECKED
            , (LPARAM)0
        )
        :
        SendMessage(GetDlgItem(g_hWndDlgTab4, IDC_ALERT_SENSOR3)
            , BM_SETCHECK
            , (WPARAM)BST_UNCHECKED
            , (LPARAM)0
        );    

    // sensor 4
    // the INT8/UINT8 casting is IMPORTANT!!!
    val = ((INT8)oFrame.payload[7] << 4) | ((UINT8)oFrame.payload[8] >> 4);
    if (val & 0x8000)
    {
        val = ~val;
        val += 1;
    }
    fTempInCelsius = (FLOAT)(val * 0.0625);
    fTempInCelsiusTimes100 = fTempInCelsius * 100.;
    sprintf_s(g_chTextBuffer
        , LEN_MAX_TEXT_BUFFER
        , "%d.%02d"
        , (INT)fTempInCelsiusTimes100 / 100
        , (UINT)fTempInCelsiusTimes100 % 100
    );
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_SENSOR4)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );    
    // alert bit
    ((oFrame.payload[8] & 2) == 2) ?
        SendMessage(GetDlgItem(g_hWndDlgTab4, IDC_ALERT_SENSOR4)
            , BM_SETCHECK
            , (WPARAM)BST_CHECKED
            , (LPARAM)0
        )
        :
        SendMessage(GetDlgItem(g_hWndDlgTab4, IDC_ALERT_SENSOR4)
            , BM_SETCHECK
            , (WPARAM)BST_UNCHECKED
            , (LPARAM)0
        );
    */

    return EXIT_SUCCESS;
}

//****************************************************************************
//*                     clcsToBit
//****************************************************************************
/*
BOOL clcsToBit(const HWND& hDlg
    , const UINT16 aResourceId[8]
    , CHAR payload[]
)
{
    FLOAT fTemp = 0.;
    INT16 iTemp;

    for (int s = 0; s < 4; s++)
    {
        // 1) temp low sensor s
        SendMessageA(GetDlgItem(hDlg, aResourceId[s * 2 + 0])
            , WM_GETTEXT
            , (LPARAM)LEN_MAX_TEXT_BUFFER
            , (WPARAM)g_chTextBuffer
        );
        // try to convert string to float
        try
        {
            fTemp = std::stof(g_chTextBuffer);
        }
        catch (const std::exception e)
        {
            return EXIT_FAILURE;
        }
        // conversion succeeded
        fTemp /= 0.0625;
        iTemp = (INT16)fTemp;
        // no 2's complement calculation
        payload[s * 4 + 0] = (iTemp & 0x0FF0) >> 4;
        // shift left 4 to get 12-bit left justified
        payload[s * 4 + 1] = (iTemp & 0x000F) << 4;

        // 2) temp high sensor s
        SendMessageA(GetDlgItem(hDlg, aResourceId[s * 2 + 1])
            , WM_GETTEXT
            , (LPARAM)LEN_MAX_TEXT_BUFFER
            , (WPARAM)g_chTextBuffer
        );
        // try to convert string to float
        try
        {
            fTemp = std::stof(g_chTextBuffer);
        }
        catch (const std::exception e)
        {
            return EXIT_FAILURE;
        }
        // conversion succeeded
        fTemp /= 0.0625;
        iTemp = (INT16)fTemp;
        // no 2's complement calculation
        payload[s * 4 + 0] = (iTemp & 0x0FF0) >> 4;
        // shift left 4 to get 12-bit left justified
        payload[s * 4 + 1] = (iTemp & 0x000F) << 4;
    }

    return EXIT_SUCCESS;
}
*/