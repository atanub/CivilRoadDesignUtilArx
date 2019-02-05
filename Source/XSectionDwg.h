// XSectionDwg.h : header file
//

#ifndef _XSECTIONDWG_H
#define _XSECTIONDWG_H

#include "PLineDataExtractor.h"



class CXSectionDwg
{
protected:
	CXSectionDwg() {;}
	~CXSectionDwg() {;}
public:
	static BOOL DrawXSectionCore(const AcGePoint3d&, const CThePointsOnAlignment&, const AcGePoint3dArray&, const XSECDATAEX&);

protected:
	static void DeAllocateXSecPtsArray(AcDbVoidPtrArray&);
	static BOOL GetChainage(const AcGePoint3dArray*, const CThePointsOnAlignment&, double&, AcGePoint3d* = 0L, BOOL* = 0L);
	static BOOL GetDatumValue(const AcGePoint3dArray&, const double&, double&);
	static BOOL GetAutoDatumValue(const AcGePoint3dArray&, const double&, double&);
	static BOOL GetAutoDatumValue(const AcGePoint3dArray& PtsArray, const double& fDatumDenominator, const double& fMinBufferInDWGUnits, const double& fYScale, double& fDatum);

	static void DrawAxis(const AcGePoint3d&, const XSECDATAEX&, const double&, const double&, const double&, const double&, AcGePoint3d&);
	static void DrawOrdinates(const AcGePoint3d&, const AcGePoint3dArray&, const AcGePoint3d&, const XSECDATAEX&, const double&, const double&, const double&);

	static BOOL DrawAllSections(const AcGePoint3d&, const CThePointsOnAlignment&, const AcGePoint3dArray&, const XSECDATAEX&);
	static BOOL DrawCSRow(const AcGePoint3d& ptLLCStart, const int& iStartIndex, const int& iEndIndex, const CThePointsOnAlignment& PIDArray, const AcGePoint3dArray& DatumRangeArray, const XSECDATAEX& RecParam, const double& fLeftOfXAxisWid, const double& fCSHeight, const CString& strCSGridLyr, const AcGePoint3d& ptULOfNPlate, int& iLastDrawnCS);
	static BOOL GetNextSectionRange(const int& iStartIndex, const double& fCurRowDepth, const CThePointsOnAlignment& PIDArray, const AcGePoint3dArray& DatumRangeArray, const XSECDATAEX& RecParam, int& iEndIndex, double& fLeftOfXAxisWid, double& fMaxHeight);

	static BOOL CalculateOffset(const AcGePoint3d&, const AcGePoint3dArray&, const AcGePoint3d&, double&);

	static void RectangleUL(const AcGePoint3d&, const double&, const double&, const char*);
	static void RectangleLR(const AcGePoint3d&, const double&, const double&, const char*);
	static BOOL IsCSDwgPossible(const AcGePoint3d&, const double&, const double&, const AcGePoint3d&, const double&, const double&);
	static BOOL IsPointWithinRect(const AcGePoint3d&, const AcGePoint3d&, const double&, const double&);

	//
	static BOOL FindMinMaxOffElevInStn(const AcGePoint3dArray&, const AcGePoint3d&, double&, double&, double&);
	static int FindMaxLenOfDatumStr(const AcGePoint3dArray&);
};

#endif //_XSECTIONDWG_H

