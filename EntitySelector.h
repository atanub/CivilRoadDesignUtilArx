#ifndef _ENTITYSELECTOR_H
#define _ENTITYSELECTOR_H
//
// CEntitySelector
//
class CEntitySelector
{
// Construction
protected:
	CEntitySelector();
public:
	virtual ~CEntitySelector();
public:
	const AcDbObjectIdArray* Select(const char* pszPromptAdd = 0L, const char* pszPromptRemove = 0L);

protected:
	virtual bool	convertSelSetToObjIDArray();
	virtual bool	getUserSelectedEntities();
	virtual bool	isValidEntity(const AcDbObject*);

protected:
	AcDbObjectIdArray	m_arrayObjIDs;
	ads_name			m_selectionSet;
	//
	const char*			m_pszPromptAdd;
	const char*			m_pszPromptRemove;
};

//
// CEntSelectorFor3DPolyline
//
class CEntSelectorFor3DPolyline : public CEntitySelector
{
	friend class CEntitySelectorFactory;
protected:
	CEntSelectorFor3DPolyline()
	{
	}
protected:
	virtual bool	getUserSelectedEntities();
	virtual bool	isValidEntity(const AcDbObject*);
};	
//
// CEntSelectorForSingle3DPolyline
//
class CEntSelectorForSingle3DPolyline : public CEntSelectorFor3DPolyline
{
	friend class CEntitySelectorFactory;
protected:
	CEntSelectorForSingle3DPolyline()
	{
	}
protected:
	virtual bool	getUserSelectedEntities();
};	
//
// CEntSelectorForSingle2DPolyline
//
class CEntSelectorForSingle2DPolyline : public CEntSelectorFor3DPolyline
{
	friend class CEntitySelectorFactory;
protected:
	CEntSelectorForSingle2DPolyline()
	{
	}
protected:
	virtual bool	getUserSelectedEntities();
	virtual bool	isValidEntity(const AcDbObject*);
};	
//
// CEntSelectorSingleLine
//
class CEntSelectorSingleLine : public CEntitySelector
{
	friend class CEntitySelectorFactory;
protected:
	CEntSelectorSingleLine()
	{
	}
protected:
	virtual bool	getUserSelectedEntities();
	virtual bool	isValidEntity(const AcDbObject*);
};	
//
// CEntSelectorMultiplePoints
//
class CEntSelectorMultiplePoints : public CEntitySelector
{
	friend class CEntitySelectorFactory;
protected:
	CEntSelectorMultiplePoints()
	{
	}
protected:
	virtual bool	getUserSelectedEntities();
	virtual bool	isValidEntity(const AcDbObject*);
};	
//
// CEntitySelectorFactory
//
class CEntitySelectorFactory
{
public:
	typedef enum SelectorType
	{
		Polyline3D			,
		SinglePolyline3D	,
		SinglePolyline2D	,
		SingleLine			,
		Points			
	};
// Construction
protected:
	CEntitySelectorFactory()
	{
	}
public:
	static CEntitySelector* GetEntitySelector(SelectorType eType);
};

//
// CEntitySelectorFactory
//
class CEntitySelectorEx
{
// Construction
public:
	CEntitySelectorEx(CEntitySelectorFactory::SelectorType eSelectorType) : m_eSelectorType(eSelectorType)
	{ 
		m_pSelector = 0L;
	}
	~CEntitySelectorEx()
	{
		if(0L != m_pSelector)
			delete m_pSelector;
	}
public:
	CEntitySelector* operator ->()
	{
		if(m_pSelector != 0L)
			return m_pSelector;

		m_pSelector = CEntitySelectorFactory::GetEntitySelector(m_eSelectorType);
		return m_pSelector;
	}

protected:
	CEntitySelector*							m_pSelector;
	const CEntitySelectorFactory::SelectorType	m_eSelectorType;
};

#endif //_ENTITYSELECTOR_H
