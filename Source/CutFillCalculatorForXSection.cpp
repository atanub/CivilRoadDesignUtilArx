// CutFillCalculatorForXSection.cpp
/////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <DbEnts.h>
#include "CutFillCalculatorForXSection.h"
#include "Util.h"
#include "DrawUtil.h"


CCutFillCalculatorForXSection::CCutFillCalculatorForXSection(const AcGePoint3dArray& arrayExistingLevels, const AcGePoint3dArray& arrayProposedLevels) : m_arrayExistingLevels(arrayExistingLevels), m_arrayProposedLevels(arrayProposedLevels)
{
}
CCutFillCalculatorForXSection::~CCutFillCalculatorForXSection()
{
}
BOOL CCutFillCalculatorForXSection::_Calculate()
{
	int i, j, iIndexLastBoundaryStarted, iIndexAtProposedProfile, iIndexAtProposedProfileLast;
	CTypedPtrArray<CPtrArray, AcGePoint3dArray*> arrayOfPolygons;
	CUIntArray arrayForCutOrFillIndication;
	AcGePoint3dArray* pBoundary;

	//Find intersection Points....
	iIndexAtProposedProfileLast = iIndexAtProposedProfile = 0;
	for(i = 1, pBoundary = 0L, iIndexLastBoundaryStarted = 0; i < m_arrayExistingLevels.length();)
	{
		AcGePoint3d ptResult;

		iIndexAtProposedProfile = _FindIntersectionPoint(iIndexAtProposedProfile+1, m_arrayExistingLevels[i - 1], m_arrayExistingLevels[i], ptResult);
		if(iIndexAtProposedProfile > 0)
		{
			if(arrayOfPolygons.GetSize() > 0)
			{
				AcGePoint3dArray* pBoundaryLast;
				AcGePoint3d ptOnExistngProfile, ptProposedProfile;


				pBoundaryLast = arrayOfPolygons[arrayOfPolygons.GetSize() - 1];
				for(j = iIndexAtProposedProfileLast; j < iIndexAtProposedProfile; j++)
				{
					pBoundaryLast->append(m_arrayProposedLevels[j]);
				}
				ptOnExistngProfile = (*pBoundaryLast)[0];
				ptProposedProfile = (*pBoundaryLast)[pBoundaryLast->length() - 1];
				
				arrayForCutOrFillIndication.Add((ptProposedProfile.y > ptOnExistngProfile.y)); // 1=Fill
				pBoundaryLast->append(ptResult);
				pBoundaryLast->append((*pBoundaryLast)[0]);//Make Polyline closed
			}

			iIndexLastBoundaryStarted = iIndexAtProposedProfile;
			pBoundary = new AcGePoint3dArray();
			pBoundary->append(ptResult);

			arrayOfPolygons.Add(pBoundary);

			iIndexLastBoundaryStarted = i;
			iIndexAtProposedProfileLast = iIndexAtProposedProfile;
		}
		else
		{
			if(pBoundary != 0L)
			{
				pBoundary->insertAt(0, m_arrayExistingLevels[i]);
			}
			i++;//Check next Edge (Existing Profile Data)
		}
	}
#ifdef _DEBUG
	LAYER("POLYLINE_CUT", 6);
	LAYER("POLYLINE_FILL", 5);
	for(i = 0; i < arrayOfPolygons.GetSize(); i++)
	{
		AcGePoint3dArray* pRec;

		pRec = arrayOfPolygons[i];
		if(arrayForCutOrFillIndication[i] == 1)
			POLYLINE(*pRec, "POLYLINE_FILL");
		else
			POLYLINE(*pRec, "POLYLINE_CUT");
	}
#endif//_DEBUG
	for(i = 0; i < arrayOfPolygons.GetSize(); i++)
	{
		AcGePoint3dArray* pRec;
	
		pRec = arrayOfPolygons[i];
		delete pRec;
	}
	arrayOfPolygons.RemoveAll();
	return TRUE;
	
}
int CCutFillCalculatorForXSection::_FindIntersectionPoint(int iStartVertex, const AcGePoint3d& ptStartParam, const AcGePoint3d& ptEndParam, AcGePoint3d& ptResultParam)
{
	int i;

	if((iStartVertex - 1) < 0)
	{
		//assert(false);
		iStartVertex = 1;
	}
	ptResultParam.x = ptResultParam.y = 0.0;
	for(i = iStartVertex; i < m_arrayProposedLevels.length(); i++)
	{
		const AcGePoint3d ptStart(m_arrayProposedLevels[i - 1].x, m_arrayProposedLevels[i - 1].y, 0.0);
		const AcGePoint3d ptEnd(m_arrayProposedLevels[i].x, m_arrayProposedLevels[i].y, 0.0);

		if(GetIntersPt2D(ptStart, ptEnd, ptStartParam, ptEndParam, ptResultParam))
		{
			return i;
		}
	}
	return -1;
}

