// ProtectApp.cpp : implementation file
//

#include "stdafx.h"
#include "Junk.h"
#include "ProtectApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProtectApp

CProtectApp::CProtectApp()
{
	
	CString str;
	char cSysDir[MAX_PATH];
	str.LoadString(IDS_FILENAME);
	if (str.IsEmpty())
		ASSERT(FALSE);
	m_StrFlname = str;
	GetSystemDirectory(cSysDir, MAX_PATH);

	CString strFlName;
	strFlName.Format("%s\\%s",cSysDir,str);
	CFileFind FlFind;
	m_bFileExist = FlFind.FindFile( strFlName,0);


}

BOOL CProtectApp::CreateFile(CString )
{
	if(m_bFileExist)
	{
		TRACE("File already exist\n");
		return FALSE;
	}
}
CProtectApp::~CProtectApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// CProtectApp message handlers


BOOL CProtectApp::Initialize(const BOOL& bIsFirstRun /*= FALSE*/)
{
	if(bIsFirstRun)
	{//Create BIN file in Sys Directory...If successful  then return TRUE
	 // & set m_bCanRunApp to TRUE
		return TRUE/FALSE;
	}
	else
	{//File exists...
		1) 'm_LastAccessDate' should always < Current Time
		2) 'm_InstallationDate' should always < Current Time
		3) 'm_iNoOfAllowableAxessDays' < 'm_iNoOfDaysAccessed'


		if successful 
			1) Set 'm_LastAccessDate'  = Today..
			2) if 
			'm_iNoOfDaysAccessed++

		//Save the file....

		return TRUE/FALSE;
	}
	return FALSE;
}
