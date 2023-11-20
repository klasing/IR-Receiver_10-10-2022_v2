#pragma once

//****************************************************************************
//*                     extern
//****************************************************************************
extern Statusbar g_oStatusbar;
extern FRAME g_oFrameTx;
extern CHAR g_chTextBuffer[LEN_MAX_TEXT_BUFFER];

//****************************************************************************
//*                     onWmInitDialog_Tab4Proc
//****************************************************************************
BOOL onWmInitDialog_Tab4Proc(const HWND& hDlg)
{
	// set the focus on the first applicable control
	SendMessage(hDlg
		, WM_NEXTDLGCTL
		, (WPARAM)GetDlgItem(hDlg, IDC_TEMP_HI_SENSOR1)
		, (LPARAM)TRUE
	);

	return EXIT_SUCCESS;
}

//****************************************************************************
//*                     clcsToBit
//****************************************************************************
BOOL clcsToBit(const HWND& hDlg
	, const UINT16 aResourceId[8]
)
{
	FLOAT fTemp = 0.;
	INT16 iTemp;

	for (int s = 0; s < 4; s++)
	{
		// temp low sensor s
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
		g_oFrameTx.payload[s * 4 + 0] = (iTemp & 0x0FF0) >> 4;
		g_oFrameTx.payload[s * 4 + 1] = (iTemp & 0x000F) << 4;

		// temp high sensor s
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
		// temp high sensor 2
		g_oFrameTx.payload[s * 4 + 2] = (iTemp & 0x0FF0) >> 4;
		g_oFrameTx.payload[s * 4 + 3] = (iTemp & 0x000F) << 4;
	}

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
		if (clcsToBit(hDlg, aResourceId) == EXIT_FAILURE)
		{
			g_oStatusbar.setTextStatusbar(3, L"Apply failed");
			// error: do nothiong
		}
		// conversion completed
		OutputDebugString(L"conversion completed\n");
		setRangeSensor(g_oFrameTx);

		g_oFrameTx.cmd = WR_RANGE_SENSOR;
		// payload is already filled
		clear_queue_and_add_one_cmd();

		return (INT_PTR)TRUE;
	} // eof BTN_RANGE_SENSOR
	} // eof switch

	return (INT_PTR)FALSE;
}
