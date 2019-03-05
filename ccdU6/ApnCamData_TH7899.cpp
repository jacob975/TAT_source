/////////////////////////////////////////////////////////////
//
// ApnCamData_TH7899.cpp:  Implementation file for the CApnCamData_TH7899 class.
//
// Copyright (c) 2003, 2004 Apogee Instruments, Inc.
//
/////////////////////////////////////////////////////////////

#include "ApnCamData_TH7899.h"

#include <stdlib.h>
#include <malloc.h>
#include <string.h>


/////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////


CApnCamData_TH7899::CApnCamData_TH7899()
{
}


CApnCamData_TH7899::~CApnCamData_TH7899()
{
}


void CApnCamData_TH7899::Initialize()
{
	strcpy( m_Sensor, "TH7899" );
	strcpy( m_CameraModel, "10" );
	m_CameraId = 14;
	m_InterlineCCD = false;
	m_SupportsSerialA = true;
	m_SupportsSerialB = true;
	m_SensorTypeCCD = true;
	m_TotalColumns = 2110;
	m_ImagingColumns = 2048;
	m_ClampColumns = 25;
	m_PreRoiSkipColumns = 5;
	m_PostRoiSkipColumns = 5;
	m_OverscanColumns = 27;
	m_TotalRows = 2054;
	m_ImagingRows = 2048;
	m_UnderscanRows = 3;
	m_OverscanRows = 3;
	m_VFlushBinning = 4;
	m_EnableSingleRowOffset = false;
	m_RowOffsetBinning = 1;
	m_HFlushDisable = false;
	m_ShutterCloseDelay = 500;
	m_PixelSizeX = 14;
	m_PixelSizeY = 14;
	m_Color = false;
	m_ReportedGainSixteenBit = 2;
	m_MinSuggestedExpTime = 20.0;
	m_CoolingSupported = true;
	m_RegulatedCoolingSupported = true;
	m_TempSetPoint = -20.0;
	m_TempRampRateOne = 700;
	m_TempRampRateTwo = 4000;
	m_TempBackoffPoint = 2.0;
	m_PrimaryADType = ApnAdType_Alta_Sixteen;
	m_AlternativeADType = ApnAdType_Alta_Twelve;
	m_DefaultGainTwelveBit = 100;
	m_DefaultOffsetTwelveBit = 255;
	m_DefaultRVoltage = 1000;

	set_vpattern();

	set_hpattern_clamp_sixteen();
	set_hpattern_skip_sixteen();
	set_hpattern_roi_sixteen();

	set_hpattern_clamp_twelve();
	set_hpattern_skip_twelve();
	set_hpattern_roi_twelve();
}


void CApnCamData_TH7899::set_vpattern()
{
	const unsigned short Mask = 0x0;
	const unsigned short NumElements = 24;
	unsigned short Pattern[NumElements] = 
	{
		0x0000, 0x0018, 0x0018, 0x001A, 0x001A, 0x0012, 0x0012, 0x0016, 0x0016, 0x0006, 
		0x0006, 0x000E, 0x000E, 0x000C, 0x000C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
		0x0000, 0x0200, 0x0201, 0x0200
	};

	m_VerticalPattern.Mask = Mask;
	m_VerticalPattern.NumElements = NumElements;
	m_VerticalPattern.PatternData = 
		(unsigned short *)malloc(NumElements * sizeof(unsigned short));

	for ( int i=0; i<NumElements; i++ )
	{
		m_VerticalPattern.PatternData[i] = Pattern[i];
	}
}


void CApnCamData_TH7899::set_hpattern_skip_sixteen()
{
	const unsigned short Mask = 0x1E;
	const unsigned short BinningLimit = 1;
	const unsigned short RefNumElements = 25;
	const unsigned short SigNumElements = 24;

	unsigned short RefPatternData[RefNumElements] = 
	{
		0x006A, 0x007A, 0x007A, 0x0078, 0x0078, 0x1078, 0x1078, 0x1078, 0x0078, 0x0058, 
		0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 
		0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8
	};

	unsigned short SigPatternData[SigNumElements] = 
	{
		0x0044, 0x0044, 0x0004, 0x0004, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 
		0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0004, 
		0x0404, 0x0004, 0x0005, 0x0074
	};

	unsigned short BinNumElements[APN_MAX_HBINNING] = 
	{
		0x0002
	};

	unsigned short BinPatternData[1][256] = {
	{
		0x0058, 0x0044
	} };

	set_hpattern(	&m_SkipPatternSixteen,
					Mask,
					BinningLimit,
					RefNumElements,
					SigNumElements,
					BinNumElements,
					RefPatternData,
					SigPatternData,
					BinPatternData );
}


