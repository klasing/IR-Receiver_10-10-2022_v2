#pragma once

//****************************************************************************
//*                     include
//****************************************************************************
#include "framework.h"

typedef struct tagFRAME
{
	const CHAR soh;
	UINT16 cmnd;
	const CHAR stx;
	INT16 payload;
	const CHAR etx;
	const CHAR etb;
	const CHAR eot;
} FRAME, * PFRAME;

typedef struct tagCONFIGURATION
{
	UINT8 oneShot : 1;
	UINT8 resolution : 2;
	UINT8 faultQueue : 2;
	UINT8 polarity : 1;
	UINT8 modeThermostat : 1;
	UINT8 shutDown : 1;
	UINT8 conversionRate : 2;
	UINT8 alert : 1;
	UINT8 modeExtended : 1;
	UINT8 : 4;
} CONFIGURATION, *PCONFIGURATION;

typedef struct tagBIT12TEMP
{
	INT16 temp : 12;
	FLOAT fTempInClcs = 0.;
	FLOAT fTempInClcsTimes100 = 0.;
	CHAR chBufferTempInCelcius[LEN_TEMP_IN_CLCS] = { 0 };
} BIT12TEMP, *PBIT12TEMP;

typedef struct tagBIT12MSRDTEMP : tagBIT12TEMP
{
	UINT8 alert : 1;
	UINT8 modeExtended : 1;
} BIT12MSRDTEMP, *PBIT12MSRDTEMP;

/*
//****************************************************************************
//*                     typedef
//*
//* when writing to the device, the device will respond with OK in the payload
//* O=79 0x4F 0100 1111
//* K=75 0x4B 0100 1011
//* payload=0x4F4B
//****************************************************************************
typedef struct tagFRAME
{
	const CHAR soh;
	UINT16 cmnd;
	const CHAR stx;
	INT16 payload;
	const CHAR etx;
	const CHAR etb;
	const CHAR eot;
} FRAME, * PFRAME;

// bitfield CONFIGURATION ////////////////////////////////////////////////////
typedef struct tagCONFIGURATION
{
	UINT8 oneShot : 1;
	UINT8 resolution : 2;
	UINT8 faultQueue : 2;
	UINT8 polarity : 1;
	UINT8 modeThermostat : 1;
	UINT8 shutDown : 1;
	UINT8 conversionRate : 2;
	UINT8 alert : 1;
	UINT8 modeExtended : 1;
	UINT8 : 4;
	VOID setHiByte(const UINT8& byte)
	{
		oneShot = (byte & 0x80) >> 7;
		resolution = (byte & 0x60) >> 5;
		faultQueue = (byte & 0x18) >> 3;
		polarity = (byte & 0x04) >> 2;
		modeThermostat = (byte & 0x02) >> 1;
		shutDown = byte & 0x01;
	}
	UINT16 getHiByte()
	{
		UINT16 word = 0;
		word = (oneShot << 7)
			| (resolution << 5)
			| (faultQueue << 3)
			| (polarity << 2)
			| (modeThermostat << 1)
			| shutDown;
		return word;
	}
	VOID setLoByte(const UINT8& byte)
	{
		conversionRate = (byte & 0xC0) >> 6;
		alert = (byte & 0x20) >> 5;
		modeExtended = (byte & 0x10) >> 4;
	}
	UINT16 getLoByte()
	{
		UINT16 word = 0;
		word = (conversionRate << 6)
			| (alert << 5)
			| (modeExtended << 4);
		return word;
	}
	VOID setOneShot(const BOOL& b)
	{
		oneShot = (b) ? TRUE : FALSE;
	}
	BOOL getOneShot()
	{
		return oneShot;
	}
	// read only
	//VOID setResolution(const CHAR& byte)
	//{
	//	resolution = byte & 0x03;
	//}
	UINT8 getResolution()
	{
		return resolution;
	}
	VOID setFaultQueue(const UINT8& byte)
	{
		faultQueue = byte & 0x03;
	}
	UINT8 getFaultQueue()
	{
		return faultQueue;
	}
	VOID setPolarity(const BOOL& b)
	{
		polarity = (b) ? TRUE : FALSE;
	}
	BOOL getPolarity()
	{
		return polarity;
	}
	VOID setModeThermostat(const BOOL& b)
	{
		modeThermostat = (b) ? TRUE : FALSE;
	}
	BOOL getModeThermostat()
	{
		return modeThermostat;
	}
	VOID setShutDown(const BOOL& b)
	{
		shutDown = (b) ? TRUE : FALSE;
	}
	BOOL getShutDown()
	{
		return shutDown;
	}
	VOID setConversionRate(const UINT8& byte)
	{
		conversionRate = byte & 0x03;
	}
	UINT8 getConversionRate()
	{
		return conversionRate;
	}
	// read only
	//VOID setAlert(const BOOL& b)
	//{
	//	alert = (b) ? TRUE : FALSE;
	//}
	BOOL getAlert()
	{
		return alert;
	}
	VOID setModeExtended(const BOOL& b)
	{
		modeExtended = (b) ? TRUE : FALSE;
	}
	BOOL getModeExtended()
	{
		return modeExtended;
	}
} CONFIGURATION, * PCONFIGURATION;

// 12-bit bitfield TEMPERATURE ///////////////////////////////////////////////
typedef struct tagBIT12TEMP
{
	INT16 temp : 12;
	FLOAT fTempInClcs = 0.;
	FLOAT fTempInClcsTimes100 = 0.;
	CHAR chBufferTempInCelcius[LEN_TEMP_IN_CLCS] = { 0 };
	VOID setHiByte(INT8 byte)
	{
		// must be called before setLoByte is called
		temp = byte << 4;
	}
	VOID setLoByte(UINT8 byte)
	{
		temp |= byte >> 4;
	}
	// this member func will also set:
	// fTempInClcs, 
	// fTempInClcsTimes100, and
	// chBufferTempInCelcius
	CHAR* getTempInClcs_toStringA()
	{
		INT16 val = temp;
		if (val & 0x8000)
		{
			val = ~val;
			val += 1;
		}
		fTempInClcs = (FLOAT)(val * 0.0625);
		fTempInClcsTimes100 = fTempInClcs * 100;
		sprintf_s(chBufferTempInCelcius
			, LEN_TEMP_IN_CLCS
			, "%d.%02d"
			, ((UINT)fTempInClcsTimes100 / 100)
			, ((UINT)fTempInClcsTimes100 % 100)
		);
		return chBufferTempInCelcius;
	}
	CHAR* getTempInClcs_toStringA_()
	{
		sprintf_s(chBufferTempInCelcius
			, LEN_TEMP_IN_CLCS
			, "%d.%02d"
			, ((UINT)fTempInClcsTimes100 / 100)
			, ((UINT)fTempInClcsTimes100 % 100)
		);
		return chBufferTempInCelcius;
	}
} BIT12TEMP, * PBIT12TEMP;

// 12-bit bitfield MEASURED TEMPERATURE //////////////////////////////////////
typedef struct tagBIT12MSRDTEMP : tagBIT12TEMP
{
	UINT8 alert : 1;
	UINT8 modeExtended : 1;
	VOID setAlert(UINT8 byte)
	{
		alert = (byte & 0x02) >> 1;
	}
	BOOL getAlert()
	{
		return alert;
	}
	VOID setModeExtended(UINT8 byte)
	{
		modeExtended = byte & 0x01;
	}
	BOOL getModeExtended()
	{
		return modeExtended;
	}
} BIT12MSRDTEMP, * PBIT12MSRDTEMP;

// 13-bit bitfield TEMPERATURE ///////////////////////////////////////////////
*/