#ifdef _DEBUG //== _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG
void CCutFillCalculatorForXSection::Test()
{
	AcDbObject *pObjE, *pObjP;
	
	if(!GetPLineSelection("\nSelect Existing Profile:", pObjE))
		return;

	if(!GetPLineSelection("\nSelect Proposed Profile:", pObjP))
		return;
	
	if(!(pObjE->isA() != AcDbPolyline::desc()) || (pObjE->isA() != AcDbPolyline::desc()))
	{
		pObjE->close();
		pObjP->close();
	}
	AcGePoint3dArray arrayExisting, arrayProposed;
	{//
		{//
			AcDbPolyline* pEntParam = (AcDbPolyline*)pObjE;
			for(unsigned int i = 0; i < pEntParam->numVerts(); i++)
			{
				AcGePoint2d pt2D;
				pEntParam->getPointAt(i, pt2D);
				arrayExisting.append(AcGePoint3d(pt2D.x, pt2D.y, 0.0));
			}
		}
		{//
			AcDbPolyline* pEntParam = (AcDbPolyline*)pObjP;
			for(unsigned int i = 0; i < pEntParam->numVerts(); i++)
			{
				AcGePoint2d pt2D;
				pEntParam->getPointAt(i, pt2D);
				arrayProposed.append(AcGePoint3d(pt2D.x, pt2D.y, 0.0));
			}
		}
	}
	pObjE->close();
	pObjP->close();

	CCutFillCalculatorForXSection util(arrayExisting, arrayProposed);

	util._Calculate();
}
BOOL CCutFillCalculatorForXSection::GetPLineSelection(const char* pszPrompt, AcDbObject*& pObj)
{
	AcGePoint3d ptTmp;
	ads_name entName;
	AcDbObjectId objId;

	if(RTNORM != ads_entsel(pszPrompt, entName, asDblArray(ptTmp)))
	{
		ads_printf("\nERROR: No entity selected\n");
		return FALSE;
	}
	if(Acad::eOk != acdbGetObjectId(objId, entName))
	{
		return FALSE;
	}
	if(Acad::eOk != acdbOpenObject(pObj, objId, AcDb::kForRead))
	{
		return FALSE;
	}
	if((pObj->isA() != AcDbPolyline::desc()) && (pObj->isA() != AcDb3dPolyline::desc()))
	{
		ads_printf("\nERROR: Entity selected is \"%s\", not a polyline\n", pObj->isA()->dxfName());
		pObj->close();
		return FALSE;
	}
	return TRUE;
}

#endif// _DEBUG //== _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG _DEBUG
/*
(defun C:B()
	(inters a b c d)
)
(defun C:A()
	(setq a (getpoint "\nEdge#1: "))
	(setq b (getpoint a "\nEdge#1: "))
	(setq c (getpoint "\nEdge#2: "))
	(setq d (getpoint c "\nEdge#2: "))
)
*/
