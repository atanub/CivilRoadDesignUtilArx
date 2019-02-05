//System Registry Related..
#include "stdafx.h"
#include "ContourApp.h"
#include "SystemRegistryManager.h"
#include "ResourceHelper.h"
#include "Protection.h"
#include "ActivateDlg.h"

#define HKEY_LOCAL_MACHINE_CUSTOM HKEY_CURRENT_USER

//
//CSystemRegistryManager
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSystemRegistryManager::CSystemRegistryManager(const CStringArray& arrayRegisteredCmds) : m_arrayRegisteredCmds(arrayRegisteredCmds)
{
	m_bResetLicenseInfo = false;
}
CSystemRegistryManager::~CSystemRegistryManager()
{
}
bool CSystemRegistryManager::ResetLicenseRelatedData()
{
	m_bResetLicenseInfo = true;
	return m_bResetLicenseInfo;
}


CString CSystemRegistryManager::_MakeRegDBKey()
{
	CTemporaryResourceOverride ResOverride;
	CString strTmp, strRegDBKey;
	const CString strSeparator("\\");
	strRegDBKey = "SOFTWARE";
	
	VERIFY(strTmp.LoadString(IDS_STRING_COMPANY));
	strRegDBKey = strRegDBKey + strSeparator + strTmp;

	VERIFY(strTmp.LoadString(IDS_STRING_SW_NAME));
	strRegDBKey = strRegDBKey + strSeparator + strTmp;

	VERIFY(strTmp.LoadString(IDS_STRING_VERSION));
	strRegDBKey = strRegDBKey + strSeparator + strTmp;
	return strRegDBKey;
}
BOOL CSystemRegistryManager::_IsRegDBOk(const CString& strRegDBKey, CString& strAuthCode)
{
	CTemporaryResourceOverride ResOverride;
	CString strTmp;
	HKEY hKey;
	long lRet;
	DWORD cbData;
	char szAuthCodeRegDB[_MAX_PATH];

	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE_CUSTOM, (LPCSTR)strRegDBKey, 0, KEY_QUERY_VALUE, &hKey);
	if(lRet != ERROR_SUCCESS)
		return FALSE; 

	//Read Authorization Code
	VERIFY(strTmp.LoadString(IDS_STRING_AUTH_CODE_TAG));
    cbData = sizeof(szAuthCodeRegDB);
	lRet = RegQueryValueEx(hKey, (LPCSTR)strTmp, NULL, NULL, (unsigned char*)szAuthCodeRegDB, &cbData);
    if(lRet != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return FALSE; 
    }
	szAuthCodeRegDB[cbData] = '\0';
	strAuthCode = szAuthCodeRegDB;
	RegCloseKey(hKey);
	return TRUE;
}
BOOL CSystemRegistryManager::_IsActivatedBefore(const CString& strRegDBKey, CString& strActCode)
{
	CTemporaryResourceOverride ResOverride;
	HKEY hKey;
	long lRet;
	DWORD cbData;
	CString strTmp;
	char szActCodeRegDB[_MAX_PATH];

	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE_CUSTOM, (LPCSTR)strRegDBKey, 0, KEY_QUERY_VALUE, &hKey);
	if(lRet != ERROR_SUCCESS)
		return FALSE; 

	//Read Activation Key Code
	VERIFY(strTmp.LoadString(IDS_STRING_ACTIVATION_CODE_TAG));
    cbData = sizeof(szActCodeRegDB);
	lRet = RegQueryValueEx(hKey, (LPCSTR)strTmp, NULL, NULL, (unsigned char*)szActCodeRegDB, &cbData);
    if(lRet != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return FALSE; 
    }
	szActCodeRegDB[cbData] = '\0';
	strActCode = szActCodeRegDB;
	RegCloseKey(hKey);
	return TRUE;
}
void CSystemRegistryManager::_Activate(const CString& strACode, const CString& strRegDBKey)
{
	CTemporaryResourceOverride ResOverride;
	HKEY hKey;
	long lRet;
	CString strTmp;

	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE_CUSTOM, (LPCSTR)strRegDBKey, 0, KEY_QUERY_VALUE|KEY_SET_VALUE, &hKey);
	if(lRet != ERROR_SUCCESS)
	{
		ASSERT(FALSE);
		return; 
    }
	VERIFY(strTmp.LoadString(IDS_STRING_ACTIVATION_CODE_TAG));
	lRet = RegSetValueEx(hKey, (LPCSTR)strTmp, 0, REG_BINARY, (unsigned char*)(LPCSTR)strACode, lstrlen((LPCSTR)strACode)+1);
    if(lRet != ERROR_SUCCESS)
	{
		ASSERT(FALSE);
    }
	RegCloseKey(hKey);
}

