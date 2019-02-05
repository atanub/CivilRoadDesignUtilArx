// XSection.cpp : implementation file
//
#include "StdAfx.h"
#include <Adscodes.h>
#include <DbEnts.h>
#include <GeAssign.h>
#include <Math.h>

#include "XSection.h"
#include "XSecDataDlg.h"
#include "DwgParamObj.h"
#include "DrawUtil.h"
#include "Util.h"
#include "TINEntity.h"
#include "TINData.h"
#include "XSectionDwg.h"
#include "HAlignment.h"

void CXSection::DrawXSection()
{
#ifndef _DEBUG
	AfxMessageBox("This facility is yet to be tested", MB_ICONINFORMATION);
#endif //_DEBUG

	const XSECDATAEX* pRec;
	DWGPARAMS DwgParams;
	AcGePoint3dArray ContourPtsArray, DatumRangeArray;
	CTrianglePtrArray ArrayTrgs;
	AcGePoint3d PtOrg;
	AcDbPolyline* pObj;
	
	pRec = 0L;
	pObj = 0L;
	DIMZIN(0);//Don't suppress leading/trailing zeros
	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		AfxMessageBox("ERROR: Contour drawing parameters not set yet", MB_ICONSTOP);
		return;
	}

	if(!CTINData::GetTINData(ContourPtsArray, &ArrayTrgs))
	{//
		CString strMsg;
		strMsg.Format("ERROR: Triangulation not done yet\nPlease Run Triangulation & then try to draw cross sections");
		AfxMessageBox(strMsg, MB_ICONSTOP);
		goto CLEANUP;
	}
	{//Dialog interface...
		CHAlignment* pEnt;
		
		pEnt = CHAlignmentJig::GetHAlignmentEntity("\nSelect Horizontal Alignment Entity :");
		if(pEnt == 0L)
			goto CLEANUP;

		XSECDATAEX& RecXSecData = pEnt->GetXSecSetting();
		pRec = &RecXSecData;
		pEnt->close();//Close Enity !! MUST !!
		{//
			pObj = pEnt->ConvertToPolyLine();
			if(0L == pObj)
			{
				assert(false);
				goto CLEANUP;
			}
		}
		if(!GetXSectionData(RecXSecData, DatumRangeArray))
			goto CLEANUP;
	}
	{//
		CPLineDataExtractorEx objDataExtractor(pObj);
		
		if(!objDataExtractor.DivideIntoSegments(pRec->data.dCSInterval, pRec->dCSIntervalMinor, pRec->data.dStartChg))
			goto CLEANUP;
	
		CThePointsOnAlignment& objPtsOnAlignment = objDataExtractor.GetPointData();
		
		#ifdef _DEBUG
			objPtsOnAlignment.DrawPoints(pRec->data.dNumAnnSize, pRec->data.dOrdInterval, "ATANU_DEBUG_1");
		#endif//_DEBUG
		
		//Draw ....
		if(RTNORM != ads_getpoint(0L, "\nEnter upper left corner of drawing sheet :", asDblArray(PtOrg)))
			goto CLEANUP;

		if(!GetAllXSecPoints(ContourPtsArray, ArrayTrgs, objPtsOnAlignment, *pRec))
			goto CLEANUP;
		

		if(!CXSectionDwg::DrawXSectionCore(PtOrg, objPtsOnAlignment, DatumRangeArray, *pRec))
			goto CLEANUP;
		
		//
		DrawPointsInPlanView(objPtsOnAlignment, *pRec, TRUE);
	}
	goto CLEANUP;
	//////////////////////////////////////////////////////////////////////
	CLEANUP :
	{//Deallocation
		CTINData::DeallocateTrgArray(ArrayTrgs);//Deallocate Triangle data...
		delete pObj;
	}
}
void CXSection::DrawPointsInPlanView(const CThePointsOnAlignment& objPtsOnAlignment, const XSECDATAEX& Rec, const BOOL bDeallocate/* = FALSE*/)
{//Plot points in plan view
	int i, j;

	for(i = 0; i < objPtsOnAlignment.GetCount(); i++)
	{
		AcGePoint3dArray* pArrayTmp;
		
		pArrayTmp = (AcGePoint3dArray*)objPtsOnAlignment.GetAt(i)->GetItemDataPtr();
		if(0L == pArrayTmp)
			continue;
		
		for(j = 0; j < pArrayTmp->length(); j++)
		{
			DRAWPOINT((*pArrayTmp)[j], Rec.data.szLyrAnnotation);
		}
		if(bDeallocate)
		{
			delete pArrayTmp;
		}
	}
}
BOOL CXSection::GetAllXSecPoints(const AcGePoint3dArray& ContourPtsArray, const CTrianglePtrArray& ArrayTrgs, CThePointsOnAlignment& PflPathArray, const XSECDATAEX& RecParam)
{
	int i, iCount;

	if(PflPathArray.GetCount() < 1)
		return FALSE;

	ads_printf("\nPlease wait...calculating points...");
	for(i = 0, iCount = 0; i < PflPathArray.GetCount(); i++)
	{
		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break encountered\n");
			return FALSE;
		}
		{//Check whether this section to be drawn or not depending upon UI settings
			if(!RecParam.bDrawSectionsAtAllIntersectionPoints)
			{
				const BOOL bIsPOI = (((i + 1) != PflPathArray.GetCount()) && (PflPathArray.GetAt(i)->GetType() & CPointOnAlignment::PointType::AtIntersection));
				if(bIsPOI)
				{
					continue;
				}
			}
		}
		AcGePoint3dArray* pArrayAcrossAlignment;
		const AcGePoint3d ptPOI = PflPathArray.GetAt(i)->GetPoint();
		const double fAngleOfAnnotationLine = PflPathArray.GetAt(i)->GetAngleOfAnnotationLine();

		if(!GetXSecPointsInSeg(ContourPtsArray, ArrayTrgs, &ptPOI, fAngleOfAnnotationLine, RecParam, pArrayAcrossAlignment))
		{
			ads_printf("\nERROR: Failed to generate C/S points at vertex #%d\n", i);
			return FALSE;
		}
		PflPathArray.GetAt(i)->SetItemDataPtr(pArrayAcrossAlignment);

		ads_printf("\rCollected %d no of points at vertex #%d", pArrayAcrossAlignment->length(), (i + 1));
		iCount++;
	}
	ads_printf("\rTotal %d# of Cross Sections generated.\n", iCount);
	return TRUE;
}
BOOL CXSection::GetXSecPointsInSeg(const AcGePoint3dArray& ContourPtsArray, const CTrianglePtrArray& ArrayTrgs, const AcGePoint3d* pPtMid, const double fAngleOfAnnotationLine, const XSECDATAEX& RecParam, AcGePoint3dArray*& pArrayResult)
{
	pArrayResult = 0L;
	{//Generate X Sectional points on each generated stations....
		AcGePoint3dArray PtsAlongXSecArray;

		if(!GetXSecPoints(ContourPtsArray, ArrayTrgs, *pPtMid, fAngleOfAnnotationLine, RecParam, PtsAlongXSecArray))
		{
			#ifdef _DEBUG
				const char* pszDbgLyr = "XSEC_POINT_NOT_WITHIN_TRG_DEBUG";
				
				LAYER(pszDbgLyr, 1);
				DRAWPOINT(*pPtMid, pszDbgLyr);
			#endif//_DEBUG
			//return FALSE;
		}
		pArrayResult = new AcGePoint3dArray(PtsAlongXSecArray);
	}
	return TRUE;
}
BOOL CXSection::GetXSecPoints(const AcGePoint3dArray& ContourPtsArray, const CTrianglePtrArray& ArrayTrgs, const AcGePoint3d& PtMidOfXec, const double& fAngle, const XSECDATAEX& RecParam, AcGePoint3dArray& PtsAlongXSecArray)
{
	AcGePoint3dArray ptArrayLeft, ptArrayRight;
	double fCumDist, fAngleTmp;
	AcGePoint3d ptTmp;
	AcGePoint3d PtMidOfXecLocal(PtMidOfXec);

	if(PtsAlongXSecArray.length() > 0)
		return FALSE;
	
	if(!TINEntity::IsPointWithinBoundary(ContourPtsArray, ArrayTrgs, PtMidOfXecLocal, &(PtMidOfXecLocal.z)))
	{
		return FALSE;
	}
	//Left side offsets...
	fAngleTmp = fAngle;
	for(fCumDist = RecParam.data.dOrdInterval; fCumDist <= RecParam.data.dMaxCSOffset; fCumDist += RecParam.data.dOrdInterval)
	{
		ads_polar(asDblArray(PtMidOfXec), fAngleTmp, fCumDist, asDblArray(ptTmp));
		if(!TINEntity::IsPointWithinBoundary(ContourPtsArray, ArrayTrgs, ptTmp, &(ptTmp.z)))
		{
			ptTmp.z = 0.0;
			continue;//Dont' draw points which are out of TIN
		}
		ptArrayLeft.append(ptTmp);
	}
	//Right side offsets...
	fAngleTmp = fAngle + PI;
	for(fCumDist = RecParam.data.dOrdInterval; fCumDist <= RecParam.data.dMaxCSOffset; fCumDist += RecParam.data.dOrdInterval)
	{
		ads_polar(asDblArray(PtMidOfXec), fAngleTmp, fCumDist, asDblArray(ptTmp));
		if(!TINEntity::IsPointWithinBoundary(ContourPtsArray, ArrayTrgs, ptTmp, &(ptTmp.z)))
		{
			ptTmp.z = 0.0;
			continue;//Dont' draw points which are out of TIN
		}
		ptArrayRight.append(ptTmp);
	}
	ptArrayLeft.reverse();
	PtsAlongXSecArray.append(ptArrayLeft);
	PtsAlongXSecArray.append(PtMidOfXecLocal);
	PtsAlongXSecArray.append(ptArrayRight);
	#ifdef _DEBUG
		const char* pszDbgLyrLeft = "XSEC_RITE_SIDE_DEBUG";
		const char* pszDbgLyrRite = "XSEC_LEFT_SIDE_DEBUG";
		
		LAYER(pszDbgLyrLeft, 2);
		LAYER(pszDbgLyrRite, 5);
		DRAWPOINTS(ptArrayLeft,		pszDbgLyrLeft);
		DRAWPOINTS(ptArrayRight,	pszDbgLyrRite);
	#endif//_DEBUG

	return TRUE;
}
double CXSection::CalculateLeftNRiteAngles(const AcGePoint3d& ptFirst, const AcGePoint3d& ptMid, const AcGePoint3d& ptThird, double& fAngleLeft, double& fAngleRite)
{
	//determine Direction....CW/CCW
	AcGeCircArc2d segArc(AcGePoint2d(ptFirst.x, ptFirst.y), AcGePoint2d(ptMid.x, ptMid.y), AcGePoint2d(ptThird.x, ptThird.y));
	double fAngle;

	fAngleLeft = fAngleRite = 0.0;
	if(Adesk::kTrue == segArc.isClockWise())
	{//CW
		fAngle = (Angle2D(ptMid, ptThird) - Angle2D(ptMid, ptFirst)) / 2.0;
		fAngle += Angle2D(ptMid, ptFirst);
		fAngleRite = fAngle;
		fAngleLeft = fAngle + PI;
		return fAngleRite + (PI / 2.0);//angle along Alignment...
	}
	//CCW
	fAngle = (Angle2D(ptMid, ptFirst) - Angle2D(ptMid, ptThird)) / 2.0;
	fAngle += Angle2D(ptMid, ptThird);
	fAngleLeft = fAngle;
	fAngleRite = fAngle + PI;
	return fAngleRite + (PI / 2.0);//angle along Alignment...
}

void CXSection::DeAllocateXSecPtsArray(AcDbVoidPtrArray& Array)
{
	long i;
	
	for(i = 0; i < Array.length(); i++)
	{
		AcGePoint3dArray* pArray;
		
		pArray = (AcGePoint3dArray*)Array.at(i);
		delete pArray;
	}
	for(; Array.length() > 0;)
	{
		Array.removeFirst();
	}
}
