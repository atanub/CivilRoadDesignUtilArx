#ifndef _DBOBJECTEX_H
#define _DBOBJECTEX_H


//
// DbObjectEx
//
class DbObjectEx
{
// Construction
public:
	DbObjectEx(const AcDbObjectId& objectId, AcDb::OpenMode eOpenMode = AcDb::kForRead);
	~DbObjectEx()
	{
		if(0L != m_pObj)
			m_pObj->close();
	}
public:
	AcDbObject* operator ->()
	{
		if(m_pObj != 0L)
			return m_pObj;

		return 0L;
	}
	AcDbObject* Object()
	{
		if(m_pObj != 0L)
			return m_pObj;

		return 0L;
	}

protected:
	AcDbObject*		m_pObj;
};

#endif //_DBOBJECTEX_H
