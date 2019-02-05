//DrawContourLines.cpp
#include "StdAfx.h"
#include <Adscodes.h>
#include <math.h>
#include "DrawContourLines.h"
#include "DrawUtil.h"
#include "Util.h"
#include "DwgParamObj.h"
#include "TINData.h"
#include "ContourAnnotation.h"
#include "Line2PLine.h"
#include "ContourParamDlg.h"

//For Command Invocation
void CDrawContourLines::DelAllContours()
{
	DWGPARAMS DwgParams;

	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		AfxMessageBox("ERROR: Contour drawing parameters no set", MB_ICONSTOP);
		return;
	}
	DelAllContours(DwgParams, TRUE);
}
BOOL CDrawContourLines::DelAllContours(const DWGPARAMS& DwgParams, const BOOL& bPopWarning)
{
	if(AfxMessageBox("About to delete all existing contours\nAre you sure ?", MB_ICONWARNING|MB_YESNO) != IDYES)
		return FALSE;

	ads_printf("\nPlease wait... Deleting existing contour lines if any\n");
	DeleteProducedContourLines(DwgParams.szAppName);
	DeletePolyLines(DwgParams.szAppName);
	DeleteProducedTextEnts(DwgParams.szAppName);
	if(bPopWarning)
		ads_printf("\nAll existing contours deleted successfully");

	return TRUE;
}
void CDrawContourLines::DrawContour()
{
	DWGPARAMS DwgParams;
	AcGePoint3dArray ArrayPts;
	CTrianglePtrArray ArrayTrgs;
	
	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		AfxMessageBox("ERROR: Contour drawing parameters no set", MB_ICONSTOP);
		return;
	}
	if(!CTINData::GetTINData(ArrayPts, &ArrayTrgs))
	{//
		CString strMsg;
		strMsg.Format("ERROR: Triangulation not done yet\nPlease Run Triangulation & then try to draw contour");
		AfxMessageBox(strMsg, MB_ICONSTOP);
		return;
	}	////
	else
	{//////////////////////////////////////////////////////////////////////////////////////////
		DelAllContours(DwgParams, TRUE);

		DrawContour(ArrayPts, ArrayTrgs, DwgParams);
		if((DwgParams.fTxtHeight > 0.0) && (DwgParams.fTxtSpaMF > 0.0))
		{
			ads_printf("\nPlease wait... Annotating contour line(s)");
			CContourAnnotation::AnnotateContourLines(DwgParams);
		}
		//Deallocate Triangle data...
		CTINData::DeallocateTrgArray(ArrayTrgs);
	}//////////////////////////////////////////////////////////////////////////////////////////
}

void CDrawContourLines::DrawContour(const AcGePoint3dArray& Pt3DArray, const CTrianglePtrArray& TrgArray, const DWGPARAMS& DwgParams)
{
	int i;
	double dMinZ, dMaxZ;
	AcGePoint3dArray BoundaryPts;
	BOOL bUsrBrk;

	DIMZIN(0);//Don't suppress leading/trailing zeros

#ifdef _DEBUG
#endif //_DEBUG
	if(!GetSelClosedPLineVertexes(BoundaryPts, &bUsrBrk))
	{
		if(bUsrBrk)
		{
			ads_printf("\nUser break\n");
			return;
		}
		ads_printf("\nNo boundary selected! Drawing contour considering all contour points\n");
	}
	
	ads_printf("\n\rPlease wait....Calculating Elevation(Z) Range\n");
	GetElevRange(Pt3DArray, DwgParams, dMinZ, dMaxZ);

	for(i = 0; i < TrgArray.GetSize(); i++)
	{
		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break\n");
			break;
		}
		//Draw Contour Lines......
		DrawContourInTriangle(Pt3DArray, TrgArray[i], BoundaryPts, DwgParams, dMinZ, dMaxZ);
		ads_printf("\rPlease wait....Proccessing Triangle #%d of %d", i+1, TrgArray.GetSize());
	}
	ads_printf("\rDone....Total %d no of Triangles proccessed", i);
}
BOOL CDrawContourLines::GetAllContourLyrNames(const AcGePoint3dArray& Pt3DArray, const DWGPARAMS& DwgParams, const char& chSeparator, CString& strLyrs)
{
	double dMinZ, dMaxZ, dElev;

	GetElevRange(Pt3DArray, DwgParams, dMinZ, dMaxZ);
	dElev = GetNearestContourElev(dMinZ, DwgParams);
	dElev -= DwgParams.fContourInterval;
	for(strLyrs = ""; dElev <= dMaxZ; dElev += DwgParams.fContourInterval)
	{
		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break\n");
			return FALSE;
		}
		if(strlen(strLyrs) > 0)
			strLyrs = strLyrs + chSeparator + GetContourLyrName(dElev, DwgParams);
		else
			strLyrs = GetContourLyrName(dElev, DwgParams);
	}
	return (strlen(strLyrs) > 0);
}

