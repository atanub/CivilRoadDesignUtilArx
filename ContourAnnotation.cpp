//ContourAnnotation.cpp
////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "ContourAnnotation.h"
#include "DrawUtil.h"
#include "Util.h"
#include "GenPtsDlg.h"
#include "AnnotationParamDlg.h"
#include "DwgParamObj.h"
#include "DrawContourLines.h"
#include "TINData.h"
#include "Line2PLine.h"
#include "ContourParamDlg.h"

BOOL CContourAnnotation::AnnotateContourLines(const DWGPARAMS& DwgParams)
{
	//for testing purpose!!
	return AnnotateContourLinesEx(DwgParams);//New Implementation

	ads_name SelSet;

	if((DwgParams.fTxtHeight <= 0.0) || (DwgParams.fTxtSpaMF <= 0.0))
	{
		AfxMessageBox("ERROR: Contour annotation parameters not set properly", MB_ICONSTOP);
		return FALSE;
	}
	
	{//Get all the imported Lines....
		struct  resbuf  Rb01, Rb02, Rb03;
		const char szEntName[] = "LINE";
		// Entity type
		Rb01.restype = 0;
		Rb01.resval.rstring = (char*)szEntName;
		Rb01.rbnext = &Rb02; 
		//Extended entity data 
		Rb02.restype = -3;
		Rb02.rbnext = &Rb03; 
		//application Name
		Rb03.restype = 1001; 
		Rb03.resval.rstring = (char*)(DwgParams.szAppName); 
		Rb03.rbnext = NULL; 
		
		if(ads_ssget("X", 0L, 0L, &Rb01, SelSet) != RTNORM)
		{
			return FALSE;
		}
	}
	{//
		ads_name Entity;
		long i, iCountAnn, iNoOfEnt;

		if(ads_sslength(SelSet, &iNoOfEnt) != RTNORM)
		{
			ads_ssfree(SelSet);//Free Selset
			return -1;//ERROR
		}
		for(i = iCountAnn = 0; i < iNoOfEnt; i++)
		{
			if(ads_usrbrk() == 1)
			{
				ads_printf("\nUser break\n");
				break;
			}

			if(ads_ssname(SelSet, i, Entity) != RTNORM)
			{
				ads_printf("\nERROR: AutoCAD System error !\n");
				ads_ssfree(SelSet);//Free Selset
				return -1;
			}
			{//Chk if Contour Compatible LINE...
				AcDbObjectId objId;
				AcDbObject* pObj;
				Acad::ErrorStatus ErrCode;

				ErrCode = acdbGetObjectId(objId, Entity);
				if(ErrCode != Acad::eOk)
				{
					ads_ssfree(SelSet);//Free Selset
					ads_printf("\nERROR: AutoCAD System error !\n");
					return -1;
				}
				ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
				if(ErrCode != Acad::eOk)
				{
					ads_ssfree(SelSet);//Free Selset
					ads_printf("\nERROR: AutoCAD System error !\n");
					return -1;
				}
				{//Chk whether native entity ...
					resbuf* pResBuf;
					pResBuf = pObj->xData(DwgParams.szAppName);
					pObj->close();
					if(pResBuf != 0L)
					{//a native entity... so Proceed..
						AcGePoint3dArray Array;
						char* pszLyrEnt;
						
						AnnotateLine((AcDbLine*)pObj, DwgParams, Array);
						pszLyrEnt = ((AcDbLine*)pObj)->layer();
						DrawLineFragments(pszLyrEnt, DwgParams.szAppName, Array);
						delete []pszLyrEnt;
						if(Array.length() > 0)
						{
							iCountAnn++;
							ads_printf("\rPlease wait..annotating line #%d", iCountAnn);
							ads_entdel(Entity);//
						}
					}
					ads_relrb(pResBuf);
				}
			}
		}
		ads_printf("\nTotal %d # of lines annotated", iCountAnn);
	}
	return TRUE;
}
BOOL CContourAnnotation::AnnotateLine(AcDbLine* pEntity, const DWGPARAMS& DwgParams, AcGePoint3dArray& Array)
{
	pEntity->assertReadEnabled();
	if(pEntity->startPoint().z != pEntity->endPoint().z)
		return FALSE;
	if(Array.length() > 0)
	{
		Array.removeSubArray(0, Array.length() - 1);
	}

	char szAnnStr[_MAX_PATH];
	const double dWidthMF = 1.0;
	double dAngle, dTxtRotAngle, dTxtLength, dCumDist, dLineLength, dTxtSpacing;
	ads_point ptResult;
	AcGePoint3d StartPt, EndPt;
	Adesk::Int16 iPrec;
	
	iPrec = (Adesk::Int16)LUPREC();
	ads_rtos(pEntity->startPoint().z, 2, iPrec, szAnnStr);

	dTxtLength	= DwgParams.fTxtHeight * dWidthMF * strlen(szAnnStr);
	dLineLength = Dist2D(pEntity->startPoint(), pEntity->endPoint());
	dAngle		= Angle2D(pEntity->startPoint(), pEntity->endPoint());
	dTxtSpacing	= DwgParams.fTxtSpaMF * dTxtLength;
	
	if(dTxtLength >= dLineLength)
		return FALSE;//Annotation not possible
	
	//Annotation possible.. so proceed......
	dTxtRotAngle = dAngle;
	if((dTxtRotAngle >= PI / 2.0) && (dTxtRotAngle <= 3.0 * PI / 2.0))
	{
		dTxtRotAngle = dTxtRotAngle - PI;
	}
	StartPt = pEntity->startPoint();
	EndPt	= pEntity->endPoint();

	if((dLineLength <= (dTxtSpacing + dTxtLength / 2.0)) && (dLineLength >=  (1.25 * dTxtLength)))
	{//For those which can't be annotated considering user defined text spacing....
		char* pszLyrEnt;

		ads_polar(asDblArray(StartPt), dAngle, (dLineLength / 2.0), ptResult);
		pszLyrEnt = pEntity->layer();
		TEXTMID(asPnt3d(ptResult), szAnnStr, dTxtRotAngle, DwgParams.fTxtHeight, pszLyrEnt);
		delete []pszLyrEnt;
		{//Add XData...
			ads_name objName;
			if(ads_entlast(objName) == RTNORM)
			{
				AddXDataForAnnText(objName, DwgParams.szAppName, szAnnStr);
			}
		}

		{//First Line...
			Array.append(StartPt);//Start
			ads_polar(asDblArray(StartPt), dAngle, ((dLineLength - dTxtLength) / 2.0), ptResult);
			Array.append(asPnt3d(ptResult));//End 
		}
		{//Last Line...
			ads_polar(asDblArray(StartPt), dAngle, ((dLineLength + dTxtLength) / 2.0), ptResult); 
			Array.append(asPnt3d(ptResult));//Start
			Array.append(EndPt);//End 
		}
		return TRUE;
	}
	for(dCumDist = dTxtSpacing; (dCumDist + dTxtLength / 2.0) < dLineLength; dCumDist += dTxtSpacing)
	{
		char* pszLyrEnt;

		ads_polar(asDblArray(StartPt), dAngle, dCumDist, ptResult);
		pszLyrEnt = pEntity->layer();
		TEXTMID(asPnt3d(ptResult), szAnnStr, dTxtRotAngle, DwgParams.fTxtHeight, pszLyrEnt);
		delete []pszLyrEnt;
		{//Add XData...
			ads_name objName;
			if(ads_entlast(objName) == RTNORM)
			{
				AddXDataForAnnText(objName, DwgParams.szAppName, szAnnStr);
			}
		}

		if(dCumDist == dTxtSpacing)
		{//First Line...
			Array.append(StartPt);//Start
			ads_polar(asDblArray(StartPt), dAngle, dCumDist - (dTxtLength / 2.0), ptResult);//End 
			Array.append(asPnt3d(ptResult));
		}
		else
		{//Other Lines
			ads_polar(asDblArray(Array.last()), dAngle, dTxtLength, ptResult);//Start
			Array.append(asPnt3d(ptResult));
			ads_polar(asDblArray(StartPt), dAngle, dCumDist - (dTxtLength / 2.0), ptResult);//End 
			Array.append(asPnt3d(ptResult));
		}

	}
	if(Array.length() > 0)
	{//Last Line...
		ads_polar(asDblArray(Array.last()), dAngle, dTxtLength, ptResult);//Start
		Array.append(asPnt3d(ptResult));
		Array.append(EndPt);
	}
	return TRUE;
}
void CContourAnnotation::DrawLineFragments(const char* pszLyr, const char* pszAppName, const AcGePoint3dArray& Array)
{
	long i;

	for(i = 1; i < Array.length(); i = i + 2)
	{
		if(Array[i - 1] != Array[i])
		{
			LINE(Array[i - 1], Array[i], pszLyr);
			AddXDataForLastAddedContourEnt(pszAppName, Array[i].z);
		}
	}
}
//Manual Annotation...........
////////////////////////////////////////////////////////////////////////////////////////
BOOL CContourAnnotation::GetAnnotationPath(AcGePoint3dArray& Array)
{
	const char* pszPmt = "\nSelect polyline as Annotation Path :";
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
	//Extract vertices...
	if(!ExtractVertices(objId, Array))
	{
		pObj->close();
		return FALSE;
	}
	pObj->close();
	return TRUE;
}
BOOL CContourAnnotation::GetPLinesForConversion(AcDbObjectIdArray& Array)
{
	ads_name SelSet;

	if(Array.length() > 0)
	{
		ads_printf("\nERROR: Invalid parameter passed... in GetPLinesForConversion() !\n");
		return FALSE;
	}
	
	{//Let the user to selcet PLines....
		struct  resbuf  Rb01;
		const char szEntName[] = "LWPOLYLINE";
		// Entity type
		Rb01.restype = 0;
		Rb01.resval.rstring = (char*)szEntName;
		Rb01.rbnext =0L; 
		
		if(ads_ssget(0L, 0L, 0L, &Rb01, SelSet) != RTNORM)
		{
			return FALSE;
		}
	}
	{//
		ads_name Entity;
		long i, iNoOfEnt;

		if(ads_sslength(SelSet, &iNoOfEnt) != RTNORM)
		{
			ads_ssfree(SelSet);//Free Selset
			return FALSE;
		}
		for(i = 0; i < iNoOfEnt; i++)
		{
			if(ads_ssname(SelSet, i, Entity) != RTNORM)
			{
				ads_printf("\nERROR: AutoCAD System error !\n");
				ads_ssfree(SelSet);//Free Selset
				return FALSE;
			}
			{//...
				AcDbObjectId objId;

				if(Acad::eOk != acdbGetObjectId(objId, Entity))
				{
					ads_ssfree(SelSet);//Free Selset
					ads_printf("\nERROR: AutoCAD System error !\n");
					return FALSE;
				}
				Array.append(objId);
			}
		}
	}
	return (Array.length() > 0);
}
BOOL CContourAnnotation::GetAllContourEntities(AcDbObjectIdArray& Array, const DWGPARAMS& DwgParams)
{
	ads_name SelSet;
	
	{//Get all the imported/Generated entities....
		struct  resbuf Rb01, Rb02;

		//Extended entity data 
		Rb01.restype = -3;
		Rb01.rbnext = &Rb02; 
		//application Name
		Rb02.restype = 1001; 
		Rb02.resval.rstring = (char*)DwgParams.szAppName; 
		Rb02.rbnext = 0L; 
		
		if(ads_ssget("X", 0L, 0L, &Rb01, SelSet) != RTNORM)
		{
			return FALSE;
		}
	}
	{//
		ads_name Entity;
		long i, iNoOfEnt;

		if(ads_sslength(SelSet, &iNoOfEnt) != RTNORM)
		{
			ads_ssfree(SelSet);//Free Selset
			return FALSE;
		}
		for(i = 0; i < iNoOfEnt; i++)
		{
			if(ads_ssname(SelSet, i, Entity) != RTNORM)
			{
				ads_printf("\nERROR: AutoCAD System error !\n");
				ads_ssfree(SelSet);//Free Selset
				return FALSE;
			}
			{//Chk if Contour Compatible LINE/PLine...
				AcDbObjectId objId;
				Acad::ErrorStatus ErrCode;
				AcDbObject* pObj;
				int iRetValue;

				ErrCode = acdbGetObjectId(objId, Entity);
				if(ErrCode != Acad::eOk)
				{
					ads_ssfree(SelSet);//Free Selset
					ads_printf("\nERROR: AutoCAD System error !\n");
					return FALSE;
				}

				iRetValue = IsLineOrPLine(objId, pObj);
				if((1 == iRetValue) || (2 == iRetValue))
				{
					Array.append(objId);
				}
				if(0L != pObj)
				{
					pObj->close();
				}
			}
		}
	}
	return TRUE;
}
//Return value
// = -1 >>Error
// =  0 >>Other
// =  1 >>Line
// =  2 >>PLine
int CContourAnnotation::IsLineOrPLine(const AcDbObjectId& objId, AcDbObject*& pObjParam)
{
	AcDbObject* pObj;

	pObjParam = 0L;
	if(Acad::eOk != acdbOpenObject(pObj, objId, AcDb::kForRead))
	{
		ads_printf("\nERROR: AutoCAD System error ! Failed to read entity data!\n");
		return -1;
	}
	if(pObj->isA() == AcDbLine::desc())
	{
		pObjParam = pObj;
		return 1;
	}
	if(pObj->isA() == AcDb2dPolyline::desc())
	{
		pObjParam = pObj;
		return 2;
	}
	pObj->close();
	return 0;//Other
}
BOOL CContourAnnotation::AnnotatePolylineManually(AcDbObject* pEntity, const DWGPARAMS& DwgParams, const ANNTNPARAMS& AnnParam, const AcGePoint3dArray& PtArray, AcDbObjectIdArray& /*NewEntsArray*/)
{
	int i;
	AcGePoint3dArray arrayPLineVertices;

	//Extract the vertices....
	if(!ExtractVertices(pEntity->objectId(), arrayPLineVertices))
	{
		return FALSE;
	}
	for(i = 1; i < PtArray.length(); i++)
	{
		const AcGePoint3d& ptStart = PtArray[i - 1];
		const AcGePoint3d& ptEnd = PtArray[i];
		int j;

		for(j = 1; j < arrayPLineVertices.length(); j++)
		{
			const AcGePoint3d& ptStartA = arrayPLineVertices[j - 1];
			const AcGePoint3d& ptEndA = arrayPLineVertices[j];
			AcGePoint3d ptInters;

			if(GetIntersPt2D(ptStart, ptEnd, ptStartA, ptEndA, ptInters))
			{
				InsertElevTextOnPLine(DwgParams, AnnParam, pEntity, ptInters);//Inserts Elev Text on the picked point 
				//DISABLED: The breaking of polyline is not desirable now!!
				//if(InstantAnnotationForPLine(DwgParams, AnnParam, pEntity, ptInters, &NewEntsArray))
				//{
				//	return TRUE;
				//}
			}
		}
	}
	return FALSE;
}

