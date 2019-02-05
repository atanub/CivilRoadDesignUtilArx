// XSectionDwg.cpp : implementation file
//
#include "stdafx.h"
#include "Util.h"
#include "DrawUtil.h"
#include "XSectionDwg.h"
#include "XData.h"

BOOL CXSectionDwg::DrawXSectionCore(const AcGePoint3d& ptULCornerOfSheet, const CThePointsOnAlignment& PIArray, const AcGePoint3dArray& DatumRangeArray, const XSECDATAEX& RecParam)
{
	BOOL bRetValue;

	bRetValue = FALSE;
	if(ads_usrbrk() == 1)
	{
		ads_printf("\nUser break encountered !\n");
		goto CLEANUP;
	}
	//Draw......
	bRetValue = DrawAllSections(ptULCornerOfSheet, PIArray, DatumRangeArray, RecParam);

	goto CLEANUP;

	//////////////////////////////////////////////////////////////////////
	CLEANUP :
	{
		//DeAllocateXSecPtsArray(XSecPtsArray);
		return bRetValue;
	}
}
void CXSectionDwg::DeAllocateXSecPtsArray(AcDbVoidPtrArray& Array)
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

BOOL CXSectionDwg::GetChainage(const AcGePoint3dArray* pArray, const CThePointsOnAlignment& PIDArray, double& fChg, AcGePoint3d* pPtMidOfXSec /*= 0L*/, BOOL* pbIsVersFrLeftToRight /*= 0L*/)
{
	long i, j;
	AcGePoint3d ptStart, ptEnd, ptOnXSec;
	
	for(i = 1, fChg = 0.0; i < PIDArray.GetCount(); i++)
	{
		ptStart = PIDArray.GetAt(i - 1)->GetPoint();
		ptEnd	= PIDArray.GetAt(i - 0)->GetPoint();
		for(j = 0; j < pArray->length(); j++)
		{
			ptOnXSec = pArray->at(j);
			if(IsPointOnLine(ptOnXSec, ptStart, ptEnd))
			{//
				fChg += Dist2D(ptStart, ptOnXSec);
				if(pPtMidOfXSec != 0L)
				{
					*pPtMidOfXSec = ptOnXSec;
				}
				if(pbIsVersFrLeftToRight != 0L)
				{
					double fAnglePI, fAngleTmp;
					
					fAnglePI = Angle2D(ptStart, ptEnd);
					fAngleTmp = Angle2D(ptOnXSec, pArray->at(0));
					*pbIsVersFrLeftToRight = (fAngleTmp > fAnglePI) && (fAngleTmp < fAnglePI + PI);
				}
				return TRUE;
			}
		}
		fChg += Dist2D(ptStart, ptEnd);
	}
	return FALSE;
}
//Drawing Routines.....
void CXSectionDwg::DrawOrdinates(const AcGePoint3d& ptGrphOrg, const AcGePoint3dArray& XSecPts, const AcGePoint3d& ptMidOfXSec, const XSECDATAEX& RecParam, const double& fMinOff, const double& fMaxOff, const double& fDatum)
{
	double fXAxisLen, fOrdDist, fOffset;
	AcGePoint3dArray ProfTopPtsArray;
	AcGePoint3d PtStart, PtEnd;
	int j, i;
	ads_name entLast;
	char szAnnTxt[_MAX_PATH];
	const int iNumOfRows = (RecParam.bDrawWithNorthingEastingData) ? 4:2;

	fXAxisLen = (fabs(fMinOff) + fMaxOff);

	for(i = 0; i < XSecPts.length(); i++)
	{
		int iCurRowNo;

		iCurRowNo = 0;
		if(!CalculateOffset(XSecPts[i], XSecPts, ptMidOfXSec, fOffset))
		{
			return;
		}
		//Calculate distnace from Drawing Axis....
		fOrdDist = (fXAxisLen - fMaxOff + fOffset) * RecParam.data.dXScale;

		//Call Draw Routines..
		
		{//Draw Ordinate...
			PtStart = ptGrphOrg;
			PtStart.x += fOrdDist;
			PtEnd = PtStart;
			PtEnd.y += (XSecPts[i].z - fDatum) * RecParam.data.dYScale;
		
			LINE(PtStart, PtEnd, RecParam.data.szLyrOrdinate);
			ProfTopPtsArray.append(PtEnd);
		}
		//Elev
		iCurRowNo = 0;
		PtStart = ptGrphOrg;
		PtStart.x  += fOrdDist;
		PtStart.y  -= (RecParam.data.dRowHt * (iCurRowNo + 0.5));
		ads_rtos (XSecPts[i].z, 2, 3, szAnnTxt);
		TEXTMID(PtStart, szAnnTxt, DTOR(90.0), RecParam.data.dNumAnnSize, RecParam.data.szLyrAnnotation);
		if(RTNORM != ads_entlast (entLast))
		{
			ads_printf("\nERROR: Fatal error!");
			return;
		}
		if(1 != CXDataPflAndCS::AttachXData(entLast, CXDataPflAndCS::m_pszElevTag, XSecPts[i].z))
		{
			ads_printf("\nERROR: Fatal error!");
			return;
		}

		// Offset
		iCurRowNo++;
		PtStart = ptGrphOrg;
		PtStart.x  += fOrdDist;
		PtStart.y  -=  (RecParam.data.dRowHt * (iCurRowNo + 0.5));
		ads_rtos (fOffset, 2, 3, szAnnTxt);
		TEXTMID(PtStart, szAnnTxt, DTOR(90.0), RecParam.data.dNumAnnSize, RecParam.data.szLyrAnnotation);
		if(RTNORM != ads_entlast (entLast))
		{
			ads_printf("\nERROR: Fatal error!");
			return;
		}
		if(RecParam.bDrawWithNorthingEastingData)
		{
			//Northing 
			iCurRowNo++;
			PtStart = ptGrphOrg;
			PtStart.x  += fOrdDist;
			PtStart.y  -= (RecParam.data.dRowHt * (iCurRowNo + 0.5));
			ads_rtos (XSecPts[i].y, 2, 3, szAnnTxt);
			TEXTMID(PtStart, szAnnTxt, DTOR(90.0), RecParam.data.dNumAnnSize, RecParam.data.szLyrAnnotation);
			if(RTNORM != ads_entlast (entLast))
			{
				ads_printf("\nERROR: Fatal error!");
				return;
			}
			if(1 != CXDataPflAndCS::AttachXData(entLast, CXDataPflAndCS::m_pszNorthTag, XSecPts[i].y))
			{
				ads_printf("\nERROR: Fatal error!");
				return;
			}
			//Easting 
			iCurRowNo++;
			PtStart = ptGrphOrg;
			PtStart.x  += fOrdDist;
			PtStart.y  -= (RecParam.data.dRowHt * (iCurRowNo + 0.5));
			ads_rtos (XSecPts[i].x, 2, 3, szAnnTxt);
			TEXTMID(PtStart, szAnnTxt, DTOR(90.0), RecParam.data.dNumAnnSize, RecParam.data.szLyrAnnotation);
			if(RTNORM != ads_entlast (entLast))
			{
				ads_printf("\nERROR: Fatal error!");
				return;
			}
			if(1 != CXDataPflAndCS::AttachXData(entLast, CXDataPflAndCS::m_pszEastTag, XSecPts[i].x))
			{
				ads_printf("\nERROR: Fatal error!");
				return;
			}
		}
		//Draw Ticks
		for(j = 0; j <= iNumOfRows; j++)
		{
			if(j == iNumOfRows)
			{
				PtStart = ptGrphOrg;
				PtStart.x  += fOrdDist;
				PtStart.y  -= ((j * RecParam.data.dRowHt)  + (RecParam.data.dNumAnnSize * 0.0));

				PtEnd = PtStart;
				PtEnd.y  += (RecParam.data.dNumAnnSize * 1.0);
			}
			else
			{//
				switch(j)
				{
				case 0:
					PtStart = ptGrphOrg;
					PtStart.x  += fOrdDist;
					PtStart.y  -= ((j * RecParam.data.dRowHt)  + (RecParam.data.dNumAnnSize * 0.0));

					PtEnd = PtStart;
					PtEnd.y  -= (RecParam.data.dNumAnnSize * 1.0);
					break;
				default:
					PtStart = ptGrphOrg;
					PtStart.x  += fOrdDist;
					PtStart.y  -= ((j * RecParam.data.dRowHt) - (RecParam.data.dNumAnnSize * 0.5));

					PtEnd = PtStart;
					PtEnd.y  -= (RecParam.data.dNumAnnSize * 1.0);
					break;
				}
			}
			LINE(PtStart, PtEnd, RecParam.data.szLyrAnnotation);
		}
	}
	POLYLINE(ProfTopPtsArray, RecParam.data.szLyrTopSurface);
}
BOOL CXSectionDwg::GetDatumValue(const AcGePoint3dArray& DatumRangeArray, const double& fChainage, double& fDatum)
{
	int i;
	BOOL bFound;

	for(i = 0, bFound = FALSE; i < DatumRangeArray.length(); i++)
	{
		if((fChainage < DatumRangeArray[i].y) && (fChainage >= DatumRangeArray[i].x))
		{
			fDatum = DatumRangeArray[i].z;
			bFound = TRUE;
			return TRUE;
		}
	}
	if(fChainage > DatumRangeArray[DatumRangeArray.length() - 1].y)
	{//Invalid datum range array.......
		fDatum = DatumRangeArray[DatumRangeArray.length() - 1].z;
		return TRUE;
	}
	return FALSE;
}
BOOL CXSectionDwg::GetAutoDatumValue(const AcGePoint3dArray& PtsArray, const double& fDatumDenominator, const double& fMinBufferInDWGUnits, const double& fYScale, double& fDatum)
{
	int i;
	double fMinZ;
	
	fMinZ = fDatum = 0.0;
	if(PtsArray.length() <= 0)
		return FALSE;

	fMinZ = PtsArray[0].z;
	for(i = 1; i < PtsArray.length(); i++)
	{
		if(PtsArray[i].z < fMinZ)
		{
			fMinZ = PtsArray[i].z;
		}
	}
	fDatum = CalculateAutoDatum(fMinZ, fYScale, fMinBufferInDWGUnits, fDatumDenominator);
 	return TRUE;
}
int CXSectionDwg::FindMaxLenOfDatumStr(const AcGePoint3dArray& DatumRangeArray)
{
	int i, iMaxDatumDigits;
	char szAnnTxt[_MAX_PATH];

	for(i = iMaxDatumDigits = 0; i < DatumRangeArray.length(); i++)
	{
		ads_rtos(DatumRangeArray[i].z, 2, 3, szAnnTxt);
		if((int)strlen(szAnnTxt) > iMaxDatumDigits)
			iMaxDatumDigits = strlen(szAnnTxt);
	}
	return iMaxDatumDigits;
}
BOOL CXSectionDwg::FindMinMaxOffElevInStn(const AcGePoint3dArray& Array, const AcGePoint3d& ptMidOfXSec, double& fMinOff, double& fMaxOff, double& fMaxElev)
{
	int i;
	double fDist;

	fMinOff = fMaxOff = fMaxElev = 0.0;

	for(i = 0; i < Array.length(); i++)
	{
		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break encountered !\n");
			return FALSE;
		}
		if(i == 0)
		{
			fMaxElev = Array[i].z;
			if(!CalculateOffset(Array[i], Array, ptMidOfXSec, fMinOff))
				return FALSE;
			fMaxOff = fMinOff;
		}
		else
		{
			if(Array[i].z > fMaxElev)
				fMaxElev = Array[i].z;
			if(!CalculateOffset(Array[i], Array, ptMidOfXSec, fDist))
				return FALSE;
			if(fDist > fMaxOff)
				fMaxOff = fDist;
			if(fDist < fMinOff)
				fMinOff = fDist;
		}
	}
	return TRUE;
}
BOOL CXSectionDwg::CalculateOffset(const AcGePoint3d& ptParam, const AcGePoint3dArray& Array, const AcGePoint3d& ptMidOfXSec, double& fOffset)
{
	int i;
	BOOL bLeft;

	for(i = 0, fOffset = 0, bLeft = TRUE; i < Array.length(); i++)
	{
		if((ptMidOfXSec == Array[i]) && bLeft)
		{
			bLeft = FALSE;
			if(ptParam == ptMidOfXSec)
			{
				fOffset = 0;
				return TRUE;
			}
		}
		else
		{
			if(ptParam == Array[i])
			{
				fOffset = Dist2D(ptMidOfXSec, Array[i]);
				if(bLeft)
				{
					fOffset = 0.0 - fOffset;
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}
void CXSectionDwg::DrawAxis(const AcGePoint3d& ptLLCOfExtent, const XSECDATAEX& RecParam, const double& fDatum, const double& fMinOff, const double& fMaxOff, const double& fLeftOfXAxisWid, AcGePoint3d& ptGraphOrg)
{
	int i, iCurRowNo;
	double	dTotalXAxisLen;
	const double fTitleXtn = RecParam.data.dRowAnnSize * 1.5 * 1.5;
	AcGePoint3d PtStart, PtEnd;

	dTotalXAxisLen = RecParam.data.dXScale * (fabs(fMinOff) + fMaxOff) + fLeftOfXAxisWid;

	const int iNumOfRows = (RecParam.bDrawWithNorthingEastingData) ? 4:2;
	//Draw Axis..Horz Lines
	for(i = 0; i <= iNumOfRows; i++)
	{
		if((i == 0) || (i == iNumOfRows))
		{
			PtStart.x	= ptLLCOfExtent.x;					PtStart.y	= ptLLCOfExtent.y + (i * RecParam.data.dRowHt);	PtStart.z	= 0.0;
			PtEnd.x		= ptLLCOfExtent.x + dTotalXAxisLen;	PtEnd.y		= ptLLCOfExtent.y + (i * RecParam.data.dRowHt);	PtEnd.z		= 0.0;
			PtStart.y += fTitleXtn;
			PtEnd.y += fTitleXtn;
			LINE(PtStart, PtEnd, RecParam.data.szLyrAxis);
			if(i == iNumOfRows)
			{
				PtStart.x	= ptLLCOfExtent.x;					PtStart.y  = ptLLCOfExtent.y	+ (i * RecParam.data.dRowHt) - RecParam.data.dNumAnnSize;	PtStart.z	= 0.0;
				PtEnd.x		= ptLLCOfExtent.x + dTotalXAxisLen;	PtEnd.y		= ptLLCOfExtent.y	+ (i * RecParam.data.dRowHt) - RecParam.data.dNumAnnSize;	PtEnd.z		= 0.0;
				PtStart.y += fTitleXtn;
				PtEnd.y += fTitleXtn;
				LINE(PtStart, PtEnd, RecParam.data.szLyrAxis);
				continue;
			}
		}
		switch(i)
		{
		case 0:
			PtStart.x	= ptLLCOfExtent.x;					PtStart.y	= ptLLCOfExtent.y + (i * RecParam.data.dRowHt) + RecParam.data.dNumAnnSize;	PtStart.z	= 0.0;
			PtEnd.x		= ptLLCOfExtent.x + dTotalXAxisLen;	PtEnd.y		= ptLLCOfExtent.y + (i * RecParam.data.dRowHt) + RecParam.data.dNumAnnSize;	PtEnd.z		= 0.0;
			PtStart.y += fTitleXtn;
			PtEnd.y += fTitleXtn;
			LINE(PtStart, PtEnd, RecParam.data.szLyrAxis);
			break;
		default:
			PtStart.x	= ptLLCOfExtent.x;					PtStart.y	= ptLLCOfExtent.y + (i * RecParam.data.dRowHt) - (RecParam.data.dNumAnnSize / 2.0);	PtStart.z = 0.0;
			PtEnd.x		= ptLLCOfExtent.x + dTotalXAxisLen;	PtEnd.y		= ptLLCOfExtent.y + (i * RecParam.data.dRowHt) - (RecParam.data.dNumAnnSize / 2.0);	PtEnd.z = 0.0;
			PtStart.y += fTitleXtn;
			PtEnd.y += fTitleXtn;
			LINE(PtStart, PtEnd, RecParam.data.szLyrAxis);
			PtStart[X]	= ptLLCOfExtent.x;					PtStart.y	= ptLLCOfExtent.y + (i * RecParam.data.dRowHt) + (RecParam.data.dNumAnnSize / 2.0);	PtStart.z = 0.0;
			PtEnd[X]	= ptLLCOfExtent.x + dTotalXAxisLen;	PtEnd.y		= ptLLCOfExtent.y + (i * RecParam.data.dRowHt) + (RecParam.data.dNumAnnSize / 2.0);	PtEnd.z = 0.0;
			PtStart.y += fTitleXtn;
			PtEnd.y += fTitleXtn;
			LINE(PtStart, PtEnd, RecParam.data.szLyrAxis);
			break;
		}
	}
	//Draw Axis..Vert Lines
	PtStart.x	= ptLLCOfExtent.x;	PtStart.y	= ptLLCOfExtent.y + (0.0		* RecParam.data.dRowHt);	PtStart.z = 0.0;
	PtEnd.x		= ptLLCOfExtent.x;	PtEnd.y		= ptLLCOfExtent.y + (iNumOfRows * RecParam.data.dRowHt);	PtEnd.z = 0.0;
	PtStart.y += fTitleXtn;
	PtEnd.y += fTitleXtn;
	LINE(PtStart, PtEnd, RecParam.data.szLyrAxis);

	//Text Labels...
	iCurRowNo = 0;
	if(RecParam.bDrawWithNorthingEastingData)
	{
		//Easting
		PtStart.x	= ptLLCOfExtent.x + (RecParam.data.dRowAnnSize / 2.0);	PtStart.y  = ptLLCOfExtent.y	+ (RecParam.data.dRowHt * (iCurRowNo + 0.5));	PtStart.z = 0.0;
		PtStart.y += fTitleXtn;
		TEXTLEFT(PtStart, RecParam.data.szEastingTag, 0.0, RecParam.data.dRowAnnSize, RecParam.data.szLyrAxis);
		//Northing
		iCurRowNo++;
		PtStart.x	= ptLLCOfExtent.x + (RecParam.data.dRowAnnSize / 2.0);	PtStart.y  = ptLLCOfExtent.y	+ (RecParam.data.dRowHt * (iCurRowNo + 0.5));	PtStart.z = 0.0;
		PtStart.y += fTitleXtn;
		TEXTLEFT(PtStart, RecParam.data.szNorthingTag, 0.0, RecParam.data.dRowAnnSize, RecParam.data.szLyrAxis);
		//
		iCurRowNo++;
	}
	//Offset 
	PtStart.x	= ptLLCOfExtent.x + (RecParam.data.dRowAnnSize / 2.0);	PtStart.y  = ptLLCOfExtent.y	+ (RecParam.data.dRowHt * (iCurRowNo + 0.5));	PtStart.z = 0.0;
	PtStart.y += fTitleXtn;
	TEXTLEFT(PtStart, RecParam.data.szOffsetTag, 0.0, RecParam.data.dRowAnnSize, RecParam.data.szLyrAxis);
	//Elev 
	iCurRowNo++;
	PtStart.x	= ptLLCOfExtent.x + (RecParam.data.dRowAnnSize / 2.0);	PtStart.y  = ptLLCOfExtent.y	+ (RecParam.data.dRowHt * (iCurRowNo + 0.5));	PtStart.z = 0.0;
	PtStart.y += fTitleXtn;
	TEXTLEFT(PtStart, RecParam.data.szElevTag, 0.0, RecParam.data.dRowAnnSize, RecParam.data.szLyrAxis);
	
	{//Datum 
		char szTmp01[_MAX_PATH];
		char szTmp02[_MAX_PATH];
		
		PtStart.x	= ptLLCOfExtent.x + (RecParam.data.dRowAnnSize / 2.0);	PtStart.y  = ptLLCOfExtent.y	+ (RecParam.data.dRowHt * iNumOfRows) + (1.25 * RecParam.data.dRowAnnSize);	PtStart.z = 0.0;
		
		strcpy(szTmp01, RecParam.data.szDatumTag);
		ads_rtos (fDatum, 2, 3, szTmp02);
		strcat(szTmp01, szTmp02);
		PtStart.y += fTitleXtn;
		TEXTLEFT(PtStart, szTmp01, 0.0, RecParam.data.dRowAnnSize, RecParam.data.szLyrAxis);
	}
	ptGraphOrg = ptLLCOfExtent;
	ptGraphOrg.x += fLeftOfXAxisWid;
	ptGraphOrg.y += (RecParam.data.dRowHt * iNumOfRows);
	ptGraphOrg.y += fTitleXtn;
}
BOOL CXSectionDwg::IsCSDwgPossible(const AcGePoint3d& ptLROfCS, const double& fCSWid, const double& fCSHt, const AcGePoint3d& ptULOfNPlate, const double& fNPWid, const double& fNPHt)
{
	AcGePoint3d ptLLCOfNPlate;

	ptLLCOfNPlate.x = ptULOfNPlate.x;
	ptLLCOfNPlate.y = ptULOfNPlate.y - fNPHt;
	ptLLCOfNPlate.z = 0.0;
	{//Chk whether any vertex of CS Dwg extent lies in the NPlate Rectangle or not....
		AcGePoint3d ptTmp;

		//LLC
		ptTmp = ptLROfCS;
		ptTmp.x = ptLROfCS.x - fCSWid;
		if(IsPointWithinRect(ptTmp, ptLLCOfNPlate, fNPWid, fNPHt))
			return FALSE;
		//LRC
		ptTmp = ptLROfCS;
		if(IsPointWithinRect(ptTmp, ptLLCOfNPlate, fNPWid, fNPHt))
			return FALSE;
		//URC
		ptTmp = ptLROfCS;
		ptTmp.y = ptLROfCS.y + fCSHt;
		if(IsPointWithinRect(ptTmp, ptLLCOfNPlate, fNPWid, fNPHt))
			return FALSE;
		//ULC
		ptTmp = ptLROfCS;
		ptTmp.y = ptLROfCS.y + fCSHt;
		ptTmp.x = ptLROfCS.x - fCSWid;
		if(IsPointWithinRect(ptTmp, ptLLCOfNPlate, fNPWid, fNPHt))
			return FALSE;
	}
	{//Chk whether any vertex of NPlate lies in the CS Dwg extent Rectangle or not....
		AcGePoint3d ptTmp, ptLLCOfCS;

		ptLLCOfCS = ptLROfCS;
		ptLLCOfCS.x = ptLROfCS.x - fCSWid;

		//LLC
		ptTmp = ptLLCOfNPlate;
		if(IsPointWithinRect(ptTmp, ptLLCOfCS, fCSWid, fCSHt))
			return FALSE;
		//LRC
		ptTmp = ptLLCOfNPlate;
		ptTmp.x += fNPWid;
		if(IsPointWithinRect(ptTmp, ptLLCOfCS, fCSWid, fCSHt))
			return FALSE;
		//URC
		ptTmp = ptLLCOfNPlate;
		ptTmp.x += fNPWid;
		ptTmp.y += fNPHt;
		if(IsPointWithinRect(ptTmp, ptLLCOfCS, fCSWid, fCSHt))
			return FALSE;
		//ULC
		ptTmp = ptLLCOfNPlate;
		ptTmp.y += fNPHt;
		if(IsPointWithinRect(ptTmp, ptLLCOfCS, fCSWid, fCSHt))
			return FALSE;
	}
	return TRUE;
}
BOOL CXSectionDwg::IsPointWithinRect(const AcGePoint3d& ptParam, const AcGePoint3d& ptLLCOfRect, const double& fWid, const double& fHt)
{
	BOOL bFlag;
	
	bFlag = (ptParam.x >= ptLLCOfRect.x) && (ptParam.x <= ptLLCOfRect.x + fWid);
	bFlag = bFlag && (ptParam.y >= ptLLCOfRect.y) && (ptParam.y <= ptLLCOfRect.y + fHt);
	return bFlag;
}
void CXSectionDwg::RectangleUL(const AcGePoint3d& ptUpperLeft, const double& fWid, const double& fHt, const char* pszLyr)
{
	AcGePoint3d Pt01, Pt02, Pt03, Pt04;
	AcGePoint3dArray ptArrayTmp;

	//Upper Left
	Pt01 = ptUpperLeft;
	//Upper Right
	Pt02 = Pt01;
	Pt02.x = Pt01.x + fWid;
	//Lower Right
	Pt03 = Pt02;
	Pt03.y = Pt02.y - fHt;
	//Lower Left
	Pt04 = Pt03;
	Pt04.x = Pt03.x - fWid;

	ptArrayTmp.append(Pt01);
	ptArrayTmp.append(Pt02);
	ptArrayTmp.append(Pt03);
	ptArrayTmp.append(Pt04);
	ptArrayTmp.append(Pt01);

	POLYLINE(ptArrayTmp, pszLyr);
}
void CXSectionDwg::RectangleLR(const AcGePoint3d& ptLowerRight, const double& fWid, const double& fHt, const char* pszLyr)
{
	AcGePoint3d ptUpperLeft;

	//Upper Left
	ptUpperLeft = ptLowerRight;
	ptUpperLeft.x = ptLowerRight.x - fWid;
	ptUpperLeft.y = ptLowerRight.y + fHt;
	
	RectangleUL(ptUpperLeft, fWid, fHt, pszLyr);
}
///////
BOOL CXSectionDwg::GetNextSectionRange(const int& iStartIndex, const double& fCurRowDepth, const CThePointsOnAlignment& PIDArray, const AcGePoint3dArray& DatumRangeArray, const XSECDATAEX& RecParam, int& iEndIndex, double& fLeftOfXAxisWid, double& fMaxHeight)
{
	const double fTitleXtn = RecParam.data.dCSLabelAnnSize * 1.5;
	const double fTxtWidMF = 1.0;
	const double fCell2CellDist = RecParam.data.dNumAnnSize * 4.0;
	int i, iMaxLenOfTagStrs, iMaxDatumDigits;
	double fBotOfXAxisLen, fCSWid, fCSHt, fCumWid, fMinOff, fMaxOff, fCurShtWid, fPrevMaxHt;
	AcGePoint3d ptMidOfXSec;
	double fChainage, fDatum, fMaxElev;

	if(iStartIndex >= PIDArray.GetCount())
		return FALSE;
	
	iMaxDatumDigits = FindMaxLenOfDatumStr(DatumRangeArray);
	if(iMaxDatumDigits <= 0)
		iMaxDatumDigits = 8;

	iMaxLenOfTagStrs = __max (strlen(RecParam.data.szElevTag),		strlen(RecParam.data.szOffsetTag));
	iMaxLenOfTagStrs = __max (strlen(RecParam.data.szNorthingTag),	strlen(RecParam.data.szOffsetTag));
	iMaxLenOfTagStrs = __max (strlen(RecParam.data.szEastingTag),	strlen(RecParam.data.szOffsetTag));
	iMaxLenOfTagStrs = __max ((int)strlen(RecParam.data.szDatumTag) + iMaxDatumDigits, iMaxLenOfTagStrs);

	fLeftOfXAxisWid = iMaxLenOfTagStrs * RecParam.data.dRowAnnSize * fTxtWidMF;
	
	fBotOfXAxisLen = RecParam.data.dRowHt * ((RecParam.bDrawWithNorthingEastingData) ? 4.0 : 2.0) + fTitleXtn;

	fCumWid = fPrevMaxHt = fMaxHeight = 0.0;
	for(i = iStartIndex, iEndIndex = iStartIndex; i < PIDArray.GetCount(); i++)
	{
		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break encountered\n");
			return FALSE;
		}
		const AcGePoint3dArray* pArrayOfCSPoints = (AcGePoint3dArray*)PIDArray.GetAt(i)->GetItemDataPtr();
		if((0L == pArrayOfCSPoints) || (pArrayOfCSPoints->length() <= 0))
			continue;
		
		if(!GetChainage(pArrayOfCSPoints, PIDArray, fChainage, &ptMidOfXSec))
			return FALSE;

		if(RecParam.data.bAutoDatumFlag)
		{
			if(!GetAutoDatumValue(*pArrayOfCSPoints, RecParam.data.dAutoDatumDenominator, RecParam.dAutoDatumMargin, RecParam.data.dYScale, fDatum))
				return FALSE;
		}
		else
		{
			if(!GetDatumValue(DatumRangeArray, fChainage, fDatum))
				return FALSE;
		}

		if(!FindMinMaxOffElevInStn(*pArrayOfCSPoints, ptMidOfXSec, fMinOff, fMaxOff, fMaxElev))
			return FALSE;

		fCSWid = (fabs(fMinOff) + fMaxOff) * RecParam.data.dXScale + fLeftOfXAxisWid + fCell2CellDist;
		fCumWid += fCSWid;
		
		const double fMaxHeightForCurrentSection = (fMaxElev - fDatum) * RecParam.data.dYScale;
		if(fMaxHeightForCurrentSection >= fMaxHeight)
		{
			fPrevMaxHt = fMaxHeight;
			fMaxHeight = fMaxHeightForCurrentSection;
		}

		fCSHt = fMaxHeight + fBotOfXAxisLen + fCell2CellDist;
		
		//Check sheet width & height ......
		if(fCSWid >= RecParam.data.dSheetWid)
		{
			ads_printf("\nERROR: Failed to draw cross section\nPossible cause: Sheet width is too small");
			return FALSE;
		}
		if(fCSHt >= RecParam.data.dSheetHeight)
		{
			ads_printf("\nERROR: Failed to draw cross section\nPossible cause: Sheet height is too small");
			return FALSE;
		}

		//
		if((RecParam.data.dSheetHeight - (fCurRowDepth + fCSHt)) <= RecParam.data.dNPlateHeight)
		{
			fCurShtWid = RecParam.data.dSheetWid - RecParam.data.dNPlateWid;
		}
		else
		{
			fCurShtWid = RecParam.data.dSheetWid;
		}
		if((RecParam.data.dSheetHeight - (fCurRowDepth + fCSHt)) < 0.0)
		{//Out of Range
			iEndIndex = i - 1;
			if((fMaxHeightForCurrentSection == fMaxHeight) && (fPrevMaxHt > 0.0))
				fMaxHeight = fPrevMaxHt;
			fMaxHeight += fBotOfXAxisLen;
			fMaxHeight += fCell2CellDist;
			return TRUE;
		}

		if(fCumWid > fCurShtWid)
		{
			iEndIndex = i - 1;
			if((fMaxHeightForCurrentSection == fMaxHeight) && (fPrevMaxHt > 0.0))
				fMaxHeight = fPrevMaxHt;
			fMaxHeight += fBotOfXAxisLen;
			fMaxHeight += fCell2CellDist;
			return TRUE;
		}
		else
		{
			if(fCumWid == fCurShtWid)
			{
				iEndIndex = i;
				fMaxHeight += fBotOfXAxisLen;
				fMaxHeight += fCell2CellDist;
				return TRUE;
			}
		}
	}
	iEndIndex = PIDArray.GetCount() - 1;
	fMaxHeight += fBotOfXAxisLen;
	fMaxHeight += fCell2CellDist;
	return TRUE;
}

BOOL CXSectionDwg::DrawCSRow(const AcGePoint3d& ptLLCStart, const int& iStartIndex, const int& iEndIndex, const CThePointsOnAlignment& PIDArray, const AcGePoint3dArray& DatumRangeArray, const XSECDATAEX& RecParam, const double& fLeftOfXAxisWid, const double& fCSHeight, const CString& strCSGridLyr, const AcGePoint3d& ptULOfNPlate, int& iLastDrawnCS)
{
	const double fTitleXtn = RecParam.data.dCSLabelAnnSize * 1.5;
	const double fTxtWidMF = 1.0;
	const double fCell2CellDist = RecParam.data.dNumAnnSize * 4.0;
	int i;
	double fCSWid, fMinOff, fMaxOff, fMaxElev, fDatum, fChainage;
	AcGePoint3d ptGrphOrg, ptLLCOfCS, ptLROfCS, ptMidOfXSec;
	
	if((iStartIndex >= PIDArray.GetCount()) || (iEndIndex >= PIDArray.GetCount()))
		return FALSE;

	//Drawing .....
	for(i = iStartIndex, iLastDrawnCS = iStartIndex, ptLLCOfCS = ptLLCStart; i <= iEndIndex; i++)
	{
		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break encountered\n");
			return FALSE;
		}
		const AcGePoint3dArray* pArrayOfCSPoints = (AcGePoint3dArray*)PIDArray.GetAt(i)->GetItemDataPtr();
		if((0L == pArrayOfCSPoints) || (pArrayOfCSPoints->length() <= 0))
		{
			iLastDrawnCS++;
			continue;
		}
		const double fChainageIdeal = PIDArray.GetAt(i)->GetChainage();
		if(!GetChainage(pArrayOfCSPoints, PIDArray, fChainage, &ptMidOfXSec))
			return FALSE;

		if(RecParam.data.bAutoDatumFlag)
		{
			if(!GetAutoDatumValue(*pArrayOfCSPoints, RecParam.data.dAutoDatumDenominator, RecParam.dAutoDatumMargin, RecParam.data.dYScale, fDatum))
				return FALSE;
		}
		else
		{
			if(!GetDatumValue(DatumRangeArray, fChainage, fDatum))
				return FALSE;
		}

		if(!FindMinMaxOffElevInStn(*pArrayOfCSPoints, ptMidOfXSec, fMinOff, fMaxOff, fMaxElev))
			return FALSE;

		fCSWid = (fabs(fMinOff) + fMaxOff) * RecParam.data.dXScale + fLeftOfXAxisWid + fCell2CellDist;

		ptLROfCS.x = ptLLCOfCS.x;
		ptLROfCS.y = ptLLCOfCS.y;
		ptLROfCS.x += fCSWid;
		
		if(fCSWid >= RecParam.data.dSheetWid)
		{
			ads_printf("\nERROR: Failed to draw cross section\nPossible cause: Sheet width is too small");
			return FALSE;
		}
		if(fCSHeight >= RecParam.data.dSheetHeight)
		{
			ads_printf("\nERROR: Failed to draw cross section\nPossible cause: Sheet height is too small");
			return FALSE;
		}
		if(!IsCSDwgPossible(ptLROfCS, fCSWid, fCSHeight, ptULOfNPlate, RecParam.data.dNPlateWid, RecParam.data.dNPlateHeight))
		{
			ads_printf("\nFATAL ERROR : DrawCSRow\n");
			return TRUE;
		}

		ads_printf("\rPlease wait...Drawing Cross Section at Chainage %f (#%d/%d)", (fChainage + RecParam.data.dStartChg), i+1, PIDArray.GetCount());
			
		DrawAxis(ptLLCOfCS, RecParam, fDatum, fMinOff, fMaxOff, fLeftOfXAxisWid, ptGrphOrg);
		DrawOrdinates(ptGrphOrg, *pArrayOfCSPoints, ptMidOfXSec, RecParam, fMinOff, fMaxOff, fDatum);

		{//XSec Label
			AcGePoint3d ptTmp;
			CString strTmp;
			const double fTitleXtn = RecParam.data.dCSLabelAnnSize * 1.5;
			char szTmp[_MAX_PATH];
	
			ads_rtos (fChainageIdeal, 2, 3, szTmp);
			strTmp.Format("%s %s", RecParam.data.szXSecLabelTag, szTmp);

			ptTmp.x = ptLLCOfCS.x;
			ptTmp.y = ptLLCOfCS.y;
			ptTmp.x += fCSWid / 2.0;
			ptTmp.y += (fTitleXtn / 2.0);
			TEXTMID(ptTmp, strTmp, 0.0, RecParam.data.dCSLabelAnnSize, RecParam.data.szLyrAxis);
		}
		RectangleLR(AcGePoint3d(ptLLCOfCS.x+fCSWid, ptLLCOfCS.y, 0.0), fCSWid, fCSHeight, strCSGridLyr);
		ptLLCOfCS.x += fCSWid;
		iLastDrawnCS++;
	}
	return TRUE;
}


BOOL CXSectionDwg::DrawAllSections(const AcGePoint3d& ptULCornerOfSheet, const CThePointsOnAlignment& PIDArray, const AcGePoint3dArray& DatumRangeArray, const XSECDATAEX& RecParam)
{
	const double fSheetShiftFac = 0.1;
	const CString strSheetLyr = "DRAWING_SHEET";
	const CString strCSGridLyr = "CROSS_SEXN_GRID";
	int i, iEndIndex;
	double fSheetOrgX, fCumY, fSheetOrgY, fTotalHeight, fLeftOfXAxisWid;
	AcGePoint3d ptULOfNPlate;
	
	LAYER(strCSGridLyr, 1);
	LAYER(strSheetLyr, 2);

	LAYER(RecParam.data.szLyrAnnotation, 7);
	LAYER(RecParam.data.szLyrTopSurface, 1);
	LAYER(RecParam.data.szLyrOrdinate, 3);
	LAYER(RecParam.data.szLyrAxis, 6);

	fSheetOrgX = ptULCornerOfSheet.x;
	fSheetOrgY = ptULCornerOfSheet.y;

	//Sheet Border
	RectangleUL(AcGePoint3d(fSheetOrgX, fSheetOrgY, 0.0), RecParam.data.dSheetWid, RecParam.data.dSheetHeight, strSheetLyr);
	{//Name Plate Border
		ptULOfNPlate.x = fSheetOrgX + RecParam.data.dSheetWid - RecParam.data.dNPlateWid;
		ptULOfNPlate.y = fSheetOrgY - RecParam.data.dSheetHeight + RecParam.data.dNPlateHeight;
		ptULOfNPlate.z = 0.0;
		RectangleUL(ptULOfNPlate, RecParam.data.dNPlateWid, RecParam.data.dNPlateHeight, strSheetLyr);
	}
	ads_printf("\nPlease wait...Drawing Cross Section(s)...\n");
	for(i = 0, fCumY = 0.0; i < PIDArray.GetCount(); )
	{
		int iLastDrawnCS;

		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break encountered\nTotal %d# of cross section(s) drawn\n", i);
			return FALSE;
		}

		if(!GetNextSectionRange(i, fCumY, PIDArray, DatumRangeArray, RecParam, iEndIndex, fLeftOfXAxisWid, fTotalHeight))
			return FALSE;
		if(!DrawCSRow(AcGePoint3d(fSheetOrgX, fSheetOrgY - fTotalHeight, 0.0), i, iEndIndex, PIDArray, DatumRangeArray, RecParam, fLeftOfXAxisWid, fTotalHeight, strCSGridLyr, ptULOfNPlate, iLastDrawnCS))
			return FALSE;
		
		fCumY += fTotalHeight;
		fSheetOrgY -= fTotalHeight;
		if(fCumY >= RecParam.data.dSheetHeight)
		{
			fCumY = 0.0;
			fSheetOrgX += RecParam.data.dSheetWid + (RecParam.data.dSheetWid * fSheetShiftFac);
			fSheetOrgY = ptULCornerOfSheet.y;
			if(i < PIDArray.GetCount())
			{
				//Sheet Border
				RectangleUL(AcGePoint3d(fSheetOrgX, fSheetOrgY, 0.0), RecParam.data.dSheetWid, RecParam.data.dSheetHeight, strSheetLyr);
				{//Name Plate Border
					ptULOfNPlate.x = fSheetOrgX + RecParam.data.dSheetWid - RecParam.data.dNPlateWid;
					ptULOfNPlate.y = fSheetOrgY - RecParam.data.dSheetHeight + RecParam.data.dNPlateHeight;
					ptULOfNPlate.z = 0.0;
					RectangleUL(ptULOfNPlate, RecParam.data.dNPlateWid, RecParam.data.dNPlateHeight, strSheetLyr);
				}
			}
		}
		i = iLastDrawnCS;
	}
	ads_printf("\nDone !\n");
	return TRUE;
}

