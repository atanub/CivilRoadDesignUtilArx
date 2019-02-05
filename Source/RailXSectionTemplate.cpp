// RailXSectionTemplate.cpp
/////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <Adscodes.h>
#include <DbEnts.h>
#include <Geassign.h>
#include <math.h>
#include "RailXSectionTemplate.h"
#include "DrawUtil.h"


//
//CXSectionSlope
/////////////////////////////////////////////////////////
CXSectionSlope::CXSectionSlope(const char* pszParam)	//formats: Either "1 in 20" OR "1:20"
{
}
CXSectionSlope::CXSectionSlope(const double fParam)	//Height/Width Ratio
{
}
CXSectionSlope::~CXSectionSlope()
{
}
CXSectionSlope::CXSectionSlope()
{
}
double CXSectionSlope::ParseInputStr()
{
	return 0.0;
}


//
//CRailXSectionTemplate
/////////////////////////////////////////////////////////
CRailXSectionTemplate::CRailXSectionTemplate()
{
}
CRailXSectionTemplate::~CRailXSectionTemplate()
{
}

