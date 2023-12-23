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
	const CHAR soh;						// 1
	UINT16 cmd;							// 2
	UINT32 functionCode;				// 4
	const CHAR stx;						// 1
	UINT8 payload[MAX_LEN_ENTRY];		// 32
	const CHAR etx;						// 1
	const CHAR etb;						// 1
	const CHAR eot;						// 1
} FRAME, *PFRAME;						// 43 byte total