CApplcationProtector::FaultType CSystemRegistryManager::IsCurInstallationOK()
{
//#ifdef _DEBUG
//	return TRUE;
//#endif //_DEBUG

	CTemporaryResourceOverride ResOverride;
	CString strRegDBKey;
	
	strRegDBKey = _MakeRegDBKey();
	
	{//Check Authorization code...
		CString strAuthCode, strAuthCodeInReg;
		
		strAuthCode.LoadString(IDS_STRING_AUTH_CODE_VAL);
		if(!_IsRegDBOk(strRegDBKey, strAuthCodeInReg))
			return CApplcationProtector::FaultType::InvalidRegistry;
		
		if(strAuthCode != strAuthCodeInReg)
			return CApplcationProtector::FaultType::InvalidLicense;
	}
	{//Check Activation code...
		CString strActCode, strActCodeInReg;
	
		strActCode.LoadString(IDS_STRING_ACTIVATION_CODE_VAL);
		if(!_IsActivatedBefore(strRegDBKey, strActCodeInReg))
		{
			//From dialog box
			if(!GetActivationData())
				return CApplcationProtector::FaultType::UserCancelledActivation;

			_Activate(strActCode, strRegDBKey);
			{//Create file to track uses of this application,,,,,
				CProtection Protection;

				if(!Protection.Initialize(TRUE))
				{
					AfxMessageBox("ERROR: Fatal error occured while in activation process", MB_ICONSTOP);
					return CApplcationProtector::FaultType::InvalidLogFile;
				}
			}
			//
			MakeDemandLoadCompatible(m_arrayRegisteredCmds);
			return CApplcationProtector::FaultType::Success;
		}
		if(strActCode != strActCodeInReg)
			return CApplcationProtector::FaultType::InvalidLicense;
	}
	return CApplcationProtector::FaultType::Success;
}

