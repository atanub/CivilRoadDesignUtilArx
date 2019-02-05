// DrawUtil.cpp
/////////////////////////////////////////////////////////
#include "Stdafx.h"
#include "DrawUtil.h"
#include "Util.h"
#include <AdsCodes.h>
#include <dbents.h>
#include <Dbsymtb.h>

static Acad::ErrorStatus IsLyrExists(const char* pszLyr, AcDbObjectId* = 0L);


int PROXYNOTICE(const short& iNewVal/*= -1*/)
{
	struct resbuf rb;
	if(iNewVal < 0)
	{
		ads_getvar("PROXYNOTICE", &rb);
		return rb.resval.rint;
	}
	rb.restype = RTSHORT;
	rb.resval.rint = iNewVal;
	ads_setvar("PROXYNOTICE", &rb);
	return 1;
}
int EXPERT()
{
	struct resbuf rb;
	ads_getvar("EXPERT", &rb);
	return rb.resval.rint;
}

BOOL TEXTSTYLE(AcDbObjectId& ID)
{
	AcDbDatabase*	pDb;

	pDb = acdbCurDwg();
	if(pDb == 0L)
		return FALSE;
	
	ID = pDb->textstyle();
	return TRUE;
}
BOOL CLAYER(CString& strCurLyr, const char* pszNewLyr /* = 0L*/)
{
	AcDbDatabase*	pDb;
	Acad::ErrorStatus		ErrCode;

	pDb = acdbCurDwg();
	if(pDb == 0L)
		return FALSE;
    
	AcDbObjectId LyrID;
	
	LyrID = pDb->clayer();
	
	{//Extract the name of the layer....
		AcDbLayerTable*			pLyrTable;

		ErrCode = pDb->getLayerTable(pLyrTable, AcDb::kForRead);
		if(ErrCode != Acad::eOk)
			return ErrCode;//ERROR!!

		{//
			AcDbLayerTableIterator* pIterator;
			AcDbLayerTableRecord*	pLyrDet;
			const char* pszLyr;
		
			pLyrTable->newIterator(pIterator);
			pIterator->seek(LyrID);
			pIterator->getRecord(pLyrDet, AcDb::kForRead);
			delete pIterator;
			
		    ErrCode = pLyrDet->getName(pszLyr);
			if(ErrCode != Acad::eOk)
			{
				ads_printf("\nERROR: AutoCAD system error occured");
			}
			strCurLyr = pszLyr;
			pLyrDet->close();
    	}
		pLyrTable->close();
	}
	if(pszNewLyr != 0L)
	{
		AcDbObjectId DbObjId;
		
		ErrCode = IsLyrExists(pszNewLyr, &DbObjId);
		if(Acad::eOk == ErrCode)
		{//Just Set
		}
		else
		{//Create ...
			ErrCode = LAYER(pszNewLyr, -1, &DbObjId);
			if(Acad::eOk != ErrCode)
			{
				ads_printf("\nERROR: \"%s\" layer creation failed", pszNewLyr);
				return FALSE;
			}
		}
		pDb->setClayer(DbObjId);
	}
	return TRUE;
}

