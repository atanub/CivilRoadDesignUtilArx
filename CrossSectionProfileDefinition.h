//CrossSectionProfileDefinition.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _CROSSSECTIONPROFILEDEFINITION_H_
#define _CROSSSECTIONPROFILEDEFINITION_H_

#include <DbPl.h>
#include "Structures.h"

class CCrossSectionProfileDefinition;
class CCrossSectionProfileSetting;
typedef CTypedPtrArray<CPtrArray, CCrossSectionProfileDefinition*> CCrossSectionProfileDefinitionArray;
typedef CTypedPtrArray<CPtrArray, CCrossSectionProfileSetting*> CCrossSectionProfileSettingArray;

//	
//CCrossSectionProfileDefinition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CCrossSectionProfileDefinition
{
public:
	CCrossSectionProfileDefinition();
	CCrossSectionProfileDefinition(const AcGePoint3dArray& arrayPoints, const int& iIndexOfCenterPoint, const char* pszName);
	~CCrossSectionProfileDefinition();


public:
	const AcGePoint3dArray&	getVertices() const 
	{
		return m_arrayPoints;
	}
	int	getIndexOfCenterPoint() const
	{
		return m_iIndexOfCenterPoint;
	}
	const char*	getName() const
	{
		return m_szName;
	}
	double getXExtent() const;
	double getLength() const;

public:
	static CCrossSectionProfileDefinition* Define();
	static CCrossSectionProfileDefinition* Find(const char* pszName, const CCrossSectionProfileDefinitionArray& array);


protected:
	AcGePoint3dArray	m_arrayPoints;
	int					m_iIndexOfCenterPoint;
	char				m_szName[MAX_PATH];
};


//	
//CCrossSectionProfileSetting
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CCrossSectionProfileSetting
{
public:
	CCrossSectionProfileSetting(const double& fStartChainage, const double& fEndChainage, const char* szProfileDefintionName);
	~CCrossSectionProfileSetting()
	{
	}

public:
	double	getStartChainage() const
	{
		return m_fStartChainage;
	}
	double	getEndChainage() const
	{
		return m_fEndChainage;
	}
	const char*	getProfileDefintionName() const
	{
		return m_szProfileDefintionName;
	}

protected:
	double	m_fStartChainage;
	double	m_fEndChainage;
	char	m_szProfileDefintionName[MAX_PATH];
};

#endif //_CROSSSECTIONPROFILEDEFINITION_H_
