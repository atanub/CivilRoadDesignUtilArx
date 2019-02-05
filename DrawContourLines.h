//DrawContourLines.h
#ifndef _DRAWCONTOURLINES_H
#define _DRAWCONTOURLINES_H

#include "Structures.h"

class CDrawContourLines
{
private:
	CDrawContourLines() {}
	~CDrawContourLines() {}

public:
	static void ContourSetParam();
	static void DrawContour();
	static void CompactContourLines();
public:
	static void DelAllContours();
	static BOOL DelAllContours(const DWGPARAMS&, const BOOL&);

	static void DrawUserSpecContour();
	static void DrawContour(const AcGePoint3dArray&, const CTrianglePtrArray&, const DWGPARAMS&);

	static CString GetContourLyrName(const double&, const DWGPARAMS&);
	static BOOL GetAllContourLyrNames(const AcGePoint3dArray&, const DWGPARAMS&, const char&, CString&);

	//for Testing Only.....
	static void TestDrawContourInTriangle();
private:
	static int DrawContourInTriangle(const AcGePoint3dArray&, const TRIANGLE*, const AcGePoint3dArray&, const DWGPARAMS&, const double&, const double&);
	static void DrawLineSegment(const AcGePoint3d*, const AcGePoint3d*, const double&, const DWGPARAMS&, const short iColor = -1);
	static CString ConvertToElevStr(const double&, const char);
	static void GetElevRange(const AcGePoint3dArray&, const DWGPARAMS&, double&, double&);
	static short GetColorID(const short&, const double&, const double&, const double&);
	static double GetNearestContourElev(const double&, const DWGPARAMS&);
	static BOOL GetContLinesInBoundary(const AcGePoint3dArray&, const AcGePoint3d&, const AcGePoint3d&, AcGePoint3dArray&);
	static void DrawContLines(const AcGePoint3dArray&, const double&, const DWGPARAMS&, const short = -1);
	static BOOL GetSelClosedPLineVertexes(AcGePoint3dArray&, BOOL* = 0L);

	static void DrawSingleContour(const AcGePoint3dArray&, const CTrianglePtrArray&, const DWGPARAMS&, const double&);
	static int CDrawContourLines::DrawUserSpecContourInTriangle(const AcGePoint3dArray& Pt3DArray, const TRIANGLE* pTrg, const AcGePoint3dArray& BoundaryPts, const DWGPARAMS& DwgParams, const double& fLevelParam);
};

#endif //_DRAWCONTOURLINES_H