BOOL CContourAnnotation::AnnotateLineManually(const AcDbLine* pEntity, const DWGPARAMS& DwgParams, const ANNTNPARAMS& AnnParam, const AcGePoint3dArray& PtArray, AcDbObjectIdArray& NewEntsArray)
{
	long i;

	const AcGePoint3d StartPt	= pEntity->startPoint();
	const AcGePoint3d EndPt		= pEntity->endPoint();
	char szAnnStr[_MAX_PATH];
	const double dWidthMF = 1.0;
	double dAngle, dTxtLength, dLineLength;
	CString strEntLyr;
	char* pszLyrEnt;
	
	pszLyrEnt = pEntity->layer();
	strEntLyr = pszLyrEnt;
	delete []pszLyrEnt;

	if(pEntity->isErased() || pEntity->isEraseStatusToggled())
	{
		return FALSE;
	}
	
	AcGePoint3d StartPtOfPath, EndPtOfPath;
	Adesk::Int16 iPrec;
			
	iPrec = (Adesk::Int16)LUPREC();
	ads_rtos(StartPt.z, 2, iPrec, szAnnStr);
	dTxtLength	= AnnParam.fTxtHeight * dWidthMF * strlen(szAnnStr);
	dLineLength = Dist2D(StartPt, EndPt);

	for(i = 1; i < PtArray.length(); i++)
	{
		AcGePoint3d Result;
		StartPtOfPath	= PtArray[i - 1];
		EndPtOfPath		= PtArray[i];

		if(GetIntersPt2D(StartPt, EndPt, StartPtOfPath, EndPtOfPath, Result))
		{//Chk for adequate line length......
			{//Calculate angle.......
				switch(AnnParam.iRotAngleType)
				{
				case 1:
					//Along the Path Specified;
					dAngle	= Angle2D(StartPtOfPath, EndPtOfPath);
					break;
				case 2:
					//User
					dAngle = AnnParam.fRotAngle;
					break;
				case 0:
				default:
					//Along Contour Line(s);
					dAngle	= Angle2D(StartPt, EndPt);
					break;
				}
			}
			{//Determine Layer.......
				switch(AnnParam.iLyrType)
				{
				case 1:
					//Respective Elev. Layer
					strEntLyr	= strEntLyr;
					break;
				case 2:
					//User
					strEntLyr = AnnParam.szLyr;
					LAYER(strEntLyr);
					break;
				case 0:
				default:
					//Current Layer
					CLAYER(strEntLyr);
					break;
				}
			}
			//
			//Annotation possible.. so proceed......
			if((dAngle > PI / 2.0) && (dAngle < 3.0 * PI / 2.0))
			{
				dAngle = dAngle - PI;
			}
			
			if(((dTxtLength / 2.0) < Dist2D(StartPt, Result)) && ((dTxtLength / 2.0) < Dist2D(EndPt, Result)))
			{//Annotation possible.. so proceed......
				TEXTMID(Result, szAnnStr, dAngle, AnnParam.fTxtHeight, strEntLyr);
				{//Add XData...
					ads_name objName;
					if(ads_entlast(objName) == RTNORM)
					{
						AddXDataForAnnText(objName, DwgParams.szAppName, szAnnStr);
					}
				}
				{//Draw 2 Lines...
					AcGePoint3d PtTemp;
					CString strElevLyr;
					ads_point ptResult;
					double dTemp;
					AcDbObjectId DbObjectIdNew;
						
					strElevLyr = CDrawContourLines::GetContourLyrName(StartPt.z, DwgParams);
					dTemp = Angle2D(StartPt, EndPt);

					ads_polar(asDblArray(Result), dTemp, (-dTxtLength / 2.0), ptResult);
					LINE(StartPt, asPnt3d(ptResult), strElevLyr);
					{//Add XData...
						AddXDataForLastAddedContourEnt(DwgParams.szAppName, StartPt.z, &NewEntsArray);
					}
					ads_polar(asDblArray(Result), dTemp, (dTxtLength / 2.0), ptResult);
					LINE(EndPt, asPnt3d(ptResult), strElevLyr);
					{//Add XData...
						AddXDataForLastAddedContourEnt(DwgParams.szAppName, StartPt.z, &NewEntsArray);
					}
				}
				break;//Break the loop because contour line splited...
			}
			else
			{//ForceAnnotation
				if(AnnParam.bForceAnnotation)
				{//Draw ...
					AcGePoint3d PtTemp;
					CString strElevLyr;
					ads_point ptResult;
					double dTemp;
					AcDbObjectId DbObjectIdNew;
					BOOL bLineDrawn;
						
					strElevLyr = CDrawContourLines::GetContourLyrName(StartPt.z, DwgParams);
					dTemp = Angle2D(StartPt, EndPt);
					bLineDrawn = FALSE;
					if(Dist2D(StartPt, Result) > Dist2D(EndPt, Result))
					{
						ads_polar(asDblArray(Result), dTemp, (-dTxtLength / 2.0), ptResult);
						bLineDrawn = Dist2D(StartPt, Result) > Dist2D(asPnt3d(ptResult), Result);
						if(bLineDrawn)
							LINE(StartPt, asPnt3d(ptResult), strElevLyr);
					}
					else
					{
						ads_polar(asDblArray(Result), dTemp, (+dTxtLength / 2.0), ptResult);
						bLineDrawn = Dist2D(EndPt, Result) > Dist2D(asPnt3d(ptResult), Result);
						if(bLineDrawn)
							LINE(EndPt, asPnt3d(ptResult), strElevLyr);
					}
					if(bLineDrawn)
					{//Add XData...
						AddXDataForLastAddedContourEnt(DwgParams.szAppName, StartPt.z, &NewEntsArray);
						TEXTMID(Result, szAnnStr, dAngle, AnnParam.fTxtHeight, strEntLyr);
						{//Add XData...
							ads_name objName;
							if(ads_entlast(objName) == RTNORM)
							{
								AddXDataForAnnText(objName, DwgParams.szAppName, szAnnStr);
							}
						}
						break;//Break the loop because contour line splited...
					}
					else
					{
						ads_printf("\nWarning : Failed to annotate contour line...text height is too big");
					}
				}
			}
		}
	}
	return TRUE;
}
//
BOOL CContourAnnotation::ConvertToContourEntity(const DWGPARAMS& DwgParams)
{
	const char* pszPmt = "\nSelect contour line to match property :";
	AcDbObjectIdArray ObjIDArray;
	int i;

	ads_printf("\nPlease select polylines to convert as contour lines.....");
	if(!GetPLinesForConversion(ObjIDArray))
	{
		ads_printf("\nNo valid entity(s) selected!");
		return FALSE;
	}
	//
	ads_printf("\nPlease wait..Conversion is being done....");
	for(i = 0; i < ObjIDArray.length(); i++)
	{
		AcDbObjectId objID;
		AcGePoint3dArray Array;
		BOOL bIsLWPLine;
		CString strEntLyr;
	
		objID = ObjIDArray[i];
		if(!ExtractVertices(objID, Array))
		{
			ads_printf("\nERROR: Failed to extract vertices (#%d)", i);
			return FALSE;
		}
		if(Array.length() <= 0)
			continue;
		
		{//Check If Native Entity
			AcDbObject* pObj;

			if(Acad::eOk != acdbOpenObject(pObj, objID, AcDb::kForRead))
			{
				return FALSE;
			}
			bIsLWPLine = (pObj->isA() == AcDbPolyline::desc());
			strEntLyr = GetLyrName((AcDbEntity*)pObj);
			pObj->close();
		}
		if((Array.length() > 0) && bIsLWPLine)
		{
			POLYLINE(Array, strEntLyr);
			AddXDataForLastAddedContourEnt(DwgParams.szAppName, Array[0].z);//Add XData...
			{//Delete..... from DB
				ads_name Entity;

				if(acdbGetAdsName(Entity, objID) == Acad::eOk)
				{
					ads_entdel(Entity);
				}
			}
		}
	}
	ads_printf("\nDone !\n");
	return TRUE;
}
BOOL CContourAnnotation::AnnContLinesManually(const DWGPARAMS& DwgParams)
{
	ANNTNPARAMS AnnParam;

	if(!CDwgParamObj::GetAnnParamData(&AnnParam))
	{
		AfxMessageBox("ERROR: Contour annotation parameters not set yet", MB_ICONSTOP);
		CDwgParamObj::CreateAnnParamObj();
		if(!CDwgParamObj::GetAnnParamData(&AnnParam))
			return FALSE;
	}
	if(!GetAnnotationParam(AnnParam))
		return FALSE;
	
	CDwgParamObj::CreateAnnParamObj(&AnnParam);//Save parameter...

	AcGePoint3dArray Array;
	if(!GetAnnotationPath(Array))
		return FALSE;

	//Start Annotation Drawing Routine.....
	AcDbObjectIdArray ObjIDArray, ObjIDArrayNew;

	if(!GetAllContourEntities(ObjIDArray, DwgParams))
		return FALSE;
	
	AcDbObjectId objId;

	ads_printf("\nPlease wait..Annotation is on progress..");
	for(;ObjIDArray.length() > 0;)
	{
		AcDbObjectIdArray ObjIDArrayNew;

		objId = ObjIDArray[0];
		{//Detect object whether it's polyline/line .....
			AcDbObject* pObj;
			int iRetValue;

			iRetValue = IsLineOrPLine(objId, pObj);
			if(1 == iRetValue)
			{
				AnnotateLineManually((AcDbLine*)pObj, DwgParams, AnnParam, Array, ObjIDArrayNew);
			}
			else if(2 == iRetValue)
			{
				AnnotatePolylineManually((AcDb2dPolyline*)pObj, DwgParams, AnnParam, Array, ObjIDArrayNew);
			}
			else
			{
				ads_printf("\nERROR: Invalid Entity found!!! Control should never reach here!");
			}
			if(0L != pObj)
			{
				pObj->close();
			}
		}
		//
		if(ObjIDArrayNew.length() > 0)
		{//Delete..... from DB
			ads_name Entity;

			if(acdbGetAdsName(Entity, objId) == Acad::eOk)
			{
				ads_entdel(Entity);
			}
			ObjIDArray.append(ObjIDArrayNew);
		}
		ObjIDArray.removeAt(0);
	}
	ads_printf("\nDone !\n");
	return TRUE;
}
BOOL CContourAnnotation::InstantAnnotation(const DWGPARAMS& DwgParams, ANNTNPARAMS& AnnParam)
{
	const char* pszPmt = "\nSelect contour line to annotate or enter 'P' to set drawing parameters :";
	AcGePoint3d PtPicked;
	BOOL bIsSelEntLine;
	AcDbObject* pObj;

	pObj = GetEntityForAnnotation(DwgParams, pszPmt, &PtPicked, &AnnParam);
	if(pObj == 0L)
		return FALSE;

	pObj->upgradeOpen();
	bIsSelEntLine = (pObj->isA() == AcDbLine::desc());
	if(bIsSelEntLine)
	{//Line Annotation
		BOOL bFlag;

		bFlag = InstantAnnotationForLine(DwgParams, AnnParam, pObj, PtPicked);
		pObj->close();
		return bFlag;
	}
	else if (pObj->isA() == AcDb2dPolyline::desc())
	{//PLine Annotation

		InsertElevTextOnPLine(DwgParams, AnnParam, (AcDb2dPolyline*)pObj, PtPicked);//Inserts Elev Text on the picked point 

		//DISABLED: The breaking of polyline is not desirable now!!
		//if(InstantAnnotationForPLine(DwgParams, AnnParam, (AcDb2dPolyline*)pObj, PtPicked))
		//{
		//	{//Delete..... from DB
		//		ads_name Entity;
		//
		//		if(acdbGetAdsName(Entity, pObj->objectId()) == Acad::eOk)
		//		{
		//			ads_entdel(Entity);
		//		}
		//	}
		//	pObj->close();
		//	return TRUE;
		//}
	}
	pObj->close();
	return FALSE;
}
//Instant Annotation for Lines...........
////////////////////////////////////////////////////////////////////////////////////////
BOOL CContourAnnotation::InstantAnnotationForLine(const DWGPARAMS& DwgParams, ANNTNPARAMS& AnnParam, AcDbObject* pObj, const AcGePoint3d& ptRes)
{
#ifdef _DEBUG
	//{//Testing
	//
	//	AcDbLeader objEnt;
	//}
#endif//_DEBUG
	ads_point ptTmp;
	AcGePoint3d StartPt, EndPt;

	StartPt = ((AcDbLine*)pObj)->startPoint();
	EndPt	= ((AcDbLine*)pObj)->endPoint();
	//
	//Start Drawing operations...
	//
	AcGePoint3dArray PtArray;
	AcDbObjectIdArray NewEntsArray;
	double dAngle;

	dAngle = Angle2D(StartPt, EndPt);
	
	ads_polar(asDblArray(ptRes), dAngle + (PI / 2.0), 100.0, ptTmp);
	PtArray.append(asPnt3d(ptTmp));
	ads_polar(asDblArray(ptRes), dAngle - (PI / 2.0), 100.0, ptTmp);
	PtArray.append(asPnt3d(ptTmp));

	AnnotateLineManually((AcDbLine*)pObj, DwgParams, AnnParam, PtArray, NewEntsArray);
	if(NewEntsArray.length() > 0)
	{//Delete..... from DB
		ads_name Entity;

		if(acdbGetAdsName(Entity, pObj->objectId()) == Acad::eOk)
		{
			ads_entdel(Entity);
		}
	}
	else
	{
		ads_printf("\nAnnotation failed...Possible cause : Annotation text size is too big\n");
		return FALSE;
	}
	return TRUE;
}