//
void CDrawContourLines::DrawContLines(const AcGePoint3dArray& Pt3DArrayGenPts, const double& dElev, const DWGPARAMS& DwgParams, const short iColor /*= -1*/)
{
	int i;
	AcGePoint3d pt01, pt02;
	
	for(i = 1; i < Pt3DArrayGenPts.length(); i += 2)
	{
		pt01 = Pt3DArrayGenPts[i - 1]; pt01.z = dElev;
		pt02 = Pt3DArrayGenPts[i - 0]; pt02.z = dElev;
		DrawLineSegment(&pt01, &pt02, dElev, DwgParams, iColor);
	}
}
BOOL CDrawContourLines::GetContLinesInBoundary(const AcGePoint3dArray& Pt3DArrayBoundary, const AcGePoint3d& ptStart, const AcGePoint3d& ptEnd, AcGePoint3dArray& Pt3DArrayGenPts)
{
	int i;
	AcGePoint3dArray ptArrayRes;

	if(Pt3DArrayGenPts.length() > 0)
	{
		ads_printf("\nERROR: [GetContLinesInBoundary] -> Array Param is not empty!!!\n\n");
		return FALSE;
	}
	{//
		AcGePoint3d pt01, pt02, ptRes;
		BOOL bXSecFound;

		ptArrayRes.append(ptStart);
		for(i = 1, bXSecFound = FALSE; i < Pt3DArrayBoundary.length(); i++)
		{
			pt01 = Pt3DArrayBoundary[i - 1];
			pt02 = Pt3DArrayBoundary[i - 0];
			if(GetIntersPt2D(ptStart, ptEnd, pt01, pt02, ptRes))
			{
				ptArrayRes.append(ptRes);
				bXSecFound = TRUE;
			}
		}
		ptArrayRes.append(ptEnd);
		if(!bXSecFound)
		{
			if(IsPointWithinBoundary(ptStart, Pt3DArrayBoundary) || IsPointWithinBoundary(ptEnd, Pt3DArrayBoundary))
			{
				Pt3DArrayGenPts.append(ptStart);
				Pt3DArrayGenPts.append(ptEnd);
				return TRUE;
			}
			#ifdef _DEBUG
				{//
					const char* pszDEBUGLyr = "ABC";
					LAYER(pszDEBUGLyr, 3);
					LINE(ptStart, ptEnd, pszDEBUGLyr);
				}
			#endif//_DEBUG
			return FALSE;
		}
		else
		{
			SortPointsOnLine(ptStart, ptArrayRes);
			for(i = 1; i < ptArrayRes.length(); i++)
			{
				double fAngle;

				pt01 = ptArrayRes[i - 1];
				pt02 = ptArrayRes[i - 0];
				fAngle = Angle2D(pt01, pt02);

				ads_polar(asDblArray(pt01), fAngle, (Dist2D(pt01, pt02) / 2.0), asDblArray(ptRes));
				if(IsPointWithinBoundary(ptRes, Pt3DArrayBoundary))
				{
					Pt3DArrayGenPts.append(pt01);
					Pt3DArrayGenPts.append(pt02);
				}
			}
			if(Pt3DArrayGenPts.length() <= 0)
			{
				#ifdef _DEBUG
					ads_printf("\nERROR: [GetContLinesInBoundary] -> Not enough points found to draw controur Line!!\n");
				#endif//_DEBUG
				return FALSE;
			}
		}
	}
	return TRUE;
}
int CDrawContourLines::DrawUserSpecContourInTriangle(const AcGePoint3dArray& Pt3DArray, const TRIANGLE* pTrg, const AcGePoint3dArray& BoundaryPts, const DWGPARAMS& DwgParams, const double& fLevelParam)
{
	double dMinZ, dMaxZ, dOtherZ;
	AcGePoint3dArray Pt3DArrayTmp;

	//Extract fro global Point3D Ptr Array
	const AcGePoint3d Pt01 = Pt3DArray[pTrg->p1];
	const AcGePoint3d Pt02 = Pt3DArray[pTrg->p2];
	const AcGePoint3d Pt03 = Pt3DArray[pTrg->p3];

	//Create Temporary Ptr Array, Sort WRT. Z, Get the Min/Max Z Values
	Pt3DArrayTmp.append(Pt01);
	Pt3DArrayTmp.append(Pt02);
	Pt3DArrayTmp.append(Pt03);
	
	SortAcPoint3DArray(Pt3DArrayTmp, 2);

	const AcGePoint3d theFirstPt = Pt3DArrayTmp[0];
	const AcGePoint3d theSecondPt = Pt3DArrayTmp[1];
	const AcGePoint3d theThirdPt = Pt3DArrayTmp[2];

	dMinZ = theFirstPt.z;
	dOtherZ = theSecondPt.z;
	dMaxZ = theThirdPt.z;
	
	if((Pt01.z == Pt02.z) && (Pt02.z == Pt03.z) && (Pt03.z == Pt01.z))
	{//All Elevs are equal...
		return 1;//No need to draw....
	}
	if((Pt01.z > fLevelParam) && (Pt02.z > fLevelParam) && (Pt03.z > fLevelParam))
	{//All Elevs are greater than the elev specified
		return 1;//No need to draw....
	}
	if((Pt01.z < fLevelParam) && (Pt02.z < fLevelParam) && (Pt03.z < fLevelParam))
	{//All Elevs are smaller than the elev specified
		return 1;//No need to draw....
	}

	if((Pt01.z != Pt02.z) && (Pt02.z != Pt03.z) && (Pt03.z != Pt01.z))
	{//All Elevs are different...
		AcGePoint3d startPt, endPt;

		if((fLevelParam > theFirstPt.z) && (fLevelParam < theThirdPt.z))
		{
			if(fLevelParam <= theSecondPt.z)
			{
				if(!Interpolate(&theFirstPt, &theSecondPt, fLevelParam, startPt))
				{
					ads_printf("\n\nERROR: [DrawUserSpecContourInTriangle] 01 A");
					return 0;//ERROR!!!
				}
				if(!Interpolate(&theFirstPt, &theThirdPt, fLevelParam, endPt))
				{
					ads_printf("\n\nERROR: [DrawUserSpecContourInTriangle] 01 B");
					return 0;//ERROR!!!
				}
			}
			else
			{
				if(!Interpolate(&theFirstPt, &theThirdPt, fLevelParam, startPt))
				{
					ads_printf("\n\nERROR: [DrawUserSpecContourInTriangle] 01 C");
					return 0;//ERROR!!!
				}
				if(!Interpolate(&theSecondPt, &theThirdPt, fLevelParam, endPt))
				{
					ads_printf("\n\nERROR: [DrawUserSpecContourInTriangle] 01 D");
					return 0;//ERROR!!!
				}
			}
			{//Draw Entities
				if(startPt != endPt)
				{
					const short iColorID = 1;

					if(BoundaryPts.length() > 0)
					{//
						AcGePoint3dArray Pt3DArrayGenPts;

						GetContLinesInBoundary(BoundaryPts, startPt, endPt, Pt3DArrayGenPts);
						DrawContLines(Pt3DArrayGenPts, fLevelParam, DwgParams, iColorID);
					}
					else
					{
						DrawLineSegment(&startPt, &endPt, fLevelParam, DwgParams, iColorID);
					}

				}
			}
		}
		else
		{
			return 1;
		}
		return 1;
	}
	{//Two Elevs are same & the third one is diffrent....
		///////////////////////////////////////////
		if(theFirstPt.z == theSecondPt.z)
		{//Equal Elev(s) are minimum
			AcGePoint3d startPt, endPt;
			if((fLevelParam >= theFirstPt.z) && (fLevelParam <= theThirdPt.z))
			{
				if(!Interpolate(&theThirdPt, &theFirstPt, fLevelParam, startPt))
				{
					ads_printf("\n\nERROR: [DrawUserSpecContourInTriangle] 02 A");
					return 0;//ERROR!!!
				}
				if(!Interpolate(&theThirdPt, &theSecondPt, fLevelParam, endPt))
				{
					ads_printf("\n\nERROR: [DrawUserSpecContourInTriangle] 02 B");
					return 0;//ERROR!!!
				}
				{//Draw Entities
					if(startPt != endPt)
					{
						const short iColorID = 1;

						if(BoundaryPts.length() > 0)
						{//
							AcGePoint3dArray Pt3DArrayGenPts;

							GetContLinesInBoundary(BoundaryPts, startPt, endPt, Pt3DArrayGenPts);
							DrawContLines(Pt3DArrayGenPts, fLevelParam, DwgParams, iColorID);
						}
						else
						{
							DrawLineSegment(&startPt, &endPt, fLevelParam, DwgParams, iColorID);
						}
					}
				}
			}
			return 1;
		}
		else
		{//Equal Elev(s) are maxm
			AcGePoint3d startPt, endPt;
			if((fLevelParam > theFirstPt.z) && (fLevelParam <= theSecondPt.z) && (fLevelParam <= theThirdPt.z))
			{
				if(!Interpolate(&theFirstPt, &theSecondPt, fLevelParam, startPt))
				{
					ads_printf("\n\nERROR: [DrawUserSpecContourInTriangle] 03 A");
					return 0;//ERROR!!!
				}
				if(!Interpolate(&theFirstPt, &theThirdPt, fLevelParam, endPt))
				{
					ads_printf("\n\nERROR: [DrawUserSpecContourInTriangle] 03 B");
					return 0;//ERROR!!!
				}
				{//Draw Entities
					if(startPt != endPt)
					{
						const short iColorID = 1;

						if(BoundaryPts.length() > 0)
						{//
							AcGePoint3dArray Pt3DArrayGenPts;

							GetContLinesInBoundary(BoundaryPts, startPt, endPt, Pt3DArrayGenPts);
							DrawContLines(Pt3DArrayGenPts, fLevelParam, DwgParams, iColorID);
						}
						else
						{
							DrawLineSegment(&startPt, &endPt, fLevelParam, DwgParams, iColorID);
						}
					}
				}
			}
			return 1;
		}
	}
	ads_printf("\n\nERROR: [DrawUserSpecContourInTriangle] 07");
	return 0;
}