Acad::ErrorStatus LAYER(const char* pszLyr, const int& iColor/* = -1*/, AcDbObjectId* pDbObjId /* = 0L*/)
{
	Acad::ErrorStatus ErrCode;
	
	ErrCode = IsLyrExists(pszLyr, pDbObjId);
	if(Acad::eOk == ErrCode)
		return ErrCode;//Layer already exists....

	{//Layer doesn't exists.. so create..
		AcDbDatabase*	pDb;
		AcDbLayerTable* pLyrTable;
		AcDbLayerTableRecord* pLyrDet;

		pDb = acdbCurDwg();

		ErrCode = pDb->getLayerTable(pLyrTable, AcDb::kForWrite);
		if(ErrCode != Acad::eOk)
			return ErrCode;//ERROR!!
		//Initialize Record....
		pLyrDet = new AcDbLayerTableRecord;
		ErrCode = pLyrDet->setName(pszLyr);
		if(iColor != -1)
		{
			AcCmColor Color;
			
			Color.setColorIndex((short)iColor);
			pLyrDet->setColor(Color);
		}
		//Initialize Record....
		if(ErrCode == Acad::eOk)
		{
			ErrCode = pLyrTable->add(pLyrDet);
			if(ErrCode != Acad::eOk)
				delete pLyrDet;

		}
		pLyrTable->close();
		pLyrDet->close();
		return ErrCode;
	}
}
static Acad::ErrorStatus IsLyrExists(const char* pszLyr, AcDbObjectId* pDbObjId /* = 0L*/)
{
	Acad::ErrorStatus ErrCode;
	AcDbDatabase*	pDb;
	AcDbLayerTable* pLyrTable;

	pDb = acdbCurDwg();

	ErrCode = pDb->getLayerTable(pLyrTable, AcDb::kForRead);
	if(Acad::eOk != ErrCode)
	{
		pLyrTable->close();
		return ErrCode;//ERROR!!
	}
	if(pDbObjId != 0L)
	{
		ErrCode = pLyrTable->getAt(pszLyr, *pDbObjId, Adesk::kTrue);
	}
	else
	{
		AcDbObjectId DbObjId;
		ErrCode = pLyrTable->getAt(pszLyr, DbObjId, Adesk::kTrue);
	}
	pLyrTable->close();
	return ErrCode;
}
void DRAWPOINT(const AcGePoint3d& Pt, const char* pszLyr)
{
	Acad::ErrorStatus ErrCode;
	
	ErrCode = IsLyrExists(pszLyr);
	if(Acad::eOk != ErrCode)
		return;//Layer doesn't exists....

    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
    AcDbBlockTableRecord* pBtblRcd;
	AcDbPoint* pPoint;

	pDb = acdbCurDwg();
	//Initialize Point data..
	pPoint = new AcDbPoint();
	pPoint->setPosition(Pt);
	pPoint->setLayer(pszLyr);
	//Initialize Point data..

    pDb->getBlockTable(pBtbl, AcDb::kForRead);
 
    pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
    pBtbl->close();
 
    pBtblRcd->appendAcDbEntity(pPoint);
    pPoint->close();
    
    pBtblRcd->close();
}
int DRAWPOINTS(const AcGePoint3dArray& Array, const char* pszLyr)
{
	int i;

	for(i = 0; i < Array.length(); i++)
	{
		DRAWPOINT(Array[i], pszLyr);
	}
	return Array.length();
}
void LINE(const AcGePoint3d& PtStart, const AcGePoint3d& PtEnd, const char* pszLyr)
{
	Acad::ErrorStatus ErrCode;
	
	ErrCode = IsLyrExists(pszLyr);
	if(Acad::eOk != ErrCode)
		return;//Layer doesn't exists....
	/////
    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
    AcDbBlockTableRecord* pBtblRcd;
	AcDbLine* pEntity;

	pDb = acdbCurDwg();
	//Initialize Line data..
	pEntity = new AcDbLine();
	pEntity->setStartPoint(PtStart);
	pEntity->setEndPoint(PtEnd);
	pEntity->setLayer(pszLyr);
	//Initialize Line data..

    pDb->getBlockTable(pBtbl, AcDb::kForRead);
 
    pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
    pBtbl->close();
 
    pBtblRcd->appendAcDbEntity(pEntity);
    pEntity->close();
    
    pBtblRcd->close();
}
void BLOCK(const char* pszBlkName, const AcGePoint3d& Pt, const char* pszLyr /* = 0L*/, const double& fRotation /* = 0.0*/, const double& fX /* = 1.0*/, const double& fY /* = 1.0*/, const double& fZ /* = 1.0*/)
{
	Acad::ErrorStatus ErrCode;
	AcDbObjectId ObjectId;

	if(pszLyr != 0L)
	{
		ErrCode = IsLyrExists(pszLyr);
		if(Acad::eOk != ErrCode)
		{
			ads_printf("\nERROR: Layer \"%s\" doesn't exists\n", pszLyr);
			return;//Layer doesn't exists....
		}
	}

	if(!IsBlockExists(pszBlkName, &ObjectId))
	{
		ads_printf("\nERROR: Block \"%s\" doesn't exists\n", pszBlkName);
		return;//Block doesn't exists....
	}
	/////
    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
    AcDbBlockTableRecord* pBtblRcd;
	AcDbBlockReference* pEntity;

	pDb = acdbCurDwg();
	//Initialize INSERT data..
	pEntity = new AcDbBlockReference();
	pEntity->setBlockTableRecord(ObjectId);
	pEntity->setPosition(Pt);
	if(pszLyr != 0L)
		pEntity->setLayer(pszLyr);
	pEntity->setScaleFactors(AcGeScale3d(fX, fY, fZ));
	pEntity->setRotation(fRotation);
	//Initialize INSERT data..

    pDb->getBlockTable(pBtbl, AcDb::kForRead);
 
    pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
    pBtbl->close();
 
    pBtblRcd->appendAcDbEntity(pEntity);
    pEntity->close();
    
    pBtblRcd->close();
}
BOOL GetBlockNames(CStringArray& BlkNameArray)
{
	Acad::ErrorStatus ErrCode;
    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
	AcDbBlockTableIterator* pIterator;
	BOOL bFound;
	
	pDb = acdbCurDwg();

    pDb->getBlockTable(pBtbl, AcDb::kForRead);
    ErrCode = pBtbl->newIterator(pIterator);
	pBtbl->close();
	if(Acad::eOk != ErrCode)
		return FALSE;//Some errors occured....
 
	for(pIterator->start(), bFound = FALSE; (pIterator->done() == Adesk::kFalse); pIterator->step())
	{
		AcDbBlockTableRecord* pBlkRec;
		char* pszName;
	    
		ErrCode = pIterator->getRecord(pBlkRec, AcDb::kForRead);
 		if(Acad::eOk != ErrCode)
		{
			delete pIterator;
			return FALSE;//Some errors occured....
		}
		if(!pBlkRec->isAnonymous() && !pBlkRec->hasAttributeDefinitions())
		{
			pBlkRec->getName(pszName);
			if(*(pszName+0) != '*')
				BlkNameArray.Add(pszName);
		}
		pBlkRec->close();
	}
	delete pIterator;
	return bFound;
}
BOOL IsBlockExists(const char* pszBlk, AcDbObjectId* pObjectId /* = 0L*/)
{
	Acad::ErrorStatus ErrCode;
    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
	AcDbBlockTableIterator* pIterator;
	BOOL bFound;
	
	pDb = acdbCurDwg();

    pDb->getBlockTable(pBtbl, AcDb::kForRead);
    ErrCode = pBtbl->newIterator(pIterator);
	pBtbl->close();
	if(Acad::eOk != ErrCode)
		return FALSE;//Some errors occured....
 
	for(pIterator->start(), bFound = FALSE; (pIterator->done() == Adesk::kFalse); pIterator->step())
	{
		AcDbBlockTableRecord* pBlkRec;
		char* pszName;
	    
		ErrCode = pIterator->getRecord(pBlkRec, AcDb::kForRead);
 		if(Acad::eOk != ErrCode)
		{
			delete pIterator;
			return FALSE;//Some errors occured....
		}
		pBlkRec->getName(pszName);
		pBlkRec->close();
		if(strcmp(pszName, pszBlk) == 0)
		{//Found....
			bFound = TRUE;
			if(pObjectId != 0L)
				*pObjectId = pBlkRec->objectId();
			break;
		}
	}
	delete pIterator;
	return bFound;
}
BOOL DrawSpotLevel(const AcGePoint3d& Pt, const char* pszBlkName, const char* pszLyr, const double& fRotation /* = 0.0*/, const double& fScale /* = 1.0*/, const double& fBlkScaleFac /*= 1.0*/)
{
	Acad::ErrorStatus ErrCode;
	char szTmp[_MAX_PATH];
	const double fSpacingMFac = 1.05;
	const char chDot = '.';
	ads_point ptIns;
	int iIndex, iLuPrec;

	if(!IsBlockExists(pszBlkName))
	{
		ads_printf("\nERROR: Block \"%s\" doesn't exists\n", pszBlkName);
		return FALSE;//Block doesn't exists....
	}
	ErrCode = IsLyrExists(pszLyr);
	if(Acad::eOk != ErrCode)
	{//Create
		ErrCode = LAYER(pszLyr);
		if(Acad::eOk != ErrCode)
		{
			ads_printf("\nERROR: \"%s\" layer creation failed", pszLyr);
			return FALSE;
		}
	}
	iLuPrec = LUPREC();
	if(iLuPrec <= 0) iLuPrec = 1;

	BLOCK(pszBlkName, Pt, pszLyr, fRotation, (fScale * fBlkScaleFac), (fScale * fBlkScaleFac), (fScale * fBlkScaleFac));
	{//Draw Left String....
		CString strTmp;

		ads_rtos(Pt.z, 2, iLuPrec, szTmp);
		strTmp = szTmp;
		iIndex = strTmp.Find(chDot);
		if(iIndex < 0)
			strTmp = strTmp;
		else if (iIndex == 0)
			strTmp = "0";
		else
			strTmp = strTmp.Mid(0, iIndex);

		ads_polar(asDblArray(Pt), (fRotation + PI), (fScale * fBlkScaleFac * fSpacingMFac), ptIns);
		TEXTRIGHT(asPnt3d(ptIns), strTmp, fRotation, fScale, pszLyr);
	}
	{//Draw Right String....
		CString strTmp;

		ads_rtos(Pt.z, 2, iLuPrec, szTmp);
		strTmp = szTmp;
		iIndex = strTmp.Find(chDot);
		strTmp = strTmp.Mid(iIndex + 1);
		
		//Remove Trailing Zeros....
		//for(i = strlen(strTmp) - 1; i >= 0; i--)
		//{
		//	if(strTmp[i] != '0')
		//	{
		//		strTmp = strTmp.Mid(0, i+1);
		//		break;
		//	}
		//}
		//if(i < 0)
		//{
		//	strTmp = "0";
		//}
		ads_polar(asDblArray(Pt), fRotation, (fScale * fBlkScaleFac * fSpacingMFac), ptIns);
		TEXTLEFT(asPnt3d(ptIns), strTmp, fRotation, fScale, pszLyr);
	}
	return TRUE;
}