//Instant Annotation for PLines...........
////////////////////////////////////////////////////////////////////////////////////////
//Inserts Annotation text on the picked point WITHOUT BREAKING the selected entity
BOOL CContourAnnotation::InsertElevTextOnPLine(const DWGPARAMS& /*DwgParams*/, const ANNTNPARAMS& AnnParam, AcDbObject* pEnt, const AcGePoint3d& ptPickedParam)
{
	AcGePoint3dArray arrayPLineVertices; 
	int i;
	AcGePoint3d ptStart, ptEnd;
	const double dWidthMF = 1.0;
	double dTxtLength;
	CString strEntLyr;
	char szAnnStr[_MAX_PATH];
	Adesk::Int16 iPrec;

	//Extract the vertices....
	if(!ExtractVertices(pEnt->objectId(), arrayPLineVertices))
	{
		ads_printf("\nFATAL ERROR: Failed to extract the vertices!!\nTRACE from InsertElevTextOnPLine()!\n");
		return FALSE;
	}
	iPrec = (Adesk::Int16)LUPREC();
	strEntLyr = GetLyrName((AcDbEntity*)pEnt);
	ads_rtos(arrayPLineVertices[0].z, 2, iPrec, szAnnStr);

	dTxtLength	= AnnParam.fTxtHeight * dWidthMF * strlen(szAnnStr);
	{//Determine Layer.......
		switch(AnnParam.iLyrType)
		{
		case 1:
			//Respective Elev. Layer
			//strEntLyr	= strEntLyr;
			break;
		case 2:
			//User
			strEntLyr = AnnParam.szLyr;
			LAYER(strEntLyr);
			break;
		case 0:
		default:
			//Current Layer
			CLAYER(strEntLyr);
			break;
		}
	}
	for(i = 1; i < arrayPLineVertices.length(); i++)
	{
		double fLineLenA, fLineLenB, fAngle;

		ptStart = arrayPLineVertices[i - 1];
		ptEnd = arrayPLineVertices[i];
		fAngle = Angle2D(ptStart, ptEnd);
		if(IsPoint2dOnLine(ptPickedParam, ptStart, ptEnd, &fLineLenA, &fLineLenB))
		{
			TEXTMID(ptPickedParam, szAnnStr, fAngle, AnnParam.fTxtHeight, strEntLyr);
		}
	}
	return FALSE;
}

