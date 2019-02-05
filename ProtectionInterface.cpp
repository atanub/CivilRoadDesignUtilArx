#include "stdafx.h"
#include "ContourApp.h"
#include "ProtectionInterface.h"
#include "LicenseKeySelectionDlg.h"

//
// CProtectionInterface
/////////////////////////////////////////////////////////////////////////////
bool CProtectionInterface::GetLicenseKeyFromUser(char*& pszKey) const
{
	const char* pszLicenseKey = CLicenseKeySelectionDlg::GetKey();
	
	if(0L == pszLicenseKey)
	{
		return false;
	}
	pszKey = new char[strlen(pszLicenseKey) + 1];
	strcpy(pszKey, pszLicenseKey);

	return true;
}