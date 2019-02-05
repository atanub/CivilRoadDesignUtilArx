//Line2PLine.cpp
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <AdsCodes.h>
#include "Line2PLine.h"
#include "Util.h"


BOOL CLine2PLine::ContourLines2PLine(const char* pszAppName, const char& chLyrNameSeparator, const CString& strLyrNames)
{
	int i;
	CStringArray strArrayLyrs;

	if(!XTractWordsFromLine(strLyrNames, strArrayLyrs, CString(chLyrNameSeparator)))
		return FALSE;

	for(i = 0; i < strArrayLyrs.GetSize(); i++)
	{
		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break\n");
			break;
		}
		ads_printf("\nPlease wait...Converting line entities of layer \"%s\"...", strArrayLyrs[i]);
		L2P(pszAppName, strArrayLyrs[i]);
	}
	ads_printf("\nDone\n");
	return TRUE;
}
BOOL CLine2PLine::L2P_old(const char* pszAppName, const char* pszLyrName, const BOOL& bEraseLines /*= TRUE*/)
{
	CPointPair3DPtrArray MainArray, Array, ArrayToDraw;
	ads_name SelSet;
	long i, iIndex;
	short iElementNo;
	BOOL bFlag;

	bFlag = CollectPtsFromContourLines(MainArray, SelSet, pszAppName, pszLyrName);
	if(!bFlag)
		return bFlag;

	Array.Append(MainArray);

	ads_printf("\nTotal %d# of points to be verified....", Array.GetSize());
	for(i = 0; Array.GetSize() != 0;)
	{
		iIndex = FindStartLine(Array, iElementNo);
		if(iIndex >= 0)
		{
			CPointPair3DPtrArray ArrayToDraw;
			POINTPAIR3D* pRec;

			pRec = Array[iIndex];
			ArrayToDraw.Add(pRec);
			Array.RemoveAt(iIndex);
			if(iElementNo == 1)
				SwapPoints(pRec);

			if(FindNextSegment(pRec->pt02, Array, ArrayToDraw))
			{
				i++;
				POLYLINE(ArrayToDraw, pszLyrName);
				{//Add XData...
					ads_name objName;
					if(ads_entlast(objName) == RTNORM)
					{
						AddXDataForContourLine(objName, pszAppName, &((ArrayToDraw[0])->pt02.z));
					}
				}
			}
		}
		else
		{
			break;
		}
	}
	ads_printf("\nTotal %d# of polylines drawn...", i);
	////////////////////////////////////////////////////////////////////////////////
	goto CLEANUP;

	CLEANUP :
	{
		if(bEraseLines)
		{
			EraseEntitiesInSelSet(SelSet);
		}
		ads_ssfree(SelSet);//Free Selset
		for(i = 0; i < MainArray.GetSize(); i++)
		{
			POINTPAIR3D* pRec;
			pRec = MainArray[i];
			delete pRec;
		}
		return bFlag;
	}
	return TRUE;
}

