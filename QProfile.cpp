// QProfile.cpp : implementation file
//
#include "stdafx.h"
#include "Util.h"
#include "DrawUtil.h"
#include "QProfile.h"
#include "XData.h"

static void DrawAxis(const PROFILEDATAEX&, const AcGePoint3d&, AcGePoint3d&, const double&);

void DrawQProfile(const AcGePoint3d& PtOrg, const CThePointsOnAlignment* pThePtsOnAlignment, const PROFILEDATAEX& RecParam)
{
	const double fPermissibleError = 1E-6;
	AcGePoint3dArray ProfTopPtsArray;
	AcGePoint3d PtAxisOrg;
	AcGePoint3d PtStart, PtEnd, PtTxtOrg;
	double dCumDist, dDistFrLastOrd;
	int i, j, iRowCount;
	char szAnnTxt[_MAX_PATH];
	ads_name entLast;
	
	assert(0L != pThePtsOnAlignment);

	iRowCount = (RecParam.data.bDrawNERows) ? 4 : 2;
	dDistFrLastOrd = 0.0;
	const CPointOnAlignment* pLastRec	= pThePtsOnAlignment->GetAt(pThePtsOnAlignment->GetCount() - 1);
	const CPointOnAlignment* pFirstRec	= pThePtsOnAlignment->GetAt(0);
	dCumDist = pLastRec->GetChainage() - pFirstRec->GetChainage();

	LAYER(RecParam.data.szLyrAnnotation, 1);
	LAYER(RecParam.data.szLyrTopSurface, 2);
	LAYER(RecParam.data.szLyrOrdinate, 3);
	LAYER(RecParam.data.szLyrAxis, 4);

	//Draw Axis
	DrawAxis(RecParam, PtOrg, PtAxisOrg, dCumDist);
	
	//Draw Ords
	dCumDist = dDistFrLastOrd = 0.0;
	for(i = 0; i < pThePtsOnAlignment->GetCount(); i++)
	{
		BOOL bIsMajorOrdinate;
		const CPointOnAlignment* pRec =	pThePtsOnAlignment->GetAt(i);

		dCumDist = pRec->GetChainage() - RecParam.data.dStartChg;
		if(i > 0)
		{
			const CPointOnAlignment* pLastRec =	pThePtsOnAlignment->GetAt(i - 1);
			dDistFrLastOrd = dCumDist - (pLastRec->GetChainage() - RecParam.data.dStartChg);
		}
		ads_printf("\rPlease wait... Drawing ordinate at chainage %f...", dCumDist);
		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break encountered\n");
			return;
		}
		bIsMajorOrdinate = FALSE;
		/*
		fTmp = fmod(dCumDist, RecParam.data.dSexonInterval);
		if(fabs(fTmp - RecParam.data.dSexonInterval) <= fPermissibleError)
			fTmp = 0.0;
		//ads_printf("\nfTmp = %f, dCumDist = %f, RecParam.data.dSexonInterval = %f", fTmp, dCumDist, RecParam.data.dSexonInterval);
		*/
		const int iType = (int)pRec->GetType();
		if((dCumDist == 0.0) || ((i+1) == pThePtsOnAlignment->GetCount()) || (iType & CPointOnAlignment::PointType::AtMajorInterval) || (iType & CPointOnAlignment::PointType::AtIntersection) || (iType & CPointOnAlignment::PointType::AtMinorInterval))
		{
			bIsMajorOrdinate = TRUE;
		}
		else
		{
			#ifdef _DEBUG
				//ads_printf("%f", fTmp);
			#endif//_DEBUG
		}

		//Set X & Y
		PtTxtOrg.x	= PtAxisOrg.x + RecParam.data.dXScale * dCumDist; PtTxtOrg.z = 0.0;

		//Cum Distance 
		PtTxtOrg.y  = PtAxisOrg.y - ((iRowCount - 0.5) * RecParam.data.dRowHt);
		if(bIsMajorOrdinate || (strlen(pRec->GetComment()) > 0))
		{
			ads_rtos (pRec->GetChainage(), 2, 3, szAnnTxt);
			TEXTMID(PtTxtOrg, szAnnTxt, DTOR(90.0), RecParam.data.dNumAnnSize, RecParam.data.szLyrAnnotation);
			if(RTNORM != ads_entlast (entLast))
			{
				ads_printf("\nERROR: Fatal error!");
				return;
			}
			if(1 != CXDataPflAndCS::AttachXData(entLast, CXDataPflAndCS::m_pszChgTag, pRec->GetChainage()))
			{
				ads_printf("\nERROR: Fatal error!");
				return;
			}
		}
		if(RecParam.data.bDrawNERows)
		{
			//Easting 
			PtTxtOrg.y  = PtAxisOrg.y	- ((iRowCount - 1.5) * RecParam.data.dRowHt);
			if(bIsMajorOrdinate || (strlen(pRec->GetComment()) > 0))
			{
				ads_rtos (pRec->GetPoint().x, 2, 3, szAnnTxt);
				TEXTMID(PtTxtOrg, szAnnTxt, DTOR(90.0), RecParam.data.dNumAnnSize, RecParam.data.szLyrAnnotation);
				if(RTNORM != ads_entlast (entLast))
				{
					ads_printf("\nERROR: Fatal error!");
					return;
				}
				if(1 != CXDataPflAndCS::AttachXData(entLast, CXDataPflAndCS::m_pszEastTag, pRec->GetPoint().x))
				{
					ads_printf("\nERROR: Fatal error!");
					return;
				}
			}
			//Northing 
			PtTxtOrg.y  = PtAxisOrg.y	- ((iRowCount - 2.5) * RecParam.data.dRowHt);
			if(bIsMajorOrdinate || (strlen(pRec->GetComment()) > 0))
			{
				ads_rtos (pRec->GetPoint().y, 2, 3, szAnnTxt);
				TEXTMID(PtTxtOrg, szAnnTxt, DTOR(90.0), RecParam.data.dNumAnnSize, RecParam.data.szLyrAnnotation);
				if(RTNORM != ads_entlast (entLast))
				{
					ads_printf("\nERROR: Fatal error!");
					return;
				}
				if(1 != CXDataPflAndCS::AttachXData(entLast, CXDataPflAndCS::m_pszNorthTag, pRec->GetPoint().y))
				{
					ads_printf("\nERROR: Fatal error!");
					return;
				}
			}
		}
		//Elev 
		PtTxtOrg.y  = PtAxisOrg.y	- (0.5 * RecParam.data.dRowHt);
		if(bIsMajorOrdinate || (strlen(pRec->GetComment()) > 0))
		{
			ads_rtos (pRec->GetPoint().z, 2, 3, szAnnTxt);
			TEXTMID(PtTxtOrg, szAnnTxt, DTOR(90.0), RecParam.data.dNumAnnSize, RecParam.data.szLyrAnnotation);
			if(RTNORM != ads_entlast (entLast))
			{
				ads_printf("\nERROR: Fatal error!");
				return;
			}
			if(1 != CXDataPflAndCS::AttachXData(entLast, CXDataPflAndCS::m_pszElevTag, pRec->GetPoint().z))
			{
				ads_printf("\nERROR: Fatal error!");
				return;
			}
		}

		{//Ord
			AcGePoint3d PtTmp;

			PtTxtOrg.x	= PtAxisOrg.x + RecParam.data.dXScale * dCumDist; PtTxtOrg.z = 0.0;
			PtTxtOrg.y  = PtAxisOrg.y	- (0.0 * RecParam.data.dRowHt);
			
			PtTmp.x = PtTxtOrg.x; PtTmp.y = PtTxtOrg.y; PtTmp.z = PtTxtOrg.z;
			PtTmp.y = PtTxtOrg.y + RecParam.data.dYScale * (pRec->GetPoint().z - RecParam.data.dDatum);
			
			if(bIsMajorOrdinate || (strlen(pRec->GetComment()) > 0))
			{
				LINE(PtTxtOrg, PtTmp, RecParam.data.szLyrOrdinate);
			}
			{//
				AcGePoint3d PtOrdVertex;
				//Add to Vertex Array...
				PtOrdVertex.x = PtAxisOrg.x + RecParam.data.dXScale * dCumDist;
				PtOrdVertex.y = PtTxtOrg[Y] + RecParam.data.dYScale * (pRec->GetPoint().z - RecParam.data.dDatum);
				PtOrdVertex.z = 0.0;
				
				ProfTopPtsArray.append(PtOrdVertex);
				
				{//Comments
					if(strlen(pRec->GetComment()) > 0)
					{
						PtOrdVertex.y += RecParam.data.dRowAnnSize;
						PtOrdVertex.x += RecParam.data.dRowAnnSize / 2.0;
						TEXTLEFT(PtOrdVertex, pRec->GetComment(), (PI / 2.0), RecParam.data.dRowAnnSize, RecParam.data.szLyrAxis);
					}
				}
			}
			//Draw Ticks
			if(bIsMajorOrdinate || (strlen(pRec->GetComment()) > 0))
			{
				for(j = 0; j <= iRowCount; j++)
				{
					if(j == 0)
					{
						PtTxtOrg.x	= PtAxisOrg.x + RecParam.data.dXScale * dCumDist;	PtTxtOrg.y	= PtOrg.y	+ (j * RecParam.data.dRowHt) + (RecParam.data.dNumAnnSize * 0.0);	PtTxtOrg.z = 0.0;
						PtTmp.x	= PtAxisOrg.x + RecParam.data.dXScale * dCumDist;		PtTmp.y	= PtOrg.y	+ (j * RecParam.data.dRowHt) + (RecParam.data.dNumAnnSize * 1.0);	PtTmp.z = 0.0;
					}
					else
					{
						if(j == iRowCount)
						{
							PtTxtOrg.x	= PtAxisOrg.x + RecParam.data.dXScale * dCumDist;	PtTxtOrg[Y]	= PtOrg.y	+ (j * RecParam.data.dRowHt) - (RecParam.data.dNumAnnSize * 1.0);	PtTxtOrg.z = 0.0;
							PtTmp.x	= PtAxisOrg.x + RecParam.data.dXScale * dCumDist;	PtTmp[Y]	= PtOrg.y	+ (j * RecParam.data.dRowHt) + (RecParam.data.dNumAnnSize * 0.0);	PtTmp.z = 0.0;
						}
						else
						{
							PtTxtOrg.x	= PtAxisOrg.x + RecParam.data.dXScale * dCumDist;	PtTxtOrg[Y]	= PtOrg.y	+ (j * RecParam.data.dRowHt) - (RecParam.data.dNumAnnSize * 0.5);	PtTxtOrg.z = 0.0;
							PtTmp.x	= PtAxisOrg.x + RecParam.data.dXScale * dCumDist;	PtTmp[Y]	= PtOrg.y	+ (j * RecParam.data.dRowHt) + (RecParam.data.dNumAnnSize * 0.5);	PtTmp.z = 0.0;
						}

					}
					LINE(PtTmp, PtTxtOrg, RecParam.data.szLyrAnnotation);
				}
			}
		}
	}
	POLYLINE(ProfTopPtsArray, RecParam.data.szLyrTopSurface);
}

