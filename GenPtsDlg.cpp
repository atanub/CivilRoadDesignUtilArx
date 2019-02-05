// GenPtsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GenPtsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGenPtsDlg dialog


CGenPtsDlg::CGenPtsDlg(const BOOL& bIsForInterpolation, const double& fDist, const double& fElev, CWnd* pParent /*=NULL*/)
	: CDialog(CGenPtsDlg::IDD, pParent), m_fDist(fDist), m_fElev(fElev), m_bIsForInterpolation(bIsForInterpolation)
{
	//{{AFX_DATA_INIT(CGenPtsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_fElevInput = 0.0;
}


void CGenPtsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenPtsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenPtsDlg, CDialog)
	//{{AFX_MSG_MAP(CGenPtsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenPtsDlg message handlers

void CGenPtsDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString strMsg;
	
	GetDlgItem(IDC_EDIT1)->GetWindowText(strMsg);
	m_iNoOfPts = atoi(strMsg);
	if(m_iNoOfPts <= 0)
	{
		AfxMessageBox("Invalid number of points specified", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT1))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT1))->SetSel(0, -1);
		return;
	}
	if(!m_bIsForInterpolation)
	{
		GetDlgItem(IDC_EDIT2)->GetWindowText(strMsg);
		m_fElevInput = atof(strMsg);
	}
	
	CDialog::OnOK();
}

BOOL CGenPtsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strMsg;

	strMsg.Format("Distance between selected points : %f\r\nElev difference = %f", m_fDist, m_fElev);
	GetDlgItem(IDC_STATIC_DIST)->SetWindowText(strMsg);
	GetDlgItem(IDC_EDIT2)->ShowWindow(!m_bIsForInterpolation ? SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_STATIC_ELEV)->ShowWindow(!m_bIsForInterpolation ? SW_SHOW:SW_HIDE);
	
	SetWindowText(m_bIsForInterpolation ? "Generate Points(Linear Interpolation)":"Generate Points(Hard Break Line)");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
