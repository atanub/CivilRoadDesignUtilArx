// LicenseIssuer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ContourApp.h"
#include "AboutDlg.h"
#include "ResourceHelper.h"

extern "C" HWND adsw_acadMainWnd();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg
CAboutDlg::CAboutDlg(const char* pszLicenseInfoForUser, CWnd* pParentWnd) : CDialog(CAboutDlg::IDD, pParentWnd), m_pszLicenseInfoForUser(pszLicenseInfoForUser)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//ON_BN_CLICKED(IDC_BUTTON_ME, OnDeveloper)
	ON_WM_RBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BUTTON_LICENSE_RESET, OnButtonLicenseReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	const CString strLicenceInfoFmt("This Product is licensed to :\r\t%s,\r\t%s");
	
	CDialog::OnInitDialog();
	CString strLicenceInfo, strToCompany, strAboutMeButtonCaption, strToCompanyAddress;

	strToCompany.LoadString(IDS_CLIENT_NAME);
	strToCompanyAddress.LoadString(IDS_CLIENT_ADDRESS);
	strLicenceInfo.Format(strLicenceInfoFmt, strToCompany, strToCompanyAddress);

	// TODO: Add extra initialization here
	GetDlgItem(IDC_EDIT_DISP_INSTALL_INFO)->SetWindowText(m_pszLicenseInfoForUser);
	//GetDlgItem(IDC_EDIT_DISP_INSTALL_INFO)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LICENSE)->SetWindowText(strLicenceInfo);

	GetDlgItem(IDC_BUTTON_LICENSE_RESET)->EnableWindow(!CContourApp::GetResetLicenseFlag());
	return FALSE;
}
void CAboutDlg::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	if((MK_CONTROL & nFlags) && (MK_SHIFT & nFlags))
	{
		if(GetDlgItem(IDC_EDIT_DISP_INSTALL_INFO)-> IsWindowVisible())
			GetDlgItem(IDC_EDIT_DISP_INSTALL_INFO)->ShowWindow(SW_HIDE);
		else
			GetDlgItem(IDC_EDIT_DISP_INSTALL_INFO)->ShowWindow(SW_SHOW);
	}
	
	CDialog::OnRButtonDblClk(nFlags, point);
}
void CAboutDlg::OnDeveloper()
{
	CAboutMeDlg Dlg(this);
	
	Dlg.DoModal();
}
//
/////////////////////////////////////////////////////////////////////////////
// CAboutMeDlg dialog
CAboutMeDlg::CAboutMeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutMeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutMeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
BOOL CAboutMeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//
	return TRUE;
}

void CAboutMeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutMeDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(CAboutMeDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutMeDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CAboutDlg::ShowAboutDialog()
{
	CWnd* pAcadWnd;
	CTemporaryResourceOverride ResOverride;
	
	pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());

	CAboutDlg Dlg(CContourApp::GetLicenseInfo(), pAcadWnd);
	Dlg.DoModal();
}

void CAboutDlg::OnButtonLicenseReset() 
{
	if(AfxMessageBox("This will reset the license info.\nAre you sure to proceed ?", MB_ICONWARNING|MB_YESNO) != IDYES)
		return;
	CContourApp::ResetLicenseInfo();
	AfxMessageBox("License info. removed. You will be prompted for license file location after restarting AutoCAD", MB_ICONINFORMATION);
	GetDlgItem(IDC_BUTTON_LICENSE_RESET)->EnableWindow(!CContourApp::GetResetLicenseFlag());
}
