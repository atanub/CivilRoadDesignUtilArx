// DwgParamObj.cpp
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DwgParamObj.h"
#include "DrawUtil.h"
#include "ContourApp.h"


ACRX_DXF_DEFINE_MEMBERS(CDwgParamObj, AcDbObject, 0, ContourDwgParam, Contour ARX Application);
MAKE_ACDBOPENOBJECT_FUNCTION(CDwgParamObj);

CDwgParamObj::CDwgParamObj()
{
	//Default Data....
	memset(&m_DwgParam, 0, sizeof(m_DwgParam));
	m_DwgParam.fContourInterval = 0.5;
	m_DwgParam.iContourColorBase = 1;
	m_DwgParam.fTxtHeight = -1.0;
	m_DwgParam.fTxtSpaMF = -1;
	m_DwgParam.bIsRegistered = FALSE;

	strcpy(m_DwgParam.szPointXDFlag, "POINT_CONTOUR");
	strcpy(m_DwgParam.szAppName,	 "ATANU_BANIK");
	strcpy(m_DwgParam.szPointLyr,	 "POINT");
	strcpy(m_DwgParam.szTriangleLyr, "TRIANGLE");
	strcpy(m_DwgParam.szElevLyrPrefix,	 "CONTOUR_");
	m_DwgParam.chDotSeparator = '-';

	strcpy(m_DwgParam.szSpotLevLyr,		"SPOT_LEVEL");
	strcpy(m_DwgParam.szSpotLevBlock,	"");
	m_DwgParam.fSpotLevTxtHt = -1.0;
	m_DwgParam.fSpotLevRotAngle = 0.0;

	//Default Data....
	memset(&m_AnnParam, 0, sizeof(ANNTNPARAMS));
	m_AnnParam.fTxtHeight = TEXTSIZE();
	m_AnnParam.fRotAngle		= -1.0;
	m_AnnParam.iRotAngleType	= 0;//0 = Along Contour Line(s);
	m_AnnParam.bForceAnnotation	= TRUE;
	m_AnnParam.bForceAnnotation	= FALSE;

	strcpy(m_AnnParam.szLyr, "");
	m_AnnParam.iLyrType = 1;		//1 = Respective Elev. Layer;
}
CDwgParamObj::~CDwgParamObj()
{
	//Deallocate....
	int i;
	for(i = 0; i < m_BoundaryDef.arrayHoleDef.GetSize(); i++)
	{
		HOLEDEF* pHole;
		
		pHole = m_BoundaryDef.arrayHoleDef[i];
		delete pHole;
	}
	m_BoundaryDef.arrayHoleDef.RemoveAll();
}

void CDwgParamObj::GetParamData(DWGPARAMS* pRec) const
{
    assertReadEnabled();
    *pRec = m_DwgParam;
}
void CDwgParamObj::GetParamData(ANNTNPARAMS* pRec) const
{
    assertReadEnabled();
    *pRec = m_AnnParam;
}
BOOL CDwgParamObj::SetParamData(const DWGPARAMS* pRec)
{
    assertWriteEnabled();
    //Validate Data....
	//.....
    m_DwgParam = *pRec;
    return TRUE;
}
BOOL CDwgParamObj::SetParamData(const ANNTNPARAMS* pRec)
{
    assertWriteEnabled();
    //Validate Data....
	//.....
    m_AnnParam = *pRec;
    return TRUE;
}
BOOL CDwgParamObj::GetBoundaryParamData(BOUNDARYDEF*) const
{
    return FALSE;
}
BOOL CDwgParamObj::SetBoundaryParamData(const BOUNDARYDEF*)
{
    return FALSE;
}

// Files data in from a DWG file
//
Acad::ErrorStatus CDwgParamObj::dwgInFields(AcDbDwgFiler* filer)
{
    assertWriteEnabled();
    AcDbObject::dwgInFields(filer);

	if(Acad::eOk != filer->readBytes((void*)(&m_DwgParam), sizeof(m_DwgParam)))
	{
		return Acad::eDwgNeedsRecovery;
	}
	if(Acad::eOk != filer->readBytes((void*)(&m_AnnParam), sizeof(m_AnnParam)))
	{
		return Acad::eDwgNeedsRecovery;
	}
	return filer->filerStatus();
}


