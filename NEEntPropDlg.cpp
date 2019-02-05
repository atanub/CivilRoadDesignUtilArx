// NEEntPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ContourApp.h"
#include "NEEntPropDlg.h"
#include "ResourceHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" HWND adsw_acadMainWnd();
/////////////////////////////////////////////////////////////////////////////
// CNEEntPropDlg dialog


CNEEntPropDlg::CNEEntPropDlg(const double& fTxtSize, const double& fArrSize, const short& iLuprec, CWnd* pParent /*=NULL*/)
	: CDialog(CNEEntPropDlg::IDD, pParent), m_fConstTxtSize(fTxtSize), m_fConstArrSize(fArrSize), m_iConstLuprec(iLuprec)
{
	//{{AFX_DATA_INIT(CNEEntPropDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}



void CNEEntPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNEEntPropDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNEEntPropDlg, CDialog)
	//{{AFX_MSG_MAP(CNEEntPropDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNEEntPropDlg message handlers

BOOL CNEEntPropDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strTmp;

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1))->SetPos(1);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1))->SetRange(1, 6);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1))->SetBuddy(GetDlgItem(IDC_EDIT1));

	strTmp.Format("%d", m_iConstLuprec);	GetDlgItem(IDC_EDIT1)->SetWindowText(strTmp);
	strTmp.Format("%f", m_fConstArrSize);	GetDlgItem(IDC_EDIT2)->SetWindowText(strTmp);
	strTmp.Format("%f", m_fConstTxtSize);	GetDlgItem(IDC_EDIT3)->SetWindowText(strTmp);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNEEntPropDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString strTmp;

	GetDlgItem(IDC_EDIT1)->GetWindowText(strTmp);
	m_iLuprec = (short)atoi(strTmp);
	if((m_iLuprec > 6) || (m_iLuprec < 0))
	{
		AfxMessageBox("ERROR: Invalid unit precision", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT1))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT1))->SetSel(0, -1);
		return;
	}

	GetDlgItem(IDC_EDIT2)->GetWindowText(strTmp);
	m_fArrSize = atof(strTmp);
	if(m_fArrSize <= 0.0)
	{
		AfxMessageBox("ERROR: Invalid Arrow Size", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT2))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT2))->SetSel(0, -1);
		return;
	}
	
	GetDlgItem(IDC_EDIT3)->GetWindowText(strTmp);
	m_fTxtSize = atof(strTmp);
	if(m_fTxtSize <= 0.0)
	{
		AfxMessageBox("ERROR: Invalid Text Size", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT3))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT3))->SetSel(0, -1);
		return;
	}
	
	CDialog::OnOK();
}
void CNEEntPropDlg::GetAttrib(double& fTxtSize, double& fArrSize, short& iLuprec)
{
	fTxtSize = m_fTxtSize;
	fArrSize = m_fArrSize;
	iLuprec = m_iLuprec;
}

///
BOOL GetNEEntProperty(double& fTxtSize, double& fArrSize, short& iLuprec)
{
	CWnd* pAcadWnd;
	CTemporaryResourceOverride ResOverride;
	
	pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());

	CNEEntPropDlg Dlg(fTxtSize, fArrSize, iLuprec, pAcadWnd);
	if(Dlg.DoModal() != IDOK)
		return FALSE;

	Dlg.GetAttrib(fTxtSize, fArrSize, iLuprec);
	return TRUE;
}
