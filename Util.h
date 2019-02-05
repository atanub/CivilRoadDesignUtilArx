//Util.h
//////////////////////////////////////////////////////////////////////////////
#ifndef _UTIL_H
#define _UTIL_H

#include <acgi.h>
#include <AdsCodes.h>
#include <GEAssign.h>
#include <Math.h>
#include "Structures.h"
#include <Dbents.h>

typedef CArray<double, double> CDoubleArray;

inline int XtractStrFrEntData(struct resbuf* pEntData, const int& iDXFCode, char* pszText)
{
	struct resbuf* pTemp;
	for(pTemp = pEntData; pTemp != 0L; pTemp = pTemp->rbnext)
	{
		if(pTemp->restype == iDXFCode)
		{
			strcpy(pszText, pTemp->resval.rstring);
			return 1;
		}
	}
	return 0;
}
inline int XtractPoint3DFrEntData(struct resbuf* pEntData, const int& iDXFCode, AcGePoint3d& Point3D)
{
	struct resbuf* pTemp;
	for(pTemp = pEntData; pTemp != 0L; pTemp = pTemp->rbnext)
	{
		if(pTemp->restype == iDXFCode)
		{
			Point3D.set(pTemp->resval.rpoint[X], pTemp->resval.rpoint[Y], pTemp->resval.rpoint[Z]);
			return 1;
		}
	}
	return 0;
}
inline double RTOD(const double& dVal)
{
	return (180.0 * dVal / PI);
}
inline double DTOR(const double& dVal)
{
	return (PI * dVal / 180.0);
}
inline double Angle2D(const AcGePoint3d& pPt01, const AcGePoint3d& pPt02)
{
	ads_point Point01, Point02;

	Point01[X] = pPt01.x;	Point01[Y] = pPt01.y;	Point01[Z] = 0.0;
	Point02[X] = pPt02.x;	Point02[Y] = pPt02.y;	Point02[Z] = 0.0;

	return ads_angle (Point01, Point02);
}
inline double Dist2D(const AcGePoint3d& pPt01, const AcGePoint3d& pPt02)
{
	return AcGePoint3d(pPt01.x, pPt01.y, 0.0).distanceTo(AcGePoint3d(pPt02.x, pPt02.y, 0.0));
}
inline BOOL GetIntersPt2D(const AcGePoint3d& Pt01AParam, const AcGePoint3d& Pt02AParam, const AcGePoint3d& Pt01BParam, const AcGePoint3d& Pt02BParam, AcGePoint3d& Result, const BOOL& bConsiderInfiniteLen = FALSE)
{
	AcGePoint3d Pt01A, Pt02A, Pt01B, Pt02B;
	ads_point ptResult;

	Pt01A = Pt01AParam;	Pt01A.z = 0.0;
	Pt02A = Pt02AParam;	Pt02A.z = 0.0;

	Pt01B = Pt01BParam;	Pt01B.z = 0.0;
	Pt02B = Pt02BParam;	Pt02B.z = 0.0;

	if(RTNORM == ads_inters(asDblArray(Pt01A), asDblArray(Pt02A), asDblArray(Pt01B), asDblArray(Pt02B), (bConsiderInfiniteLen) ? 0 : 1, ptResult))
	{
		Result = asPnt3d(ptResult);
		Result.z = Pt01AParam.z;
		return TRUE;
	}
	return FALSE;
}
inline int Interpolate(const AcGePoint3d* pPt1, const AcGePoint3d* pPt2, const double& dElev, AcGePoint3d& PtResult)
{
	double dDist;
	ads_point PointBase, PointResult;


	dDist = (dElev - pPt1->z) * Dist2D(*pPt1, *pPt2) / (pPt2->z - pPt1->z);
	
	PointBase[X] = pPt1->x;	PointBase[Y] = pPt1->y;	PointBase[Z] = 0.0;
	ads_polar(PointBase, Angle2D(*pPt1, *pPt2), dDist, PointResult);
	
	PtResult.x = PointResult[X];
	PtResult.y = PointResult[Y];
	PtResult.z = dElev;
	return 1;//Success:
}