BOOL CSystemRegistryManager::GetActivationData()
{
	CActivateDlg::GetActivationData();//Verifies user input with the corresponding value in string table
	return TRUE;
}
const char* CSystemRegistryManager::GetLicenseInfo() const
{
	return (LPCSTR)m_strLicenseInfo;
}
			
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Implement Demand Loading........
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Demand Loading........
BOOL CSystemRegistryManager::MakeDemandLoadCompatible(const CStringArray& strArrayCmds)
{
	CTemporaryResourceOverride ResOverride;
	HKEY hAppRootKey;
	DWORD dword;
	CString strAppRegPath, strAppName, strRegPath;
	long iERRCode;

	AcadApp::ErrorStatus ErrStat;

	strAppName.LoadString(IDS_STRING_THIS_ARX_NAME);
	strRegPath.LoadString(IDS_STRING_THIS_ARX_REG_PATH);

	ErrStat = acrxRegisterApp((AcadApp::LoadReasons)(AcadApp::kOnProxyDetection|AcadApp::kOnCommandInvocation), strAppName, strRegPath, 2);
	if(AcadApp::eKeyExists == ErrStat)
		return TRUE;
	if(AcadApp::eOk != ErrStat)
		return FALSE;

	strAppRegPath.Format("%s\\%s", strRegPath, strAppName);
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE_CUSTOM, strAppRegPath, dword, KEY_WRITE, &hAppRootKey) == ERROR_SUCCESS)
	{
		//Assuming rest of the sub-entries are Ok....
		//
		//
		RegCloseKey(hAppRootKey);
		return TRUE;//Already registered....
	}
	else
	{
		if(RegCreateKeyEx(HKEY_LOCAL_MACHINE_CUSTOM, strAppRegPath, 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ|KEY_CREATE_SUB_KEY, NULL, &hAppRootKey, &dword) != ERROR_SUCCESS)
		{
			return FALSE;//ERROR..
		}
		RegCloseKey(hAppRootKey);
	}

	//Open Application Root Key....
	iERRCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE_CUSTOM, strAppRegPath, dword, KEY_WRITE, &hAppRootKey);
	if(iERRCode != ERROR_SUCCESS)
	{
		{//
			LPVOID lpMsgBuf;
			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, NULL, iERRCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);// Display the string.
			AfxMessageBox((char*)lpMsgBuf, MB_OK|MB_ICONINFORMATION);
			LocalFree(lpMsgBuf);
		}
		return FALSE;//ERROR..
	}
	{//Create the "Loader" key
		HKEY hKey;
		CString strAppPath;

		strAppPath = CContourApp::GetThisAppPath();

		RegCreateKeyEx(hAppRootKey, "Loader", 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL, &hKey, &dword);
		//Set the "MODULE" value (Application path)
		RegSetValueEx(hKey, "MODULE", 0, REG_SZ, (LPBYTE)(LPCTSTR)strAppPath, strAppPath.GetLength() + 1);
		RegCloseKey(hKey);
	}
	{//Create the "Commands" key
		HKEY hKey;
		int i;

		RegCreateKeyEx(hAppRootKey, "Commands", 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL, &hKey, &dword);
		for(i = 0; i < strArrayCmds.GetSize(); i++)
		{
			RegSetValueEx(hKey, (LPCSTR)(strArrayCmds[i]), 0, REG_SZ, (const unsigned char *)(LPCSTR)(strArrayCmds[i]), (strArrayCmds[i]).GetLength() + 1);
		}
		RegCloseKey(hKey);
	}
	{//Create the "Name" key
		HKEY hKey;

		RegCreateKeyEx(hAppRootKey, "Name", 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL, &hKey, &dword);
		RegSetValueEx(hKey, strAppName, 0, REG_SZ, (const unsigned char *)(LPCSTR)(strAppName), strAppName.GetLength() + 1);

		RegCloseKey(hKey);
	}
	RegCloseKey(hAppRootKey);
	return TRUE;
}

//////////////////////////
//CSystemRegistryManagerEx
//////////////////////////////////////////////////////////////////////////////
CSystemRegistryManagerEx::CSystemRegistryManagerEx(const CStringArray& arrayRegisteredCmds) : CSystemRegistryManager(arrayRegisteredCmds)
{
	m_pProtectionInterface = new CProtectionInterface();
	m_pApplcationProtector = new CApplcationProtector(CLicenseManager::AppType::ContourARX, m_pProtectionInterface);
}
CSystemRegistryManagerEx::~CSystemRegistryManagerEx()
{
	if(m_bResetLicenseInfo)
	{
		m_pApplcationProtector->ResetLicenseRelatedData(CLicenseManager::AppType::ContourARX);
	}
	delete m_pProtectionInterface;
	delete m_pApplcationProtector;
}
CApplcationProtector::FaultType CSystemRegistryManagerEx::IsCurInstallationOK()
{
	char* pszLicenseInfoForUser;
	CApplcationProtector::FaultType eFaultType;
	
	m_strLicenseInfo = "";
	eFaultType = m_pApplcationProtector->Initialize(&pszLicenseInfoForUser);
	if(CApplcationProtector::FaultType::Success == eFaultType)
	{
		if(0L != pszLicenseInfoForUser) 
		{
			m_strLicenseInfo = pszLicenseInfoForUser;
		}
		MakeDemandLoadCompatible(m_arrayRegisteredCmds);
		delete []pszLicenseInfoForUser;
		return eFaultType;
	}
	if(0L != pszLicenseInfoForUser) 
	{
		m_strLicenseInfo = pszLicenseInfoForUser;
	}

	delete []pszLicenseInfoForUser;
	return eFaultType;
}
