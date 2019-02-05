// resourcehelper.cpp
/////////////////////////////////////////////////////////

#ifdef _DEBUG
#define _DEBUG_WAS_DEFINED
#undef _DEBUG
#endif
#include "stdafx.h"
#ifdef _DEBUG_WAS_DEFINED
#define _DEBUG
#undef _DEBUG_WAS_DEFINED
#endif

#include "ResourceHelper.h"

// Temporary Resource Override
HINSTANCE CTemporaryResourceOverride::m_hInstanceDefault = NULL;

void CTemporaryResourceOverride::SetDefaultResource(HINSTANCE hInstDefault)
{
  ASSERT(hInstDefault != NULL);
  m_hInstanceDefault = hInstDefault;
}

void CTemporaryResourceOverride::CommonConstruction(HINSTANCE hInst)
{
	m_hInstanceOld = AfxGetResourceHandle();
	AfxSetResourceHandle(hInst);
}

/*
CTemporaryResourceOverride::CTemporaryResourceOverride(HINSTANCE hInstNew)
{
	ASSERT(hInstNew != NULL);
	CommonConstruction(hInstNew);
}
*/
CTemporaryResourceOverride::CTemporaryResourceOverride()
{
	ASSERT(m_hInstanceDefault != NULL);
	CommonConstruction(m_hInstanceDefault);
}

CTemporaryResourceOverride::~CTemporaryResourceOverride()
{
	AfxSetResourceHandle(m_hInstanceOld);
}