//Inserts Annotation text on the picked point BREAKING the selected entity
//static BOOL InstantAnnotationForPLine(const DWGPARAMS& DwgParams, const ANNTNPARAMS& AnnParam, AcDbObject* pEnt, const AcGePoint3d& ptPickedParam, AcDbObjectIdArray* pNewEntsArray /* = 0L*/)
//{
//	AcGePoint3dArray arrayPLineVertices; 
//	int i;
//	AcGePoint3d ptStart, ptEnd;
//	const double dWidthMF = 1.0;
//	double dTxtLength;
//	CString strEntLyr;
//	char szAnnStr[_MAX_PATH];
//	Adesk::Int16 iPrec;
//
//	//Extract the vertices....
//	if(!ExtractVertices(pEnt->objectId(), arrayPLineVertices))
//	{
//		ads_printf("\nFATAL ERROR: Failed to extract the vertices!!\nTRACE from InstantAnnotationForPLine()!\n");
//		return FALSE;
//	}
//	iPrec = (Adesk::Int16)LUPREC();
//	strEntLyr = GetLyrName((AcDbEntity*)pEnt);
//	ads_rtos(arrayPLineVertices[0].z, 2, iPrec, szAnnStr);
//
//	dTxtLength	= AnnParam.fTxtHeight * dWidthMF * strlen(szAnnStr);
//	{//Determine Layer.......
//		switch(AnnParam.iLyrType)
//		{
//		case 1:
//			//Respective Elev. Layer
//			//strEntLyr	= strEntLyr;
//			break;
//		case 2:
//			//User
//			strEntLyr = AnnParam.szLyr;
//			LAYER(strEntLyr);
//			break;
//		case 0:
//		default:
//			//Current Layer
//			CLAYER(strEntLyr);
//			break;
//		}
//	}
//	for(i = 1; i < arrayPLineVertices.length(); i++)
//	{
//		double fLineLenA, fLineLenB, fAngle;
//
//		ptStart = arrayPLineVertices[i - 1];
//		ptEnd = arrayPLineVertices[i];
//		fAngle = Angle2D(ptStart, ptEnd);
//		if(IsPoint2dOnLine(ptPickedParam, ptStart, ptEnd, &fLineLenA, &fLineLenB))
//		{
//			if(((fLineLenA > dTxtLength / 2.0) && (fLineLenB > dTxtLength / 2.0)) ||
//			    ((fLineLenA + fLineLenB) > dTxtLength)
//			  )
//			{
//				int j;
//				AcGePoint3d ptA, ptB;
//				AcGePoint3dArray VertexPtsPLineA, VertexPtsPLineB; 
//				AcGePoint3d ptPickedDummy;
//
//				if(!((fLineLenA > dTxtLength / 2.0) && (fLineLenB > dTxtLength / 2.0)))
//				{//Annotation on the Mid point of the line segment is possible..
//					if(AnnParam.bForceAnnotation)
//						ads_polar(asDblArray(ptStart), fAngle, (fLineLenA + fLineLenB) / 2.0, asDblArray(ptPickedDummy));
//					else
//						return FALSE;
//				}
//				else
//				{//Normal Annotation possible
//					ptPickedDummy = ptPickedParam;
//				}
//				//... 
//				ads_polar(asDblArray(ptPickedDummy), (fAngle +  PI), (dTxtLength / 2.0), asDblArray(ptA));
//				ads_polar(asDblArray(ptPickedDummy), (fAngle + 0.0), (dTxtLength / 2.0), asDblArray(ptB));
//				//... 
//				for(j = 0; j <= (i - 1); j++)
//				{
//					VertexPtsPLineA.append(arrayPLineVertices[j]);
//				}
//				VertexPtsPLineA.append(ptA);
//				//... 
//				VertexPtsPLineB.append(ptB);
//				for(j = i; j < arrayPLineVertices.length(); j++)
//				{
//					VertexPtsPLineB.append(arrayPLineVertices[j]);
//				}
//				POLYLINE(VertexPtsPLineA, strEntLyr);
//				{//Add XData...
//					AddXDataForLastAddedContourEnt(DwgParams.szAppName, ptStart.z, pNewEntsArray);
//				}
//				POLYLINE(VertexPtsPLineB, strEntLyr);
//				{//Add XData...
//					AddXDataForLastAddedContourEnt(DwgParams.szAppName, ptStart.z, pNewEntsArray);
//				}
//				if((fAngle > PI / 2.0) && (fAngle < 3.0 * PI / 2.0))
//				{
//					fAngle = fAngle - PI;
//				}
//				TEXTMID(ptPickedDummy, szAnnStr, fAngle, AnnParam.fTxtHeight, strEntLyr);
//				return TRUE;
//			}
//			else if(AnnParam.bForceAnnotation)
//			{
//				int j;
//				AcGePoint3d ptA, ptB, ptInsert;
//				AcGePoint3dArray VertexPtsPLineA, VertexPtsPLineB; 
//				double fAngleTmp;
//				
//				if(fLineLenA > (dTxtLength / 2.0))
//				{
//					//'ptA' will be considerd as Text's left most point
//					ads_polar(asDblArray(ptPickedParam), (fAngle +  PI), (dTxtLength / 2.0), asDblArray(ptA));
//				}
//				else
//				{
//					//'ptA' will be considerd as Text's left most point
//					ptA = ptPickedParam;
//				}
//				for(j = 0; j < i; j++)
//				{
//					VertexPtsPLineA.append(arrayPLineVertices[j]);
//				}
//				VertexPtsPLineA.append(ptA);
//				
//				//
//				for(j = i, fAngleTmp = 0.0; j < arrayPLineVertices.length(); j++)
//				{
//					if(Dist2D(ptA, arrayPLineVertices[j]) > dTxtLength)
//					{
//						fAngleTmp = Angle2D(ptA, arrayPLineVertices[j]);
//						//'ptB' will be considerd as Text's right most point
//						ads_polar(asDblArray(ptA), fAngleTmp, dTxtLength, asDblArray(ptB));
//						ads_polar(asDblArray(ptA), fAngleTmp, (dTxtLength / 2.0), asDblArray(ptInsert));
//						{//
//							int k;
//							
//							VertexPtsPLineB.append(ptB);
//							for(k = j; k < arrayPLineVertices.length(); k++)
//							{
//								VertexPtsPLineB.append(arrayPLineVertices[k]);
//							}
//						}
//						break;
//					}
//				}
//				//
//				if((VertexPtsPLineA.length() > 1) && (VertexPtsPLineB.length() > 1))
//				{
//
//					POLYLINE(VertexPtsPLineA, strEntLyr);
//					//Add XData...
//					AddXDataForLastAddedContourEnt(DwgParams.szAppName, ptStart.z, pNewEntsArray);
//					
//					POLYLINE(VertexPtsPLineB, strEntLyr);
//					//Add XData...
//					AddXDataForLastAddedContourEnt(DwgParams.szAppName, ptStart.z, pNewEntsArray);
//
//					if((fAngleTmp > PI / 2.0) && (fAngleTmp < 3.0 * PI / 2.0))
//					{
//						fAngleTmp = fAngleTmp - PI;
//					}
//					TEXTMID(ptInsert, szAnnStr, fAngleTmp, AnnParam.fTxtHeight, strEntLyr);
//					return TRUE;
//				}
//			}
//			return FALSE;
//		}
//	}
//	return FALSE;
//}
BOOL CContourAnnotation::IsPoint2dOnLine(const AcGePoint3d& ptToTest, const AcGePoint3d& pt01, const AcGePoint3d& pt02, double* pfDistA /* = 0L*/, double* pfDistB /* = 0L*/)
{
	double dDist;
	const double fErr = 1e-10;

	if(pfDistA != 0L)
	{
		*pfDistA = Dist2D(pt01, ptToTest);
	}
	if(pfDistB != 0L)
	{
		*pfDistB = Dist2D(ptToTest, pt02);
	}
	dDist = Dist2D(ptToTest, pt01) + Dist2D(ptToTest, pt02);
	dDist = dDist - Dist2D(pt01, pt02);
	dDist = fabs(dDist);
	return (dDist <= fErr);
}
AcDbObject* CContourAnnotation::GetEntityForAnnotation(const DWGPARAMS& DwgParams, const char* pszPrompt, AcGePoint3d* pPtPicked/* = 0L*/, ANNTNPARAMS* pAnnParam /* = 0L*/)
{
    int iRet;
	ads_name entName;
	BOOL bEntSelected;
	AcGePoint3d ptPickedLocal;

	bEntSelected = FALSE;

	for(;bEntSelected == FALSE;)
	{
		if(0L != pAnnParam)
		{
			ads_initget(RSG_OTHER, "Property _Property");
		}
		iRet = ads_entsel(pszPrompt, entName, asDblArray(ptPickedLocal));
		switch (iRet)
		{
		case RTNORM:
			//Entity selection successful....
			bEntSelected = TRUE;
			break;
		case RTKWORD:
			if(0L != pAnnParam)
			{//Set Param first ....
				if(GetAnnotationParam(*pAnnParam))
					CDwgParamObj::CreateAnnParamObj(pAnnParam);
				else
					CDwgParamObj::GetAnnParamData(pAnnParam);
			}
			else
			{
				ads_printf("\nERROR: No entity selected\n");
				return 0L;
			}
			break;
		default:
			ads_printf("\nERROR: No entity selected\n");
			return 0L;
		}
	}
	{//Check If Native Entity
		AcDbObject* pObj;
		Acad::ErrorStatus ErrCode;
		AcDbObjectId objId;

		ErrCode = acdbGetObjectId(objId, entName);
		if(ErrCode != Acad::eOk)
		{
			return 0L;
		}
		ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
		if(ErrCode != Acad::eOk)
		{
			return 0L;
		}
		if((pObj->isA() != AcDbLine::desc()) && (pObj->isA() != AcDb2dPolyline::desc()) && (pObj->isA() != AcDbPolyline::desc()))
		{
			ads_printf("\nERROR: Entity selected is not a valid contour\n");
			pObj->close();
			return 0L;
		}
		{//Chk whether native entity ...
			resbuf* pResBuf;
			pResBuf = pObj->xData(DwgParams.szAppName);
			if(pResBuf == 0L)
			{
				ads_printf("\nERROR: Entity selected is not contour generated\n");
				pObj->close();
				return 0L;
			}
			ads_relrb(pResBuf);
		}
		{//Convert picked point to nearest point to the entity selected
			if((pObj->isA() == AcDbLine::desc()) && (0L != pPtPicked))
			{
				ErrCode = ((AcDbLine*)pObj)->getClosestPointTo(ptPickedLocal, *pPtPicked);
				if(ErrCode != Acad::eOk)
				{
					pObj->close();
					return 0L;
				}
			}
			else if(pObj->isA() == AcDb2dPolyline::desc())
			{
				if(((AcDb2dPolyline*)pObj)->polyType() != AcDb::Poly2dType::k2dSimplePoly)
				{
					ads_printf("\nERROR: Entity selected is a polyline with curve\n");
					pObj->close();
					return 0L;
				}
				if(0L != pPtPicked)
				{
					ErrCode = ((AcDb2dPolyline*)pObj)->getClosestPointTo(ptPickedLocal, *pPtPicked);
					if(ErrCode != Acad::eOk)
					{
						pObj->close();
						return 0L;
					}
				}
			}
			else if(pObj->isA() == AcDbPolyline::desc())
			{
				{//More validation
					if(Adesk::kTrue != ((AcDbPolyline*)pObj)->isOnlyLines())
					{
						ads_printf("\nERROR: Selected polyline contains arc segments\n");
						pObj->close();
						return 0L;
					}
					if(Adesk::kTrue != ((AcDbPolyline*)pObj)->isPlanar())
					{
						ads_printf("\nERROR: Selected polyline vertices are not co-planer\n");
						pObj->close();
						return 0L;
					}
				}
				if(0L != pPtPicked)
				{
					ErrCode = ((AcDbPolyline*)pObj)->getClosestPointTo(ptPickedLocal, *pPtPicked);
					if(ErrCode != Acad::eOk)
					{
						pObj->close();
						return 0L;
					}
				}
			}
		}
		pObj->close();
		return pObj;
	}
	return 0L;
}
void CContourAnnotation::SetContourAnnotationParam()
{
	ANNTNPARAMS AnnParam;

	if(!CDwgParamObj::GetAnnParamData(&AnnParam))
	{
		CDwgParamObj::CreateAnnParamObj();
		if(!CDwgParamObj::GetAnnParamData(&AnnParam))
			return;
	}
	GetAnnotationParam(AnnParam);
}

