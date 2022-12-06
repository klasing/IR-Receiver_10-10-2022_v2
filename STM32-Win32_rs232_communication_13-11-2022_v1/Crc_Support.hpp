#pragma once
//*****************************************************************************
//*                     include
//*****************************************************************************
#include "framework.h"

//*****************************************************************************
//*                     calcCrc8
//*****************************************************************************
BOOL calcCrc8(const CHAR ch
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
//*                     calcCrcEx
//*****************************************************************************
BOOL calcCrcEx(const CHAR* chBuffer
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
		// TODO: calcCrc16();
		break;

	} // eof 2
	case 3:
	{
		// TODO:
		// 1) calcCrc16(); 
		// 2) calcCrc8();
		break;

	} // eof 3
	} // eof switch
	return EXIT_SUCCESS;
}
