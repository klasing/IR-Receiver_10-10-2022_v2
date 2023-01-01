#pragma once

//****************************************************************************
//*                     include
//****************************************************************************
#include "framework.h"
#include "resource.h"

typedef struct tagFRAME
{
	const CHAR soh;
	UINT16 cmnd;
	const CHAR stx;
	CHAR payload[LEN_MAX_ENTRY]; //INT16 payload;
	const CHAR etx;
	const CHAR etb;
	const CHAR eot;
} FRAME, * PFRAME;
