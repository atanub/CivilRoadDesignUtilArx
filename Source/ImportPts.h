// ImportPts.h
/////////////////////////////////////////////////////////
#ifndef _IMPORTPTS_H_
#define _IMPORTPTS_H_

#include "Structures.h"


class CImportPtsUtil
{
private:
	CImportPtsUtil() {}
	~CImportPtsUtil() {}

public:
	static int ImportPointsIntoDWG(const CPointDataPtrArray&, const DWGPARAMS&);
	static int GetContourPointsFromDWG(AcGePoint3dArray&, const char*, const BOOL&, AcDbObjectIdArray* = 0L, AcGePoint3dArray* = 0L);
	static int GetPointsForTINGeneration(AcGePoint3dArray&, const char*, const BOOL&, AcDbObjectIdArray* = 0L, AcGePoint3dArray* = 0L);

	static BOOL GeneratePoints(const DWGPARAMS&);
	
	static void ContourImportData();
	static void GenHardBreakLine();
	static void AddPoints();
	static void ChkContourPoints();
	static void GenPoints();
	static void DrawSpotLevOnSelPts();
	static void GenPtsFrMultiSeg();
private:
	static BOOL FindOccurance(const AcGePoint3d&, const AcGePoint3dArray, long&, BOOL&, const BOOL = FALSE);
	static void GetIntermediatePts(const AcGePoint3d&, const AcGePoint3d&, const int&, AcGePoint3dArray&);
	static int ExtractNChkDuplicatePts(ads_name&, AcGePoint3dArray&, const BOOL&, AcDbObjectIdArray* = 0L, AcGePoint3dArray* = 0L);
	static BOOL RemoveDupContourPts(const char*);
	static BOOL MakeContourCompatiblePoints(const DWGPARAMS&);
	static BOOL GenerateHardBreakLine(const DWGPARAMS&);

	static BOOL GetDwgParam(DWGPARAMS&);
};


#endif // _IMPORTPTS_H_


