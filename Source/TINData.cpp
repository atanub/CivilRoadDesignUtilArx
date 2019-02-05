// TINData.cpp
////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TINData.h"
#include "DrawUtil.h"
#include "ContourApp.h"
#include "DwgParamObj.h"
#include "ImportPts.h"
#include "TIN.h"

ACRX_DXF_DEFINE_MEMBERS(CTINData, AcDbObject, 0, ContourData, Contour ARX Application);
MAKE_ACDBOPENOBJECT_FUNCTION(CTINData);

void CTINData::DeallocateTrgArray(CTrianglePtrArray& TrgArray)
{
	int i;
	TRIANGLE* pRec;
	for(i = 0; i < TrgArray.GetSize(); i++)
	{
		pRec = TrgArray[i];
		delete pRec;
	}
	if(TrgArray.GetSize() > 0)
		TrgArray.RemoveAll();
}
CTINData::CTINData()
{
	DeallocateTrgArray(m_TrgPtrArray);
}

CTINData::~CTINData()
{
	//Deallocate Triangle data ...
}
void CTINData::FindElevRange(const AcGePoint3dArray& Array, double& fMinElev, double& fMaxElev)
{
	long i;
	
	fMinElev = 0.0;
	fMaxElev = 0.0;
	if(Array.length() <= 0)
	{
		return;
	}
	fMinElev = Array[0].z;
	fMaxElev = Array[0].z;
	for(i = 1; i < Array.length(); i++)
	{
		if(Array[i].z < fMinElev)
		{
			fMinElev = Array[i].z;
		}
		if(Array[i].z > fMaxElev)
		{
			fMaxElev = Array[i].z;
		}
	}
}
BOOL CTINData::SetTINData(const AcGePoint3dArray& PtArray, const CTrianglePtrArray& TrgPtrArray)
{
    //assertWriteEnabled();

    {//Validate Data....
		//.....
	}
	{//Deallocate prev Data...
		//Point data...
		if(m_PointArray.length() > 0)
			m_PointArray.removeSubArray(0, m_PointArray.length() - 1);
	
		//Triangle data...
		DeallocateTrgArray(m_TrgPtrArray);
	}
	{//Set Data....
		//Point data...
		m_PointArray.append(PtArray);

		//Triangle data...
		Adesk::Int32 i;
		TRIANGLE* pRec;
		TRIANGLE* pNewRec;
		for(i = 0; i < TrgPtrArray.GetSize(); i++)
		{
			pRec = TrgPtrArray[i];
			pNewRec = new TRIANGLE;
			*pNewRec = *pRec;
			m_TrgPtrArray.Add(pNewRec);
		}
	}
    return TRUE;
}
// Files data in from a DWG file
//
Acad::ErrorStatus CTINData::dwgInFields(AcDbDwgFiler* filer)
{
    assertWriteEnabled();

    AcDbObject::dwgInFields(filer);

	Adesk::Int32 i, lCount;

	{//Reset
		//Point data...
		if(m_PointArray.length() > 0)
			m_PointArray.removeSubArray(0, m_PointArray.length() - 1);

		//Triangle data...
		DeallocateTrgArray(m_TrgPtrArray);
	}
	{//Read
		//all the Points.......
		filer->readInt32(&lCount);
		for(i = 0; i < lCount; i++)
		{
			AcGePoint3d PtTmp;
			
			filer->readPoint3d(&PtTmp);
			m_PointArray.append(PtTmp);
		}
		//all the Triangles.......
		filer->readInt32(&lCount);
		for(i = 0; i < lCount; i++)
		{
			TRIANGLE Rec;
			TRIANGLE* pRec;
			
			filer->readInt32(&(Rec.p1));
			filer->readInt32(&(Rec.p2));
			filer->readInt32(&(Rec.p3));
			
			pRec = new TRIANGLE;
			*pRec = Rec;
			m_TrgPtrArray.Add(pRec);
		}
	}
    return filer->filerStatus();
}
// Files data out to a DWG file
//
Acad::ErrorStatus CTINData::dwgOutFields(AcDbDwgFiler* filer) const
{
    assertReadEnabled();

    AcDbObject::dwgOutFields(filer);
	
	//Write all the Points.......
	Adesk::Int32 i, lCount;
	
	lCount = m_PointArray.length();
	filer->writeInt32(lCount);
	for(i = 0; i < lCount; i++)
	{
		filer->writePoint3d(m_PointArray.at(i));
	}
	//Write all the Triangles.......
	lCount = m_TrgPtrArray.GetSize();
	filer->writeInt32(lCount);
	for(i = 0; i < lCount; i++)
	{
		TRIANGLE* pRec;
		
		pRec = m_TrgPtrArray[i];
		
		filer->writeInt32(pRec->p1);
		filer->writeInt32(pRec->p2);
		filer->writeInt32(pRec->p3);
	}
	return filer->filerStatus();
}
BOOL CTINData::GetElevAt(const double& fX, const double& fY, double& fZ) const
{
	return TINEntity::IsPointWithinBoundary(m_PointArray, m_TrgPtrArray, AcGePoint3d(fX, fY, 0.0), &fZ);
}
//Global Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTINData::CreateTINDataObj(const AcGePoint3dArray& ptArray, const CTrianglePtrArray& TrgArray)
{
    AcDbDictionary*	pNamedobj;
    AcDbDatabase*	pCurDwg;
    AcDbDictionary* pDict;
	Acad::ErrorStatus ErrStat;
    
    pDict = NULL;
	pCurDwg = acdbCurDwg();

    pCurDwg->getNamedObjectsDictionary(pNamedobj, AcDb::kForWrite);
    if (pNamedobj->getAt(CContourApp::gStrDictionaryName, (AcDbObject*&)pDict, AcDb::kForWrite) == Acad::eKeyNotFound)
    {
        AcDbObjectId DictId;
        
		pDict = new AcDbDictionary;
        pNamedobj->setAt(CContourApp::gStrDictionaryName, pDict, DictId);
    }
    pNamedobj->close();

	{//Check whether 'CTINData' exists or not.... 
		CTINData*	pObjXisting;
		
		ErrStat = pDict->getAt(CContourApp::gStrTINDataSexn, (AcDbObject*&)pObjXisting, AcDb::OpenMode::kForWrite);
		if(Acad::ErrorStatus::eOk == ErrStat)
		{
			pObjXisting->SetTINData(ptArray, TrgArray);
			pObjXisting->close();
		}
		else
		{//Create New object ..
  			CTINData*	pObj;
		    AcDbObjectId	objId;

			pObj = new CTINData;
			pObj->SetTINData(ptArray, TrgArray);
			pCurDwg->addAcDbObject(objId, pObj);// Append object database without setting an owner
			ErrStat = pDict->setAt(CContourApp::gStrTINDataSexn, pObj, objId);//Add new..
			if(ErrStat != Acad::ErrorStatus::eOk)
			{
				CString strMsg;
				
				strMsg.Format("\"%s\" entry not found in dictionary \"%s\"\nPlease contact developer", CContourApp::gStrTINDataSexn, CContourApp::gStrDictionaryName);
				AfxMessageBox(strMsg, MB_ICONSTOP);
			}
			pObj->close();
		}
	}
    pDict->close();
}
void CTINData::RemoveTINDataObj(AcDbDatabase* pCurDwg)
{
    AcDbDictionary*	pNamedobj;
	AcDbObjectId returnIdRoot, returnIdSub;
    
    pCurDwg->getNamedObjectsDictionary(pNamedobj, AcDb::kForWrite);

    if (pNamedobj->remove(CContourApp::gStrDictionaryName, returnIdRoot) == Acad::eOk)
    {//Removal possible
		AcDbObject* pRootObj;

		if(Acad::ErrorStatus::eOk == acdbOpenAcDbObject(pRootObj, returnIdRoot, AcDb::kForWrite))
		{
			if(Acad::ErrorStatus::eOk == ((AcDbDictionary*)pRootObj)->remove(CContourApp::gStrTINDataSexn, returnIdSub))
			{
				AcDbObject* pSubObj;
				if(Acad::ErrorStatus::eOk == acdbOpenAcDbObject(pSubObj, returnIdSub, AcDb::kForWrite))
				{
					pSubObj->erase();
					pSubObj->close();
				}
			}
			pRootObj->erase();
			pRootObj->close();
		}
    }
    pNamedobj->close();
}
void CTINData::ExtractTINData(AcGePoint3dArray& ptArray, CTrianglePtrArray* pTrgArray /*= 0L*/) const
{
    assertReadEnabled();

	if(ptArray.length() > 0)
		ptArray.removeSubArray(0, ptArray.length() - 1);

	ptArray.append(m_PointArray);
	if(0L != pTrgArray)
	{//Set Triangle data param now...
		Adesk::Int32 i;
		TRIANGLE* pRec;
		TRIANGLE* pNewRec;

		DeallocateTrgArray(*pTrgArray);
		for(i = 0; i < m_TrgPtrArray.GetSize(); i++)
		{
			pRec = m_TrgPtrArray[i];
			pNewRec = new TRIANGLE;
			*pNewRec = *pRec;
			pTrgArray->Add(pNewRec);
		}
	}
}
BOOL CTINData::GetTINData(AcGePoint3dArray& ptArray, CTrianglePtrArray* pTrgArray /* = 0L*/)
{
	Acad::ErrorStatus ErrStat;
    AcDbDictionary *pNamedobj;
    AcDbDictionary *pDict;
    BOOL bFlag;
	
	bFlag = FALSE;
	if(acdbCurDwg() == 0L)
		return FALSE;
	
	acdbCurDwg()->getNamedObjectsDictionary(pNamedobj, AcDb::kForWrite);

    // get a pointer to the 'CContourApp::gStrDictionaryName' dictionary
	ErrStat = pNamedobj->getAt(CContourApp::gStrDictionaryName,(AcDbObject*&)pDict, AcDb::kForRead);
    pNamedobj->close();
    if(ErrStat != Acad::ErrorStatus::eOk)
	{
		return FALSE;
	}
	{//
		AcDbObject* pAcDb;

		ErrStat = pDict->getAt(CContourApp::gStrTINDataSexn, pAcDb, AcDb::OpenMode::kForRead);
		if(ErrStat != Acad::ErrorStatus::eOk)
		{
			CString strMsg;
	
			pDict->close();
			return FALSE;
		}
		if(pAcDb->isA() == CTINData::desc())
		{
			((CTINData*)pAcDb)->ExtractTINData(ptArray, pTrgArray);
			bFlag = TRUE;
		}
		pAcDb->close();
	}
    pDict->close();
	return bFlag;
}
BOOL CTINData::IsPresentInDatabase()
{
	Acad::ErrorStatus ErrStat;
    AcDbDictionary *pNamedobj;
    AcDbDictionary *pDict;
    BOOL bFlag;
	
	bFlag = FALSE;
	if(acdbCurDwg() == 0L)
		return FALSE;
	
	acdbCurDwg()->getNamedObjectsDictionary(pNamedobj, AcDb::kForWrite);

    // get a pointer to the 'CContourApp::gStrDictionaryName' dictionary
	ErrStat = pNamedobj->getAt(CContourApp::gStrDictionaryName,(AcDbObject*&)pDict, AcDb::kForRead);
    pNamedobj->close();
    if(ErrStat != Acad::ErrorStatus::eOk)
	{
		return FALSE;
	}
	{//
		AcDbObject* pAcDb;

		ErrStat = pDict->getAt(CContourApp::gStrTINDataSexn, pAcDb, AcDb::OpenMode::kForRead);
		if(ErrStat != Acad::ErrorStatus::eOk)
		{
			CString strMsg;
	
			pDict->close();
			strMsg.Format("\"%s\" entry not found in dictionary \"%s\"\nPlease contact developer", CContourApp::gStrTINDataSexn, CContourApp::gStrDictionaryName);
			AfxMessageBox(strMsg, MB_ICONSTOP);
			return FALSE;
		}
		if(pAcDb->isA() == CTINData::desc())
		{
			bFlag = TRUE;
		}
		pAcDb->close();
	}
    pDict->close();
	return bFlag;
}
CTINData* CTINData::GetTINObject(AcDb::OpenMode iOpenMode)
{
	Acad::ErrorStatus ErrStat;
    AcDbDictionary *pNamedobj;
    AcDbDictionary *pDict;
    CTINData* pObjToRet;
	
	pObjToRet = 0L;
	if(acdbCurDwg() == 0L)
		return 0L;
	
	acdbCurDwg()->getNamedObjectsDictionary(pNamedobj, iOpenMode);

    // get a pointer to the 'CContourApp::gStrDictionaryName' dictionary
	ErrStat = pNamedobj->getAt(CContourApp::gStrDictionaryName,(AcDbObject*&)pDict, iOpenMode);
    pNamedobj->close();
    if(ErrStat != Acad::ErrorStatus::eOk)
	{
		return 0L;
	}
	{//
		AcDbObject* pAcDb;

		ErrStat = pDict->getAt(CContourApp::gStrTINDataSexn, pAcDb, iOpenMode);
		if(ErrStat != Acad::ErrorStatus::eOk)
		{
			CString strMsg;
	
			pDict->close();
			strMsg.Format("\"%s\" entry not found in dictionary \"%s\"\nPlease contact developer", CContourApp::gStrTINDataSexn, CContourApp::gStrDictionaryName);
			AfxMessageBox(strMsg, MB_ICONSTOP);
			return 0L;
		}
		if(pAcDb->isA() == CTINData::desc())
		{
			pObjToRet = (CTINData*)pAcDb;
		}
	}
    pDict->close();
	return pObjToRet;
}
void CTINData::GenTIN()
{
	AcGePoint3dArray Array;
	CTrianglePtrArray TrgArray;
	DWGPARAMS DwgParams;
	int iNoOfPoints;
	//UINT iRet;
	BOOL bChkForDuplicatePt;
	
	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		AfxMessageBox("ERROR: Contour drawing parameters not set", MB_ICONSTOP);
		return;
	}
	
	DeleteProducedTrgEnts();
	//ads_printf("\nPlease wait... Collecting points from drawing for triangulation....");
	//
	//iRet = AfxMessageBox("Check for duplicate data points is highly recommended\nWould you like to check ?", MB_YESNOCANCEL|MB_ICONQUESTION);
	//if(IDCANCEL == iRet)
	//{
	//	return;
	//}
	//bChkForDuplicatePt = (IDYES == iRet);
	bChkForDuplicatePt = TRUE;
	
	//iNoOfPoints = GetContourPointsFromDWG(Array, DwgParams.szAppName, bChkForDuplicatePt);
	iNoOfPoints = CImportPtsUtil::GetPointsForTINGeneration(Array, DwgParams.szAppName, bChkForDuplicatePt);//21-06-2003
	switch(iNoOfPoints)
	{
	case  0:
		//AfxMessageBox("No points imported yet...\nTriangulation Failed !", MB_ICONSTOP);
		return;
	case -1:
		AfxMessageBox("Internal error occured...\nTriangulation Failed !", MB_ICONSTOP);
		return;
	case -2:
		//User termination
		return;
	default:
		break;
	}
	if(ads_usrbrk() == 1)
	{
		ads_printf("\nUser break encountered\n");
		goto CLEANUP;
	}
	ads_printf("\nPlease wait... Triangulation is in progress");
	if(TIN::Triangulate(Array, TrgArray) == 0)
	{//Failure....
		AfxMessageBox("Triangulation failed !!", MB_ICONINFORMATION);
		ads_printf("\nTriangulation failed!!!");
	}
	else
	{//Create TIN Entity....
		CTINData::CreateTINDataObj(Array, TrgArray);
		ads_printf("\nTriangulation done successfully");
		if(IDYES == AfxMessageBox("Triangulation done successfully\nDo you want to create TINEntity (draw triangles) ?", MB_YESNO|MB_ICONQUESTION))
		{
			LAYER(DwgParams.szTriangleLyr);
			DrawTINEntity(Array, TrgArray, DwgParams.szTriangleLyr);
		}
	}
	
	goto CLEANUP;
	//////////////////////////////////////////////////////////////////////
	CLEANUP :
	{//Deallocation
		int i;
		TRIANGLE* pRec;
		for(i = 0; i < TrgArray.GetSize(); i++)
		{
			pRec = TrgArray[i];
			delete pRec;
		}
	}
	
}
