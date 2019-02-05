// ImportPts.cpp
/////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <Adscodes.h>
#include <DbEnts.h>
#include <Geassign.h>
#include <math.h>
#include "ImportPts.h"
#include "DrawUtil.h"
#include "Util.h"
#include "GenPtsDlg.h"
#include "resourcehelper.h"
#include "DwgParamObj.h"
#include "ContourParamDlg.h"
#include "GenPtsFrMultiSegDlg.h"
#include "ResultDlg.h"
#include "ReadPtData.h"


int CImportPtsUtil::ImportPointsIntoDWG(const CPointDataPtrArray& Array, const DWGPARAMS& DwgParams)
{
	int i;
	POINTDATA* pRec;
	BOOL bSpLevFlag;
	
	LAYER(DwgParams.szPointLyr);
	for(i = 0, bSpLevFlag = TRUE; i < Array.GetSize(); i++)
	{
		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break encountered\n");
			break;
		}
		pRec = Array[i];
	
		DRAWPOINT(pRec->pt, DwgParams.szPointLyr);
		{//Add XData...
			ads_name objName;
			if(ads_entlast(objName) == RTNORM)
			{
				AddXDataForPoint(objName, DwgParams.szAppName, pRec->szDesc);
			}
		}
		//Draw Spot Level(s)............
		if((DwgParams.fSpotLevTxtHt > 0.0) && bSpLevFlag)
		{
			if(!DrawSpotLevel(pRec->pt, DwgParams.szSpotLevBlock, DwgParams.szSpotLevLyr, DwgParams.fSpotLevRotAngle, DwgParams.fSpotLevTxtHt, DwgParams.fSpotLevBlkRelScale))
			{
				ads_printf("\nERROR: Failed to draw spot level(s)\n");
				bSpLevFlag = FALSE;
			}
		}
		ads_printf("\rPlease wait..Importing point #%d", (i+1));
	}
	ads_printf("\nTotal %d # of points imported successfully", i);
	return i;
}
//
//Return Values...
//(iRet >=  0)	->> No of points got
//(iRet == -1)	->> Some internal error
//(iRet == -2)	->> User termination
//
int CImportPtsUtil::GetContourPointsFromDWG(AcGePoint3dArray& Array, const char* pszAppName, const BOOL& bChkForDuplicatePt, AcDbObjectIdArray* pDupObjIdArray /*= 0L*/, AcGePoint3dArray* pArrayRemovedPts /*= 0L*/)
{
    struct  resbuf  Rb01, Rb02, Rb03;
    const char szEntName[] = "POINT";
	ads_name SelSet;
	long iNoOfEnt;

	if(pDupObjIdArray != 0L)
	{
		if(pDupObjIdArray->length() != 0)
			return -1;//ERROR
		if(!bChkForDuplicatePt)
			return -1;//ERROR
	}
	if(pArrayRemovedPts != 0L)
	{
		if(pArrayRemovedPts->length() != 0)
			return -1;//ERROR
	}
	if(Array.length() != 0)
		return -1;//ERROR

	// Entity type
	Rb01.restype = 0;
	Rb01.resval.rstring = (char*)szEntName;
	Rb01.rbnext = &Rb02; 
	//Extended entity data 
	Rb02.restype = -3;
	Rb02.rbnext = &Rb03; 
	//application Name
	Rb03.restype = 1001; 
	Rb03.resval.rstring = (char*)pszAppName; 
	Rb03.rbnext = NULL; 
	
	if(ads_ssget("X", 0L, 0L, &Rb01, SelSet) != RTNORM)
	{
		return 0;//ERROR
	}
	iNoOfEnt = 0;
	if(ads_sslength(SelSet, &iNoOfEnt) != RTNORM)
	{
		return -1;//ERROR
	}
	iNoOfEnt = ExtractNChkDuplicatePts(SelSet, Array, bChkForDuplicatePt, pDupObjIdArray, pArrayRemovedPts);
	ads_ssfree(SelSet);//Free Selset
	return iNoOfEnt;
}
//
//Return Values...
//(iRet >=  0)	->> No of points got
//(iRet == -1)	->> Some internal error
//(iRet == -2)	->> User termination
//
int CImportPtsUtil::GetPointsForTINGeneration(AcGePoint3dArray& Array, const char* pszAppName, const BOOL& bChkForDuplicatePt, AcDbObjectIdArray* pDupObjIdArray /*= 0L*/, AcGePoint3dArray* pArrayRemovedPts /*= 0L*/)
{
    struct  resbuf  Rb01, Rb02, Rb03;
    const char szEntName[] = "POINT";
	ads_name SelSet;
	long iNoOfEnt;

	if(pDupObjIdArray != 0L)
	{
		if(pDupObjIdArray->length() != 0)
			return -1;//ERROR
		if(!bChkForDuplicatePt)
			return -1;//ERROR
	}
	if(pArrayRemovedPts != 0L)
	{
		if(pArrayRemovedPts->length() != 0)
			return -1;//ERROR
	}
	if(Array.length() != 0)
		return -1;//ERROR

	// Entity type
	Rb01.restype = 0;
	Rb01.resval.rstring = (char*)szEntName;
	Rb01.rbnext = &Rb02; 
	//Extended entity data 
	Rb02.restype = -3;
	Rb02.rbnext = &Rb03; 
	//application Name
	Rb03.restype = 1001; 
	Rb03.resval.rstring = (char*)pszAppName; 
	Rb03.rbnext = NULL; 
	
	if(ads_ssget(0L, 0L, 0L, &Rb01, SelSet) != RTNORM)
	{
		return 0;//ERROR
	}
	iNoOfEnt = 0;
	if(ads_sslength(SelSet, &iNoOfEnt) != RTNORM)
	{
		return -1;//ERROR
	}
	iNoOfEnt = ExtractNChkDuplicatePts(SelSet, Array, bChkForDuplicatePt, pDupObjIdArray, pArrayRemovedPts);
	ads_ssfree(SelSet);//Free Selset
	return iNoOfEnt;
}
//Return Values...
//(iRet >=  0)	->> No of points got
//(iRet == -1)	->> Some internal error
//(iRet == -2)	->> User termination
//
int CImportPtsUtil::ExtractNChkDuplicatePts(ads_name& SelSet, AcGePoint3dArray& Array, const BOOL& bChkForDuplicatePt, AcDbObjectIdArray* pDupObjIdArray /*= 0L*/, AcGePoint3dArray* pArrayRemovedPts /*= 0L*/)
{
	long iNoOfEnt;

	iNoOfEnt = 0;
	if(ads_sslength(SelSet, &iNoOfEnt) != RTNORM)
	{
		return -1;//ERROR
	}
	{//Extract Data points....
		long i, iIndex;
		BOOL bIsUserCancelled;
		ads_name Entity;
		Acad::ErrorStatus ErrCode;
		AcDbObjectId objId;
		//AcDbPoint* pObj;
		AcDbObject* pObj;

		ads_printf("\n");
		for(i = 0, bIsUserCancelled = FALSE; i < iNoOfEnt; i++)
		{
			if(ads_usrbrk() == 1)
			{
				ads_printf("\nUser break encountered\n");
				return -2;
			}
			ads_printf("\rPlease wait...Extracting data from point #%d of %d", i+1, iNoOfEnt);

			if(ads_ssname(SelSet, i, Entity) != RTNORM)
			{
				ads_printf("\nERROR: AutoCAD System error !\n");
				return -1;
			}
			ErrCode = acdbGetObjectId(objId, Entity);
			if(ErrCode != Acad::eOk)
			{
				return -1;
			}
			ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
			if(ErrCode != Acad::eOk)
			{
				return -1;
			}
			if(pObj->isA() != AcDbPoint::desc())
			{
				pObj->close();
				return -1;
			}
			if(bChkForDuplicatePt)
			{
				if(!FindOccurance(((AcDbPoint*)pObj)->position(), Array, iIndex, bIsUserCancelled, FALSE))
				{
					if(!bIsUserCancelled)
						Array.append(((AcDbPoint*)pObj)->position());
				}
				if(bIsUserCancelled)
				{
					pObj->close();
					return -2;//User Termination
				}
				if((iIndex >= 0) && !bIsUserCancelled)
				{
					if(pDupObjIdArray != 0L)
					{
						pDupObjIdArray->append(objId);
					}
					else
					{
						ads_printf("\nWarning: Duplicate point (%f, %f, %f) ignored!\n", ((AcDbPoint*)pObj)->position().x, ((AcDbPoint*)pObj)->position().y, ((AcDbPoint*)pObj)->position().z);
					}
					if(pArrayRemovedPts != 0L)
					{
						pArrayRemovedPts->append(((AcDbPoint*)pObj)->position());
					}
				}
			}
			else
			{
				Array.append(((AcDbPoint*)pObj)->position());
			}
			pObj->close();
		}
	}
	//
	if(pDupObjIdArray == 0L)
		ads_printf("\nTotal %d# of points extracted", iNoOfEnt);

	return iNoOfEnt;
}