BOOL CContourAnnotation::AddXDataForLastAddedContourEnt(const char* pszAppName, const double& fZVal, AcDbObjectIdArray* pNewEntsArray /*= 0L*/)
{
	ads_name objName;

	if(ads_entlast(objName) == RTNORM)
	{
		AddXDataForContourLine(objName, pszAppName, &fZVal);
		if(0L != pNewEntsArray)
		{
			AcDbObjectId objID;

			if(Acad::eOk != acdbGetObjectId(objID, objName))
			{
				return FALSE;
			}
			pNewEntsArray->append(objID);
		}
		return TRUE;
	}
	return FALSE;
}
/*
static BOOL AddXData(const char* pszAppName, const double& fZVal, const AcDbObjectId& objID)
{
	ads_name Entity;

	if(Acad::eOk != acdbGetAdsName(Entity, objID))
		return FALSE;

	AddXDataForContourLine(Entity, pszAppName, &fZVal);
	return TRUE;
}
*/
BOOL CContourAnnotation::AnnotateContourLinesEx(const DWGPARAMS& DwgParams)
{
	const char chLyrNameSeparator = '\t';
	const double fSpacing = DwgParams.fTxtHeight * DwgParams.fTxtSpaMF;
	AcGePoint3dArray ArrayPts;
	CString strLyrNames;

	if((DwgParams.fTxtHeight <= 0.0) || (DwgParams.fTxtSpaMF <= 0.0))
	{
		AfxMessageBox("ERROR: Contour annotation parameters not set properly", MB_ICONSTOP);
		return FALSE;
	}
	if(!CTINData::GetTINData(ArrayPts))
	{//
		CString strMsg;
		strMsg.Format("ERROR: Triangulation not done yet\nPlease Run Triangulation & then try to draw contour");
		AfxMessageBox(strMsg, MB_ICONSTOP);
		return FALSE;
	}
	////
	if(CDrawContourLines::GetAllContourLyrNames(ArrayPts, DwgParams, chLyrNameSeparator, strLyrNames))
	{
		CStringArray strArrayLyrs;
		int i;
		AcDbObjectIdArray objIdArrayAll;

		if(!XTractWordsFromLine(strLyrNames, strArrayLyrs, "\t"))
			return FALSE;

		for(i = 0; i < strArrayLyrs.GetSize(); i++)
		{
			AcDbVoidPtrArray arraySegments;
			AcDbObjectIdArray objIdArray;
			int j;

			if(ads_usrbrk() == 1)
			{
				ads_printf("\nUser break\n");
				break;
			}
			ads_printf("\nPlease wait...Checking line entities of layer \"%s\"(%d of %d)...", strArrayLyrs[i], (i+1), strArrayLyrs.GetSize());

			if(!CLine2PLine::ContourL2PGetVertices(DwgParams.szAppName, strArrayLyrs[i], arraySegments, &objIdArray))
			{
				goto CLEANUP;
			}
			//Browse all the segments.....
			for(j = 0; j < arraySegments.length(); j++)
			{
				AcGePoint3dArray* pSeg;
				
				pSeg = (AcGePoint3dArray*)arraySegments[j];
				{//Divide Poly line.......
					AcGePoint3dArray arrayInsPts;
					AcGeDoubleArray arrayAlgnAngles;

					DividePolyline(*pSeg, fSpacing, arrayInsPts, arrayAlgnAngles);
					InsAnnText(*pSeg, arrayInsPts, arrayAlgnAngles, DwgParams.fTxtHeight, strArrayLyrs[i]);
				}

				//
				delete pSeg;//Deallocation!!
			}
			if(objIdArray.length() > 0)
			{
				objIdArrayAll.append(objIdArray);
			}
		}
		ads_printf("\nDone\n");

	}
	////////////////////////////////////////////////////////////////////////////////
	goto CLEANUP;

	CLEANUP :
	{
	}
	return TRUE;
}
void CContourAnnotation::DividePolyline(const AcGePoint3dArray& arrayVertices, const double& fInterval, AcGePoint3dArray& arrayInsPts, AcGeDoubleArray& arrayAlgnAngles)
{
	int i;
	double fTotalLength, fCumDist;

	if((arrayInsPts.length() > 0) || (arrayAlgnAngles.length() > 0))
	{
		ads_printf("\nFATAL ERROR: Invalid parameter passed!! -> DividePolyline()\n");
		return;
	}
	//Calculate Length
	for(i = 1, fTotalLength = 0.0; i < arrayVertices.length(); i++)
	{
		fTotalLength += Dist2D(arrayVertices[i - 1], arrayVertices[i]);
	}
	if(fInterval >= fTotalLength)
	{
		return;
	}
	fCumDist = 0.0;
	while(fCumDist < fTotalLength)
	{
		AcGePoint3d ptNew;
		double fAngle;

		fCumDist += fInterval;
		if(!CalcInsertionPtNAngle(fCumDist, arrayVertices, ptNew, fAngle))
		{
			break;
		}
		arrayInsPts.append(ptNew);
		arrayAlgnAngles.append(fAngle);
	}
}
BOOL CContourAnnotation::CalcInsertionPtNAngle(const double& fDistParam, const AcGePoint3dArray& arrayVertices, AcGePoint3d& ptResult, double& fAlgnAngle)
{
	const double fPrecision = 1.0E-6;
	double dCumDist;
	int i;

	fAlgnAngle = 0.0;
	//Calculate the total distance
	for(i = 1, dCumDist = 0.0; i < arrayVertices.length(); i++)
	{
		double fDist;

		fDist = Dist2D(arrayVertices[i - 1], arrayVertices[i]);
		dCumDist += fDist;
		if(fabs(dCumDist - fDistParam) <= fPrecision)
		{
			fAlgnAngle = Angle2D(arrayVertices[i - 1], arrayVertices[i]);
			ptResult = arrayVertices[i];
			return TRUE;
		}
		if(dCumDist > fDistParam)
		{
			fAlgnAngle = Angle2D(arrayVertices[i], arrayVertices[i - 1]);
			ads_polar(asDblArray(arrayVertices[i]), fAlgnAngle, (dCumDist - fDistParam), asDblArray(ptResult));
			ptResult.z = arrayVertices[i].z + ((arrayVertices[i - 1].z - arrayVertices[i].z) * (dCumDist - fDistParam)) / fDist;
			fAlgnAngle = Angle2D(arrayVertices[i - 1], arrayVertices[i]);

			return TRUE;
		}
	}

	return FALSE;
}
BOOL CContourAnnotation::InsAnnText(const AcGePoint3dArray& arrayVertices, const AcGePoint3dArray& arrayInsPts, const AcGeDoubleArray& arrayAlgnAngles, const double& fTextSize, const char* pszLyrEnt)
{
	int i;
	Adesk::Int16 iPrec;
	char szAnnStr[_MAX_PATH];

	if(arrayInsPts.length() <= 0)
		return FALSE;

	
	iPrec = (Adesk::Int16)LUPREC();
	ads_rtos(arrayVertices[0].z, 2, iPrec, szAnnStr);

	for(i = 0; i < arrayInsPts.length(); i++)
	{
		double fAngle;

		fAngle = arrayAlgnAngles[i];
		fAngle = ((fAngle >= PI / 2.0) && (fAngle <= 3.0 * PI / 2.0)) ? (fAngle - PI) : fAngle;
		TEXTMID(arrayInsPts[i], szAnnStr, fAngle, fTextSize, pszLyrEnt);
	}
	return TRUE;
}