void TEXTMID(const AcGePoint3d& Pt, const char* pszTxt, const double& fRotation, const double& fHt, const char* pszLyr)
{
	Acad::ErrorStatus ErrCode;
	
	ErrCode = IsLyrExists(pszLyr);
	if(Acad::eOk != ErrCode)
		return;//Layer doesn't exists....
	/////
    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
    AcDbBlockTableRecord* pBtblRcd;
	AcDbText* pEntity;

	pDb = acdbCurDwg();
	//Initialize data..
	pEntity = new AcDbText();

	pEntity->setPosition(Pt);
	pEntity->setRotation(fRotation);
	pEntity->setHeight(fHt);
	pEntity->setTextString(pszTxt);
	pEntity->setHorizontalMode(AcDb::TextHorzMode::kTextCenter);
	pEntity->setVerticalMode(AcDb::TextVertMode::kTextVertMid);
	pEntity->setAlignmentPoint(Pt);
	pEntity->setLayer(pszLyr);
	//Initialize data..

    pDb->getBlockTable(pBtbl, AcDb::kForRead);
 
    pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
    pBtbl->close();
 
    pBtblRcd->appendAcDbEntity(pEntity);
    pEntity->close();
    
    pBtblRcd->close();
}
void TEXTLEFT(const AcGePoint3d& Pt, const char* pszTxt, const double& fRotation, const double& fHt, const char* pszLyr)
{
	Acad::ErrorStatus ErrCode;
	
	ErrCode = IsLyrExists(pszLyr);
	if(Acad::eOk != ErrCode)
		return;//Layer doesn't exists....
	/////
    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
    AcDbBlockTableRecord* pBtblRcd;
	AcDbText* pEntity;

	pDb = acdbCurDwg();
	//Initialize data..
	pEntity = new AcDbText();
	pEntity->setHorizontalMode(AcDb::TextHorzMode::kTextLeft);
	pEntity->setVerticalMode(AcDb::TextVertMode::kTextVertMid);
	pEntity->setRotation(fRotation);
	pEntity->setHeight(fHt);
	pEntity->setTextString(pszTxt);
	pEntity->setLayer(pszLyr);
	pEntity->setAlignmentPoint(Pt);
	//Initialize data..

    pDb->getBlockTable(pBtbl, AcDb::kForRead);
 
    pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
    pBtbl->close();
 
    pBtblRcd->appendAcDbEntity(pEntity);
    pEntity->close();
    
    pBtblRcd->close();
}
void TEXTRIGHT(const AcGePoint3d& Pt, const char* pszTxt, const double& fRotation, const double& fHt, const char* pszLyr)
{
	Acad::ErrorStatus ErrCode;
	
	ErrCode = IsLyrExists(pszLyr);
	if(Acad::eOk != ErrCode)
		return;//Layer doesn't exists....
	/////
    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
    AcDbBlockTableRecord* pBtblRcd;
	AcDbText* pEntity;

	pDb = acdbCurDwg();
	//Initialize data..
	pEntity = new AcDbText();
	pEntity->setHorizontalMode(AcDb::TextHorzMode::kTextRight);
	pEntity->setVerticalMode(AcDb::TextVertMode::kTextVertMid);
	pEntity->setRotation(fRotation);
	pEntity->setHeight(fHt);
	pEntity->setTextString(pszTxt);
	pEntity->setLayer(pszLyr);
	pEntity->setAlignmentPoint(Pt);
	//Initialize data..

    pDb->getBlockTable(pBtbl, AcDb::kForRead);
 
    pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
    pBtbl->close();
 
    pBtblRcd->appendAcDbEntity(pEntity);
    pEntity->close();
    
    pBtblRcd->close();
}
void SPLINE(const CPointPair3DPtrArray& Array, const char* pszLyr)
{
	AcGePoint3dArray Vertices;
	
	//Initialize Line data..
	if(Array.GetSize() > 1)
	{
		int i;

		for(i = 0; i < Array.GetSize(); i++)
		{
			Vertices.append((Array[i])->pt01);
		}
		if(Array.GetSize() > 1)
		{
			if((Array[0])->pt01 == (Array[Array.GetSize() - 1])->pt02)
			{
				Vertices.append((Array[0])->pt01);
			}
			else
				Vertices.append((Array[Array.GetSize() - 1])->pt02);
		}
	}
	else
	{//
		Vertices.append((Array[0])->pt01);
		Vertices.append((Array[0])->pt02);
	}
	SPLINE(Vertices, pszLyr);
}
void SPLINE(const AcGePoint3dArray& Array, const char* pszLyr)
{
	Acad::ErrorStatus ErrCode;
	
	ErrCode = IsLyrExists(pszLyr);
	if(Acad::eOk != ErrCode)
		return;//Layer doesn't exists....
	/////
    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
    AcDbBlockTableRecord* pBtblRcd;
	AcDbSpline* pEntity;

	pDb = acdbCurDwg();
	pEntity = new AcDbSpline(Array);
	pEntity->setLayer(pszLyr);
	//Initialize Line data..

    pDb->getBlockTable(pBtbl, AcDb::kForRead);
 
    pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
    pBtbl->close();
 
    pBtblRcd->appendAcDbEntity(pEntity);
    pEntity->close();
    
    pBtblRcd->close();
}
void POLYLINE3D(AcGePoint3dArray& Array, const char* pszLyr)
{
	Acad::ErrorStatus ErrCode;
	
	ErrCode = IsLyrExists(pszLyr);
	if(Acad::eOk != ErrCode)
		return;//Layer doesn't exists....

	if(Array.length() <= 0)
		return;//Drawing not possible

    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
    AcDbBlockTableRecord* pBtblRcd;
 
	pDb = acdbCurDwg();

	AcDb3dPolyline* pNewEnt;
	pNewEnt = new AcDb3dPolyline(AcDb::Poly3dType::k3dSimplePoly, Array);
	pNewEnt->setLayer(pszLyr);

	pDb->getBlockTable(pBtbl, AcDb::kForRead);
	pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
	pBtbl->close();
	pBtblRcd->appendAcDbEntity(pNewEnt);
	pNewEnt->close();
	pBtblRcd->close();
}
void POLYLINE(AcGePoint3dArray& Array, const char* pszLyr)
{
	Acad::ErrorStatus ErrCode;
	
	ErrCode = IsLyrExists(pszLyr);
	if(Acad::eOk != ErrCode)
		return;//Layer doesn't exists....

	if(Array.length() < 2)
		return;//Drawing not possible

    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
    AcDbBlockTableRecord* pBtblRcd;
	Adesk::Boolean bIsClosed;
	AcDb2dPolyline* pNewEnt;
 
	pDb = acdbCurDwg();

	bIsClosed = FALSE;
	//Initialize data..
	pNewEnt = new AcDb2dPolyline(AcDb::Poly2dType::k2dSimplePoly, Array, (Array[0]).z, bIsClosed, 0.0, 0.0, NULL);
	pNewEnt->setLayer(pszLyr);
	//Initialize data..

    pDb->getBlockTable(pBtbl, AcDb::kForRead);
    pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
    pBtbl->close();
    pBtblRcd->appendAcDbEntity(pNewEnt);
    pNewEnt->close();
    pBtblRcd->close();
}
void ConvertPtPairsToVertices(const CPointPair3DPtrArray& Array, AcGePoint3dArray& arrayResult, BOOL* pbIsClosed /*= 0L*/)
{
	if(0L != pbIsClosed)
		*pbIsClosed = FALSE;
	
	if(Array.GetSize() > 1)
	{
		int i;

		for(i = 0; i < Array.GetSize(); i++)
		{
			arrayResult.append((Array[i])->pt01);
		}
		if(Array.GetSize() > 1)
		{
			if((Array[0])->pt01 == (Array[Array.GetSize() - 1])->pt02)
			{
				arrayResult.append((Array[0])->pt01);//Need to check this line of code!!>> May 17'2003 
				if(0L != pbIsClosed)
					*pbIsClosed = TRUE;

			}
			else
			{
				arrayResult.append((Array[Array.GetSize() - 1])->pt02);
			}
		}
	}
	else
	{//
		arrayResult.append((Array[0])->pt01);
		arrayResult.append((Array[0])->pt02);
	}
}

