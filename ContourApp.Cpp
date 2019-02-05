// File Name: ContourApp.Cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ContourApp.h"
#include "Resource.h"
#include <rxmfcapi.h>
#include "edreact.h"
#include "SubRoutines.h"
#include "SystemRegistryManager.h"
#include "DrawUtil.h"

/////////////////////////////////////////////////////////////////////////////
// ContourApp
const CString CContourApp::gStrDictionaryName("ATANU_BANIK");
const CString CContourApp::gStrTINDataSexn ("CONTDATA");
const CString CContourApp::gStrDwgParamSexn("DWGPARAM");
const CString CContourApp::m_strConstThisARXName("ContourARX");
//
CStringArray	CContourApp::m_arrayRegisteredCmdGroups;
CStringArray	CContourApp::m_arrayRegisteredCmds;
//
CContourApp*	CContourApp::m_pTheOnlyInstance = 0L;

CContourApp::CContourApp(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	m_pToolBar = 0L;
	m_pToolBarHandler = 0L;
	m_pSystemRegistryManager = 0L;

	CreateToolBar();
	CEditorReactor::StartEditorReactor();
	CExportedSubRoutines::Initialize();
}
CContourApp::~CContourApp()
{
	if(m_pToolBar != 0L)
		delete m_pToolBar;
	
	if(m_pToolBarHandler != 0L)
		delete m_pToolBarHandler;
	
	if(acdbCurDwg())
	{
		CMDIFrameWnd *pFrame;
		pFrame = acedGetAcadFrame();
		pFrame->RecalcLayout(); // need this step to force some MFC initialization
	}
	if(m_pSystemRegistryManager != 0L)
		delete m_pSystemRegistryManager;

	CEditorReactor::EndEditorReactor();
}
CContourApp* CContourApp::Initialize(HINSTANCE hInstance)
{
	CString strLicenseInfo;

	if(0L != m_pTheOnlyInstance)
		return m_pTheOnlyInstance;

	m_pTheOnlyInstance = new CContourApp(hInstance);
	{//Set up logger for Protection Process debugging
		char szModuleName[_MAX_PATH];
		CFileFind objFileFind;
		CString strLogFile;
	
		::GetModuleFileName(m_pTheOnlyInstance->m_hInstance, szModuleName, _MAX_PATH);
		objFileFind.FindFile(szModuleName);
		objFileFind.FindNextFile();
		strLogFile.Format("%s\\%s", objFileFind.GetRoot(), CApplcationProtector::GetDebugLogFileName(CLicenseManager::AppType::ContourARX));
		CProtectionDebugging::StartDebugLog(strLogFile, (EXPERT() > 4));
		if(EXPERT() > 4)
		{
			ads_printf("\nApplication log [%s] has been created. Please contact developer with the log file, if you are facing any problem.", strLogFile);
		}
	}

	{//Licensing....
		CApplcationProtector::FaultType eFaultType;
		CContourApp::Instance()->m_pSystemRegistryManager = new CSystemRegistryManagerEx(m_arrayRegisteredCmds);
		
		eFaultType = CContourApp::Instance()->m_pSystemRegistryManager->IsCurInstallationOK();
		if(CApplcationProtector::FaultType::Success != eFaultType)
		{
			int iRet;

			ads_printf("... Load failed!\nPossible cause: Invalid license information!\n");
			if(CApplcationProtector::FaultType::UserCancelledActivation != eFaultType)
			{
				iRet = MessageBox(adsw_acadMainWnd(), "Failed to load application\nPossible cause: Invalid license information!\nWould you like to reset License Info ?\nNOTE: Reseting License Info will require new license key for proper functioning.", CContourApp::m_strConstThisARXName, MB_ICONSTOP|MB_YESNO);
				if(IDYES == iRet)
				{
					CContourApp::ResetLicenseInfo();
					AfxMessageBox("License info. removed. You will be prompted to provide license key file after restarting AutoCAD.", MB_ICONINFORMATION);
				}
			}
			return 0L;
		}
		if(0L != CContourApp::Instance()->m_pSystemRegistryManager->GetLicenseInfo())
		{
			strLicenseInfo = CContourApp::Instance()->m_pSystemRegistryManager->GetLicenseInfo();
		}
	}
	m_pTheOnlyInstance->m_bHasValidLicense = TRUE;
	m_pTheOnlyInstance->m_strLicenseInfo = strLicenseInfo;

	return m_pTheOnlyInstance;
}
BOOL CContourApp::ResetLicenseInfo()
{
	return CContourApp::Instance()->m_pSystemRegistryManager->ResetLicenseRelatedData();
}
BOOL CContourApp::GetResetLicenseFlag()
{
	CContourApp* pTheOnlyInstance;

	pTheOnlyInstance = CContourApp::Instance();
	return pTheOnlyInstance->m_pSystemRegistryManager->GetResetLicenseFlag();
}
void CContourApp::UnInitialize()
{
	delete m_pTheOnlyInstance;
	{//Remove all the cmd groups
		int i;

		for(i = 0; i < m_arrayRegisteredCmdGroups.GetSize(); i++)
		{
			acedRegCmds->removeGroup(m_arrayRegisteredCmdGroups[i]);
		}
	}
	CProtectionDebugging::EndDebugLog();
}
BOOL CContourApp::HasValidLicense()
{
	return CContourApp::Instance()->m_bHasValidLicense;
}
const char* CContourApp::GetLicenseInfo()
{
	return (LPCSTR)(CContourApp::Instance()->m_strLicenseInfo);
}