void CContourAnnotation::AnnotateAllContours()
{
	DWGPARAMS DwgParams;

	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		AfxMessageBox("ERROR: Contour drawing parameters not set yet", MB_ICONSTOP);
		return;
	}
	for(;(DwgParams.fTxtHeight <= 0.0) || (DwgParams.fTxtSpaMF <= 0.0);)
	{
		if(IDNO == AfxMessageBox("Contour annotation parameters not set properly\nWould you like to set parameters now ?", MB_ICONQUESTION|MB_YESNO))
			return;

		if(CContourParamDlg::GetContourParam(DwgParams))
		{
			if(!CDwgParamObj::CreateDwgParamObj(&DwgParams))
			{
				ads_printf("\nFATAL ERROR: Failed to attach drawing parameter data. Drawing file may be corrupted !");
				return;
			}
		}
	}
	DIMZIN(0);//Don't suppress leading/trailing zeros
	CContourAnnotation::AnnotateContourLines(DwgParams);
}
void CContourAnnotation::AnnotateSelContours()
{
	DWGPARAMS DwgParams;

	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		AfxMessageBox("ERROR: Contour drawing parameters not set yet", MB_ICONSTOP);
		return;
	}
	DIMZIN(0);//Don't suppress leading/trailing zeros
	CContourAnnotation::AnnContLinesManually(DwgParams);
}
void CContourAnnotation::ConvertSelPLinesAsContour()
{
	DWGPARAMS DwgParams;

	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		AfxMessageBox("ERROR: Contour drawing parameters not set yet", MB_ICONSTOP);
		return;
	}
	DIMZIN(0);//Don't suppress leading/trailing zeros
	CContourAnnotation::ConvertToContourEntity(DwgParams);
}
void CContourAnnotation::InstantAnnotatation()
{
	DWGPARAMS DwgParams;
	ANNTNPARAMS AnnParam;

	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		AfxMessageBox("ERROR: Contour drawing parameters not set yet", MB_ICONSTOP);
		return;
	}
	if(!CDwgParamObj::GetAnnParamData(&AnnParam))
	{
		AfxMessageBox("ERROR: Contour annotation parameters not set yet", MB_ICONSTOP);
		return;
	}
	DIMZIN(0);//Don't suppress leading/trailing zeros
	if(!CContourAnnotation::InstantAnnotation(DwgParams, AnnParam))
	{
		ads_printf("\nERROR: Instant annotation failed\n");
	}
	else
	{
		ads_printf("\nInstant annotation successfully done\n");
	}
}
