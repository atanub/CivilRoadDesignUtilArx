// Volume.cpp
/////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <Adscodes.h>
#include <DbEnts.h>
#include <Geassign.h>
#include <math.h>
#include "DrawUtil.h"
#include "Util.h"
#include "Volume.h"
#include "ResultDlg.h"
#include "TINData.h"

static BOOL GetSelClosedPLineVertexes(AcGePoint3dArray&, double* = 0L);
static BOOL GetYesOrNo(const char*, BOOL&);
//////////////////////////////////////////////////////////////////////////////////////////
void CalculateVolume()
{
	AcGePoint3dArray ArrayContourPts;
	CTrianglePtrArray ArrayTrgs;
    AcGePoint3dArray VertexArray;
    double fArea, fVolm, fFormLevel;

	if(!GetSelClosedPLineVertexes(VertexArray, &fArea))
	{
		return;
	}
	if(!CTINData::GetTINData(ArrayContourPts, &ArrayTrgs))
	{//
		CString strMsg;
		strMsg.Format("ERROR: Triangulation not done yet\nPlease Run Triangulation & then try to draw contour");
		AfxMessageBox(strMsg, MB_ICONSTOP);
		return;
	}	////
	{//Get formation level input....
		double fMinElev, fMaxElev;

		CTINData::FindElevRange(ArrayContourPts, fMinElev, fMaxElev);
		fFormLevel = 0.0;
		ads_printf("\nElevation Range is as following\nMinimum Elevation = %f, Maximum Elevation = %f", fMinElev, fMaxElev);
		if(ads_getreal("\nEnter formation level <0.00> :", &fFormLevel) == RTNORM)
		{
			ads_printf("\nCalculating volume assuming Formation Level : %f\n", fFormLevel);
		}
	}
	{//////////////////////////////////////////////////////////////////////////////////////////
		CStringArray strArrayRep;
		BOOL bYesToReport;

		if(!TINEntity::CalculateVolume(ArrayContourPts, ArrayTrgs, VertexArray, fArea, fFormLevel, fVolm, &strArrayRep))
		{
			ads_printf("\nERROR: Volume Calculation failed!!\n");
		}
		if(GetYesOrNo("\nDo you want detail calculation report <Yes/No> ?", bYesToReport))
		{
			if(bYesToReport)
				ShowResultDlg(strArrayRep);
		}
		CTINData::DeallocateTrgArray(ArrayTrgs);//Deallocate Triangle data...
	}//////////////////////////////////////////////////////////////////////////////////////////
}

static BOOL GetSelClosedPLineVertexes(AcGePoint3dArray& Array, double* pfArea /* = 0L*/)
{
	const char* pszPmt = "\nSelect bounded (closed) polyline for volume calculation :";
	ads_name entName;
	ads_point ptRes;
	AcDbObjectId objId;
	AcDbObject* pObj;
	Acad::ErrorStatus ErrCode;


	if(Array.length() > 0)
	{
		ads_printf("\nERROR: Array parameter is not empty\n");
		return FALSE;
	}
	if(RTNORM != ads_entsel(pszPmt, entName, ptRes))
	{
		ads_printf("\nERROR: No entity selected\n");
		return FALSE;
	}

	ErrCode = acdbGetObjectId(objId, entName);
	if(ErrCode != Acad::eOk)
	{
		return FALSE;
	}
	ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
	if(ErrCode != Acad::eOk)
	{
		return FALSE;
	}
	if(pObj->isA() != AcDbPolyline::desc())
	{
		ads_printf("\nERROR: Entity selected is not a polyline\n");
		pObj->close();
		return FALSE;
	}
	//Extract vertexes...
	unsigned int i, iLen;
	AcDbPolyline* pEnt;
	AcGePoint3d pt3D;
	
	pEnt = (AcDbPolyline*)pObj;
	iLen = pEnt->numVerts();
	if(Adesk::kTrue != pEnt->isClosed())
	{
		ads_printf("\nERROR: Selected polyline is not closed\n");
		pObj->close();
		return FALSE;
	}
	if(Adesk::kTrue != pEnt->isOnlyLines())
	{
		ads_printf("\nERROR: Selected polyline contains arc segments\n");
		pObj->close();
		return FALSE;
	}
	if(Adesk::kTrue != pEnt->isPlanar())
	{
		ads_printf("\nERROR: Selected polyline vertexes are not in a single plane\n");
		pObj->close();
		return FALSE;
	}
	iLen = pEnt->numVerts();
	for(i = 0; i < iLen; i++)
	{
		if(Acad::ErrorStatus::eOk != pEnt->getPointAt(i, pt3D))
		{
			ads_printf("\nERROR: Some unknown errors occured\n");
			pObj->close();
			return FALSE;
		}
		Array.append(pt3D);
	}
	//Since it's a closed PLine
	pEnt->getPointAt(0, pt3D);
	Array.append(pt3D);
	
	if(pfArea != 0L)
	{
		if(pEnt->getArea(*pfArea) != Acad::eOk)
		{
 			ads_printf("\nERROR: Selected polyline's area calculation failed\n");
		}
	}
	pObj->close();
	return TRUE;
}
static BOOL GetYesOrNo(const char* pszPrompt, BOOL& bYes)
{
    char szOption[256];
	int iRet;
    
	bYes = TRUE;
    
    ads_initget(0, "Yes No");
    iRet = ads_getkword(pszPrompt, szOption);
    
    switch (iRet)
	{
    case RTNORM:
        bYes = (strcmp(szOption, "Yes") == 0);
		return TRUE;
    case RTNONE:
        bYes = TRUE;
		return TRUE;
    case RTCAN:
    default:
		return FALSE;
    }
    return FALSE;
}