void CContourApp::SendCmdToAutoCAD(CString& strCmd)
{
	COPYDATASTRUCT cmdMsg;

	cmdMsg.dwData = (DWORD)1;
	cmdMsg.cbData = (DWORD)_tcslen(strCmd) + 1;
	cmdMsg.lpData = strCmd.GetBuffer(strCmd.GetLength()+1) ;                   

	SendMessage(adsw_acadMainWnd(), WM_COPYDATA, (WPARAM)adsw_acadMainWnd(), (LPARAM)&cmdMsg);
}
CString CContourApp::GetThisAppPath()
{
	CTemporaryResourceOverride ResOverride;
	CString strPath;
	char szPath[_MAX_PATH+1];
	
	if(GetModuleFileName(CTemporaryResourceOverride::m_hInstanceDefault, szPath, _MAX_PATH) > 0)
	{
		strPath = szPath;
		return strPath;
	}
	strPath = "!!!ERROR!!!";
	return strPath;
}

BOOL CContourApp::IsValidACADSymName(const char* pszSymName)
{
	int i, iLim;
	BOOL bIsValid;
	iLim = strlen(pszSymName);
	if(iLim <= 0) return FALSE;
	for(i = 0; i < iLim; i++)
	{
		const char chMemb = *(pszSymName + i);
		bIsValid = (((chMemb <= '9') && (chMemb >= '0')) ||
					((chMemb <= 'z') && (chMemb >= 'a')) || ((chMemb <= 'Z') && (chMemb >= 'A')) ||
					((chMemb == '$') || (chMemb == '_') || (chMemb == '-'))
				   );
		if(!bIsValid)
			return FALSE;
	}
	return TRUE;
}
void CContourApp::CreateToolBar()
{
	CTemporaryResourceOverride Res;
	CMDIFrameWnd *pAcadFrame;
	BOOL bFlag;

	ASSERT(m_pToolBar == 0L);
	m_pToolBar = new  CContourToolbar();
	if(m_pToolBar == 0L)
	{
		ads_alert("ERROR: Failed to create toolbar object");
		return;
	}
	
	pAcadFrame = acedGetAcadFrame();
	pAcadFrame->EnableDocking(CBRS_ALIGN_ANY);	// Enable AutoCAD's mainframe for docking
	pAcadFrame->RecalcLayout();					// need this step to force some MFC initialization

	bFlag = m_pToolBar->Create(pAcadFrame, WS_CHILD|WS_VISIBLE|CBRS_SIZE_DYNAMIC|CBRS_FLYBY|CBRS_TOP|CBRS_TOOLTIPS);
	if(!bFlag)
	{
		ads_alert("ERROR: Failed to create toolbar object");
		delete m_pToolBar;
		m_pToolBar = NULL;
		return;
	}
	bFlag = m_pToolBar->LoadToolBar(IDR_TOOLBAR_CONTOUR_NEW);
	if(!bFlag)
	{
		ads_alert("ERROR: Failed to load toolbar object");
		delete m_pToolBar;
		m_pToolBar = NULL;
		return;
	}
	// create toolbar handler
	m_pToolBarHandler = new CTBHandler();
	
	if(m_pToolBarHandler == 0L)
	{
		ads_alert("ERROR: Failed to create toolbar object");
		delete m_pToolBar;
		m_pToolBar = NULL;
		return;
	}
	bFlag = m_pToolBarHandler->Create(NULL, NULL, WS_CHILD|WS_MINIMIZE, CRect(0,0,1,1), pAcadFrame, 10);
	if(!bFlag)
	{
		ads_alert("ERROR: Failed to create toolbar object");
		delete m_pToolBarHandler;
		m_pToolBarHandler = 0L;
		delete m_pToolBar;
		m_pToolBar = NULL;
		return;
	}
	m_pToolBar->GetToolBarCtrl().SetOwner(m_pToolBarHandler);
	/////////////////////////////////////////////////////////////////////////	
	m_pToolBar->EnableDocking(CBRS_ALIGN_ANY);
	m_pToolBar->SetWindowText(_T("Contour"));
	pAcadFrame->DockControlBar(m_pToolBar, AFX_IDW_DOCKBAR_BOTTOM);
	pAcadFrame->ShowControlBar(m_pToolBar, TRUE, TRUE);
}

void CContourApp::RegisterCmd(const char* pszCmd)
{
	int i;

	for(i = 0; i < m_arrayRegisteredCmds.GetSize(); i++)
	{
		if(m_arrayRegisteredCmds[i].CompareNoCase(pszCmd) == 0)
			return;//Already present
	}
	m_arrayRegisteredCmds.Add(pszCmd);
}
void CContourApp::RegisterCmdGroup(const char* pszCmd)
{
	int i;

	for(i = 0; i < m_arrayRegisteredCmdGroups.GetSize(); i++)
	{
		if(m_arrayRegisteredCmdGroups[i].CompareNoCase(pszCmd) == 0)
			return;//Already present
	}
	m_arrayRegisteredCmdGroups.Add(pszCmd);
}