BOOL CLine2PLine::CollectPtsFromContourLines(CPointPair3DPtrArray& Array, ads_name& SelSet, const char* pszAppName, const char* pszLyrName)
{
	if(Array.GetSize() != 0)
		return 0;
	
	{//Get all the Lines....
		struct  resbuf  Rb01, Rb02, Rb03, Rb04;
		const char szEntName[] = "LINE";
		// Entity type
		Rb01.restype = 0;
		Rb01.resval.rstring = (char*)szEntName;
		Rb01.rbnext = &Rb02; 
		//Layer Name
		Rb02.restype = 8;
		Rb02.resval.rstring = (char*)pszLyrName;
		Rb02.rbnext = &Rb03; 
		//Extended entity data 
		Rb03.restype = -3;
		Rb03.rbnext = &Rb04; 

		//application Name
		Rb04.restype = 1001; 
		Rb04.resval.rstring = (char*)pszAppName; 
		Rb04.rbnext = NULL; 

		if(0L == pszAppName)
		{
			Rb03.rbnext = 0L;
		}
		
		if(ads_ssget("X", 0L, 0L, &Rb01, SelSet) != RTNORM)
		{
			return FALSE;
		}
	}
	{//Entity(s)....
		long i, iNoOfEnt;
		ads_name Entity;
		
		iNoOfEnt = 0;
		if(ads_sslength(SelSet, &iNoOfEnt) != RTNORM)
		{
			ads_ssfree(SelSet);//Free Selset
			return FALSE;//ERROR
		}
		for(i = 0; i < iNoOfEnt; i++)
		{
			if(ads_ssname(SelSet, i, Entity) != RTNORM)
			{
				ads_printf("\nERROR: AutoCAD System error !\n");
				ads_ssfree(SelSet);//Free Selset
				return FALSE;//ERROR
			}
			{//Extract Data...
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
				if(pObj->isA() != AcDbLine::desc())
				{
					pObj->close();
					ads_ssfree(SelSet);//Free Selset
					ads_printf("\nERROR: AutoCAD System error !\n");
					return -1;
				}
				{//Append to the Array 
					POINTPAIR3D* pRec;
					
					pRec = new POINTPAIR3D;
					
					pRec->pt01 = ((AcDbLine*)pObj)->startPoint();
					pRec->pt02 = ((AcDbLine*)pObj)->endPoint();
					
					Array.Add(pRec);
				}
				pObj->close();
			}
		}
		return TRUE;
	}
	//Don't Free Selset....
	return TRUE;
}
BOOL CLine2PLine::ContourL2PGetVertices(const char* pszAppName, const char* pszLyrName, AcDbVoidPtrArray& arraySegments, AcDbObjectIdArray* pObjIdArray/* = 0L*/)
{
	CPointPair3DPtrArray MainArray, Array, ArrayToDraw;
	ads_name SelSet;
	long i, iIndex;
	short iElementNo;
	BOOL bFlag;

	if(arraySegments.length() > 0)
		return FALSE;

	bFlag = CollectPtsFromContourLines(MainArray, SelSet, pszAppName, pszLyrName);
	if(!bFlag)
	{
		bFlag = TRUE;//Assuming No entities found,,,,,,
		goto CLEANUP;
	}

	if(0L != pObjIdArray)
	{
		ExtratObjectIDs(SelSet, *pObjIdArray);
	}
	ads_ssfree(SelSet);//Free Selset


	Array.Append(MainArray);
	ads_printf("\nTotal %d# of points to be verified....", Array.GetSize());
	for(i = 0; Array.GetSize() != 0;)
	{
		iIndex = FindStartLine(Array, iElementNo);
		if(iIndex >= 0)
		{
			CPointPair3DPtrArray ArrayToDraw;
			POINTPAIR3D* pRec;

			pRec = Array[iIndex];
			ArrayToDraw.Add(pRec);
			Array.RemoveAt(iIndex);
			if(iElementNo == 1)
				SwapPoints(pRec);

			if(FindNextSegment(pRec->pt02, Array, ArrayToDraw))
			{
				i++;
				AcGePoint3dArray* pArrayNewSeg;

				pArrayNewSeg = new AcGePoint3dArray();
				ConvertPtPairsToVertices(ArrayToDraw, *pArrayNewSeg);
				arraySegments.append(pArrayNewSeg);
			}
		}
		else
		{
			break;
		}
	}
	////////////////////////////////////////////////////////////////////////////////
	goto CLEANUP;

	CLEANUP :
	{
		for(i = 0; i < MainArray.GetSize(); i++)
		{
			POINTPAIR3D* pRec;
			pRec = MainArray[i];
			delete pRec;
		}
		return bFlag;
	}
	return TRUE;
}

