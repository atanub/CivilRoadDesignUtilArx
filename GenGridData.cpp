// GenGridData.cpp
/////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <Adscodes.h>
#include <DbEnts.h>
#include <Geassign.h>
#include <math.h>
#include "DrawUtil.h"
#include "Util.h"
#include "GenGridData.h"
#include "GenGridDataDlg.h"
#include "TINData.h"
#include "DwgParamObj.h"


void CGenGridPoints::GenGridPoints()
{
	DWGPARAMS DwgParams;
	
	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		AfxMessageBox("ERROR: Contour drawing parameters no set", MB_ICONSTOP);
		return;
	}
	DIMZIN(0);//Don't suppress leading/trailing zeros
	GenGridData(DwgParams);
}


BOOL CGenGridPoints::GenGridData(const DWGPARAMS&)
{
	AcGePoint3dArray ArrayContourPts, BoundaryPts;
	CTrianglePtrArray ArrayTrgs;
	double fWid, fHt, fROffOrdX, fROffOrdY;
	BOOL bDrawGrid, bUsrBrk;
	CString strPtLyr = "POINTS_ON_GRID";
	CString strLineLyr = "GRID_LINES";
	
	fWid = fHt = 10.0;
	fROffOrdX = fROffOrdY = 0.0;
	bDrawGrid = TRUE;

	if(!CTINData::GetTINData(ArrayContourPts, &ArrayTrgs))
	{//
		CString strMsg;
		strMsg.Format("ERROR: Triangulation not done yet\nPlease Run Triangulation & then try to generate grid points");
		AfxMessageBox(strMsg, MB_ICONSTOP);
		return FALSE;
	}	////
	if(!GetGenGridData(fWid, fHt, fROffOrdX, fROffOrdY, bDrawGrid, strPtLyr, strLineLyr))
	{
		CTINData::DeallocateTrgArray(ArrayTrgs);//Deallocate Triangle data...
		return FALSE;
	}
	if(!GetSelClosedPLineVertexes(BoundaryPts, &bUsrBrk))
	{
		if(bUsrBrk)
		{
			ads_printf("\nUser break\n");
			CTINData::DeallocateTrgArray(ArrayTrgs);//Deallocate Triangle data...
			return FALSE;
		}
		ads_printf("\nNo boundary selected! Generating points considering all contour points\n");
	}

	{//Generate Grid..
		double fMinX, fMinY, fMaxX, fMaxY, fCurX, fCurY;
		long iCount;
		
		TINEntity::GetExtents(ArrayContourPts, fMinX, fMinY, fMaxX, fMaxY, fROffOrdX, fROffOrdY);

		if(bDrawGrid)
		{//Draw Grid lines....
			if(BoundaryPts.length() > 0)
			{
				double fMinXTmp, fMinYTmp, fMaxXTmp, fMaxYTmp;
				
				CalculateRectXtent(BoundaryPts, fROffOrdX, fROffOrdY, fMinXTmp, fMinYTmp, fMaxXTmp, fMaxYTmp);
				if(!DrawGrid(fMinXTmp, fMinYTmp, fMaxXTmp, fMaxYTmp, fWid, fHt, strLineLyr))
				{
					//User break encountered 
					CTINData::DeallocateTrgArray(ArrayTrgs);//Deallocate Triangle data...
					return FALSE;
				}
			}
			else
			{
				if(!DrawGrid(fMinX, fMinY, fMaxX, fMaxY, fWid, fHt, strLineLyr))
				{
					//User break encountered 
					CTINData::DeallocateTrgArray(ArrayTrgs);//Deallocate Triangle data...
					return FALSE;
				}
			}
		}
		//Draw Points....
		LAYER(strPtLyr);
		for(fCurX = fMinX, iCount = 0; fCurX <= fMaxX; fCurX += fWid)
		{
			for(fCurY = fMinY; fCurY <= fMaxY; fCurY += fHt)
			{
				AcGePoint3d PtOnGrid;
				double fZValue;

				if(ads_usrbrk() == 1)
				{
					ads_printf("\nUser break encountered !\n");
					CTINData::DeallocateTrgArray(ArrayTrgs);//Deallocate Triangle data...
					return FALSE;
				}
				PtOnGrid.x = fCurX; PtOnGrid.y = fCurY; PtOnGrid.z = 0.0;
				//Check whether within user defined boundary
				if(BoundaryPts.length() > 0)
				{
					if(!IsPointWithinBoundary(PtOnGrid, BoundaryPts))
					{
						continue;
					}
				}
				if(TINEntity::IsPointWithinBoundary(ArrayContourPts, ArrayTrgs, PtOnGrid, &fZValue))
				{
					PtOnGrid.z = fZValue;
					iCount++;
					DRAWPOINT(PtOnGrid, strPtLyr);
					ads_printf("\r%d # of points generated", iCount);

				}
			}
		}
		ads_printf("\nTotal %d points generated", iCount);
	}
	CTINData::DeallocateTrgArray(ArrayTrgs);//Deallocate Triangle data...
	return TRUE;
}

