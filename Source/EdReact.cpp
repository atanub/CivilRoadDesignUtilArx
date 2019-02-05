// edreact.cpp
/////////////////////////////////////////////////////////


// includes from ObjectARX Wizard
#ifdef _DEBUG
#define _DEBUG_WAS_DEFINED
#undef _DEBUG
#endif
#include "stdafx.h"           // MFC stuff
#ifdef _DEBUG_WAS_DEFINED
#define _DEBUG
#undef _DEBUG_WAS_DEFINED
#endif
#include "acdb.h"             // acdb definitions
#include "adslib.h"           // ads defs
#include "aced.h"             // aced stuff
#include "dbmain.h"           // object and entity reactors
#include "dbdict.h"           // dictionaries
#include "actrans.h"          // transaction manager
#include "resourcehelper.h"   // temporary resource class
#include "rxmfcapi.h"         // ACAD MFC stuff
#include "dbidmap.h"          // id mapping
#include "ContourApp.h"
#include "edreact.h"
#include "DrawUtil.h"
#include "DwgParamObj.h"
// TODO: add your own includes or declarations

// here are the reactors

///////////////////////////////////////////////////////
// editor reactor CEditorReactor
// here are the reactors
void CEditorReactor::commandWillStart(const char* cmdStr)
{
	if(IsCmdReqdReaction(cmdStr))
	{
		DWGPARAMS DwgParams;
		
		if(CDwgParamObj::GetDwgParamData(&DwgParams))
		{
			if(DwgParams.bIsRegistered)
				return;
		}

		//DISABLED ->> DeleteProducedEntities(DwgParams.szAppName);

#ifdef _DEBUG
		{//Show MSG Box
			CString strMsg;
		
			strMsg.Format("\nTRAPPED: About to run command \"%s\"\n", cmdStr);
			ads_printf(strMsg);
		}
#endif //_DEBUG
	}
}

BOOL CEditorReactor::IsCmdReqdReaction(const CString& strCmd)
{
	const char* pszReact[] = {"QUIT", "OPEN", "QSAVE", "WBLOCK", "BLOCK", "DXFOUT", "SAVEAS", "EXPORT", "PLOT", "PRINT"};
	int i;
	for(i = 0; i < sizeof(pszReact) / sizeof(char*); i++)
	{
		if(strCmd.CompareNoCase(pszReact[i]) == 0)
			return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////
// editor reactor CEditorReactor

static CEditorReactor* pCEditorReactor = 0L;

void CEditorReactor::StartEditorReactor()
{
	if(pCEditorReactor == 0L)
	{
		pCEditorReactor = new CEditorReactor();
		acedEditor->addReactor( pCEditorReactor);
	}
}
void CEditorReactor::EndEditorReactor()
{
	if(pCEditorReactor != 0L)
	{
		acedEditor->removeReactor(pCEditorReactor);
		delete pCEditorReactor;
	}
}