inline int Interpolate(const AcGePoint3d* pPt1, const AcGePoint3d* pPt2, const AcGePoint3d* pPtResult, double& fElev)
{
	double dElevDiff, dLineLen;

	dElevDiff = pPt2->z - pPt1->z;
	dLineLen = Dist2D(*pPt1, *pPt2);

#ifdef _DEBUG
	double dX;
	
	dX = Dist2D(*pPt1, *pPtResult) + Dist2D(*pPtResult, *pPt2);
	if(fabs(dLineLen - dX) >= 1.0e-9)
	{
		ads_printf("\nERROR: Interpolation faild ...point parameters are incorrect!!!\n");
		return 0;//Failure:
	}
#endif //_DEBUG

	fElev = pPt1->z + (dElevDiff * Dist2D(*pPt1, *pPtResult)) / dLineLen;
	return 1;//Success:
}


//*****************************************************************************
//   Determines which side of a line the point (x,y) lies.
//   The line goes from (x1,y1) to (x2,y2)
//
//   Return codes are 	-1 for points to the left
//		      	 0 for points on the line
//		       	+1 for points to the right
inline int WhichSideOfLine(const double& x, const double& y, const double& x1, const double& y1, const double& x2, const double& y2)
{
    double dist;

    dist = (y - y1) * (x2 - x1) - (y2 - y1) * (x - x1);
    if (dist > 0)
       return(-1);
    else if (dist < 0)
       return(1);
    else
       return(0);

	return(0);
}

//
//   Returns TRUE if the point (xp,yp) lies inside the projection triangle
//   on the x-y plane with the vertices p1,p2,p3
//   A point is in the centre if it is on the same side of all the edges
//   or if it lies on one of the edges.
//
inline BOOL IsPointWithinTrg(const AcGePoint3d& PtParam, const AcGePoint3d& p1, const AcGePoint3d& p2, const AcGePoint3d& p3)
{
	int iSide1, iSide2, iSide3;

	iSide1 = WhichSideOfLine(PtParam.x, PtParam.y, p1.x, p1.y, p2.x, p2.y);
	iSide2 = WhichSideOfLine(PtParam.x, PtParam.y, p2.x, p2.y, p3.x, p3.y);
	iSide3 = WhichSideOfLine(PtParam.x, PtParam.y, p3.x, p3.y, p1.x, p1.y);

	if (iSide1 == 0 && iSide2 == 0)
	  return(TRUE);
	if (iSide1 == 0 && iSide3 == 0)
	  return(TRUE);
	if (iSide2 == 0 && iSide3 == 0)
	  return(TRUE);

	if (iSide1 == 0 && (iSide2 == iSide3))
	  return(TRUE);
	if (iSide2 == 0 && (iSide1 == iSide3))
	  return(TRUE);
	if (iSide3 == 0 && (iSide1 == iSide2))
	  return(TRUE);

	if ((iSide1 == iSide2) && (iSide2 == iSide3))
	  return(TRUE);
		
	return(FALSE);
}
inline BOOL IsLinesColinear(const AcGePoint3d& PtParam01A, const AcGePoint3d& PtParam01B, const AcGePoint3d& PtParam02A, const AcGePoint3d& PtParam02B)
{
	if((Angle2D(PtParam01A, PtParam01B) == Angle2D(PtParam02A, PtParam02B)) ||
	   (Angle2D(PtParam01A, PtParam01B) == Angle2D(PtParam02B, PtParam02A))
	  )
	{
		if((Angle2D(PtParam01A, PtParam02A) == Angle2D(PtParam01B, PtParam02B)) ||
		   (Angle2D(PtParam01A, PtParam02A) == Angle2D(PtParam02B, PtParam01B))
		  )
		  return TRUE;
	}
	return FALSE;
}

