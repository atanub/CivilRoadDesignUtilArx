//Line2PLine.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _LINE2PLINE_H
#define _LINE2PLINE_H

#include "DrawUtil.h"

class CLine2PLine
{
private:
	CLine2PLine() {}
	~CLine2PLine() {}

public:
	static BOOL ContourLines2PLine(const char*, const char&, const CString&);
	static BOOL ContourL2PGetVertices(const char*, const char*, AcDbVoidPtrArray&, AcDbObjectIdArray* = 0L);

private:
	static BOOL L2P(const char*, const char*, const BOOL& = TRUE);
	static BOOL CLine2PLine::L2P_old(const char* pszAppName, const char* pszLyrName, const BOOL& bEraseLines = TRUE);
	static long FindStartLine(const CPointPair3DPtrArray&, short&);

	static BOOL FindNextSegment(const AcGePoint3d&, CPointPair3DPtrArray&, CPointPair3DPtrArray&);
	static int	FindOccuranceInArray(const CPointPair3DPtrArray&, const AcGePoint3d&);
	static int	FindInArray(const CPointPair3DPtrArray&, const AcGePoint3d&, short&);
	static void SwapPoints(POINTPAIR3D*);
	static BOOL CollectPtsFromContourLines(CPointPair3DPtrArray& Array, ads_name& SelSet, const char* pszAppName, const char* pszLyrName);
};

#endif //_LINE2PLINE_H
