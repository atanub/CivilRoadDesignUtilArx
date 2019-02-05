// DrawUtil.h
/////////////////////////////////////////////////////////
#ifndef _DRAWUTIL_H_
#define _DRAWUTIL_H_

#include <dbspline.h>
#include "Structures.h"
#include "TINEntity.h"

void AddToACDBase(AcDbEntity*);

Acad::ErrorStatus LAYER(const char*, const int& = -1, AcDbObjectId* = 0L);
void	DRAWPOINT(const AcGePoint3d&, const char*);
int		DRAWPOINTS(const AcGePoint3dArray& Array, const char* pszLyr);
void	LINE(const AcGePoint3d&, const AcGePoint3d&, const char*);

BOOL IsBlockExists(const char*, AcDbObjectId* = 0L);
void BLOCK(const char*, const AcGePoint3d&, const char* = 0L, const double& = 0.0, const double& = 1.0, const double& = 1.0, const double& = 1.0);
BOOL GetBlockNames(CStringArray&);

void TEXTMID(const AcGePoint3d&, const char*, const double&, const double&, const char*);
void TEXTLEFT(const AcGePoint3d&, const char*, const double&, const double&, const char*);
void TEXTRIGHT(const AcGePoint3d&, const char*, const double&, const double&, const char*);

void ConvertPtPairsToVertices(const CPointPair3DPtrArray&, AcGePoint3dArray&, BOOL* = 0L);
void SPLINE(const CPointPair3DPtrArray&, const char*);
void SPLINE(const AcGePoint3dArray&, const char*);
void POLYLINE(const CPointPair3DPtrArray&, const char*);
void POLYLINE(AcGePoint3dArray&, const char*);
void POLYLINE3D(AcGePoint3dArray&, const char*);
void DrawTINEntity(const AcGePoint3dArray&, const CTrianglePtrArray&, const char*);

BOOL DrawSpotLevel(const AcGePoint3d&, const char*, const char*, const double& = 0.0, const double& = 1.0, const double& = 1.0);

void AddXDataForPoint(ads_name, const char*, const char*);
void AddXDataForContourLine(ads_name, const char*, const double*);
void AddXDataForAnnText(ads_name, const char*, const char*);

BOOL ExtratObjectIDs(ads_name&, AcDbObjectIdArray&);

void DeleteProducedEntities(const char*);
BOOL DeleteProducedContourLines(const char*);
BOOL DeleteProducedTrgEnts();
BOOL DeleteImportedPts(const char*);
BOOL DeleteProducedTextEnts(const char*);
BOOL DeletePolyLines(const char*);

BOOL EraseEntitiesInSelSet(ads_name&);
BOOL EraseEntities(const AcDbObjectIdArray&);

int		IsTINEntityExists(AcDbEntity*&);
void	EraseAllTINEntities(AcDbDatabase*);

int		PROXYNOTICE(const short& = -1);
int		EXPERT();
BOOL	CLAYER(CString&, const char* pszNewLyr = 0L);
BOOL	TEXTSTYLE(AcDbObjectId&);

//
void DONUT(const AcGePoint3d&, const double&, const double&, const char*);

inline  int DIMZIN(const int& iNewSetting = 0)
{
	short iPrec;
	AcDbDatabase*	pDb;

	pDb = acdbCurDwg();
	if(pDb == 0L)
		return -1;
	
	iPrec = (short)pDb->dimzin();
	if((iNewSetting >= 0) && (iNewSetting != iPrec))
		pDb->setDimzin((short)iNewSetting);
	
	return iPrec;
}
inline  int LUPREC(const int& iNewSetting = -1)
{
	short iPrec;
	AcDbDatabase*	pDb;

	pDb = acdbCurDwg();
	if(pDb == 0L)
		return -1;
	
	iPrec = pDb->luprec();
	if((iNewSetting >= 0) && (iNewSetting != iPrec))
		pDb->setLuprec((short)iNewSetting);
	
	return iPrec;
}
inline  double TEXTSIZE(const double& fVal = -1.0)
{
	double fCurVal;
	AcDbDatabase*	pDb;

	pDb = acdbCurDwg();
	if(pDb == 0L)
		return -1;
	
	fCurVal = pDb->textsize();
	if((fVal > 0.0) && (fCurVal != fVal))
		pDb->setTextsize(fVal);
	
	return fCurVal;
}
inline CString RTOS(const double& fValue)
{
	CString strToRet;
	char szVal[64];
	int iPrec;
	
	iPrec = DIMZIN(0);
	ads_rtos(fValue, 2, LUPREC(), szVal);
	strToRet.Format("%s", szVal);
	DIMZIN(iPrec);
	
	return strToRet;
}
inline CString GetLyrName(const AcDbEntity* pEnt)
{
	CString strToRet;
	char* pszLyrEnt;
	
	pszLyrEnt = pEnt->layer();
	strToRet = pszLyrEnt;
	delete []pszLyrEnt;
	
	return strToRet;
}
#endif // _DRAWUTIL_H_