inline BOOL CalculateTrgArea(const AcGePoint3d& Pt01, const AcGePoint3d& Pt02, const AcGePoint3d& Pt03, double& fArea)
{
	fArea = 0.0;
	if(IsLinesColinear(Pt01, Pt02, Pt02, Pt03))
		return FALSE;
	
	double d1, d2, d3, dS;
	
	d1 = Dist2D(Pt01, Pt02);
	d2 = Dist2D(Pt02, Pt03);
	d3 = Dist2D(Pt03, Pt01);
	dS = (d1 + d2 + d3) / 2.0;
	fArea = dS * (dS - d1) * (dS - d2) * (dS - d3);
	fArea = sqrt(fArea);
	return TRUE;
}
inline BOOL CalculateTrgVolume(const AcGePoint3d& Pt01, const AcGePoint3d& Pt02, const AcGePoint3d& Pt03, double& fVolume)
{
	double fArea;
	fVolume = 0.0;

	if(!CalculateTrgArea(Pt01, Pt02, Pt03, fArea))
		return FALSE;
	fVolume = fArea * (Pt01.z + Pt02.z + Pt03.z) / 3.0;
	return TRUE;
}
inline BOOL IsPointWithinBoundary(const AcGePoint3d& PtParam, const AcGePoint3dArray& PtArray)
{
	//
	//NOTE: Vertex Array parameter is in 3D
	//but this func. consideres the array elements as 2D Points
	//
	int i, iXSecCount;
	double fMaxX;
	AcGePoint3d PtParamXtn, Result;
	BOOL bValidXSec;

	//find Max X...
	for(i = 0; i < PtArray.length(); i++)
	{
		if(i == 0)
		{
			fMaxX = PtArray[i].x;
		}
		else
		{
			if(PtArray[i].x > fMaxX)
			{
				fMaxX = PtArray[i].x;
			}
		}
	}
	if(fMaxX > 0.0)
		fMaxX *= 2.0;
	else
		fMaxX *= -2.0;

	PtParamXtn.x = fMaxX; PtParamXtn.y = PtParam.y;	PtParamXtn.z = PtParam.z;
	for(i = 1, iXSecCount = 0; i < PtArray.length(); i++)
	{
		if(GetIntersPt2D(PtParam, PtParamXtn, PtArray[i - 1], PtArray[i - 0], Result))
		{
			bValidXSec = !((Result.x == (PtArray[i - 0]).x) && (Result.y == (PtArray[i - 0]).y));
			bValidXSec = bValidXSec && !((Result.x == (PtArray[i - 1]).x) && (Result.y == (PtArray[i - 1]).y));
			if(bValidXSec)
				iXSecCount++;
		}

	}
	return ((iXSecCount % 2) != 0);
}
inline BOOL XTractWordsFromLine(const CString& strDataLine, CStringArray& strArray, const char* pszWhiteChars)
{
	if(strArray.GetSize() != 0)
	{
		ads_printf("\nERROR: Invalid array parameter passed!! Array is not empty\n");
		return FALSE;
	}
	CString strTemp;
	BOOL bFlag;
	int iIndex;
	
	strTemp = strDataLine;
	do
	{
		iIndex = strTemp.FindOneOf(pszWhiteChars);
		bFlag = (iIndex >= 0);
		if(!bFlag)
		{
			if(strlen(strTemp) > 0)
				strArray.Add(strTemp);
			break;
		}
		{//Add only valid words.........
			CString strWord;

			strWord = strTemp.Mid(0, iIndex);
			strWord.TrimLeft();
			strWord.TrimRight();

			if(strlen(strWord) > 0)
				strArray.Add(strWord);
		}
		strTemp = strTemp.Mid(++iIndex);
	}
	while(bFlag);
	return (strArray.GetSize() > 0);
}
inline BOOL XTractWordsFromLine(const CString& strDataLine, CStringArray& strArray)
{
	const char* pszWhiteChars = " \t";

	return XTractWordsFromLine(strDataLine, strArray, pszWhiteChars);
}
inline int FindNearestPoint(const AcGePoint3d& PtBase, const AcGePoint3dArray& PtArray)
{
	int i, iIndex;
	
	if(PtArray.length() <= 0)
		return -1;
	
	for(i = 1, iIndex = 0; i < PtArray.length(); i++)
	{
		if(Dist2D(PtBase, PtArray[i]) < Dist2D(PtBase, PtArray[iIndex]))
		{
			iIndex = i;
		}
	}
	return iIndex;
}
inline  void SortPointsOnLine(const AcGePoint3d& PtBase, AcGePoint3dArray& PtArray)
{
	int iIndex;
	AcGePoint3dArray PtArrayResult;

	if(PtArray.length() <= 0)
		return;
	
	for(; PtArray.length() > 0;)
	{
		iIndex = FindNearestPoint(PtBase, PtArray);
		if(iIndex >= 0)
		{
			PtArrayResult.append(PtArray[iIndex]);
			PtArray.removeAt(iIndex);
		}
	}
	if(PtArray.length() != 0)
	{
		ads_printf("\nERROR: in 'SortPointsOnLine()'\n");
		return;
	}
	PtArray.append(PtArrayResult);
}
inline BOOL IsPointOnLine(const AcGePoint3d& ptParam, const AcGePoint3d& ptStart, const AcGePoint3d& ptEnd, const double fPrecision = 1.0E-9)
{
	double fVal;
	fVal = (Dist2D(ptStart, ptParam) + Dist2D(ptParam, ptEnd)) - Dist2D(ptStart, ptEnd);
	fVal = fabs(fVal);
	return (fVal <= fPrecision);
}
inline int IsPointOnPolyLine(const AcGePoint3d& ptCenter, const AcGePoint3dArray& arrayVertices, const double fPrecision = 1.0E-9)
{
	int i;

	for(i = 1; i < arrayVertices.length(); i++)
	{
		if(IsPointOnLine(ptCenter, arrayVertices[i - 1], arrayVertices[i], fPrecision))
			return i;
	}
	return -1;
}
inline CString GetEntLayer(const AcDbEntity* pEntParam)
{
	CString strToRet;
	char *pszLyrEnt;

	pszLyrEnt = pEntParam->layer();
	strToRet = pszLyrEnt;
	delete []pszLyrEnt;
	return strToRet;
}
//Extract points from 3D simple polyline...
inline BOOL Extract3DPLineVertices(AcDb3dPolyline* pObj, AcGePoint3dArray& arrayPts)
{
	assert(0L != pObj);
	assert(arrayPts.length() <= 0);
	assert(AcDb::Poly3dType::k3dSimplePoly == pObj->polyType());

	AcDbObjectIterator* pIterator;
	
	pIterator = pObj->vertexIterator();
	for(pIterator->start(); (pIterator->done() == Adesk::kFalse); pIterator->step())
	{
		AcDb3dPolylineVertex* pVert;
		
		if(Acad::eOk != pObj->openVertex(pVert, pIterator->objectId(), AcDb::kForRead))
		{
			delete pIterator;
			ads_printf("\nERROR: Error occured while iterating vertexes!\n");
			return FALSE;
		}
		arrayPts.append(pVert->position());
		pVert->close();
	}
	delete pIterator;
	return TRUE;
}
inline double CalculateAutoDatum(const double& fMinElev, const double& fYScale, const double& fMinBufferInDWGUnits, const double& fDatumIsMultipleOf)
{
	double fDatum, fCumVal;

	fDatum = fMinElev - fMinBufferInDWGUnits / fYScale;
	if(fDatum >= fDatumIsMultipleOf)
	{
		fCumVal = fDatumIsMultipleOf;
	}
	else
	{
		const int iMF = 1 + (int)(fabs(fDatum) / fDatumIsMultipleOf);

		fCumVal = 0.0 - iMF * fDatumIsMultipleOf;
	}
	while(TRUE)
	{
		if(fCumVal >= fDatum)
		{
			if(fCumVal == fDatum)
				return fCumVal;
			return (fCumVal - fDatumIsMultipleOf);
		}
		fCumVal += fDatumIsMultipleOf;
	}
	return fCumVal;
}
inline BOOL ExtractVertices(AcDb2dPolyline* pEnt, AcGePoint3dArray& Array)
{
	if(Array.length() > 0)
	{
		ads_printf("\nERROR: Array parameter is not empty\n");
		return FALSE;
	}
	{//Extracts the vertices....
		AcDbObjectIterator* pVertexes;
		AcDbObjectId   ObjId;
		AcDb2dVertex*  pVertex;

		pVertex = 0L;
		pVertexes = 0L;

		pVertexes = pEnt->vertexIterator();
		for (; !pVertexes->done(); pVertexes->step())
		{
			ObjId = pVertexes->objectId();
			pEnt->openVertex(pVertex, ObjId, AcDb::kForRead);
			Array.append(pVertex->position());
			pVertex->close();
		}
		if((Adesk::kTrue == pEnt->isClosed()) && (Array.length() > 2))
		{
			if(Array[0] != Array[Array.length() - 1])
				Array.append(Array[0]);
		}
		delete pVertexes;
	}
	return TRUE;
}
inline BOOL ExtractVertices(AcDbPolyline* pEnt, AcGePoint3dArray& Array)
{
	if(Array.length() > 0)
	{
		ads_printf("\nERROR: Array parameter is not empty\n");
		return FALSE;
	}
	{//Extract vertices...
		unsigned int i, iLen;
		AcGePoint3d pt3D;
		
		iLen = pEnt->numVerts();
		if(Adesk::kTrue != pEnt->isOnlyLines())
		{
			ads_printf("\nERROR: Selected polyline contains arc segments\n");
			return FALSE;
		}
		if(Adesk::kTrue != pEnt->isPlanar())
		{
			ads_printf("\nERROR: Selected polyline vertices are no co-planer\n");
			return FALSE;
		}
		iLen = pEnt->numVerts();
		for(i = 0; i < iLen; i++)
		{
			if(Acad::ErrorStatus::eOk != pEnt->getPointAt(i, pt3D))
			{
				ads_printf("\nERROR: Some unknown errors occured\n");
				return FALSE;
			}
			Array.append(pt3D);
		}
		if((Adesk::kTrue == pEnt->isClosed()) && (Array.length() > 2))
		{
			if(Array[0] != Array[Array.length() - 1])
				Array.append(Array[0]);
		}
	}
	return TRUE;
}
inline BOOL ExtractVertices(const AcDbObjectId& objId, AcGePoint3dArray& Array)
{
	AcDbObject* pObj;
	Acad::ErrorStatus ErrCode;
	BOOL bFlag;

	bFlag = FALSE;
	if(Array.length() > 0)
	{
		ads_printf("\nERROR: Array parameter is not empty... from ExtractVertices()\n");
		return FALSE;
	}
	ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
	if(ErrCode != Acad::eOk)
	{
		return FALSE;
	}
	if(pObj->isA() == AcDbPolyline::desc())
	{
		bFlag = ExtractVertices((AcDbPolyline*)pObj, Array);
		pObj->close();
		return bFlag;
	}
	if(pObj->isA() == AcDb2dPolyline::desc())
	{
		bFlag = ExtractVertices((AcDb2dPolyline*)pObj, Array);
		pObj->close();
		return bFlag;
	}
	if(pObj->isA() == AcDbLine::desc())
	{
		Array.append(((AcDbLine*)pObj)->startPoint());
		Array.append(((AcDbLine*)pObj)->endPoint());
		if(Array[0].z != Array[1].z)
		{
			pObj->close();
			return FALSE;
		}
		pObj->close();
		return TRUE;
	}
	pObj->close();
	return bFlag;
}

#endif // _UTIL_H


