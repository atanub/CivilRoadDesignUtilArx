// Purge.cpp
/////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <AdsCodes.h>
#include <dbents.h>
#include <Dbsymtb.h>
#include "Purge.h"
#include "DwgParamObj.h"
#include "TINData.h"
#include "DrawUtil.h"

static BOOL GetAllLyrObjIDs(AcDbDatabase*, AcDbObjectIdArray&);
static BOOL GetAllBlkObjIDs(AcDbDatabase*, AcDbObjectIdArray&);
static BOOL GetAllDimStyleObjIDs(AcDbDatabase*, AcDbObjectIdArray&);
static BOOL GetAllLTypesObjIDs(AcDbDatabase*, AcDbObjectIdArray&);
static BOOL GetAllTxtStylesObjIDs(AcDbDatabase*, AcDbObjectIdArray&);

void PurgeAll()
{
	AcDbDatabase* pDb;
	AcDbObjectIdArray ObjIdArray;
	int i;

	pDb = acdbCurDwg();
	if(pDb == 0L)
		return;

	if(AfxMessageBox("About to purge all unused AutoCAD objects and Contour generated objects\nAre you sure to proceed ?", MB_ICONWARNING|MB_YESNO) != IDYES)
		return;
	//Blocks/LAyers/Dimstyles/LTypes/SHapes/STyles/Mlinestyles
	GetAllBlkObjIDs(pDb, ObjIdArray);
	GetAllLyrObjIDs(pDb, ObjIdArray);
	GetAllDimStyleObjIDs(pDb, ObjIdArray);
	GetAllLTypesObjIDs(pDb, ObjIdArray);
	GetAllTxtStylesObjIDs(pDb, ObjIdArray);

	CDwgParamObj::RemoveDwgParamObj(pDb);
	CTINData::RemoveTINDataObj(pDb);
	EraseAllTINEntities(pDb);

	//Start Purging.....
	pDb->purge(ObjIdArray);
	for(i = 0; i < ObjIdArray.length(); i++)
	{
		Acad::ErrorStatus ErrCode;
		AcDbObject* pObj;
		
		ErrCode = acdbOpenAcDbObject(pObj, ObjIdArray[i], AcDb::kForWrite);
		if(Acad::eOk == ErrCode)
		{
			pObj->erase();
		}
		pObj->close();
	}
	ads_printf("\n%d# of objects purged successflly\n", ObjIdArray.length());
}

