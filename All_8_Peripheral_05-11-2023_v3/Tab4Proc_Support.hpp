#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern HWND g_hWndDlgTab4;
extern CHAR g_chTextBuffer[LEN_MAX_TEXT_BUFFER];

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
    switch (LOWORD(wParam))
    {
    case BTN_RANGE_SENSOR:
    {
        OutputDebugString(L"BTN_RANGE_SENSOR\n");

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

    ///*
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
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_SENSOR1)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );
    //*/

    ///*
    // sensor 2
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
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_SENSOR2)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );
    //*/

    ///*
    // sensor 3
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
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_SENSOR3)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );
    //*/

    ///*
    // sensor 4
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
    SendMessageA(GetDlgItem(g_hWndDlgTab4, IDC_TEMP_SENSOR4)
        , WM_SETTEXT
        , (WPARAM)0
        , (LPARAM)g_chTextBuffer
    );
    //*/

    return EXIT_SUCCESS;
}