int CDrawContourLines::DrawContourInTriangle(const AcGePoint3dArray& Pt3DArray, const TRIANGLE* pTrg, const AcGePoint3dArray& BoundaryPts, const DWGPARAMS& DwgParams, const double& dMinZInDB, const double& dMaxZInDB)
{
	double dMinZ, dMaxZ, dOtherZ, dStartContourElev;
	AcGePoint3dArray Pt3DArrayTmp;

	//Extract fro global Point3D Ptr Array
	const AcGePoint3d Pt01 = Pt3DArray[pTrg->p1];
	const AcGePoint3d Pt02 = Pt3DArray[pTrg->p2];
	const AcGePoint3d Pt03 = Pt3DArray[pTrg->p3];

	//Create Temporary Ptr Array, Sort WRT. Z, Get the Min/Max Z Values
	Pt3DArrayTmp.append(Pt01);
	Pt3DArrayTmp.append(Pt02);
	Pt3DArrayTmp.append(Pt03);
	
	SortAcPoint3DArray(Pt3DArrayTmp, 2);

	const AcGePoint3d theFirstPt = Pt3DArrayTmp[0];
	const AcGePoint3d theSecondPt = Pt3DArrayTmp[1];
	const AcGePoint3d theThirdPt = Pt3DArrayTmp[2];

	dMinZ = theFirstPt.z;
	dOtherZ = theSecondPt.z;
	dMaxZ = theThirdPt.z;
	
	if((Pt01.z == Pt02.z) && (Pt02.z == Pt03.z) && (Pt03.z == Pt01.z))
	{//All Elevs are equal...
		return 1;//No need to draw....
	}

	dStartContourElev = GetNearestContourElev(dMinZ, DwgParams);

	if((Pt01.z != Pt02.z) && (Pt02.z != Pt03.z) && (Pt03.z != Pt01.z))
	{//All Elevs are different...
		AcGePoint3d startPt, endPt;
		for(;;)
		{
			if((dStartContourElev > theFirstPt.z) && (dStartContourElev < theThirdPt.z))
			{
				if(dStartContourElev <= theSecondPt.z)
				{
					if(!Interpolate(&theFirstPt, &theSecondPt, dStartContourElev, startPt))
					{
						ads_printf("\n\nERROR: [DrawContourInTriangle] 01 A");
						return 0;//ERROR!!!
					}
					if(!Interpolate(&theFirstPt, &theThirdPt, dStartContourElev, endPt))
					{
						ads_printf("\n\nERROR: [DrawContourInTriangle] 01 B");
						return 0;//ERROR!!!
					}
				}
				else
				{
					if(!Interpolate(&theFirstPt, &theThirdPt, dStartContourElev, startPt))
					{
						ads_printf("\n\nERROR: [DrawContourInTriangle] 01 C");
						return 0;//ERROR!!!
					}
					if(!Interpolate(&theSecondPt, &theThirdPt, dStartContourElev, endPt))
					{
						ads_printf("\n\nERROR: [DrawContourInTriangle] 01 D");
						return 0;//ERROR!!!
					}
				}
				{//Draw Entities
					if(startPt != endPt)
					{
						short iColorID;

						iColorID = GetColorID(DwgParams.iContourColorBase, dStartContourElev, dMinZInDB, dMaxZInDB);
						
						if(BoundaryPts.length() > 0)
						{//
							AcGePoint3dArray Pt3DArrayGenPts;

							GetContLinesInBoundary(BoundaryPts, startPt, endPt, Pt3DArrayGenPts);
							DrawContLines(Pt3DArrayGenPts, dStartContourElev, DwgParams, iColorID);
						}
						else
						{
							DrawLineSegment(&startPt, &endPt, dStartContourElev, DwgParams, iColorID);
						}

					}
				}
			}
			else
			{
				return 1;
			}
			dStartContourElev += DwgParams.fContourInterval;
		}
		return 1;
	}
	{//Two Elevs are same & the third one is diffrent....
		///////////////////////////////////////////
		if(theFirstPt.z == theSecondPt.z)
		{//Equal Elev(s) are minimum
			AcGePoint3d startPt, endPt;
			for(;;)
			{
				if((dStartContourElev >= theFirstPt.z) && (dStartContourElev <= theThirdPt.z))
				{
					if(!Interpolate(&theThirdPt, &theFirstPt, dStartContourElev, startPt))
					{
						ads_printf("\n\nERROR: [DrawContourInTriangle] 02 A");
						return 0;//ERROR!!!
					}
					if(!Interpolate(&theThirdPt, &theSecondPt, dStartContourElev, endPt))
					{
						ads_printf("\n\nERROR: [DrawContourInTriangle] 02 B");
						return 0;//ERROR!!!
					}
					{//Draw Entities
						if(startPt != endPt)
						{
							short iColorID;

							iColorID = GetColorID(DwgParams.iContourColorBase, dStartContourElev, dMinZInDB, dMaxZInDB);
							if(BoundaryPts.length() > 0)
							{//
								AcGePoint3dArray Pt3DArrayGenPts;

								GetContLinesInBoundary(BoundaryPts, startPt, endPt, Pt3DArrayGenPts);
								DrawContLines(Pt3DArrayGenPts, dStartContourElev, DwgParams, iColorID);
							}
							else
							{
								DrawLineSegment(&startPt, &endPt, dStartContourElev, DwgParams, iColorID);
							}
						}
					}
				}
				else
				{
					return 1;
				}
				dStartContourElev += DwgParams.fContourInterval;
			}		
			return 1;
		}
		else
		{//Equal Elev(s) are maxm
			AcGePoint3d startPt, endPt;
			for(;;)
			{
				if((dStartContourElev > theFirstPt.z) && (dStartContourElev <= theSecondPt.z) && (dStartContourElev <= theThirdPt.z))
				{
					if(!Interpolate(&theFirstPt, &theSecondPt, dStartContourElev, startPt))
					{
						ads_printf("\n\nERROR: [DrawContourInTriangle] 03 A");
						return 0;//ERROR!!!
					}
					if(!Interpolate(&theFirstPt, &theThirdPt, dStartContourElev, endPt))
					{
						ads_printf("\n\nERROR: [DrawContourInTriangle] 03 B");
						return 0;//ERROR!!!
					}
					{//Draw Entities
						if(startPt != endPt)
						{
							short iColorID;

							iColorID = GetColorID(DwgParams.iContourColorBase, dStartContourElev, dMinZInDB, dMaxZInDB);
							if(BoundaryPts.length() > 0)
							{//
								AcGePoint3dArray Pt3DArrayGenPts;

								GetContLinesInBoundary(BoundaryPts, startPt, endPt, Pt3DArrayGenPts);
								DrawContLines(Pt3DArrayGenPts, dStartContourElev, DwgParams, iColorID);
							}
							else
							{
								DrawLineSegment(&startPt, &endPt, dStartContourElev, DwgParams, iColorID);
							}
						}
					}
				}
				else
				{
					return 1;
				}
				dStartContourElev += DwgParams.fContourInterval;
			}
			return 1;
		}
	}
	ads_printf("\n\nERROR: [DrawContourInTriangle] 07");
	return 0;
}
CString CDrawContourLines::GetContourLyrName(const double& dElev, const DWGPARAMS& DwgParams)
{
	CString strLyrName;

	strLyrName = DwgParams.szElevLyrPrefix;
	strLyrName = strLyrName + ConvertToElevStr(dElev, DwgParams.chDotSeparator);
	return strLyrName;
}