BOOL CLine2PLine::L2P(const char* pszAppName, const char* pszLyrName, const BOOL& bEraseLines /*= TRUE*/)
{
	long i;
	BOOL bFlag;
	AcDbVoidPtrArray arraySegments;
	AcDbObjectIdArray arrayObjIDs;

	bFlag = TRUE;
	if(!ContourL2PGetVertices(pszAppName, pszLyrName, arraySegments, &arrayObjIDs))
	{
		bFlag = FALSE;
		goto CLEANUP;
	}

	for(i = 0; i < arraySegments.length(); i++)
	{
		AcGePoint3dArray* pSeg;
		
		pSeg = (AcGePoint3dArray*)arraySegments[i];
		POLYLINE(*pSeg, pszLyrName);
		if(0L != pszAppName)
		{//Add XData...
			ads_name objName;

			if(ads_entlast(objName) == RTNORM)
			{
				AddXDataForContourLine(objName, pszAppName, &(((*pSeg)[0]).z));
			}
		}
		#ifdef _DEBUG
		//SPLINE(ArrayToDraw, "0");
		#endif// _DEBUG
		//		
		delete pSeg;//Deallocation!!
	}
	ads_printf("\nTotal %d# of polylines drawn...", i);
	////////////////////////////////////////////////////////////////////////////////
	goto CLEANUP;

	CLEANUP :
	{
		if(bEraseLines)
		{
			bFlag = EraseEntities(arrayObjIDs);
		}
		return bFlag;
	}
	return TRUE;
}
long CLine2PLine::FindStartLine(const CPointPair3DPtrArray& Array, short& iElementSlNo)
{
	long iOccuranceA, iOccuranceB, i;
	
	for(i = 0; i < Array.GetSize(); i++)
	{
		iOccuranceA = FindOccuranceInArray(Array, (Array[i])->pt01);
		if(iOccuranceA == 1)
		{
			iElementSlNo = 0;
			return i;//Success
		}
		iOccuranceB = FindOccuranceInArray(Array, (Array[i])->pt02);
		if(iOccuranceB == 1)
		{
			iElementSlNo = 1;
			return i;//Success
		}
	}
	//Chk if closed...
	for(i = 0; i < Array.GetSize(); i++)
	{
		iOccuranceA = FindOccuranceInArray(Array, (Array[i])->pt01);
		if((iOccuranceA % 2) != 0)
		{
			return -1L;//Failure
		}
		iOccuranceB = FindOccuranceInArray(Array, (Array[i])->pt02);
		if((iOccuranceB % 2) != 0)
		{
			return -1L;//Failure
		}
	}
	iElementSlNo = 0;
	return 0;//Success
}
int CLine2PLine::FindOccuranceInArray(const CPointPair3DPtrArray& Array, const AcGePoint3d& Pt)
{
	long iOccurance, i;
	
	for(i = 0, iOccurance = 0; i < Array.GetSize(); i++)
	{
		const POINTPAIR3D* pRecToVerify;
		
		pRecToVerify = Array[i];
		if((pRecToVerify->pt01 == Pt) || (pRecToVerify->pt02== Pt))
			iOccurance++;
	}
	return iOccurance;
}

int CLine2PLine::FindInArray(const CPointPair3DPtrArray& Array, const AcGePoint3d& Pt, short& iPtIndex)
{
	long i;
	
	for(i = 0, iPtIndex = 0; i < Array.GetSize(); i++)
	{
		const POINTPAIR3D* pRecToVerify;
		
		pRecToVerify = Array[i];
		if((pRecToVerify->pt01 == Pt) || (pRecToVerify->pt02== Pt))
		{
			iPtIndex = (pRecToVerify->pt01 == Pt) ? (short)0 : (short)1;
			return i;
		}
	}
	return -1;
}
BOOL CLine2PLine::FindNextSegment(const AcGePoint3d& PtParam, CPointPair3DPtrArray& ArrayMain, CPointPair3DPtrArray& ArrayOfVertexes)
{
	long iIndex;
	short iPtIndex;
	AcGePoint3d PtToFind;
	
	for(PtToFind = PtParam;;)
	{
		iIndex = FindInArray(ArrayMain, PtToFind, iPtIndex);
		if(iIndex < 0)
		{
			break;
		}
		else
		{
			POINTPAIR3D* pRec;

			pRec = ArrayMain[iIndex];
			if(iPtIndex == 1)
			{//Swap...
				SwapPoints(pRec);
			}
			ArrayOfVertexes.Add(pRec);
			ArrayMain.RemoveAt(iIndex);
			PtToFind = pRec->pt02;
		}
	}
	return (ArrayOfVertexes.GetSize() > 0);
}
void CLine2PLine::SwapPoints(POINTPAIR3D* pRec)
{
	AcGePoint3d	ptTmp;

	ptTmp = pRec->pt01;
	pRec->pt01 = pRec->pt02;
	pRec->pt02 = ptTmp;
}