static BOOL GetAllLyrObjIDs(AcDbDatabase* pDb, AcDbObjectIdArray& ObjIdArray)
{
	int iCount;
	AcDbObjectId ObjID;
	Acad::ErrorStatus ErrCode;

	if(pDb == 0L)
		return FALSE;
	
	iCount = 0;
	{//Layers....
		AcDbLayerTable* pLyrTable;
		AcDbLayerTableIterator* pIterator;

		ErrCode = pDb->getLayerTable(pLyrTable, AcDb::kForRead);
		if(Acad::eOk != ErrCode)
		{
			pLyrTable->close();
			return FALSE;//ERROR!!
		}

		ErrCode = pLyrTable->newIterator(pIterator);
		if(Acad::eOk != ErrCode)
		{
			pLyrTable->close();
			return FALSE;//ERROR!!
		}
		for(pIterator->start(), iCount = 0; (pIterator->done() == Adesk::kFalse); pIterator->step())
		{
			AcDbLayerTableRecord* pLyrRec;
			
			ErrCode = pIterator->getRecord(pLyrRec, AcDb::kForRead);
 			if(Acad::eOk != ErrCode)
			{
				pLyrTable->close();
				delete pIterator;
				return FALSE;//Some errors occured....
			}
			ObjID = pLyrRec->objectId();
			ObjIdArray.append(ObjID);
			iCount++;
			pLyrRec->close();
		}
		pLyrTable->close();
		delete pIterator;
	}
	return (iCount > 0);
}
static BOOL GetAllBlkObjIDs(AcDbDatabase* pDb, AcDbObjectIdArray& ObjIdArray)
{
	int iCount;
	AcDbObjectId ObjID;
	Acad::ErrorStatus ErrCode;

	if(pDb == 0L)
		return FALSE;
	
	iCount = 0;
	{//Blocks....
		AcDbBlockTable* pBtbl;
		AcDbBlockTableIterator* pIterator;
		
		pDb->getBlockTable(pBtbl, AcDb::kForRead);
		ErrCode = pBtbl->newIterator(pIterator);
		pBtbl->close();
		if(Acad::eOk != ErrCode)
			return FALSE;//Some errors occured....
 
		for(pIterator->start(), iCount = 0; (pIterator->done() == Adesk::kFalse); pIterator->step())
		{
			AcDbBlockTableRecord* pBlkRec;
			
			ErrCode = pIterator->getRecord(pBlkRec, AcDb::kForRead);
 			if(Acad::eOk != ErrCode)
			{
				delete pIterator;
				return FALSE;//Some errors occured....
			}
			ObjIdArray.append(pBlkRec->objectId());
			pBlkRec->close();
		}
		delete pIterator;
	}
	return (iCount > 0);
}
static BOOL GetAllDimStyleObjIDs(AcDbDatabase* pDb, AcDbObjectIdArray& ObjIdArray)
{
	int iCount;
	Acad::ErrorStatus ErrCode;

	if(pDb == 0L)
		return FALSE;
	
	iCount = 0;
	{//Dimension Styles....
		AcDbDimStyleTable* pTbl;
		AcDbDimStyleTableIterator* pIterator;
		
		pDb->getDimStyleTable(pTbl, AcDb::kForRead);
		ErrCode = pTbl->newIterator(pIterator);
		pTbl->close();
		if(Acad::eOk != ErrCode)
			return FALSE;//Some errors occured....
 
		for(pIterator->start(), iCount = 0; (pIterator->done() == Adesk::kFalse); pIterator->step())
		{
			AcDbDimStyleTableRecord* pRec;
			
			ErrCode = pIterator->getRecord(pRec, AcDb::kForRead);
 			if(Acad::eOk != ErrCode)
			{
				delete pIterator;
				return FALSE;//Some errors occured....
			}
			ObjIdArray.append(pRec->objectId());
			pRec->close();
		}
		delete pIterator;
	}
	return (iCount > 0);
}
static BOOL GetAllLTypesObjIDs(AcDbDatabase* pDb, AcDbObjectIdArray& ObjIdArray)
{
	int iCount;
	Acad::ErrorStatus ErrCode;

	if(pDb == 0L)
		return FALSE;
	
	iCount = 0;
	{//Dimension Styles....
		AcDbLinetypeTable* pTbl;
		AcDbLinetypeTableIterator* pIterator;
		
		pDb->getLinetypeTable(pTbl, AcDb::kForRead);
		ErrCode = pTbl->newIterator(pIterator);
		pTbl->close();
		if(Acad::eOk != ErrCode)
			return FALSE;//Some errors occured....
 
		for(pIterator->start(), iCount = 0; (pIterator->done() == Adesk::kFalse); pIterator->step())
		{
			AcDbLinetypeTableRecord* pRec;
			
			ErrCode = pIterator->getRecord(pRec, AcDb::kForRead);
 			if(Acad::eOk != ErrCode)
			{
				delete pIterator;
				return FALSE;//Some errors occured....
			}
			ObjIdArray.append(pRec->objectId());
			pRec->close();
		}
		delete pIterator;
	}
	return (iCount > 0);
}
static BOOL GetAllTxtStylesObjIDs(AcDbDatabase* pDb, AcDbObjectIdArray& ObjIdArray)
{
	int iCount;
	Acad::ErrorStatus ErrCode;

	if(pDb == 0L)
		return FALSE;
	
	iCount = 0;
	{//Dimension Styles....
		AcDbTextStyleTable* pTbl;
		AcDbTextStyleTableIterator* pIterator;
		
		pDb->getTextStyleTable(pTbl, AcDb::kForRead);
		ErrCode = pTbl->newIterator(pIterator);
		pTbl->close();
		if(Acad::eOk != ErrCode)
			return FALSE;//Some errors occured....
 
		for(pIterator->start(), iCount = 0; (pIterator->done() == Adesk::kFalse); pIterator->step())
		{
			AcDbTextStyleTableRecord* pRec;
			
			ErrCode = pIterator->getRecord(pRec, AcDb::kForRead);
 			if(Acad::eOk != ErrCode)
			{
				delete pIterator;
				return FALSE;//Some errors occured....
			}
			ObjIdArray.append(pRec->objectId());
			pRec->close();
		}
		delete pIterator;
	}
	return (iCount > 0);
}

