// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ContourApp.h"
#include "LicenseKeySelectionDlg.h"
#include "ResourceHelper.h"

extern "C" HWND adsw_acadMainWnd();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLicenseKeySelectionDlg dialog


CLicenseKeySelectionDlg::CLicenseKeySelectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLicenseKeySelectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLicenseKeySelectionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLicenseKeySelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLicenseKeySelectionDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLicenseKeySelectionDlg, CDialog)
	//{{AFX_MSG_MAP(CLicenseKeySelectionDlg)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnButtonFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLicenseKeySelectionDlg message handlers

void CLicenseKeySelectionDlg::OnOK() 
{
	CString strFile;

	GetDlgItem(IDC_EDIT1)->GetWindowText(strFile);
	if(strlen(strFile) <= 0)
	{
		AfxMessageBox("Invalid License file name provided!", MB_ICONSTOP);
		GetDlgItem(IDC_BUTTON_FILE)->SetFocus();
		return;
	}
	{//
		char* pszLicenseInfo;
		CString	strInfo;

		CLicenseManager::ReadLiceseInfo(strFile, &pszLicenseInfo);
		m_strLicenseKey = "";
		strInfo = pszLicenseInfo;
		delete []pszLicenseInfo;
		//
		CLicenseManager objLicenseManager(CLicenseManager::AppType::ContourARX, 0, 0);
		if(!objLicenseManager.CheckValidity(strInfo))
		{
			//AfxMessageBox("Invalid License information provided!", MB_ICONSTOP);
			GetDlgItem(IDC_BUTTON_FILE)->SetFocus();
			return;
		}
		m_strLicenseKey = strInfo;
	}
	
	CDialog::OnOK();
}

void CLicenseKeySelectionDlg::OnButtonFile() 
{
	CString strFile;
	
	{//
		DWORD dwFlags;
		const CString strConstFOpenFilter = CString("License Key Files (*.TXT)|*.TXT||");

		dwFlags = OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
		CFileDialog Dlg(TRUE, 0L, 0L, dwFlags, strConstFOpenFilter);
		if(Dlg.DoModal() == IDOK)
		{
			strFile = Dlg.GetPathName();
			GetDlgItem(IDC_EDIT1)->SetWindowText(strFile);
		}
	}
}

///
// 
const char* CLicenseKeySelectionDlg::GetKey()
{
	CWnd* pAcadWnd;
	CTemporaryResourceOverride ResOverride;
	
	pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());

	CLicenseKeySelectionDlg Dlg(pAcadWnd);
	if(IDOK == Dlg.DoModal())
		return Dlg.GetLicenseKey();
		
	return 0L;
}