static void DrawAxis(const PROFILEDATAEX& RecParam, const AcGePoint3d& PtOrg, AcGePoint3d& PtAxisOrg, const double& dXAxisLen)
{
	int i, iMaxLen, iRowCount;
	double	dTotalXAxisLen, dXtraXAxisLen;
	AcGePoint3d PtStart, PtEnd;

	//Find Max lengthy str...
	iMaxLen = __max (__max (__max (strlen(RecParam.data.szElevTag), strlen(RecParam.data.szNorthingTag)), __max (strlen(RecParam.data.szEastingTag), strlen(RecParam.data.szCumDistanceTag))), strlen(RecParam.data.szDistanceTag));
	
	dXtraXAxisLen = iMaxLen * (RecParam.data.dRowAnnSize * 0.9) * 1.20;
	dTotalXAxisLen = dXAxisLen * RecParam.data.dXScale + dXtraXAxisLen;

	iRowCount = (RecParam.data.bDrawNERows) ? 4:2;
	PtAxisOrg.x	= PtOrg.x + dXtraXAxisLen;	PtAxisOrg.y  = PtOrg.y	+ (iRowCount * RecParam.data.dRowHt);	PtAxisOrg.z = 0.0;

	//Draw Profile Label
	PtStart  = PtOrg;
	PtStart.y  -= 1.5 * RecParam.data.dPflLabelSize;
	PtStart.x  += dTotalXAxisLen * 0.5;
	TEXTMID(PtStart, RecParam.data.szProfileTag, 0.0, RecParam.data.dPflLabelSize, RecParam.data.szLyrAnnotation);

	//Draw Axis..Horz Lines
	for(i = 0; i <= iRowCount; i++)
	{
		if((i == 0) || (i == iRowCount))
		{
			PtStart.x	= PtOrg.x;					PtStart.y	= PtOrg.y	+ (i * RecParam.data.dRowHt);	PtStart.z	= 0.0;
			PtEnd.x		= PtOrg.x + dTotalXAxisLen;	PtEnd.y		= PtOrg.y	+ (i * RecParam.data.dRowHt);	PtEnd.z		= 0.0;
			LINE(PtStart, PtEnd, RecParam.data.szLyrAxis);
			if(i == 0)
			{
				PtStart.x	= PtOrg.x;					PtStart.y	= PtOrg.y	+ (i * RecParam.data.dRowHt) + RecParam.data.dNumAnnSize;	PtStart.z	= 0.0;
				PtEnd.x		= PtOrg.x + dTotalXAxisLen;	PtEnd.y		= PtOrg.y	+ (i * RecParam.data.dRowHt) + RecParam.data.dNumAnnSize;	PtEnd.z		= 0.0;
				LINE(PtStart, PtEnd, RecParam.data.szLyrAxis);
			}
			else
			{
				PtStart.x	= PtOrg.x;					PtStart.y  = PtOrg.y	+ (i * RecParam.data.dRowHt) - RecParam.data.dNumAnnSize;	PtStart.z	= 0.0;
				PtEnd.x		= PtOrg.x + dTotalXAxisLen;	PtEnd.y		= PtOrg.y	+ (i * RecParam.data.dRowHt) - RecParam.data.dNumAnnSize;	PtEnd.z		= 0.0;
				LINE(PtStart, PtEnd, RecParam.data.szLyrAxis);
			}
		}
		else
		{
			PtStart.x	= PtOrg.x;					PtStart.y	= PtOrg.y	+ (i * RecParam.data.dRowHt) - (RecParam.data.dNumAnnSize / 2.0);	PtStart.z = 0.0;
			PtEnd.x		= PtOrg.x + dTotalXAxisLen;	PtEnd.y		= PtOrg.y	+ (i * RecParam.data.dRowHt) - (RecParam.data.dNumAnnSize / 2.0);	PtEnd.z = 0.0;
			LINE(PtStart, PtEnd, RecParam.data.szLyrAxis);
			PtStart[X]	= PtOrg.x;					PtStart.y	= PtOrg.y	+ (i * RecParam.data.dRowHt) + (RecParam.data.dNumAnnSize / 2.0);	PtStart.z = 0.0;
			PtEnd[X]	= PtOrg.x + dTotalXAxisLen;	PtEnd.y		= PtOrg.y	+ (i * RecParam.data.dRowHt) + (RecParam.data.dNumAnnSize / 2.0);	PtEnd.z = 0.0;
			LINE(PtStart, PtEnd, RecParam.data.szLyrAxis);
		}
	}
	//Draw Axis..Vert Lines
	PtStart.x	= PtOrg.x;	PtStart.y	= PtOrg.y	+ (0.0 * RecParam.data.dRowHt);	PtStart.z = 0.0;
	PtEnd.x		= PtOrg.x;	PtEnd.y		= PtOrg.y	+ (iRowCount * RecParam.data.dRowHt);	PtEnd.z = 0.0;
	LINE(PtStart, PtEnd, RecParam.data.szLyrAxis);

	if(RecParam.data.bDrawNERows)
	{
		//Text Labels...
		//Cum Distance 
		PtStart.x	= PtOrg.x + (RecParam.data.dRowAnnSize / 2.0);	PtStart.y  = PtOrg.y	+ (0.5 * RecParam.data.dRowHt);	PtStart.z = 0.0;
		TEXTLEFT(PtStart, RecParam.data.szCumDistanceTag, 0.0, RecParam.data.dRowAnnSize, RecParam.data.szLyrAxis);
		//Easting 
		PtStart.x	= PtOrg.x + (RecParam.data.dRowAnnSize / 2.0);	PtStart.y  = PtOrg.y	+ (1.5 * RecParam.data.dRowHt);	PtStart.z = 0.0;
		TEXTLEFT(PtStart, RecParam.data.szEastingTag, 0.0, RecParam.data.dRowAnnSize, RecParam.data.szLyrAxis);
		//Northing 
		PtStart.x	= PtOrg.x + (RecParam.data.dRowAnnSize / 2.0);	PtStart.y  = PtOrg.y	+ (2.5 * RecParam.data.dRowHt);	PtStart.z = 0.0;
		TEXTLEFT(PtStart, RecParam.data.szNorthingTag, 0.0, RecParam.data.dRowAnnSize, RecParam.data.szLyrAxis);
		//Elev 
		PtStart.x	= PtOrg.x + (RecParam.data.dRowAnnSize / 2.0);	PtStart.y  = PtOrg.y	+ (3.5 * RecParam.data.dRowHt);	PtStart.z = 0.0;
		TEXTLEFT(PtStart, RecParam.data.szElevTag, 0.0, RecParam.data.dRowAnnSize, RecParam.data.szLyrAxis);
	}
	else
	{
		//Text Labels...
		//Cum Distance 
		PtStart.x	= PtOrg.x + (RecParam.data.dRowAnnSize / 2.0);	PtStart.y  = PtOrg.y	+ (0.5 * RecParam.data.dRowHt);	PtStart.z = 0.0;
		TEXTLEFT(PtStart, RecParam.data.szCumDistanceTag, 0.0, RecParam.data.dRowAnnSize, RecParam.data.szLyrAxis);
		//Elev 
		PtStart.x	= PtOrg.x + (RecParam.data.dRowAnnSize / 2.0);	PtStart.y  = PtOrg.y	+ (1.5 * RecParam.data.dRowHt);	PtStart.z = 0.0;
		TEXTLEFT(PtStart, RecParam.data.szElevTag, 0.0, RecParam.data.dRowAnnSize, RecParam.data.szLyrAxis);
	}
	{//Datum 
		char szTmp01[_MAX_PATH];
		char szTmp02[_MAX_PATH];
		
		PtStart.x	= PtOrg.x + (RecParam.data.dRowAnnSize / 2.0);	PtStart.y  = PtOrg.y + (iRowCount * RecParam.data.dRowHt) + (1.25 * RecParam.data.dRowAnnSize);	PtStart.z = 0.0;
		
		strcpy(szTmp01, RecParam.data.szDatumTag);
		ads_rtos (RecParam.data.dDatum, 2, 3, szTmp02);
		strcat(szTmp01, szTmp02);
		TEXTLEFT(PtStart, szTmp01, 0.0, RecParam.data.dRowAnnSize, RecParam.data.szLyrAxis);
	}
}