void CApnCamData_TH7899::set_hpattern_clamp_sixteen()
{
	const unsigned short Mask = 0x1E;
	const unsigned short BinningLimit = 1;
	const unsigned short RefNumElements = 25;
	const unsigned short SigNumElements = 24;

	unsigned short RefPatternData[RefNumElements] = 
	{
		0x006A, 0x007A, 0x007A, 0x0078, 0x0078, 0x1078, 0x1078, 0x1078, 0x0078, 0x0058, 
		0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 
		0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8
	};

	unsigned short SigPatternData[SigNumElements] = 
	{
		0x0044, 0x0044, 0x0004, 0x0004, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 
		0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0004, 
		0x0404, 0x0004, 0x0005, 0x0074
	};

	unsigned short BinNumElements[APN_MAX_HBINNING] = 
	{
		0x0002
	};

	unsigned short BinPatternData[1][256] = {
	{
		0x0058, 0x0044
	} };

	set_hpattern(	&m_ClampPatternSixteen,
					Mask,
					BinningLimit,
					RefNumElements,
					SigNumElements,
					BinNumElements,
					RefPatternData,
					SigPatternData,
					BinPatternData );
}


void CApnCamData_TH7899::set_hpattern_roi_sixteen()
{
	const unsigned short Mask = 0x1E;
	const unsigned short BinningLimit = 10;
	const unsigned short RefNumElements = 25;
	const unsigned short SigNumElements = 27;

	unsigned short RefPatternData[RefNumElements] = 
	{
		0x006A, 0x007A, 0x007A, 0x007A, 0x1078, 0x1078, 0x1078, 0x0078, 0x00F8, 0x00D8, 
		0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8, 
		0x00D8, 0x00D8, 0x00D8, 0x00D8, 0x00D8
	};

	unsigned short SigPatternData[SigNumElements] = 
	{
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0144, 0x0104, 0x0104, 
		0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 0x0104, 
		0x0104, 0x0104, 0x0104, 0x0104, 0x8004, 0x8015, 0x0414
	};

	unsigned short BinNumElements[APN_MAX_HBINNING] = 
	{
		0x0002, 0x002A, 0x0052, 0x007A, 0x00A2, 0x00CA, 0x00B6, 0x00C6, 0x00A2, 0x00B6
	};

	unsigned short BinPatternData[10][256] = {
	{
		0x0058, 0x0054
	},
	{
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044
	},
	{
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044
	},
	{
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044
	},
	{
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044
	},
	{
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044
	},
	{
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044
	},
	{
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0044, 0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0044
	},
	{
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044
	},
	{
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 
		0x0044, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 
		0x0048, 0x0044
	} };

	set_hpattern(	&m_RoiPatternSixteen,
					Mask,
					BinningLimit,
					RefNumElements,
					SigNumElements,
					BinNumElements,
					RefPatternData,
					SigPatternData,
					BinPatternData );
}


void CApnCamData_TH7899::set_hpattern_skip_twelve()
{
	const unsigned short Mask = 0x0;
	const unsigned short BinningLimit = 1;
	const unsigned short RefNumElements = 0;
	const unsigned short SigNumElements = 0;

	unsigned short *RefPatternData = NULL;

	unsigned short *SigPatternData = NULL;

	unsigned short BinNumElements[APN_MAX_HBINNING] = 
	{
		0x000B
	};

	unsigned short BinPatternData[1][256] = {
	{
		0x000A, 0x021A, 0x0018, 0x0018, 0x0018, 0x2018, 0x0014, 0x0004, 0x0004, 0x0005, 
		0x4014
	} };

	set_hpattern(	&m_SkipPatternTwelve,
					Mask,
					BinningLimit,
					RefNumElements,
					SigNumElements,
					BinNumElements,
					RefPatternData,
					SigPatternData,
					BinPatternData );
}


void CApnCamData_TH7899::set_hpattern_clamp_twelve()
{
	const unsigned short Mask = 0x0;
	const unsigned short BinningLimit = 1;
	const unsigned short RefNumElements = 0;
	const unsigned short SigNumElements = 0;

	unsigned short *RefPatternData = NULL;

	unsigned short *SigPatternData = NULL;

	unsigned short BinNumElements[APN_MAX_HBINNING] = 
	{
		0x000C
	};

	unsigned short BinPatternData[1][256] = {
	{
		0x080A, 0x081A, 0x0818, 0x0818, 0x0818, 0x0818, 0x0814, 0x0804, 0x0804, 0x0804, 
		0x0805, 0x0814
	} };

	set_hpattern(	&m_ClampPatternTwelve,
					Mask,
					BinningLimit,
					RefNumElements,
					SigNumElements,
					BinNumElements,
					RefPatternData,
					SigPatternData,
					BinPatternData );
}