void CDrawContourLines::DrawLineSegment(const AcGePoint3d* pPt1, const AcGePoint3d* pPt2, const double& dElev, const DWGPARAMS& DwgParams, const short iColor /*= -1*/)
{
	AcGePoint3d Pt1, Pt2;
	CString strLyrName;

	Pt1 = *pPt1;	Pt1.z = dElev;
	Pt2 = *pPt2;	Pt2.z = dElev;

	strLyrName = GetContourLyrName(dElev, DwgParams);
	LAYER(strLyrName, iColor);
	LINE(Pt1, Pt2, strLyrName);
	{//Add XData...
		ads_name objName;
		if(ads_entlast(objName) == RTNORM)
		{
			AddXDataForContourLine(objName, DwgParams.szAppName, &dElev);
		}
	}
}
CString CDrawContourLines::ConvertToElevStr(const double& dElev, const char chDotReplacer)
{
	int i, iIndex;
	CString strParam;
	
	strParam.Format("%.6f", dElev);
	iIndex = strParam.Find('.');
	if(iIndex < 0)
		return strParam;

	for(i = 0; i < (int)strlen(strParam); i++)
	{
		if(i == iIndex)
		{
			strParam.SetAt(i, chDotReplacer);
			break;
		}
	}
	//Remove Trailing Zeros....
	for(i = strlen(strParam) - 1; i >= 0; i--)
	{
		if(strParam[i] != '0')
		{
			strParam = strParam.Mid(0, i+1);
			return strParam;
		}
	}
	return strParam;
}
void CDrawContourLines::GetElevRange(const AcGePoint3dArray& Pt3DArray, const DWGPARAMS& DwgParams, double& dMinZ, double& dMaxZ)
{
	int i;

	for(i = 0; i < Pt3DArray.length(); i++)
	{
		if(i == 0)
		{
			dMinZ = Pt3DArray[i].z;
			dMaxZ = Pt3DArray[i].z;
		}
		else
		{
			if(Pt3DArray[i].z < dMinZ)
				dMinZ = Pt3DArray[i].z;
			if(Pt3DArray[i].z > dMaxZ)
				dMaxZ = Pt3DArray[i].z;
		}
	}
	dMinZ = GetNearestContourElev(dMinZ, DwgParams);
	dMaxZ = GetNearestContourElev(dMaxZ, DwgParams);
}
short CDrawContourLines::GetColorID(const short& iBaseColor, const double& dElev, const double& dMinZ, const double& dMaxZ)
{
	const short iFirstColor = 1;
	const short iLastColor = 254;
	short lColGroups, lColID;

	if(iBaseColor < 1)
	{
		return 1;
	}
	else
	{
		lColGroups = (short)(iLastColor - iBaseColor); 
		{
			double dTmpA, dTmpB;

			if(dElev > dMaxZ)
				dTmpA = dMaxZ - dMinZ;
			else
				dTmpA = dElev - dMinZ;
			
			dTmpB = dMaxZ - dMinZ;

			lColID = (short)(lColGroups * (dTmpA / dTmpB));
			lColID = lColID + (short)iBaseColor;
		}
	}
	if((lColID <= 0) || (lColID >= 255))
	{
		ads_alert("Warning : Invalid color ID calculated!!!\nPlease Contact Developer");
		lColID = 1;
	}
	return lColID;
}

