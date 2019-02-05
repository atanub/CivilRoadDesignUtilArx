//PLineDataExtractor.CPP
//
#include "StdAfx.h"
#include <Math.h>


#include "CrossSectionProfileDefinition.h"
#include "DrawUtil.h"
#include "Util.h"
#include "DbObjectEx.h"
#include "EntitySelector.h"

//
//CCrossSectionProfileDefinition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCrossSectionProfileDefinition::CCrossSectionProfileDefinition()
{
	m_iIndexOfCenterPoint = -1;
	memset(m_szName, 0, sizeof(m_szName));
}
CCrossSectionProfileDefinition::CCrossSectionProfileDefinition(const AcGePoint3dArray& arrayPoints, const int& iIndexOfCenterPoint, const char* pszName)
{
	m_arrayPoints.append(arrayPoints);
	m_iIndexOfCenterPoint = iIndexOfCenterPoint;
	assert(strlen(pszName) < MAX_PATH);
	strcpy(m_szName, pszName);
}
CCrossSectionProfileDefinition::~CCrossSectionProfileDefinition()
{
}
double CCrossSectionProfileDefinition::getXExtent() const
{
	if(m_arrayPoints.length() <= 1)
	{
		return 0.0;
	}
	return fabs(m_arrayPoints[0].x - m_arrayPoints[m_arrayPoints.length() - 1].x);
}
double CCrossSectionProfileDefinition::getLength() const
{
	if(m_arrayPoints.length() <= 1)
	{
		return 0.0;
	}
	double fLength = 0.0;
	for(int i = 1; i < m_arrayPoints.length(); i++)
	{
		fLength += Dist2D(m_arrayPoints[i - 1], m_arrayPoints[i]);
	}
	return fLength;
}

CCrossSectionProfileDefinition* CCrossSectionProfileDefinition::Define()
{
    AcGePoint3d ptCenter;
	CEntitySelectorEx plineSelector(CEntitySelectorFactory::SelectorType::SinglePolyline2D);
	AcGePoint3dArray arrayVertices;
	int iIndex;
	CCrossSectionProfileDefinition* pDefinition;
	char szName[MAX_PATH];

	pDefinition = 0L;
	
	strcpy(szName, "");
	do
	{
		//Input Name...
		if(RTNORM != ads_getstring(0, "\nEnter Profile Name :", szName))
		{
			ads_printf("\nERROR: Profile Name not provided!");
			return pDefinition;
		}
		if(strlen(szName) <= 0)
		{
			ads_printf("\nInvalid Profile Name not provided...");
		}
	}
	while(strlen(szName) <= 0);
	//Select Polyline...
	const AcDbObjectIdArray* pArray = plineSelector->Select("\nSelect 2D Polyline to define Cross Section Profile (Design):");
	if(pArray == 0L)
		return pDefinition;

	DbObjectEx entity((*pArray)[0]);
	
	AcDbPolyline* pPolyLine = (AcDbPolyline*)entity.Object();
	if(!ExtractVertices(pPolyLine, arrayVertices))
	{
		return pDefinition;
	}

	//Select center point...
	do
	{
		if(RTNORM != ads_getpoint(0L, "\nPick center point of profile:", asDblArray(ptCenter)))
			return pDefinition;
		
		//Check: Center point of profile MUST be on the selected Polyline!
		if(Adesk::kTrue != arrayVertices.find(ptCenter, iIndex))
		{
			iIndex = IsPointOnPolyLine(ptCenter, arrayVertices);
		}
		if(iIndex < 0)
		{
			ads_printf("\nCenter point of profile MUST be on the selected Polyline");
		}
		else
		{
			if(Adesk::kTrue != arrayVertices.contains(ptCenter))
			{
				arrayVertices.insertAt(iIndex, ptCenter);
			}
		}
	}
	while(iIndex < 0);

	//Translate centre of Section to (0.0, 0.0, 0.0)
	ptCenter = arrayVertices[iIndex];
	pDefinition = new CCrossSectionProfileDefinition();
	for(int i = 0; i < arrayVertices.length(); i++)
	{
		const double fX = arrayVertices[i].x - ptCenter.x;
		const double fY = arrayVertices[i].y - ptCenter.y;

		pDefinition->m_arrayPoints.append(AcGePoint3d(fX, fY, 0.0));
	}
	pDefinition->m_iIndexOfCenterPoint = iIndex;
	assert(strlen(szName) < MAX_PATH);
	strcpy(pDefinition->m_szName, szName);

#ifdef _DEBUG
	POLYLINE(pDefinition->m_arrayPoints, "0");
#endif//_DEBUG
	return pDefinition;
}
CCrossSectionProfileDefinition* CCrossSectionProfileDefinition::Find(const char* pszName, const CCrossSectionProfileDefinitionArray& array)
{
	for(int i = 0; i < array.GetSize(); i++)
	{
		if(stricmp(array[i]->m_szName, pszName) == 0)
			return array[i];
	}

	return 0L;
}

//	
//CCrossSectionProfileSetting
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCrossSectionProfileSetting::CCrossSectionProfileSetting(const double& fStartChainage, const double& fEndChainage, const char* pszProfileDefintionName)
{
	m_fStartChainage = fStartChainage;
	m_fEndChainage = fEndChainage;
	strcpy(m_szProfileDefintionName, pszProfileDefintionName);
}