void CApnCamData_TH7899::set_hpattern_roi_twelve()
{
	const unsigned short Mask = 0x681E;
	const unsigned short BinningLimit = 10;
	const unsigned short RefNumElements = 0;
	const unsigned short SigNumElements = 0;

	unsigned short *RefPatternData = NULL;

	unsigned short *SigPatternData = NULL;

	unsigned short BinNumElements[APN_MAX_HBINNING] = 
	{
		0x000A, 0x0011, 0x0019, 0x0021, 0x0029, 0x0031, 0x0039, 0x0041, 0x0049, 0x0051
	};

	unsigned short BinPatternData[10][256] = {
	{
		0x000A, 0x021A, 0x0018, 0x0018, 0x0018, 0x0014, 0x2004, 0x0004, 0x8005, 0xC004
	},
	{
		0x0014, 0x021A, 0x0018, 0x0018, 0x0018, 0x2014, 0x0014, 0x0014, 0x0014, 0x0018, 
		0x0018, 0x0018, 0x0018, 0x0014, 0x0004, 0x8005, 0xC004
	},
	{
		0x0004, 0x020A, 0x0008, 0x0008, 0x0008, 0x2004, 0x0004, 0x0004, 0x0004, 0x0008, 
		0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 
		0x0008, 0x0004, 0x0004, 0x8005, 0xC004
	},
	{
		0x0004, 0x020A, 0x0008, 0x0008, 0x0008, 0x2004, 0x0004, 0x0004, 0x0004, 0x0008, 
		0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 
		0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 
		0x0004, 0x8005, 0xC004
	},
	{
		0x0004, 0x020A, 0x0008, 0x0008, 0x0008, 0x2004, 0x0004, 0x0004, 0x0004, 0x0008, 
		0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 
		0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 
		0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x8005, 
		0xC004
	},
	{
		0x0004, 0x020A, 0x0008, 0x0008, 0x0008, 0x2004, 0x0004, 0x0004, 0x0004, 0x0008, 
		0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 
		0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 
		0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 
		0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x8005, 0xC004
	},
	{
		0x0004, 0x020A, 0x0008, 0x0008, 0x0008, 0x2004, 0x0004, 0x0004, 0x0004, 0x0008, 
		0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 
		0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 
		0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 
		0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 
		0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x8005, 0xC004
	},
	{
		0x0004, 0x020A, 0x0008, 0x0008, 0x0008, 0x2004, 0x0004, 0x0004, 0x0004, 0x0008, 
		0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 
		0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 
		0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 
		0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 
		0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 
		0x0008, 0x0004, 0x0004, 0x8005, 0xC004
	},
	{
		0x0004, 0x020A, 0x0008, 0x0008, 0x0008, 0x2004, 0x0004, 0x0004, 0x0004, 0x0008, 
		0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 
		0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 
		0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 
		0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 
		0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 
		0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 
		0x0004, 0x8005, 0xC004
	},
	{
		0x0004, 0x020A, 0x0008, 0x0008, 0x0008, 0x2004, 0x0004, 0x0004, 0x0004, 0x0008, 
		0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 
		0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 
		0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 
		0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 
		0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 
		0x0008, 0x0004, 0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 
		0x0004, 0x0004, 0x0004, 0x0008, 0x0008, 0x0008, 0x0008, 0x0004, 0x0004, 0x8005, 
		0xC004
	} };

	set_hpattern(	&m_RoiPatternTwelve,
					Mask,
					BinningLimit,
					RefNumElements,
					SigNumElements,
					BinNumElements,
					RefPatternData,
					SigPatternData,
					BinPatternData );
}


void CApnCamData_TH7899::set_hpattern(	APN_HPATTERN_FILE	*Pattern,
											unsigned short	Mask,
											unsigned short	BinningLimit,
											unsigned short	RefNumElements,
											unsigned short	SigNumElements,
											unsigned short	BinNumElements[],
											unsigned short	RefPatternData[],
											unsigned short	SigPatternData[],
											unsigned short	BinPatternData[][APN_MAX_PATTERN_ENTRIES] )
{
	int i, j;

	Pattern->Mask = Mask;
	Pattern->BinningLimit = BinningLimit;
	Pattern->RefNumElements = RefNumElements;
	Pattern->SigNumElements = SigNumElements;

	if ( RefNumElements > 0 )
	{
		Pattern->RefPatternData = 
			(unsigned short *)malloc(RefNumElements * sizeof(unsigned short));

		for ( i=0; i<RefNumElements; i++ )
		{
			Pattern->RefPatternData[i] = RefPatternData[i];
		}
	}

	if ( SigNumElements > 0 )
	{
		Pattern->SigPatternData = 
			(unsigned short *)malloc(SigNumElements * sizeof(unsigned short));

		for ( i=0; i<SigNumElements; i++ )
		{
			Pattern->SigPatternData[i] = SigPatternData[i];
		}
	}

	if ( BinningLimit > 0 )
	{
		for ( i=0; i<BinningLimit; i++ )
		{
			Pattern->BinNumElements[i] = BinNumElements[i];

			Pattern->BinPatternData[i] = 
				(unsigned short *)malloc(BinNumElements[i] * sizeof(unsigned short));

			for ( j=0; j<BinNumElements[i]; j++ )
			{
				Pattern->BinPatternData[i][j] = BinPatternData[i][j];
			}
		}
	}
}