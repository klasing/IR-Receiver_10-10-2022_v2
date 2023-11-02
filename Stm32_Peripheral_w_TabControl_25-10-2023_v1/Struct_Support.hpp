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
	const CHAR soh;
	UINT16 cmd;
	const CHAR stx;
	CHAR payload[LEN_MAX_ENTRY];
	const CHAR etx;
	const CHAR etb;
	const CHAR eot;
} FRAME, *PFRAME;