double CDrawContourLines::GetNearestContourElev(const double& dElev, const DWGPARAMS& DwgParams)
{
	double dContourElev;

	dContourElev = fmod(dElev, DwgParams.fContourInterval);
	if(dContourElev == 0.0)
	{
		dContourElev = dElev + DwgParams.fContourInterval;
	}
	else
	{
		if(dContourElev > 0.0)
		{
			dContourElev = dElev - dContourElev;
			dContourElev += DwgParams.fContourInterval;
		}
		else
		{
			dContourElev = dElev - dContourElev;
		}
	}
	return dContourElev;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//for Testing Purpose Only.....
////////////////////////////////////////////////////////////////////////////////////////////////
void CDrawContourLines::TestDrawContourInTriangle()
{
#ifndef _DEBUG
	AfxMessageBox("This utility is for internal use", MB_ICONINFORMATION);
	return;
#endif //_DEBUG

	AcGePoint3dArray Pt3DArray, BoundaryPts;
	TRIANGLE Trg;
	DWGPARAMS DwgParams;
	double dMinZInDB, dMaxZInDB;
	ads_point PtResult;

	if(RTNORM != ads_getpoint(0L, "\nEnter point <1> :", PtResult))
	{
		return;
	}
	Pt3DArray.append(asPnt3d(PtResult));

	if(RTNORM != ads_getpoint(0L, "\nEnter point <2> :", PtResult))
	{
		return;
	}
	Pt3DArray.append(asPnt3d(PtResult));

	if(RTNORM != ads_getpoint(0L, "\nEnter point <3> :", PtResult))
	{
		return;
	}
	Pt3DArray.append(asPnt3d(PtResult));

	Trg.p1 = 0;
	Trg.p2 = 1;
	Trg.p3 = 2;

	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		ads_printf("\nERROR: Contour drawing parameters not set yet\n");
		return;
	}
	GetElevRange(Pt3DArray, DwgParams, dMinZInDB, dMaxZInDB);
	DrawContourInTriangle(Pt3DArray, &Trg, BoundaryPts, DwgParams, dMinZInDB, dMaxZInDB);
}