void POLYLINE(const CPointPair3DPtrArray& Array, const char* pszLyr)
{
	Acad::ErrorStatus ErrCode;
	
	ErrCode = IsLyrExists(pszLyr);
	if(Acad::eOk != ErrCode)
		return;//Layer doesn't exists....

	if(Array.GetSize() < 1)
		return;//Drawing not possible

    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
    AcDbBlockTableRecord* pBtblRcd;
	BOOL bIsClosed;
	AcGePoint3dArray Vertices;
	AcDb2dPolyline* pNewEnt;
 
	pDb = acdbCurDwg();

	ConvertPtPairsToVertices(Array, Vertices, &bIsClosed);
	//Initialize data..
	pNewEnt = new AcDb2dPolyline(AcDb::Poly2dType::k2dSimplePoly, Vertices, (Array[0])->pt01.z, (bIsClosed) ? Adesk::kTrue:Adesk::kFalse);
	pNewEnt->setLayer(pszLyr);
	//
    pDb->getBlockTable(pBtbl, AcDb::kForRead);
    pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
    pBtbl->close();
    pBtblRcd->appendAcDbEntity(pNewEnt);
    pNewEnt->close();
    pBtblRcd->close();
}

void DONUT(const AcGePoint3d& PtParam, const double& fInsideRad, const double& fOutsideRad, const char* pszLyr)
{
	Acad::ErrorStatus ErrCode;
	
	ErrCode = IsLyrExists(pszLyr);
	if(Acad::eOk != ErrCode)
		return;//Layer doesn't exists....

    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
    AcDbBlockTableRecord* pBtblRcd;
	AcGePoint3dArray Vertices;
	AcDbPolyline* pNewEnt;
 
	pDb = acdbCurDwg();

	//Initialize data..
	pNewEnt = new AcDbPolyline();
	pNewEnt->setLayer(pszLyr);
	pNewEnt->setElevation(PtParam.z);
	pNewEnt->setClosed(Adesk::kTrue);
	//Initialize data..

	AcGePoint2d ptTmp;

	ptTmp.x = PtParam.x - (fInsideRad + (fOutsideRad - fInsideRad) / 2.0); 
	ptTmp.y = PtParam.y;
	pNewEnt->addVertexAt(0, ptTmp, tan((PI - 0.0) / 4.0), (fOutsideRad - fInsideRad), (fOutsideRad - fInsideRad));

	ptTmp.x = PtParam.x + (fInsideRad + (fOutsideRad - fInsideRad) / 2.0); 
	ptTmp.y = PtParam.y;
	pNewEnt->addVertexAt(0, ptTmp, tan((PI - 0.0) / 4.0), (fOutsideRad - fInsideRad), (fOutsideRad - fInsideRad));

    pDb->getBlockTable(pBtbl, AcDb::kForRead);
    pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
    pBtbl->close();
    pBtblRcd->appendAcDbEntity(pNewEnt);
    pNewEnt->close();
    pBtblRcd->close();
}

