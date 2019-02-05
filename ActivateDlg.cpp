// ActivateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ActivateDlg.h"
#include "ResourceHelper.h"

extern "C" HWND adsw_acadMainWnd();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActivateDlg dialog


CActivateDlg::CActivateDlg(const CString& strMsg, const CString& strACode, CWnd* pParent /*=NULL*/)
	: CDialog(CActivateDlg::IDD, pParent), m_ConstStrMsg(strMsg), m_ConstActCode(strACode)
{
	//{{AFX_DATA_INIT(CActivateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
void CActivateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CActivateDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CActivateDlg, CDialog)
	//{{AFX_MSG_MAP(CActivateDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActivateDlg message handlers

BOOL CActivateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	GetDlgItem(IDC_STATIC_MSG)->SetWindowText(m_ConstStrMsg);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CActivateDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString strACode;
	static int iTry = 0;

	if(iTry > 2)
	{
		AfxMessageBox("Failed to activate application", MB_ICONSTOP);
		EndDialog(IDCANCEL);
		return;
	}
	GetDlgItem(IDC_EDIT)->GetWindowText(strACode);
	if(strACode.Compare(m_ConstActCode) != 0)
	{
		AfxMessageBox("Invalid activation key specified", MB_ICONSTOP);
		iTry++;
		GetDlgItem(IDC_EDIT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT))->SetSel(0, -1);
		return;
	}

	CDialog::OnOK();
}
////////
BOOL CActivateDlg::GetActivationData()
{
	const CString strWelcomeMsg("Contour Application© is running first time in your system.\nPlease enter the activation code\nThis software needs to be activated immidiate after installation");
	CString strConstActCode;
	CWnd* pAcadWnd;
	CTemporaryResourceOverride ResOverride;
	
	pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());
	VERIFY(strConstActCode.LoadString(IDS_STRING_ACTIVATION_CODE_VAL));

	CActivateDlg Dlg(strWelcomeMsg, strConstActCode, pAcadWnd);
	return (Dlg.DoModal() == IDOK);
}