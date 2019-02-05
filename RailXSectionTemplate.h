//RailXSectionTemplate.h
/////////////////////////////////////////////////////////
#ifndef _RAILXSECTIONTEMPLATE_H_
#define _RAILXSECTIONTEMPLATE_H_

#include "Structures.h"
#include "Util.h"


class CXSectionSlope
{
public:
	CXSectionSlope(const char*);	//formats: Either "1 in 20" OR "1:20"
	CXSectionSlope(const double);	//Height/Width Ratio
	~CXSectionSlope();
protected:
	CXSectionSlope();
protected:
	static double ParseInputStr();

protected:
	double m_fSlope; //Height/Width Ratio
};

typedef CTypedPtrArray<CPtrArray, CXSectionSlope*> CXSectionSlopeArray;

class CRailXSectionTemplate
{
public:
	CRailXSectionTemplate();
	~CRailXSectionTemplate();

public:
protected:
	CXSectionSlopeArray m_arraySlopes;	//0 th Element is the slope starting from the Centre point
										//last Element is the slope @ the extreme side
	CDoubleArray		m_arrayHeights;
	CDoubleArray		m_arrayWidths;
};


#endif // _RAILXSECTIONTEMPLATE_H_


