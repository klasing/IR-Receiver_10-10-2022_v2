#pragma once
//*****************************************************************************
//*                     include
//*****************************************************************************
#include "framework.h"

//*****************************************************************************
//*                     calcCrc8
//*****************************************************************************
BOOL calcCrc8(const UCHAR& ch
	, UINT32& valCrc
)
{
	UINT32 inpData = ch << 24;
	UINT32 initCrc = valCrc;
	valCrc = initCrc ^ inpData;
	UINT8 bindex = 0;
	while (bindex < 8)
	{
		if (valCrc & 0x80000000)
		{
			valCrc <<= 1;
			valCrc ^= POLY_CRC;
		}
		else
		{
			valCrc <<= 1;
		}
		++bindex;
	}
	return EXIT_SUCCESS;
}

//*****************************************************************************
//*                     calcCrc16
//*****************************************************************************
BOOL calcCrc16(const UCHAR& ch1
	, const UCHAR& ch2
	, UINT32& valCrc
)
{
	UINT32 inpData = ch1 << 24
		| ch2 << 16;
	UINT32 initCrc = valCrc;
	valCrc = initCrc ^ inpData;
	UINT8 bindex = 0;
	while (bindex < 16)
	{
		if (valCrc & 0x80000000)
		{
			valCrc <<= 1;
			valCrc ^= POLY_CRC;
		}
		else
		{
			valCrc <<= 1;
		}
		++bindex;
	}
	return EXIT_SUCCESS;
}

//*****************************************************************************
//*                     calcCrcEx
//*****************************************************************************
BOOL calcCrcEx(const UCHAR* chBuffer
	, const UINT& lengthBuffer
	, UINT32& valCrc
)
{
	if (lengthBuffer == 0) return EXIT_FAILURE;
	UINT32 inpData = 0;
	UINT32 initCrc = 0;
	UINT8 bindex = 0;
	UINT i = 0;
	while (i < lengthBuffer / 4)
	{
		inpData = (chBuffer[i * 4 + 0] << 24)
			| (chBuffer[i * 4 + 1] << 16)
			| (chBuffer[i * 4 + 2] << 8)
			| (chBuffer[i * 4 + 3]);
		initCrc = (i == 0) ? 0xFFFFFFFF : valCrc;
		valCrc = initCrc ^ inpData;
		bindex = 0;
		while (bindex < 32)
		{
			if (valCrc & 0x80000000)
			{
				valCrc <<= 1;
				valCrc ^= POLY_CRC;
			}
			else
			{
				valCrc <<= 1;
			}
			++bindex;
		}
		++i;
	}
	switch (lengthBuffer % 4)
	{
	case 0:
	{
		break;
	} // eof 0
	case 1:
	{
		calcCrc8(chBuffer[i * 4 + 0]
			, valCrc
		);
		break;
	} // eof 1
	case 2:
	{
		// 1) calcCrc16();
		calcCrc16(chBuffer[i * 4 + 0]
			, chBuffer[i * 4 + 1]
			, valCrc
		);
		break;

	} // eof 2
	case 3:
	{
		// 1) calcCrc16(); 
		calcCrc16(chBuffer[i * 4 + 0]
			, chBuffer[i * 4 + 1]
			, valCrc
		);
		// 2) calcCrc8();
		calcCrc8(chBuffer[i * 4 + 2]
			, valCrc
		);
		break;

	} // eof 3
	} // eof switch
	return EXIT_SUCCESS;
}