// Files data out to a DWG file
//
Acad::ErrorStatus CDwgParamObj::dwgOutFields(AcDbDwgFiler* filer) const
{
    assertReadEnabled();
    AcDbObject::dwgOutFields(filer);

	filer->writeBytes((void*)(&m_DwgParam), sizeof(m_DwgParam));
	filer->writeBytes((void*)(&m_AnnParam), sizeof(m_AnnParam));
    
	return filer->filerStatus();
}
//
BOOL CDwgParamObj::CreateDwgParamObj(const DWGPARAMS* pRec /*= 0L*/)
{
    AcDbDictionary*	pNamedobj;
    AcDbDatabase*	pCurDwg;
    AcDbDictionary* pDict;
	Acad::ErrorStatus ErrStat;
 	BOOL bRetValue;
   
	bRetValue = TRUE;
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

	{//Check whether 'CDwgParamObj' exists or not.... 
		CDwgParamObj*	pObjXisting;
		
		ErrStat = pDict->getAt(CContourApp::gStrDwgParamSexn, (AcDbObject*&)pObjXisting, AcDb::OpenMode::kForWrite);
		if(Acad::ErrorStatus::eOk == ErrStat)
		{
			if(pRec != 0L)
			{
				pObjXisting->SetParamData(pRec);
			}
			pObjXisting->close();
		}
		else
		{//Create New object ..
  			CDwgParamObj*	pObj;
		    AcDbObjectId	objId;

			pObj = new CDwgParamObj;
			if(pRec != 0L)
			{
				pObj->SetParamData(pRec);
			}
			pCurDwg->addAcDbObject(objId, pObj);// Append object database without setting an owner
			ErrStat = pDict->setAt(CContourApp::gStrDwgParamSexn, pObj, objId);//Add new..
			if(ErrStat != Acad::ErrorStatus::eOk)
			{
				CString strMsg;
				
				strMsg.Format("\"%s\" entry not found in dictionary \"%s\"\nPlease contact developer", CContourApp::gStrDwgParamSexn, CContourApp::gStrDictionaryName);
				AfxMessageBox(strMsg, MB_ICONSTOP);
				bRetValue = FALSE;
			}
			pObj->close();
		}
	}
    pDict->close();
    return bRetValue;
}
BOOL CDwgParamObj::CreateAnnParamObj(const ANNTNPARAMS* pRec /*= 0L*/)
{
    AcDbDictionary*	pNamedobj;
    AcDbDatabase*	pCurDwg;
    AcDbDictionary* pDict;
	Acad::ErrorStatus ErrStat;
	BOOL bRetValue;
    
    bRetValue = TRUE;
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

	{//Check whether 'CDwgParamObj' exists or not.... 
		CDwgParamObj*	pObjXisting;
		
		ErrStat = pDict->getAt(CContourApp::gStrDwgParamSexn, (AcDbObject*&)pObjXisting, AcDb::OpenMode::kForWrite);
		if(Acad::ErrorStatus::eOk == ErrStat)
		{
			if(pRec != 0L)
			{
				pObjXisting->SetParamData(pRec);
			}
			pObjXisting->close();
		}
		else
		{//Create New object ..
  			CDwgParamObj*	pObj;
		    AcDbObjectId	objId;

			pObj = new CDwgParamObj;
			if(pRec != 0L)
			{
				pObj->SetParamData(pRec);
			}
			pCurDwg->addAcDbObject(objId, pObj);// Append object database without setting an owner
			ErrStat = pDict->setAt(CContourApp::gStrDwgParamSexn, pObj, objId);//Add new..
			if(ErrStat != Acad::ErrorStatus::eOk)
			{
				CString strMsg;
				
				strMsg.Format("\"%s\" entry not found in dictionary \"%s\"\nPlease contact developer", CContourApp::gStrDwgParamSexn, CContourApp::gStrDictionaryName);
				AfxMessageBox(strMsg, MB_ICONSTOP);
			    bRetValue = FALSE;
			}
			pObj->close();
		}
	}
    pDict->close();
	return bRetValue;
}
BOOL CDwgParamObj::RemoveDwgParamObj(AcDbDatabase* pCurDwg)
{
    AcDbDictionary*	pNamedobj;
	AcDbObjectId returnIdRoot, returnIdSub;
    BOOL bFlag;
    
	bFlag = FALSE;
    pCurDwg->getNamedObjectsDictionary(pNamedobj, AcDb::kForWrite);

    if (pNamedobj->remove(CContourApp::gStrDictionaryName, returnIdRoot) == Acad::eOk)
    {//Removal possible
		AcDbObject* pRootObj;

		if(Acad::ErrorStatus::eOk == acdbOpenAcDbObject(pRootObj, returnIdRoot, AcDb::kForWrite))
		{
			if(Acad::ErrorStatus::eOk == ((AcDbDictionary*)pRootObj)->remove(CContourApp::gStrDwgParamSexn, returnIdSub))
			{
				AcDbObject* pSubObj;
				if(Acad::ErrorStatus::eOk == acdbOpenAcDbObject(pSubObj, returnIdSub, AcDb::kForWrite))
				{
					pSubObj->erase();
					pSubObj->close();
					bFlag = TRUE;
				}
			}
			pRootObj->erase();
			pRootObj->close();
		}
    }
    pNamedobj->close();
	return bFlag;
}
BOOL CDwgParamObj::GetDwgParamData(DWGPARAMS* pRec/*= 0L*/)
{
	Acad::ErrorStatus ErrStat;
    AcDbDictionary *pNamedobj;
    AcDbDictionary *pDict;
    BOOL bFlag;
	
	memset(pRec, 0, sizeof(DWGPARAMS));
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

		ErrStat = pDict->getAt(CContourApp::gStrDwgParamSexn, pAcDb, AcDb::OpenMode::kForRead);
		if(ErrStat != Acad::ErrorStatus::eOk)
		{
			CString strMsg;
	
			pDict->close();
			strMsg.Format("\"%s\" entry not found in dictionary \"%s\"\nPlease contact developer", CContourApp::gStrDwgParamSexn, CContourApp::gStrDictionaryName);
			AfxMessageBox(strMsg, MB_ICONSTOP);
			return FALSE;
		}
		if(pAcDb->isA() == CDwgParamObj::desc())
		{
			if(pRec != 0L)
			{
				((CDwgParamObj*)pAcDb)->GetParamData(pRec);
				pRec->chDotSeparator = '-';//for compatibilty with older versions
			}
			bFlag = TRUE;
		}
		pAcDb->close();
	}
    pDict->close();
	return bFlag;
}