//
//Generates points interpolating two user specified contour compatible points
//
void CImportPtsUtil::GenPoints()
{
	DWGPARAMS DwgParams;

	if(!GetDwgParam(DwgParams))
		return;

	DIMZIN(0);//Don't suppress leading/trailing zeros
	GeneratePoints(DwgParams);
}

BOOL CImportPtsUtil::GeneratePoints(const DWGPARAMS& DwgParams)
{
	ads_name entName;
	AcGePoint3d ptStart, ptEnd;
	int iNoOfPts;

	{//First Entity...
		const char* pszPmt = "\nSelect first point for interpolation :";
		ads_point ptRes;
		AcDbObjectId objId;
		AcDbObject* pObj;
		Acad::ErrorStatus ErrCode;

		if(RTNORM != ads_entsel(pszPmt, entName, ptRes))
		{
			ads_printf("\nERROR: First point not selected\n");
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
		if(pObj->isA() != AcDbPoint::desc())
		{
			ads_printf("\nERROR: Entity selected is not a Point\n");
			pObj->close();
			return FALSE;
		}
		{//Chk whether native entity ...
			resbuf* pResBuf;
		    
			pResBuf = pObj->xData(DwgParams.szAppName);
			if(pResBuf == 0L)
			{
				ads_printf("\nERROR: Point selected is not compatible with contour application\n");
				pObj->close();
				return FALSE;
			}
			ads_relrb(pResBuf);
		}
		//Extract point...
		ptStart = ((AcDbPoint*)pObj)->position();
 		pObj->close();
	}
	{//Second Entity...
		const char* pszPmt = "\nSelect second point for interpolation :";
		ads_point ptRes;
		AcDbObjectId objId;
		AcDbObject* pObj;
		Acad::ErrorStatus ErrCode;

		if(RTNORM != ads_entsel(pszPmt, entName, ptRes))
		{
			ads_printf("\nERROR: Second point not selected\n");
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
		if(pObj->isA() != AcDbPoint::desc())
		{
			ads_printf("\nERROR: Entity selected is not a Point\n");
			pObj->close();
			return FALSE;
		}
		{//Chk whether native entity ...
			resbuf* pResBuf;
		    
			pResBuf = pObj->xData(DwgParams.szAppName);
			if(pResBuf == 0L)
			{
				ads_printf("\nERROR: Point selected is not compatible with contour application\n");
				pObj->close();
				return FALSE;
			}
			ads_relrb(pResBuf);
		}
		//Extract point...
		ptEnd = ((AcDbPoint*)pObj)->position();
 		pObj->close();
	}
	if(((ptEnd.x == ptStart.x) && (ptEnd.y == ptStart.y)) || (ptEnd == ptStart))
	{
		ads_printf("\nERROR: Interpolation is not possible with the selected points\n");
		return FALSE;
	}
	{//Get #of segments using GUI
		double fDist;
		fDist = Dist2D(ptStart, ptEnd);
		{//
			CTemporaryResourceOverride Res;
			CGenPtsDlg Dlg(TRUE, fDist, fabs(ptStart.z - ptEnd.z));
			
			if(Dlg.DoModal() != IDOK)
				return FALSE;

			iNoOfPts = Dlg.GetAttrib();
		}
	}
	{//Interpoaltion.....
		const char* pszComments = "Interpolated";
		double dDist, dElev, dAngle;
		ads_point ptResult;
		int i;
		BOOL bSpLevFlag;

		dElev	= (ptEnd.z - ptStart.z);
		dDist	= Dist2D(ptStart, ptEnd) / (iNoOfPts + 1);
		dAngle	= Angle2D(ptStart, ptEnd);

		LAYER(DwgParams.szPointLyr);
		for(i = 1, bSpLevFlag = TRUE; i <= iNoOfPts; i++)
		{
			ads_polar(asDblArray(ptStart), dAngle, dDist * i, ptResult);
			ptResult[Z] = ptStart.z + (i * dElev) / (iNoOfPts + 1);

			//Draw...
			DRAWPOINT(asPnt3d(ptResult), DwgParams.szPointLyr);
			{//Add XData...
				ads_name objName;
				if(ads_entlast(objName) == RTNORM)
				{
					AddXDataForPoint(objName, DwgParams.szAppName, pszComments);
				}
			}
			//Draw Spot Level(s)............
			if((DwgParams.fSpotLevTxtHt > 0.0) && bSpLevFlag)
			{
				if(!DrawSpotLevel(asPnt3d(ptResult), DwgParams.szSpotLevBlock, DwgParams.szSpotLevLyr, DwgParams.fSpotLevRotAngle, DwgParams.fSpotLevTxtHt, DwgParams.fSpotLevBlkRelScale))
				{
					ads_printf("\nERROR: Failed to draw spot level(s)\n");
					bSpLevFlag = FALSE;
				}
			}
			ads_printf("\rPlease wait..Generating point #%d", (i+1));
		}
		ads_printf("\nTotal %d # of points generated successfully", i);
	}
	return TRUE;
}
void CImportPtsUtil::AddPoints()
{//Adds points from current dwg....
	DWGPARAMS DwgParams;

	if(!GetDwgParam(DwgParams))
	{
		return;
	}
	DIMZIN(0);//Don't suppress leading/trailing zeros
	MakeContourCompatiblePoints(DwgParams);
}
//
//Converts AutoCAD General points into contour compatible points
//
BOOL CImportPtsUtil::MakeContourCompatiblePoints(const DWGPARAMS& DwgParams)
{
	ads_name SelSet;
	
	{//Get all the points....
		struct  resbuf  Rb01;
		const char szEntName[] = "POINT";
		// Entity type
		Rb01.restype = 0;
		Rb01.resval.rstring = (char*)szEntName;
		Rb01.rbnext = 0L;
		
		if(ads_ssget(0L, 0L, 0L, &Rb01, SelSet) != RTNORM)
		{
			return FALSE;
		}
	}
	{//
		const char* pszXDataStr = "Imported";
		ads_name Entity;
		long i, iCount, iNoOfEnt;
		BOOL bSpLevFlag;

		if(ads_sslength(SelSet, &iNoOfEnt) != RTNORM)
		{
			ads_ssfree(SelSet);//Free Selset
			return FALSE;//ERROR
		}
		for(i = iCount = 0, bSpLevFlag = TRUE; i < iNoOfEnt; i++)
		{
			if(ads_usrbrk() == 1)
			{
				ads_printf("\nUser break encountered\n");
				ads_ssfree(SelSet);//Free Selset
				return FALSE;
			}
			if(ads_ssname(SelSet, i, Entity) != RTNORM)
			{
				ads_printf("\nERROR: AutoCAD System error !\n");
				ads_ssfree(SelSet);//Free Selset
				return FALSE;
			}
			{//Chk if Contour Compatible Point...
				AcDbObjectId objId;
				AcDbObject* pObj;
				Acad::ErrorStatus ErrCode;

				ErrCode = acdbGetObjectId(objId, Entity);
				if(ErrCode != Acad::eOk)
				{
					ads_ssfree(SelSet);//Free Selset
					ads_printf("\nERROR: AutoCAD System error !\n");
					return FALSE;
				}
				ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
				if(ErrCode != Acad::eOk)
				{
					ads_ssfree(SelSet);//Free Selset
					ads_printf("\nERROR: AutoCAD System error !\n");
					return FALSE;
				}
				{//Chk whether native entity ...
					resbuf* pResBuf;
					
					pResBuf = pObj->xData(DwgParams.szAppName);
					pObj->close();
					if(pResBuf == 0L)
					{//Not a native entity... so add XData
						AddXDataForPoint(Entity, DwgParams.szAppName, pszXDataStr);
						iCount++;
						//Draw Spot Level(s)............
						if((DwgParams.fSpotLevTxtHt > 0.0) && bSpLevFlag)
						{
							if(!DrawSpotLevel(((AcDbPoint*)pObj)->position(), DwgParams.szSpotLevBlock, DwgParams.szSpotLevLyr, DwgParams.fSpotLevRotAngle, DwgParams.fSpotLevTxtHt, DwgParams.fSpotLevBlkRelScale))
							{
								ads_printf("\nERROR: Failed to draw spot level(s)\n");
								bSpLevFlag = FALSE;
							}
						}
						ads_printf("\rPlease wait..Importing point #%d", iCount);
					}
					ads_relrb(pResBuf);
				}
				pObj->close();
			}
		}
		ads_printf("\nTotal %d # of points imported", iCount);
	}
	return TRUE;
}
void CImportPtsUtil::GenHardBreakLine()
{
	DWGPARAMS DwgParams;

	if(!GetDwgParam(DwgParams))
		return;
	DIMZIN(0);//Don't suppress leading/trailing zeros
	GenerateHardBreakLine(DwgParams);
}

//
//Generates points of equal elevation for HardBreak Line
//Input : two(2) user specified contour compatible points
//
BOOL CImportPtsUtil::GenerateHardBreakLine(const DWGPARAMS& DwgParams)
{
	ads_name entName;
	AcGePoint3d ptStart, ptEnd;
	int iNoOfPts;
	double dElev;

	{//First Entity...
		const char* pszPmt = "\nSelect first point for hard break line :";
		ads_point ptRes;
		AcDbObjectId objId;
		AcDbObject* pObj;
		Acad::ErrorStatus ErrCode;

		if(RTNORM != ads_entsel(pszPmt, entName, ptRes))
		{
			ads_printf("\nERROR: First point not selected\n");
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
		if(pObj->isA() != AcDbPoint::desc())
		{
			ads_printf("\nERROR: Entity selected is not a Point\n");
			pObj->close();
			return FALSE;
		}
		{//Chk whether native entity ...
			resbuf* pResBuf;
		    
			pResBuf = pObj->xData(DwgParams.szAppName);
			if(pResBuf == 0L)
			{
				ads_printf("\nERROR: Point selected is not compatible with contour application\n");
				pObj->close();
				return FALSE;
			}
			ads_relrb(pResBuf);
		}
		//Extract point...
		ptStart = ((AcDbPoint*)pObj)->position();
 		pObj->close();
	}
	{//Second Entity...
		const char* pszPmt = "\nSelect second point for hard break line :";
		ads_point ptRes;
		AcDbObjectId objId;
		AcDbObject* pObj;
		Acad::ErrorStatus ErrCode;

		if(RTNORM != ads_entsel(pszPmt, entName, ptRes))
		{
			ads_printf("\nERROR: Second point not selected\n");
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
		if(pObj->isA() != AcDbPoint::desc())
		{
			ads_printf("\nERROR: Entity selected is not a Point\n");
			pObj->close();
			return FALSE;
		}
		{//Chk whether native entity ...
			resbuf* pResBuf;
		    
			pResBuf = pObj->xData(DwgParams.szAppName);
			if(pResBuf == 0L)
			{
				ads_printf("\nERROR: Point selected is not compatible with contour application\n");
				pObj->close();
				return FALSE;
			}
			ads_relrb(pResBuf);
		}
		//Extract point...
		ptEnd = ((AcDbPoint*)pObj)->position();
 		pObj->close();
	}
	if(((ptEnd.x == ptStart.x) && (ptEnd.y == ptStart.y)) || (ptEnd == ptStart))
	{
		ads_printf("\nERROR: Point generation is not possible with the selected points\n");
		return FALSE;
	}
	{//Get #of segments using GUI
		double fDist;
		fDist = Dist2D(ptStart, ptEnd);
		{//
			CTemporaryResourceOverride Res;
			CGenPtsDlg Dlg(FALSE, fDist, fabs(ptStart.z - ptEnd.z));
			
			if(Dlg.DoModal() != IDOK)
				return FALSE;

			iNoOfPts = Dlg.GetAttrib(&dElev);
		}
	}
	{//Generation.........
		const char* pszComments = "HardBreak";
		double dDist, dAngle;
		ads_point ptResult;
		int i;
		BOOL bSpLevFlag;

		dDist	= Dist2D(ptStart, ptEnd) / (iNoOfPts + 1);
		dAngle	= Angle2D(ptStart, ptEnd);

		LAYER(DwgParams.szPointLyr);
		for(i = 1, bSpLevFlag = TRUE; i <= iNoOfPts; i++)
		{
			ads_polar(asDblArray(ptStart), dAngle, dDist * i, ptResult);
			ptResult[Z] = dElev;

			//Draw...
			DRAWPOINT(asPnt3d(ptResult), DwgParams.szPointLyr);
			{//Add XData...
				ads_name objName;
				if(ads_entlast(objName) == RTNORM)
				{
					AddXDataForPoint(objName, DwgParams.szAppName, pszComments);
				}
			}
			//Draw Spot Level(s)............
			if((DwgParams.fSpotLevTxtHt > 0.0) && bSpLevFlag)
			{
				if(!DrawSpotLevel(asPnt3d(ptResult), DwgParams.szSpotLevBlock, DwgParams.szSpotLevLyr, DwgParams.fSpotLevRotAngle, DwgParams.fSpotLevTxtHt, DwgParams.fSpotLevBlkRelScale))
				{
					ads_printf("\nERROR: Failed to draw spot level(s)\n");
					bSpLevFlag = FALSE;
				}
			}
			ads_printf("\rPlease wait..Generating point #%d", (i+1));
		}
		ads_printf("\nTotal %d # of points generated successfully", i);
	}
	return TRUE;
}

BOOL CImportPtsUtil::FindOccurance(const AcGePoint3d& PointParam, const AcGePoint3dArray Array, long& iIndex, BOOL& bIsUserCancelled, const BOOL bConsiderZ /* = FALSE*/)
{
	int i;
	
	for(i = 0, iIndex = -1, bIsUserCancelled = FALSE; i < Array.length(); i++)
	{
		if(ads_usrbrk() == 1)
		{
			bIsUserCancelled = TRUE;
			ads_printf("\nUser break encountered !\n");
			return FALSE;
		}
		if(bConsiderZ)
		{
			if((Array[i].x == PointParam.x) && (Array[i].y == PointParam.y) && (Array[i].z == PointParam.z))
			{
				iIndex = i;
				return TRUE;
			}
		}
		else
		{
			if((Array[i].x == PointParam.x) && (Array[i].y == PointParam.y))
			{
				iIndex = i;
				return TRUE;
			}
		}
	}
	return FALSE;
}

//
//Draws Spot levels on selected points...
//
void CImportPtsUtil::DrawSpotLevOnSelPts()
{
	const char* szprompts[] = {"Select points for Spot Level insertion :", "Remove Points :"};
	DWGPARAMS DwgParams;
	ads_name SelSet, Entity;
    int i, iCount, iRet;
    long iNoOfEnt;

	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		ads_printf("\nSetting Contour Parameter(s) first time...");
		CDwgParamObj::CreateDwgParamObj();
	}
	if(DwgParams.fSpotLevTxtHt <= 0.0)
	{
		ads_printf("\nERROR: Spot Level parameter not set yet...Set contour parameter & then try again\n");
		return;
	}

	{//Get user selected  points....
		struct  resbuf  Rb01;
		const char szEntName[] = "POINT";
		// Entity type
		Rb01.restype = 0;
		Rb01.resval.rstring = (char*)szEntName;
		Rb01.rbnext = 0L;
		
		iRet = ads_ssget(":$", szprompts, 0L, &Rb01, SelSet);
		if(iRet != RTNORM)
		{
			ads_printf("\nERROR: No entity selected\n");
			return;
		}
	}
	if(ads_sslength(SelSet, &iNoOfEnt) != RTNORM)
	{
		ads_ssfree(SelSet);//Free Selset
		return;//ERROR
	}
	LAYER(DwgParams.szSpotLevLyr);
	DIMZIN(0);//Don't suppress leading/trailing zeros
	//Extract 'n Draw Points
	for(i = iCount = 0; i < iNoOfEnt; iCount++, i++)
	{
		if(ads_ssname(SelSet, i, Entity) != RTNORM)
		{
			ads_printf("\nERROR: AutoCAD System error !\n");
			ads_ssfree(SelSet);//Free Selset
			return;
		}
		{//Xtract Point...
			AcDbObjectId objId;
			AcDbObject* pObj;
			Acad::ErrorStatus ErrCode;

			ErrCode = acdbGetObjectId(objId, Entity);
			if(ErrCode != Acad::eOk)
			{
				ads_ssfree(SelSet);//Free Selset
				ads_printf("\nERROR: AutoCAD System error !\n");
				return;
			}
			ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
			if(ErrCode != Acad::eOk)
			{
				ads_ssfree(SelSet);//Free Selset
				ads_printf("\nERROR: AutoCAD System error !\n");
				return;
			}
			if(!DrawSpotLevel(((AcDbPoint*)pObj)->position(), DwgParams.szSpotLevBlock, DwgParams.szSpotLevLyr, DwgParams.fSpotLevRotAngle, DwgParams.fSpotLevTxtHt, DwgParams.fSpotLevBlkRelScale))
			{
				ads_printf("\nERROR: Failed to draw spot level #d\n", i+1);
			}
			pObj->close();
		}
	}
	ads_ssfree(SelSet);//Free Selset
	ads_printf("\nTotal %d # of points annotated", iCount);
}


void CImportPtsUtil::GenPtsFrMultiSeg()
{
	DWGPARAMS DwgParams;
	AcGePoint3dArray ArrayPts;
	AcDbIntArray NoOfPtsArray;
	ads_name entName;
	int i;

	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		AfxMessageBox("ERROR: Contour drawing parameters no set", MB_ICONSTOP);
		return;
	}
	DIMZIN(0);//Don't suppress leading/trailing zeros
	{//Get Entity...
		const char* pszPmt = "\nSelect polyline for interpolation :";
		ads_point ptRes;
		AcDbObjectId objId;
		AcDbObject* pObj;
		Acad::ErrorStatus ErrCode;

		if(RTNORM != ads_entsel(pszPmt, entName, ptRes))
		{
			ads_printf("\nERROR: First point not selected\n");
			return;
		}
	
		ErrCode = acdbGetObjectId(objId, entName);
		if(ErrCode != Acad::eOk)
		{
			return;
		}
		ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
		if(ErrCode != Acad::eOk)
		{
			return;
		}
		if(pObj->isA() != AcDb3dPolyline::desc())
		{
			ads_printf("\nERROR: Entity selected is not a 3D Polyline !\n");
			pObj->close();
			return;
		}
		if(AcDb::Poly3dType::k3dSimplePoly != ((AcDb3dPolyline*)pObj)->polyType())
		{
			ads_printf("\nERROR: Selected 3D Polyline contains curved segments !\n");
			pObj->close();
			return;
		}
		{//Extract points...
			AcDbObjectIterator* pIterator;
			
			pIterator = ((AcDb3dPolyline*)pObj)->vertexIterator();
			for(pIterator->start(); (pIterator->done() == Adesk::kFalse); pIterator->step())
			{
				AcDbObjectId objectIdTmp;
				AcDb3dPolylineVertex* pVert;
			    
				objectIdTmp = pIterator->objectId();

				ErrCode = ((AcDb3dPolyline*)pObj)->openVertex(pVert, objectIdTmp, AcDb::kForRead);
				if(ErrCode != Acad::eOk)
				{
					delete pIterator;
					ads_printf("\nERROR: Error occured while iterating vertexes!\n");
					return;
				}
				ArrayPts.append(pVert->position());
				NoOfPtsArray.append(1);
				pVert->close();
			}
			delete pIterator;
		}

 		pObj->close();
	}
	if(!GetGenPtsFrMultiSegParam(ArrayPts, NoOfPtsArray))
		return;

	AcGePoint3dArray ArrayGenPts;
	BOOL bSpLevFlag;
	const char* pszComments = "Interpolated";

	if(NoOfPtsArray.length() != ArrayPts.length())
		return;
	//Generate Points
	for(i = 1; i < ArrayPts.length(); i++)
	{
		GetIntermediatePts(ArrayPts[i - 1], ArrayPts[i - 0], NoOfPtsArray[i-1], ArrayGenPts);
	}
	//Draw Points
	LAYER(DwgParams.szPointLyr);
	for(i = 0, bSpLevFlag = TRUE; i < ArrayGenPts.length(); i++)
	{
		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break encountered\n");
			break;
		}
	
		DRAWPOINT(ArrayGenPts[i], DwgParams.szPointLyr);
		{//Add XData...
			ads_name objName;
			if(ads_entlast(objName) == RTNORM)
			{
				AddXDataForPoint(objName, DwgParams.szAppName, pszComments);
			}
		}
		//Draw Spot Level(s)............
		if((DwgParams.fSpotLevTxtHt > 0.0) && bSpLevFlag)
		{
			if(!DrawSpotLevel(ArrayGenPts[i], DwgParams.szSpotLevBlock, DwgParams.szSpotLevLyr, DwgParams.fSpotLevRotAngle, DwgParams.fSpotLevTxtHt, DwgParams.fSpotLevBlkRelScale))
			{
				ads_printf("\nERROR: Failed to draw spot level(s)\n");
				bSpLevFlag = FALSE;
			}
		}
		ads_printf("\rPlease wait..Interpolating point #%d", (i+1));
	}
	ads_printf("\nTotal %d # of points generated successfully\n", i);
}
void CImportPtsUtil::GetIntermediatePts(const AcGePoint3d& PointStart, const AcGePoint3d& PointEnd, const int& iNoOfPts, AcGePoint3dArray& ArrayGenPts)
{
	int i;
	double dSegLen, dElevChange, dAngle;
	AcGePoint3d PointTmp;

	dAngle = Angle2D(PointStart, PointEnd);
	dSegLen = Dist2D(PointStart, PointEnd) / (iNoOfPts + 1);
	dElevChange = dSegLen * (PointEnd.z - PointStart.z) / Dist2D(PointStart, PointEnd);

	for(i = 1; i <= iNoOfPts; i++)
	{
		ads_polar(asDblArray(PointStart), dAngle, (dSegLen * i), asDblArray(PointTmp));
		PointTmp.z = PointStart.z + (dElevChange * i);
		ArrayGenPts.append(PointTmp);
	}
}
void CImportPtsUtil::ChkContourPoints()
{
	DWGPARAMS DwgParams;

	if(!GetDwgParam(DwgParams))
		return;
	RemoveDupContourPts(DwgParams.szAppName);
}

BOOL CImportPtsUtil::RemoveDupContourPts(const char* pszAppName)
{
	int i, iNoOfPoints;
	AcGePoint3dArray Array, ArrayPtsRemoved;
	AcDbObjectIdArray ObjIdArray;

	iNoOfPoints = GetContourPointsFromDWG(Array, pszAppName, TRUE, &ObjIdArray, &ArrayPtsRemoved);
	switch(iNoOfPoints)
	{
	case  0:
		ads_printf("\nNo points imported yet...checking can't be done\n");
		return TRUE;
	case -1:
		AfxMessageBox("Internal error occured...checking failed", MB_ICONSTOP);
		return FALSE;
	case -2:
		//User termination
		return FALSE;
	default:
		break;
	}


	if(ObjIdArray.length() > 0)
	{
		CString strMsg;

		ads_printf("\n%d# of duplicate contour points found...Removing all these points\n", ObjIdArray.length());
		
		strMsg.Format("%d# of duplicate point(s) removed\nWould you like to see the removed point(s) ?", ArrayPtsRemoved.length());
		if(IDYES == AfxMessageBox(strMsg, MB_ICONQUESTION|MB_YESNO))
		{
			CStringArray ArrayRep;
			for(i = 0; i < ArrayPtsRemoved.length(); i++)
			{
				CString strTmp;

				strTmp.Format("%4d) X=%f, Y=%f, Z=%f", i+1, ArrayPtsRemoved[i].x, ArrayPtsRemoved[i].y, ArrayPtsRemoved[i].z);
				ArrayRep.Add(strTmp);
			}
			ShowResultDlg(ArrayRep);
		}
		return EraseEntities(ObjIdArray);
	}
	return TRUE;
}
BOOL CImportPtsUtil::GetDwgParam(DWGPARAMS& DwgParams)
{
	memset(&DwgParams, 0, sizeof(DwgParams));

	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		AfxMessageBox("ERROR: Contour drawing parameters not set yet", MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}
void CImportPtsUtil::ContourImportData()
{
	DWGPARAMS DwgParams;
	
	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		AfxMessageBox("ERROR: Contour drawing parameters not set yet", MB_ICONSTOP);
		return;
	}
	//..............................................................................
	CPointDataPtrArray Array;
	int i;

	DIMZIN(0);//Don't suppress leading/trailing zeros
	if(ReadPointsFromTXTFile(Array))
	{
		CString strMsg;

		i = CImportPtsUtil::ImportPointsIntoDWG(Array, DwgParams);
		strMsg.Format("%d No of points added successfully", i);
		AfxMessageBox(strMsg, MB_ICONINFORMATION);
		ads_printf("\n%s", strMsg);
	}
	
	{//Deallocate...
		POINTDATA* pRec;
		for(i = 0; i < Array.GetSize(); i++)
		{
			pRec = Array[i];
			delete pRec;
		}
	}
	//..............................................................................
}