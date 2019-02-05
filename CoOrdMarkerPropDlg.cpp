// CoOrdMarkerPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ContourApp.h"
#include "CoOrdMarkerPropDlg.h"
#include "ResourceHelper.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" HWND adsw_acadMainWnd();
/////////////////////////////////////////////////////////////////////////////
// CCoOrdMarkerPropDlg dialog


CCoOrdMarkerPropDlg::CCoOrdMarkerPropDlg(const double& fTxtSize, const double& fMarkerScale, const double& fAngInRad, const short& iLuprec, CWnd* pParent /*=NULL*/)
	: CDialog(CCoOrdMarkerPropDlg::IDD, pParent), m_fConstTxtSize(fTxtSize), m_fConstMarkerScale(fMarkerScale), m_fConstAngleInRad(fAngInRad), m_iConstLuprec(iLuprec)
{
	//{{AFX_DATA_INIT(CCoOrdMarkerPropDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}



void CCoOrdMarkerPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCoOrdMarkerPropDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCoOrdMarkerPropDlg, CDialog)
	//{{AFX_MSG_MAP(CCoOrdMarkerPropDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoOrdMarkerPropDlg message handlers

BOOL CCoOrdMarkerPropDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strTmp;

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1))->SetPos(1);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1))->SetRange(1, 6);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1))->SetBuddy(GetDlgItem(IDC_EDIT1));

	{//
		UDACCEL AccelRec;
	
		AccelRec.nSec = 1;
		AccelRec.nInc = 5;

		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN2))->SetAccel(1, &AccelRec);

		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN2))->SetPos(1);
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN2))->SetRange(0, 359);
		((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN2))->SetBuddy(GetDlgItem(IDC_EDIT4));
	}

	strTmp.Format("%d", m_iConstLuprec);			GetDlgItem(IDC_EDIT1)->SetWindowText(strTmp);
	strTmp.Format("%f", m_fConstMarkerScale);		GetDlgItem(IDC_EDIT2)->SetWindowText(strTmp);
	strTmp.Format("%f", m_fConstTxtSize);			GetDlgItem(IDC_EDIT3)->SetWindowText(strTmp);
	strTmp.Format("%f", RTOD(m_fConstAngleInRad));	GetDlgItem(IDC_EDIT4)->SetWindowText(strTmp);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCoOrdMarkerPropDlg::OnOK() 
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
	m_fMarkerScale = atof(strTmp);
	if((m_fMarkerScale > 1.0) || (m_fMarkerScale < 0.0))
	{
		AfxMessageBox("ERROR: Invalid Marker Scale", MB_ICONSTOP);
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
	
	GetDlgItem(IDC_EDIT4)->GetWindowText(strTmp);
	m_fAngleInDeg = atof(strTmp);
	if(m_fTxtSize <= 0.0)
	{
		AfxMessageBox("ERROR: Invalid rotation angle", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT4))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT4))->SetSel(0, -1);
		return;
	}
	
	CDialog::OnOK();
}
void CCoOrdMarkerPropDlg::GetAttrib(double& fTxtSize, double& fMarkerScale, double& fAngInRad, short& iLuprec)
{
	fTxtSize = m_fTxtSize;
	fMarkerScale = m_fMarkerScale;
	fAngInRad = DTOR(m_fAngleInDeg);
	iLuprec = m_iLuprec;
}

///
BOOL GetCoOrdMarkerProperty(double& fTxtSize, double& fMarkerScale, double& fAngInRad, short& iLuprec)
{
	CWnd* pAcadWnd;
	CTemporaryResourceOverride ResOverride;
	
	pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());

	CCoOrdMarkerPropDlg Dlg(fTxtSize, fMarkerScale, fAngInRad, iLuprec, pAcadWnd);
	if(Dlg.DoModal() != IDOK)
		return FALSE;

	Dlg.GetAttrib(fTxtSize, fMarkerScale, fAngInRad, iLuprec);
	return TRUE;
}