BOOL CDrawContourLines::GetSelClosedPLineVertexes(AcGePoint3dArray& Array, BOOL* pbUsrBrk /* = 0L*/)
{
	const char* pszPmt = "\nSelect bounded (closed) polyline for contour boundary definition :";
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
void CDrawContourLines::DrawUserSpecContour()
{
	DWGPARAMS DwgParams;
	AcGePoint3dArray ArrayPts;
	CTrianglePtrArray ArrayTrgs;
	double fLevelParam = 0.0;
	
	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		AfxMessageBox("ERROR: Contour drawing parameters no set", MB_ICONSTOP);
		return;
	}
	if(!CTINData::GetTINData(ArrayPts, &ArrayTrgs))
	{//
		CString strMsg;
		strMsg.Format("ERROR: Triangulation not done yet\nPlease Run Triangulation & then try to draw contour");
		AfxMessageBox(strMsg, MB_ICONSTOP);
		return;
	}	////
	else
	{//////////////////////////////////////////////////////////////////////////////////////////
		DIMZIN(0);//Don't suppress leading/trailing zeros
		if(ads_getreal("\nEnter level for which Contour to be drawn :", &fLevelParam) != RTNORM)
		{
			ads_printf("\nUser break encountered\n");
		}
		else
		{
			DelAllContours(DwgParams, TRUE);

			DrawSingleContour(ArrayPts, ArrayTrgs, DwgParams, fLevelParam);

			//DISABLED --- AutoAnnotation -------------
			//if((DwgParams.fTxtHeight > 0.0) && (DwgParams.fTxtSpaMF > 0.0))
			//{
			//	ads_printf("\nPlease wait... Annotating contour line(s)");
			//	AnnotateContourLines(DwgParams);
			//}
		}
		//Deallocate Triangle data...
		CTINData::DeallocateTrgArray(ArrayTrgs);
	}//////////////////////////////////////////////////////////////////////////////////////////
}



