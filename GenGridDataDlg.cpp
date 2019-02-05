// GenGridDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ContourApp.h"       // the Application
#include "GenGridDataDlg.h"
#include "ResourceHelper.h"
#include "DrawUtil.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" HWND adsw_acadMainWnd();
/////////////////////////////////////////////////////////////////////////////
// CGenGridDataDlg dialog

CGenGridDataDlg::CGenGridDataDlg(const double& fWidth, const double& fHeight, const double& fXRoff, const double& fYRoff, const BOOL& bFlag, const CString& strPtLyr, const CString& strLineLyr, CWnd* pParent /*=NULL*/)
	: CDialog(CGenGridDataDlg::IDD, pParent), m_fWidth(fWidth), m_fHeight(fHeight), m_fXROff(fXRoff), m_fYROff(fYRoff), m_bDrawGrid(bFlag), m_strPtLyr(strPtLyr), m_strLineLyr(strLineLyr)
{

	//{{AFX_DATA_INIT(CGenGridDataDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CGenGridDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenGridDataDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenGridDataDlg, CDialog)
	//{{AFX_MSG_MAP(CGenGridDataDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenGridDataDlg message handlers

void CGenGridDataDlg::OnOK() 
{
	CString strTmp;
	
	GetDlgItem(IDC_EDIT1)->GetWindowText(strTmp);
	m_fWidth = atof(strTmp);
	if(m_fWidth <= 0.0)
	{
		AfxMessageBox("ERROR: Invalid Width", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT1))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT1))->SetSel(0, -1);
		return;
	}
	
	GetDlgItem(IDC_EDIT2)->GetWindowText(strTmp);
	m_fHeight = atof(strTmp);
	if(m_fHeight <= 0.0)
	{
		AfxMessageBox("ERROR: Invalid Height", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT2))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT2))->SetSel(0, -1);
		return;
	}
	GetDlgItem(IDC_EDIT_X_ROFF)->GetWindowText(strTmp);
	m_fXROff = atof(strTmp);
	if(m_fXROff < 0.0)
	{
		AfxMessageBox("ERROR: Invalid value for X-Ordinate round off factor", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT_X_ROFF))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_X_ROFF))->SetSel(0, -1);
		return;
	}
	if(!CGenGridDataDlg::CheckROffValue(m_fWidth, m_fXROff))
	{
		AfxMessageBox("ERROR: Round off factor is greater than grid cell width,\nor grid cell width is not divisible with Round off factor", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT_X_ROFF))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_X_ROFF))->SetSel(0, -1);
		return;
	}
	GetDlgItem(IDC_EDIT_Y_ROFF)->GetWindowText(strTmp);
	m_fYROff = atof(strTmp);
	if(m_fYROff < 0.0)
	{
		AfxMessageBox("ERROR: Invalid value for Y-Ordinate round off factor", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT_Y_ROFF))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_Y_ROFF))->SetSel(0, -1);
		return;
	}
	if(!CGenGridDataDlg::CheckROffValue(m_fHeight, m_fYROff))
	{
		AfxMessageBox("ERROR: Round off factor is greater than grid cell height,\nor grid cell height is not divisible with Round off factor", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT_Y_ROFF))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_Y_ROFF))->SetSel(0, -1);
		return;
	}
	
	GetDlgItem(IDC_EDIT3)->GetWindowText(m_strPtLyr);
	if(!CContourApp::IsValidACADSymName(m_strPtLyr))
	{
		AfxMessageBox("ERROR: Invalid layer name specified", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT3))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT3))->SetSel(0, -1);
		return;
	}


	//
	m_bDrawGrid = ((CButton*)GetDlgItem(IDC_CHECK))->GetCheck();
	if(m_bDrawGrid)
	{
		GetDlgItem(IDC_EDIT4)->GetWindowText(m_strLineLyr);
		if(!CContourApp::IsValidACADSymName(m_strLineLyr))
		{
			AfxMessageBox("ERROR: Invalid layer name specified", MB_ICONSTOP);
			((CEdit*)GetDlgItem(IDC_EDIT4))->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT4))->SetSel(0, -1);
			return;
		}
	}
	else
	{
		m_strLineLyr = "0";
	}

	CDialog::OnOK();
}

BOOL CGenGridDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strTmp;
	
	strTmp.Format("%f", m_fWidth);
	GetDlgItem(IDC_EDIT1)->SetWindowText(strTmp);
	strTmp.Format("%f", m_fHeight);
	GetDlgItem(IDC_EDIT2)->SetWindowText(strTmp);

	((CButton*)GetDlgItem(IDC_CHECK))->SetCheck(m_bDrawGrid);
	GetDlgItem(IDC_EDIT3)->SetWindowText(m_strPtLyr);
	GetDlgItem(IDC_EDIT4)->SetWindowText(m_strLineLyr);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
BOOL CGenGridDataDlg::CheckROffValue(const double& fVal, const double& fROff) 
{
	if(fROff > fVal)
		return FALSE;

	{//
		double fRem;

		fRem = fmod(fVal, fROff);
		if(fRem != 0.0)
			return FALSE;
	}
	return TRUE;
}

BOOL GetGenGridData(double& fWidth, double& fHeight, double& fXROff, double& fYROff, BOOL& bDrawGrid, CString& strPtLyr, CString& strLineLyr)
{
	CWnd* pAcadWnd;
	CTemporaryResourceOverride ResOverride;
	
	pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());

	CGenGridDataDlg Dlg(fWidth, fHeight, fXROff, fYROff, bDrawGrid, strPtLyr, strLineLyr, pAcadWnd);
	if(Dlg.DoModal() != IDOK)
		return FALSE;

	Dlg.GetAttrib(fWidth, fHeight, fXROff, fYROff, bDrawGrid, strPtLyr, strLineLyr);
	return TRUE;
}
