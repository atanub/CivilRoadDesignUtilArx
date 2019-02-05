// Protection.cpp : implementation file
//

#include "stdafx.h"           // MFC stuff
#include "Protection.h"
#include "adslib.h"           // ads defs

const CString	CProtection::m_strFileName = CString("SurvSys.DLX");
const int		CProtection::m_iNoOfAllowableAxessDays = 730;// ************ TO CHANGE ************ //

CProtection::CProtection()
{
	m_bCanRunApp = FALSE;
	m_iNoOfDaysAccessed = -1;
}
CProtection::~CProtection()
{
}

//Creates the file with default data...
BOOL CProtection::_ReadFile(const CString& strFile)
{
	CFile File;
	PROTECTIONDATA Rec;

	if(!File.Open(strFile, CFile::modeRead|CFile::typeBinary))
		return FALSE;

	if(sizeof(PROTECTIONDATA) != File.Read(&Rec, sizeof(PROTECTIONDATA)))
		return FALSE;
	
	//Set Member Vars....
	m_InstallationDate = (DATE)(Rec.fInstallDate);
	m_LastAccessDate = (DATE)(Rec.fLastAccessDate);
	m_iNoOfDaysAccessed = Rec.iNoOfDaysAccessed;

	return TRUE;
}
BOOL CProtection::_WriteFile(const CString& strFile, const BOOL& bWithDefaultData /*= FALSE*/)
{
	CFile File;
	PROTECTIONDATA Rec;

	if(bWithDefaultData)
	{
		COleDateTime CurDateTime;
		
		CurDateTime = COleDateTime::GetCurrentTime();
		Rec.fInstallDate = (DATE)CurDateTime;
		Rec.fLastAccessDate = (DATE)CurDateTime;
		Rec.iNoOfDaysAccessed = 0;
	}
	else
	{
		Rec.fInstallDate = (DATE)(m_InstallationDate);
		Rec.fLastAccessDate = (DATE)(m_LastAccessDate);
		Rec.iNoOfDaysAccessed = m_iNoOfDaysAccessed;
	}
	if(!File.Open(strFile, CFile::modeWrite|CFile::modeCreate|CFile::typeBinary))
		return FALSE;
	
	File.Write(&Rec, sizeof(PROTECTIONDATA));
	return TRUE;
}

BOOL CProtection::_GetFileName(CString& strPath)
{
	char szSysDir[_MAX_PATH + 1];

	if(GetSystemDirectory(szSysDir, _MAX_PATH) == 0)
		return FALSE; 
 
	strPath = CString(szSysDir) + CString("\\") + CProtection::m_strFileName;
	return TRUE;
}
BOOL CProtection::_IsFileExists(const CString& strFile)
{
	CFileFind FileFind;

	return FileFind.FindFile(strFile);
}

BOOL CProtection::Initialize(const BOOL& bIsFirstRun /*= FALSE*/)
{
	COleDateTime CurDateTime;
	COleDateTimeSpan TimeElapsed;
	CString strDataFile;

	
	CurDateTime = COleDateTime::GetCurrentTime();
	m_bCanRunApp = FALSE;
	if(!CProtection::_GetFileName(strDataFile))
		return FALSE;

	if(bIsFirstRun)
	{//Create BIN file in Sys Directory...if doesn't exists
		if(!CProtection::_IsFileExists(strDataFile))
		{
			m_bCanRunApp = _WriteFile(strDataFile, TRUE);
			return m_bCanRunApp;
		}
	}
	{//File exists...so check different criteria....
		//Read the file from Disk....
		if(!CProtection::_IsFileExists(strDataFile))
		{
			ads_printf("\nLock file doesn't exist!");
			return FALSE;
		}

		if(!_ReadFile(strDataFile))
		{
			ads_printf("\nLock file doesn't exist!");
			return FALSE;
		}

		//Check the values......
		if(m_InstallationDate > CurDateTime)
		{
			ads_printf("\nInstallation date is after today!");
			return FALSE;
		}
		if(m_LastAccessDate > CurDateTime)
		{
			ads_printf("\nLast access date is after today!");
			return FALSE;
		}

		if(! ((m_LastAccessDate.GetYear() == CurDateTime.GetYear()) && (m_LastAccessDate.GetMonth() == CurDateTime.GetMonth()) && (m_LastAccessDate.GetDay() == CurDateTime.GetDay())))
		{
			m_LastAccessDate = CurDateTime;
		}
		TimeElapsed = CurDateTime - m_InstallationDate;
		m_iNoOfDaysAccessed = TimeElapsed.GetDays();

#ifdef _DEBUG
		{
			CString strMsg;
			
			strMsg.Format("Accessed %d days, allowable %d days", m_iNoOfDaysAccessed, m_iNoOfAllowableAxessDays);
			ads_printf("\n%s\n", (LPCSTR)strMsg);
			//AfxMessageBox(strMsg, MB_ICONINFORMATION);
		}
#endif //_DEBUG
		ads_printf("\nWelcome to ContourARX\n");
		if(m_iNoOfDaysAccessed > m_iNoOfAllowableAxessDays)
			return FALSE;
		
		//Save the file....
		m_bCanRunApp = _WriteFile(strDataFile);
		
		return m_bCanRunApp;
	}
	return FALSE;
}
CString CProtection::GetDispStr()
{
	if(m_iNoOfDaysAccessed >= 0)
	{
		CString strDate, strTmp;
		
		strDate = m_InstallationDate.Format("%d %B'%Y");
		strTmp.Format("Installed on : %s($%d$), Accessd %d# of Days", (LPCSTR)strDate, CProtection::m_iNoOfAllowableAxessDays, m_iNoOfDaysAccessed);
		return strTmp;
	}
	return CString("-- Not Initialized yet --");
}