void DrawTINEntity(const AcGePoint3dArray& PtArray, const CTrianglePtrArray& TrgPtrArray, const char* pszLyr)
{
	Acad::ErrorStatus ErrCode;
	
	ErrCode = IsLyrExists(pszLyr);
	if(Acad::eOk != ErrCode)
		return;//Layer doesn't exists....

    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
    AcDbBlockTableRecord* pBtblRcd;
	TINEntity* pNewEnt;

	pDb = acdbCurDwg();
	//Initialize Point data..
	pNewEnt = new TINEntity(PtArray, TrgPtrArray);
	pNewEnt->setLayer(pszLyr);
	//Initialize Point data..

    pDb->getBlockTable(pBtbl, AcDb::kForRead);
 
    pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
    pBtbl->close();
 
    pBtblRcd->appendAcDbEntity(pNewEnt);
    pNewEnt->close();
    
    pBtblRcd->close();
}
void AddXDataForPoint(ads_name objName, const char* pszAppName, const char* pszXDString) 
{
    struct  resbuf  *pRb, *pTemp;
    
	Acad::ErrorStatus ErrCode;
	AcDbObjectId objId;
	AcDbObject* pObj;
	
	ErrCode = acdbGetObjectId(objId, objName);
	if(ErrCode != Acad::eOk)
	{
		return;
	}
	ErrCode = acdbOpenObject(pObj, objId, AcDb::kForWrite);
	if(ErrCode != Acad::eOk)
	{
		return;
	}
    
	pRb = pObj->xData(pszAppName);
    if (pRb != NULL)
	{// If xData is present then walk to the end of the list
        for (pTemp = pRb; pTemp->rbnext != NULL; pTemp = pTemp->rbnext)
		{
		}
    }
	else
	{// XData is not present, register the application
        ads_regapp(pszAppName);

        pRb = ads_newrb(AcDb::kDxfRegAppName);
        pTemp = pRb;
        pTemp->resval.rstring = (char*) malloc(strlen(pszAppName) + 1);
        strcpy(pTemp->resval.rstring, pszAppName);
    }
    // Add specified string to the Xdata.
    //
    pTemp->rbnext = ads_newrb(AcDb::kDxfXdAsciiString);
    pTemp = pTemp->rbnext;
    pTemp->resval.rstring = (char*) malloc(strlen(pszXDString) + 1);
    strcpy(pTemp->resval.rstring, pszXDString);

    // Following code shows the use of upgradeOpen
    // to change the entity from read to write.
    //
    pObj->upgradeOpen();
    pObj->setXData(pRb);
    
    pObj->close();
    ads_relrb(pRb);
}
void AddXDataForAnnText(ads_name objName, const char* pszAppName, const char* pszXDString) 
{
    struct  resbuf  *pRb, *pTemp;
    
	Acad::ErrorStatus ErrCode;
	AcDbObjectId objId;
	AcDbObject* pObj;
	
	ErrCode = acdbGetObjectId(objId, objName);
	if(ErrCode != Acad::eOk)
	{
		return;
	}
	ErrCode = acdbOpenObject(pObj, objId, AcDb::kForWrite);
	if(ErrCode != Acad::eOk)
	{
		return;
	}
    
	pRb = pObj->xData(pszAppName);
    if (pRb != NULL)
	{// If xData is present then walk to the end of the list
        for (pTemp = pRb; pTemp->rbnext != NULL; pTemp = pTemp->rbnext)
		{
		}
    }
	else
	{// XData is not present, register the application
        ads_regapp(pszAppName);

        pRb = ads_newrb(AcDb::kDxfRegAppName);
        pTemp = pRb;
        pTemp->resval.rstring = (char*) malloc(strlen(pszAppName) + 1);
        strcpy(pTemp->resval.rstring, pszAppName);
    }
    // Add specified string to the Xdata.
    //
    pTemp->rbnext = ads_newrb(AcDb::kDxfXdAsciiString);
    pTemp = pTemp->rbnext;
    pTemp->resval.rstring = (char*) malloc(strlen(pszXDString) + 1);
    strcpy(pTemp->resval.rstring, pszXDString);

    pObj->upgradeOpen();
    pObj->setXData(pRb);
    
    pObj->close();
    ads_relrb(pRb);
}
void AddXDataForContourLine(ads_name objName, const char* pszAppName, const double* pfContourVal) 
{
    struct  resbuf  *pRb, *pTemp;
    
	Acad::ErrorStatus ErrCode;
	AcDbObjectId objId;
	AcDbObject* pObj;
	
	ErrCode = acdbGetObjectId(objId, objName);
	if(ErrCode != Acad::eOk)
	{
		return;
	}
	ErrCode = acdbOpenObject(pObj, objId, AcDb::kForWrite);
	if(ErrCode != Acad::eOk)
	{
		return;
	}
    
	pRb = pObj->xData(pszAppName);
    if (pRb != NULL)
	{// If xData is present then walk to the end of the list
        for (pTemp = pRb; pTemp != 0L; pTemp = pTemp->rbnext)
		{
			if(AcDb::kDxfXdReal == pTemp->restype)
			{
				pTemp->resval.rreal = *pfContourVal;
			}
		}
    }
	else
	{// XData is not present, register the application
        ads_regapp(pszAppName);

        pRb = ads_newrb(AcDb::kDxfRegAppName);
        pTemp = pRb;
        pTemp->resval.rstring = (char*) malloc(strlen(pszAppName) + 1);
        strcpy(pTemp->resval.rstring, pszAppName);

		// Add specified real value to the Xdata.
		pTemp->rbnext = ads_newrb(AcDb::kDxfXdReal);
		pTemp = pTemp->rbnext;
		pTemp->resval.rreal = *pfContourVal;

		pObj->upgradeOpen();
		pObj->setXData(pRb);
    }
    
    pObj->close();
    ads_relrb(pRb);
}
void DeleteProducedEntities(const char* pszAppName)
{
	DeleteImportedPts(pszAppName);
	DeleteProducedTrgEnts();

	DeleteProducedContourLines(pszAppName);
	DeleteProducedTextEnts(pszAppName);
	DeletePolyLines(pszAppName);
}
BOOL DeleteImportedPts(const char* pszAppName)
{
	ads_name SelSetPts;
	
	{//Get all the imported points....
		struct  resbuf  Rb01, Rb02, Rb03;
		const char szEntName[] = "POINT";
		// Entity type
		Rb01.restype = 0;
		Rb01.resval.rstring = (char*)szEntName;
		Rb01.rbnext = &Rb02; 
		//Extended entity data 
		Rb02.restype = -3;
		Rb02.rbnext = &Rb03; 
		//application Name
		Rb03.restype = 1001; 
		Rb03.resval.rstring = (char*)pszAppName; 
		Rb03.rbnext = NULL; 
		
		if(ads_ssget("X", 0L, 0L, &Rb01, SelSetPts) != RTNORM)
		{
			return FALSE;
		}
	}
	return EraseEntitiesInSelSet(SelSetPts);//Func. itself will Free the Selset
}
BOOL DeletePolyLines(const char* pszAppName)
{
	ads_name SelSetPts;
	
	{//Get all the imported points....
		struct  resbuf  Rb01, Rb02, Rb03;
		const char szEntName[] = "POLYLINE";
		// Entity type
		Rb01.restype = 0;
		Rb01.resval.rstring = (char*)szEntName;
		Rb01.rbnext = &Rb02; 
		//Extended entity data 
		Rb02.restype = -3;
		Rb02.rbnext = &Rb03; 
		//application Name
		Rb03.restype = 1001; 
		Rb03.resval.rstring = (char*)pszAppName; 
		Rb03.rbnext = NULL; 
		
		if(ads_ssget("X", 0L, 0L, &Rb01, SelSetPts) != RTNORM)
		{
			return FALSE;
		}
	}
	return EraseEntitiesInSelSet(SelSetPts);//Func. itself will Free the Selset
}
BOOL DeleteProducedTextEnts(const char* pszAppName)
{
	ads_name SelSetPts;
	
	{//Get all the imported points....
		struct  resbuf  Rb01, Rb02, Rb03;
		const char szEntName[] = "TEXT";
		// Entity type
		Rb01.restype = 0;
		Rb01.resval.rstring = (char*)szEntName;
		Rb01.rbnext = &Rb02; 
		//Extended entity data 
		Rb02.restype = -3;
		Rb02.rbnext = &Rb03; 
		//application Name
		Rb03.restype = 1001; 
		Rb03.resval.rstring = (char*)pszAppName; 
		Rb03.rbnext = NULL; 
		
		if(ads_ssget("X", 0L, 0L, &Rb01, SelSetPts) != RTNORM)
		{
			return FALSE;
		}
	}
	return EraseEntitiesInSelSet(SelSetPts);//Func. itself will Free the Selset
}
BOOL DeleteProducedContourLines(const char* pszAppName)
{
	ads_name SelSetPts;
	
	{//Get all the imported Lines....
		struct  resbuf  Rb01, Rb02, Rb03;
		const char szEntName[] = "LINE";
		// Entity type
		Rb01.restype = 0;
		Rb01.resval.rstring = (char*)szEntName;
		Rb01.rbnext = &Rb02; 
		//Extended entity data 
		Rb02.restype = -3;
		Rb02.rbnext = &Rb03; 
		//application Name
		Rb03.restype = 1001; 
		Rb03.resval.rstring = (char*)pszAppName; 
		Rb03.rbnext = NULL; 
		
		if(ads_ssget("X", 0L, 0L, &Rb01, SelSetPts) != RTNORM)
		{
			return FALSE;
		}
	}
	return EraseEntitiesInSelSet(SelSetPts);//Func. itself will Free the Selset
}
BOOL DeleteProducedTrgEnts()
{
	ads_name SelSet;
	long iNoOfEnt;
	
	{//Get all TINEntities.... //<< (ssget "X" (list (cons 100 "AcDbEntity"))) >>
		struct  resbuf  Rb01;
		const char szEntName[] = "AcDbEntity";
		// Entity type
		Rb01.restype = 100;
		Rb01.resval.rstring = (char*)szEntName;
		Rb01.rbnext = 0L; 
		
		if(ads_ssget("X", 0L, 0L, &Rb01, SelSet) != RTNORM)
		{
			return FALSE;
		}
	}
	{//Erase Triangle Entity....
		long i;
		ads_name Entity;
		
		{//Erase all Triangle
			iNoOfEnt = 0;
			if(ads_sslength(SelSet, &iNoOfEnt) != RTNORM)
			{
				ads_ssfree(SelSet);//Free Selset
				return -1;//ERROR
			}
			for(i = 0; i < iNoOfEnt; i++)
			{
				if(ads_ssname(SelSet, i, Entity) != RTNORM)
				{
					ads_printf("\nERROR: AutoCAD System error !\n");
					ads_ssfree(SelSet);//Free Selset
					return -1;
				}
				{//Chk if TINEntity...
					AcDbObjectId objId;
					AcDbObject* pObj;
					Acad::ErrorStatus ErrCode;

					ErrCode = acdbGetObjectId(objId, Entity);
					if(ErrCode != Acad::eOk)
					{
						ads_ssfree(SelSet);//Free Selset
						ads_printf("\nERROR: AutoCAD System error !\n");
						return -1;
					}
					ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
					if(ErrCode != Acad::eOk)
					{
						ads_ssfree(SelSet);//Free Selset
						ads_printf("\nERROR: AutoCAD System error !\n");
						return -1;
					}
					if(pObj->isA() == TINEntity::desc())
					{
						pObj->close();
						if(ads_entdel(Entity) != RTNORM)
						{
							ads_ssfree(SelSet);//Free Selset
							ads_printf("\nERROR: AutoCAD System error !\n");
							return -1;
						}
					}
					else
					{
						pObj->close();
					}
				}
			}
		}
	}
	ads_ssfree(SelSet);//Free Selset
	return TRUE;
}
void EraseAllTINEntities(AcDbDatabase* pDb)
{
	//..............................................................................
    AcDbBlockTable* pBtbl;
	AcDbBlockTableRecord* pBtblRcd;
	int iCountTrg;

    pDb->getBlockTable(pBtbl, AcDb::kForRead);

    pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
    pBtbl->close();
	
	iCountTrg = 0;
	{
		AcDbBlockTableRecordIterator *pBlkTblRcdItr;
		pBtblRcd->newIterator(pBlkTblRcdItr);
		for(pBlkTblRcdItr->start(); !pBlkTblRcdItr->done(); pBlkTblRcdItr->step())
		{
			AcDbEntity* pEnt;

			pBlkTblRcdItr->getEntity(pEnt, AcDb::kForWrite);
			if(pEnt->isA() == TINEntity::desc())
			{
				iCountTrg++;
				pEnt->erase();
			}
			pEnt->close();
		}
		pBtblRcd->close();
		delete pBlkTblRcdItr;
	}
}
BOOL EraseEntities(const AcDbObjectIdArray& ObjIdArray)
{
	int i;
	AcDbObject* pObj;
	Acad::ErrorStatus ErrCode;

	for(i = 0; i < ObjIdArray.length(); i++)
	{
		ErrCode = acdbOpenObject(pObj, ObjIdArray[i], AcDb::kForWrite);
		if(ErrCode != Acad::eOk)
		{
			return FALSE;
		}
		pObj->erase();
		pObj->close();
	}
	return TRUE;
}
int IsTINEntityExists(AcDbEntity*& pEntityParam)
{
	//..............................................................................
    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
	AcDbBlockTableRecord* pBtblRcd;
	int iCountTrg;

	pDb = acdbCurDwg();
    pDb->getBlockTable(pBtbl, AcDb::kForRead);

    pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
    pBtbl->close();
	
	iCountTrg = 0;
	{
		AcDbBlockTableRecordIterator *pBlkTblRcdItr;
		pBtblRcd->newIterator(pBlkTblRcdItr);
		for(pBlkTblRcdItr->start(); !pBlkTblRcdItr->done(); pBlkTblRcdItr->step())
		{
			AcDbEntity* pEnt;

			pBlkTblRcdItr->getEntity(pEnt, AcDb::kForRead);
			if(pEnt->isA() == TINEntity::desc())
			{
				iCountTrg++;
				pEntityParam = pEnt;
			}
			pEnt->close();
			//if(iCountTrg > 1)
			//	break;//More than one TINEntity found
		}
		pBtblRcd->close();
		delete pBlkTblRcdItr;
	}
	return iCountTrg;
}
//NB: This func. frees the selset ...
BOOL EraseEntitiesInSelSet(ads_name& SelSet)
{
	//Erase Entity(s)....
	long i, iNoOfEnt;
	ads_name Entity;
	
	{//Erase all Lines
		iNoOfEnt = 0;
		if(ads_sslength(SelSet, &iNoOfEnt) != RTNORM)
		{
			ads_ssfree(SelSet);//Free Selset
			return FALSE;//ERROR
		}
		for(i = 0; i < iNoOfEnt; i++)
		{
			if(ads_ssname(SelSet, i, Entity) != RTNORM)
			{
				ads_printf("\nERROR: AutoCAD System error !\n");
				ads_ssfree(SelSet);//Free Selset
				return FALSE;//ERROR
			}
			if(ads_entdel(Entity) != RTNORM)
			{
				ads_printf("\nERROR: AutoCAD System error !\n");
				ads_ssfree(SelSet);//Free Selset
				return FALSE;//ERROR
			}
		}
	}
	ads_ssfree(SelSet);//Free Selset
	return TRUE;
}
//NB: This func. DOES NOT FREE the selset ...
BOOL ExtratObjectIDs(ads_name& SelSet, AcDbObjectIdArray& arrayResult)
{
	//Erase Entity(s)....
	long i, iNoOfEnt;
	ads_name Entity;
	
	{//Erase all Lines
		iNoOfEnt = 0;
		if(ads_sslength(SelSet, &iNoOfEnt) != RTNORM)
		{
			return FALSE;//ERROR
		}
		for(i = 0; i < iNoOfEnt; i++)
		{
			AcDbObjectId objId;

			if(ads_ssname(SelSet, i, Entity) != RTNORM)
			{
				ads_printf("\nERROR: AutoCAD System error -> ExtratObjectIDs() !\n");
				return FALSE;//ERROR
			}
			if(Acad::eOk != acdbGetObjectId(objId, Entity))
			{
				ads_printf("\nERROR: AutoCAD System error -> ExtratObjectIDs() !\n");
				return FALSE;
			}
			arrayResult.append(objId);
		}
	}
	ads_ssfree(SelSet);//Free Selset
	return TRUE;
}

void AddToACDBase(AcDbEntity* pEntParam)
{
	assert(0L != pEntParam);
	
	/////
    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
    AcDbBlockTableRecord* pBtblRcd;

	pDb = acdbCurDwg();

    pDb->getBlockTable(pBtbl, AcDb::kForRead);
 
    pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
    pBtbl->close();
 
    pBtblRcd->appendAcDbEntity(pEntParam);
    pEntParam->close();
    
    pBtblRcd->close();
}
