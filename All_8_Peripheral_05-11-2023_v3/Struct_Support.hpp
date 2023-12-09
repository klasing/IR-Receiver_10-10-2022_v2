#pragma once

//****************************************************************************
//*                     include
//****************************************************************************
#include "framework.h"
#include "resource.h"

//****************************************************************************
//*                     FRAME
//****************************************************************************
typedef struct tagFRAME
{
	const CHAR soh;					// 1
	// the command has gone to 32-bit ( 08-12-2023 )
	UINT32 cmd;						// 4
	/*UINT16 cmd;					// 2*/
	const CHAR stx;					// 1
	UINT8 payload[LEN_MAX_ENTRY];	// 32
	/*UCHAR payload[LEN_MAX_ENTRY];	// 32*/
	/*CHAR payload[LEN_MAX_ENTRY];	// 32*/
	const CHAR etx;					// 1
	const CHAR etb;					// 1
	const CHAR eot;					// 1
} FRAME, * PFRAME;					// 41 byte total
/*} FRAME, * PFRAME;				// 39 byte total*/