BOOL CGenGridPoints::GetSelClosedPLineVertexes(AcGePoint3dArray& Array, BOOL* pbUsrBrk /* = 0L*/)
{
	const char* pszPmt = "\nSelect bounded (closed) polyline for boundary definition :";
	ads_name entName;
	ads_point ptRes;
	AcDbObjectId objId;
	AcDbObject* pObj;
	Acad::ErrorStatus ErrCode;
	int iAdsRetCode;


	if(Array.length() > 0)
	{
		ads_printf("\nERROR: Array parameter is not empty\n");
		return FALSE;
	}
	iAdsRetCode = ads_entsel(pszPmt, entName, ptRes);
	if(pbUsrBrk != 0L)
		*pbUsrBrk = (RTCAN == iAdsRetCode);

	if(RTNORM != iAdsRetCode)
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
	
	pObj->close();
	return TRUE;
}

BOOL CGenGridPoints::DrawGrid(const double& fMinX, const double& fMinY, const double& fMaxX, const double& fMaxY, const double& fWid, const double& fHt, const CString& strLineLyr)
{
	double fCurY, fCurX;
	AcGePoint3d PointStart, PointEnd;

	LAYER(strLineLyr);
	
	ads_printf("\nPlease wait... Drawing vertical grid lines...");
	for(fCurY = 0.0; fCurY <= fMaxY - fMinY; fCurY += fHt)
	{
		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break encountered\n");
			return FALSE;
		}
		
		PointStart.x = fMinX;
		PointStart.y = fMinY + fCurY;
		PointStart.z = 0.0;

		PointEnd.x = fMaxX;
		PointEnd.y = fMinY + fCurY;
		PointEnd.z = 0.0;

		LINE(PointStart, PointEnd, strLineLyr);
	}
	if(fCurY != (fMaxY - fMinY))
	{
		PointStart.x = fMinX;
		PointStart.y = fMaxY;
		PointStart.z = 0.0;

		PointEnd.x = fMaxX;
		PointEnd.y = fMaxY;
		PointEnd.z = 0.0;

		LINE(PointStart, PointEnd, strLineLyr);
	}

	ads_printf("\nPlease wait... Drawing horizontal grid lines...");
	for(fCurX = 0.0; fCurX <= fMaxX - fMinX; fCurX += fWid)
	{
		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break encountered\n");
			return FALSE;
		}
		PointStart.x = fMinX + fCurX;
		PointStart.y = fMinY;
		PointStart.z = 0.0;

		PointEnd.x = fMinX + fCurX;
		PointEnd.y = fMaxY;
		PointEnd.z = 0.0;

		LINE(PointStart, PointEnd, strLineLyr);
	}
	if(fCurX != (fMaxX - fMinX))
	{
		PointStart.x = fMaxX;
		PointStart.y = fMinY;
		PointStart.z = 0.0;

		PointEnd.x = fMaxX;
		PointEnd.y = fMaxY;
		PointEnd.z = 0.0;

		LINE(PointStart, PointEnd, strLineLyr);
	}
	return TRUE;
}
void CGenGridPoints::CalculateRectXtent(const AcGePoint3dArray& arrayBoundaryPts, const double& fROffOrdX, const double& fROffOrdY, double& fMinX, double& fMinY, double& fMaxX, double& fMaxY)
{
	int i;

	if(arrayBoundaryPts.length() <= 0)
		return;

	fMinX = fMaxX = arrayBoundaryPts[0].x;
	fMinY = fMaxY = arrayBoundaryPts[0].y;
	for(i = 1; i < arrayBoundaryPts.length(); i++)
	{
		if(arrayBoundaryPts[i].x < fMinX)
			fMinX = arrayBoundaryPts[i].x;
		if(arrayBoundaryPts[i].x > fMaxX)
			fMaxX = arrayBoundaryPts[i].x;
		
		if(arrayBoundaryPts[i].y < fMinY)
			fMinY = arrayBoundaryPts[i].y;
		if(arrayBoundaryPts[i].y > fMaxY)
			fMaxY = arrayBoundaryPts[i].y;
	}
	if(fROffOrdX != 0.0)
	{
		double fRem, fInteger;

		fRem = fmod(fMinX, fROffOrdX);
		fInteger = (fMinX - fRem) / fROffOrdX;
		fMinX = (fInteger + 1) * fROffOrdX;
	}
	if(fROffOrdY != 0.0)
	{
		double fRem, fInteger;

		fRem = fmod(fMinY, fROffOrdY);
		fInteger = (fMinY - fRem) / fROffOrdY;
		fMinY = (fInteger + 1) * fROffOrdY;
	}
}