void CDrawContourLines::DrawSingleContour(const AcGePoint3dArray& Pt3DArray, const CTrianglePtrArray& TrgArray, const DWGPARAMS& DwgParams, const double& fLevelParam)
{
	int i;
	AcGePoint3dArray BoundaryPts;
	BOOL bUsrBrk;

	if(!GetSelClosedPLineVertexes(BoundaryPts, &bUsrBrk))
	{
		if(bUsrBrk)
		{
			ads_printf("\nUser break encountered\n");
			return;
		}
		ads_printf("\nNo boundary selected! Drawing contour considering all contour points\n");
	}
	
	for(i = 0; i < TrgArray.GetSize(); i++)
	{
		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break encountered\n");
			break;
		}

		//Draw Contour Lines......
		DrawUserSpecContourInTriangle(Pt3DArray, TrgArray[i], BoundaryPts, DwgParams, fLevelParam);
		ads_printf("\rPlease wait....Proccessing Triangle #%d of %d", i+1, TrgArray.GetSize());
	}
	ads_printf("\rDone....Total %d no of Triangles proccessed", i);
}

void CDrawContourLines::CompactContourLines()
{
	AcGePoint3dArray ArrayPts;
	DWGPARAMS DwgParams;
	
	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		AfxMessageBox("ERROR: Contour drawing parameters no set", MB_ICONSTOP);
		return;
	}
	if(!CTINData::GetTINData(ArrayPts))
	{//
		CString strMsg;
		strMsg.Format("ERROR: Triangulation not done yet\nPlease Run Triangulation & then try to draw contour");
		AfxMessageBox(strMsg, MB_ICONSTOP);
		return;
	}	////
	else
	{//////////////////////////////////////////////////////////////////////////////////////////
		CString strLyrNames;
		const char chLyrNameSeparator = '\t';

		if(GetAllContourLyrNames(ArrayPts, DwgParams, chLyrNameSeparator, strLyrNames))
		{
			CLine2PLine::ContourLines2PLine(DwgParams.szAppName, chLyrNameSeparator, strLyrNames);
		}
	}//////////////////////////////////////////////////////////////////////////////////////////
}
void CDrawContourLines::ContourSetParam()
{
	DWGPARAMS DwgParams;
	
	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		ads_printf("\nSetting Contour Parameter(s) first time...");
		if(!CDwgParamObj::CreateDwgParamObj())
		{
			ads_printf("\nFATAL ERROR: Failed to attach drawing parameter data. Drawing file may be corrupted !");
			return;
		}
	}
	//..............................................................................
	if(CContourParamDlg::GetContourParam(DwgParams))
	{
		if(!CDwgParamObj::CreateDwgParamObj(&DwgParams))
		{
			ads_printf("\nFATAL ERROR: Failed to attach drawing parameter data. Drawing file may be corrupted !");
			return;
		}
		ads_printf("\nContour Parameter(s) set successfully");
	}
}
