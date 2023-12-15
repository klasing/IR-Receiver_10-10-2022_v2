#pragma once

//****************************************************************************
//*                     include
//****************************************************************************
#include "framework.h"
#include "resource.h"

//****************************************************************************
//*                     frame
//****************************************************************************
typedef struct tagFRAME
{
	const UINT8 soh;					// 1
	UINT16 cmd;							// 2
	UINT32 functionCode;				// 4
	const UINT8 stx;					// 1
	UINT8 payload[MAX_LEN_ENTRY];		// 32
	const UINT8 etx;					// 1
	const UINT8 etb;					// 1
	const UINT8 eot;					// 1
} FRAME, *PFRAME;						// 43 byte total