BOOL CDwgParamObj::GetAnnParamData(ANNTNPARAMS* pRec /* = 0L*/)
{
	Acad::ErrorStatus ErrStat;
    AcDbDictionary *pNamedobj;
    AcDbDictionary *pDict;
    BOOL bFlag;
	
	if(0L != pRec)
	{
		memset(pRec, 0, sizeof(ANNTNPARAMS));
	}
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

		ErrStat = pDict->getAt(CContourApp::gStrDwgParamSexn, pAcDb, AcDb::OpenMode::kForRead);
		if(ErrStat != Acad::ErrorStatus::eOk)
		{
			CString strMsg;
	
			pDict->close();
			strMsg.Format("\"%s\" entry not found in dictionary \"%s\"\nPlease contact developer", CContourApp::gStrDwgParamSexn, CContourApp::gStrDictionaryName);
			AfxMessageBox(strMsg, MB_ICONSTOP);
			return FALSE;
		}
		if(pAcDb->isA() == CDwgParamObj::desc())
		{
			if(pRec != 0L)
			{
				((CDwgParamObj*)pAcDb)->GetParamData(pRec);
			}
			bFlag = TRUE;
		}
		pAcDb->close();
	}
    pDict->close();
	return bFlag;
}

BOOL CDwgParamObj::CreateBoundaryParamObj(const BOUNDARYDEF*)
{
	return FALSE;
}
BOOL CDwgParamObj::GetBoundaryParamData(BOUNDARYDEF* /*= 0L*/)
{
	return FALSE;
}


