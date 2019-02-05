//XData.Cpp
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Util.h"
#include "XData.h"
#include <AdsCodes.h>
#include <dbents.h>
#include "ModifyAnnotationDlg.h"

const char* CXDataPflAndCS::m_pszAppName		= "Atanu_Banik";
const char* CXDataPflAndCS::m_pszChgTag			= "CHAINAGE";
const char* CXDataPflAndCS::m_pszEastTag		= "EASTING";
const char* CXDataPflAndCS::m_pszNorthTag		= "NORTHING";
const char* CXDataPflAndCS::m_pszElevTag		= "ELEVATION";

BOOL CXDataPflAndCS::RegApp(const char* pszAppName)
{
	struct resbuf* pResBuf;

	pResBuf = ads_tblsearch("APPID", pszAppName, false);
	if(pResBuf == NULL)
	{
		if (ads_regapp(pszAppName) != RTNORM)
		{
			ads_printf("Cannot register application %s.\n", pszAppName);
			return FALSE;
		}
	}
	else
	{
		ads_relrb(pResBuf);
	}
	return TRUE;
}
BOOL CXDataPflAndCS::AttachXData(ads_name objName, const char* pszXDString, const double& fXDVal)
{
    struct  resbuf  *pRb, *pTemp;
    
	Acad::ErrorStatus ErrCode;
	AcDbObjectId objId;
	AcDbObject* pObj;
	
	if(!CXDataPflAndCS::RegApp(CXDataPflAndCS::m_pszAppName))
	{
		ads_printf("\nERROR: Failed to register application \"%s\"\n", CXDataPflAndCS::m_pszAppName);
		return FALSE;
	}

	ErrCode = acdbGetObjectId(objId, objName);
	if(ErrCode != Acad::eOk)
	{
		ads_printf("\nFATAL ERROR: Please Contact Developer\n");
		return FALSE;
	}
	ErrCode = acdbOpenObject(pObj, objId, AcDb::kForWrite);
	if(ErrCode != Acad::eOk)
	{
		ads_printf("\nFATAL ERROR: Please Contact Developer\n");
		return FALSE;
	}
    
	pRb = pObj->xData(CXDataPflAndCS::m_pszAppName);
    if (pRb != NULL)
	{// If xData is present then walk to the end of the list
        for (pTemp = pRb; pTemp != NULL; pTemp = pTemp->rbnext)
		{
		}
    }
	else
	{// XData is not present, register the application
        pRb = ads_newrb(AcDb::kDxfRegAppName);
        pTemp = pRb;
        pTemp->resval.rstring = (char*) malloc(strlen(CXDataPflAndCS::m_pszAppName) + 1);
        strcpy(pTemp->resval.rstring, CXDataPflAndCS::m_pszAppName);
    }
    // Add specified string to the Xdata.
    //
    pTemp->rbnext = ads_newrb(AcDb::kDxfXdAsciiString);
    pTemp = pTemp->rbnext;
    pTemp->resval.rstring = (char*) malloc(strlen(pszXDString) + 1);
    strcpy(pTemp->resval.rstring, pszXDString);

	//Add 'Double' value
    pTemp->rbnext = ads_newrb(AcDb::kDxfXdReal);
    pTemp = pTemp->rbnext;
    pTemp->resval.rreal = fXDVal;
	
    pObj->upgradeOpen();
    pObj->setXData(pRb);
    
    pObj->close();
    ads_relrb(pRb);
	return TRUE;
}
//Dumps Result types....
void CXDataPflAndCS::PrintResBuf(struct resbuf *pResBuf)
{
	struct resbuf *pTemp;

	ads_printf("\nPrinting Result Buffer's each element's type...\n");
	for(pTemp = pResBuf; pTemp != 0L; pTemp = pTemp->rbnext)
	{
		ads_printf("Type = %d\n", pTemp->restype);
	}
}
BOOL CXDataPflAndCS::ModifyXData(ads_name Entity, const CString& strData, const double& fValueParam)
{
	double fValue;

	if(!CXDataPflAndCS::InquireXData(Entity, strData, fValue))
	{
		ads_printf("\nERROR: XData not defined for the entity parameter...\n");
		return FALSE;
	}
	{//Update Value........
		AcDbObject* pObj;
		AcDbObjectId objId;
		Acad::ErrorStatus ErrCode;
		AcGePoint3d ptTmp;

		ErrCode = acdbGetObjectId(objId, Entity);
		if(ErrCode != Acad::eOk)
		{
			ads_printf("\nERROR: AutoCAD System error !\n");
			return FALSE;
		}
		ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
		if(ErrCode != Acad::eOk)
		{
			ads_printf("\nERROR: AutoCAD System error !\n");
			return FALSE;
		}
		if(pObj->isA() != AcDbText::desc())
		{
			ads_printf("\nERROR: Entity selected is not a Text Entity\n");
			pObj->close();
			return FALSE;
		}
		{//Inquire & Set XData
			resbuf* pXDataResBuf;
			resbuf *pTemp;
			
			pXDataResBuf = pObj->xData(CXDataPflAndCS::m_pszAppName);
			if(pXDataResBuf != 0L)
			{
				for (pTemp = pXDataResBuf; pTemp != NULL; pTemp = pTemp->rbnext)
				{
					if(pTemp->restype == AcDb::kDxfXdReal)
					{//Real
						pTemp->resval.rreal = fValueParam;
						pObj->upgradeOpen();
						if(pObj->setXData(pXDataResBuf) != Acad::eOk)
						{
							ads_printf("\nERROR: Failed to set XData!\n");
						}
						else
						{
							pObj->close();
							return TRUE;
						}
						break;
					}
				}
				ads_relrb(pXDataResBuf);
			}
		}
		pObj->close();
	}
	return FALSE;
}
BOOL CXDataPflAndCS::InquireXData(ads_name Entity, const CString& strData, double& fValue)
{//
	AcDbObject* pObj;
	AcDbObjectId objId;
	Acad::ErrorStatus ErrCode;
	AcGePoint3d ptTmp;

	ErrCode = acdbGetObjectId(objId, Entity);
	if(ErrCode != Acad::eOk)
	{
		ads_printf("\nERROR: AutoCAD System error !\n");
		return FALSE;
	}
	ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
	if(ErrCode != Acad::eOk)
	{
		ads_printf("\nERROR: AutoCAD System error !\n");
		return FALSE;
	}
	//if(pObj->isA() != AcDbText::desc())
	//{
	//	ads_printf("\nERROR: Entity selected is not a valid Text\n");
	//	pObj->close();
	//	return;
	//}
	
	{//Inquire XData
		resbuf* pXDataResBuf;
		resbuf *pTemp;
		
		pXDataResBuf = pObj->xData(CXDataPflAndCS::m_pszAppName);
		if(pXDataResBuf != 0L)
		{
			int iCountValFound;

			for (pTemp = pXDataResBuf, iCountValFound = 0; pTemp != NULL; pTemp = pTemp->rbnext)
			{
				if(iCountValFound == 2)
					break;
				
				if(pTemp->restype == AcDb::kDxfXdAsciiString)
				{//String.....
					if(0 == strData.Compare(pTemp->resval.rstring))
					{
						iCountValFound++;
					}
					else
					{
						iCountValFound = -100;//To be treated as error
						break;
					}
				}
				if(pTemp->restype == AcDb::kDxfXdReal)
				{//Real
					fValue = pTemp->resval.rreal;
					iCountValFound++;
				}
			}
			ads_relrb(pXDataResBuf);
			pObj->close();
			return (iCountValFound >= 2);
		}
	}
	pObj->close();
	return FALSE;
}
BOOL CXDataPflAndCS::ModifyText(ads_name Entity, const char* pszNewTxt)
{//
	AcDbObject* pObj;
	AcDbObjectId objId;
	Acad::ErrorStatus ErrCode;
	AcGePoint3d ptTmp;

	ErrCode = acdbGetObjectId(objId, Entity);
	if(ErrCode != Acad::eOk)
	{
		ads_printf("\nERROR: AutoCAD System error !\n");
		return FALSE;
	}
	ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
	if(ErrCode != Acad::eOk)
	{
		ads_printf("\nERROR: AutoCAD System error !\n");
		return FALSE;
	}
	if(pObj->isA() != AcDbText::desc())
	{
		ads_printf("\nERROR: Entity selected is not a valid Text\n");
		pObj->close();
		return FALSE;
	}
    pObj->upgradeOpen();
	((AcDbText*)pObj)->setTextString(pszNewTxt);
	pObj->close();

	return TRUE;
}
void ModifyPflAndCSecnAnn()
{
	double fVal;
	CString strTag;
	AnnotationType objAnnType;
	ads_name SelSet;

	if(!GetModAnnotationParam(fVal, objAnnType))
		return;
	
	{//Get all the imported points....
		struct  resbuf  Rb01;
		const char szEntName[] = "TEXT";
		char* szprompts[] = {"Select Annotations (Text Entities) to modify :", "Remove Annotations (Text Entities) from current selection :"};
		// Entity type
		Rb01.restype = 0;
		Rb01.resval.rstring = (char*)szEntName;
		Rb01.rbnext = 0L;
		
		if(ads_ssget(":$", szprompts, 0L, &Rb01, SelSet) != RTNORM)
		{
			ads_printf("\nERROR: No entity selected\n");
			return;
		}
	}
	switch(objAnnType)
	{
	case Elevation:
		strTag = CXDataPflAndCS::m_pszElevTag;
		break;
	case Northing:
		strTag = CXDataPflAndCS::m_pszNorthTag;
		break;
	case Easting:
		strTag = CXDataPflAndCS::m_pszEastTag;
		break;
	case Chainage:
		strTag = CXDataPflAndCS::m_pszChgTag;
		break;
	default:
		ads_ssfree(SelSet);//Free Selset
		ads_printf("\nERROR: Invalid Annotation type!!\n");
		return;
	}
	{//
		ads_name Entity;
		long i, iNoOfEnt, iCount;

		if(ads_sslength(SelSet, &iNoOfEnt) != RTNORM)
		{
			ads_ssfree(SelSet);//Free Selset
			ads_printf("\nFATAL ERROR: Can't continue!\n");
			return;
		}
		for(i = 0, iCount = 0; i < iNoOfEnt; i++)
		{
			double fValueOrg;
			
			if(ads_ssname(SelSet, i, Entity) != RTNORM)
			{
				ads_printf("\nERROR: AutoCAD System error !\n");
				ads_ssfree(SelSet);//Free Selset
				return;
			}
			if(CXDataPflAndCS::InquireXData(Entity, strTag, fValueOrg))
			{
				if(CXDataPflAndCS::ModifyXData(Entity, strTag, (fValueOrg + fVal)))
				{
					char szNewTxt[255];
					
					ads_rtos((fValueOrg + fVal), 2, 3, szNewTxt);
					if(!CXDataPflAndCS::ModifyText(Entity, szNewTxt))
					{
						ads_printf("\n\nERROR: Failed to update text!!\n");
					}
					iCount++;
					ads_printf("\rTotal %d out of %d # of annotations modified successfully", iCount, iNoOfEnt);
				}
			}
		}
		ads_printf("\n\nTotal %d out of %d # of annotations modified successfully", iCount, iNoOfEnt);
	}
	ads_ssfree(SelSet);//Free Selset
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Tester Func....
void PflAndCSecnXDQuery()
{
	char szTag[133];
	ads_name Entity;
	AcGePoint3d pt3D;
	CString strData;
	double fValue;

	if(RTNORM != ads_getstring(0, "\nEnter Tag :", szTag))
		return;
	if(RTNORM != ads_entsel("\nSelect Entity :", Entity, asDblArray(pt3D)))
	{
		ads_printf("\nERROR: No entity selected\n");
		return;
	}

	strData = szTag;
	if(!CXDataPflAndCS::InquireXData(Entity, strData, fValue))
	{
		ads_printf("\nERROR: No XData found!");
		return;
	}
	ads_printf("\nSUCCESS: XData found, Tag =\"%s\" Value=\"%f\"", strData, fValue);


	//Set Value provided by user 
	if(RTNORM != ads_getreal ("\nEnter Value to Set :", &fValue))
		return;

	if(!CXDataPflAndCS::ModifyXData(Entity, strData, fValue))
		return;

	//Inquire again & Show.......
	if(!CXDataPflAndCS::InquireXData(Entity, strData, fValue))
	{
		ads_printf("\nERROR: No XData found!");
		return;
	}
	ads_printf("\nSUCCESS: Modified XData ->> Tag =\"%s\" Value=\"%f\"", strData, fValue);
}
