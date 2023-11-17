#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern HWND g_hWndDlgTab4;
extern const FLOAT DEFAULT_T_LO;
extern const FLOAT DEFAULT_T_HI;
extern CHAR g_chTextBuffer[LEN_MAX_TEXT_BUFFER];

//****************************************************************************
//*                     setRangeSensor
//****************************************************************************
BOOL setRangeSensor()
{
	OutputDebugString(L"setRangeSensor()\n");
	UINT16 aResourceId[8] = { IDC_TEMP_HI_SENSOR1 
		, IDC_TEMP_HI_SENSOR2
		, IDC_TEMP_HI_SENSOR3
		, IDC_TEMP_HI_SENSOR4
		, IDC_TEMP_LO_SENSOR1
		, IDC_TEMP_LO_SENSOR2
		, IDC_TEMP_LO_SENSOR3
		, IDC_TEMP_LO_SENSOR4
	};
	sprintf_s(g_chTextBuffer, LEN_MAX_TEXT_BUFFER, "%.02f", DEFAULT_T_HI);
	for (int i = 0; i < 4; i++)
	{
		SendMessageA(GetDlgItem(g_hWndDlgTab4, aResourceId[i])
			, WM_SETTEXT
			, (WPARAM)0
			, (LPARAM)g_chTextBuffer
		);
	}
	sprintf_s(g_chTextBuffer, LEN_MAX_TEXT_BUFFER, "%.02f", DEFAULT_T_LO);
	for (int i = 4; i < 8; i++)
	{
		SendMessageA(GetDlgItem(g_hWndDlgTab4, aResourceId[i])
			, WM_SETTEXT
			, (WPARAM)0
			, (LPARAM)g_chTextBuffer
		);
	}
	return EXIT_SUCCESS;
}