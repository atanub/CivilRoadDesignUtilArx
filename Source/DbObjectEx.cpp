#include "stdafx.h"
#include "DrawUtil.h"
#include "Util.h"
#include "DbObjectEx.h"

//
// DbObjectEx
//
DbObjectEx::DbObjectEx(const AcDbObjectId& objectId, AcDb::OpenMode eOpenMode /*= AcDb::kForRead*/)
{
	if(Acad::eOk != acdbOpenObject(m_pObj, objectId, eOpenMode))
	{
		m_pObj = 0L;
		assert(false);
	}
}