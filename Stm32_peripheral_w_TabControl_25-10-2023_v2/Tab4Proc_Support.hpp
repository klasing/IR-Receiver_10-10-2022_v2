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
	, const UINT16 aResourceId[4][2]
)
{
	for (int c = 0; c < 4; c++)
	{
		for (int r = 0; r < 2; r++)
		{
			FLOAT fTemp = 0.;
			SendMessageA(GetDlgItem(hDlg, aResourceId[r][c])
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
			INT16 iTemp = (INT16)fTemp << 4;
			g_oFrameTx.payload[c * 2 + r + 0] = (iTemp & 0xFF00) >> 8;
			g_oFrameTx.payload[c * 2 + r + 1] = (iTemp & 0xFF);
		}
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
	static const UINT16 aResourceId[4][2] = { { IDC_TEMP_LO_SENSOR1, IDC_TEMP_HI_SENSOR1 }
		, { IDC_TEMP_LO_SENSOR2, IDC_TEMP_HI_SENSOR2 }
		, { IDC_TEMP_LO_SENSOR3, IDC_TEMP_HI_SENSOR3 }
		, { IDC_TEMP_LO_SENSOR4, IDC_TEMP_HI_SENSOR4 }
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

		g_oFrameTx.cmd = WR_RANGE_SENSOR;
		// payload is already filled
		clear_queue_and_add_one_cmd();

		return (INT_PTR)TRUE;
	} // eof BTN_RANGE_SENSOR
	} // eof switch

	return (INT_PTR)FALSE;
}
