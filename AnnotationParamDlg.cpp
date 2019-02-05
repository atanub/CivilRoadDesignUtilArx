// AnnotationParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ContourApp.h"
#include "AnnotationParamDlg.h"
#include "ResourceHelper.h"
#include "DrawUtil.h"
#include "Util.h"
#include "DwgParamObj.h"
#include <ads.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" HWND adsw_acadMainWnd();

/////////////////////////////////////////////////////////////////////////////
// CAnnotationParamDlg dialog


CAnnotationParamDlg::CAnnotationParamDlg(const ANNTNPARAMS& Rec, CWnd* pParent /*=NULL*/)
	: CDialog(CAnnotationParamDlg::IDD, pParent), m_Rec(Rec)
{
	//{{AFX_DATA_INIT(CAnnotationParamDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	memset(&m_RecModified, 0, sizeof(m_RecModified));
}


void CAnnotationParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnnotationParamDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnnotationParamDlg, CDialog)
	//{{AFX_MSG_MAP(CAnnotationParamDlg)
	ON_BN_CLICKED(IDC_RADIO_ANG_CNT_LINES, OnRadioAngCntLines)
	ON_BN_CLICKED(IDC_RADIO_ANG_PATH, OnRadioAngPath)
	ON_BN_CLICKED(IDC_RADIO_ANG_USER, OnRadioAngUser)
	ON_BN_CLICKED(IDC_RADIO_LYR_AUTO, OnRadioLyrAuto)
	ON_BN_CLICKED(IDC_RADIO_LYR_CUR, OnRadioLyrCur)
	ON_BN_CLICKED(IDC_RADIO_LYR_USER, OnRadioLyrUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnnotationParamDlg message handlers

BOOL CAnnotationParamDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strTmp;
	
	strTmp.Format("%f", m_Rec.fTxtHeight);
	GetDlgItem(IDC_EDIT_TXT_HT)->SetWindowText(strTmp);
	strTmp.Format("%f", RTOD(m_Rec.fRotAngle));
	GetDlgItem(IDC_EDIT_ROTN)->SetWindowText(strTmp);

	GetDlgItem(IDC_EDIT_LYR)->SetWindowText(m_Rec.szLyr);

	((CButton*)GetDlgItem(IDC_RADIO_ANG_CNT_LINES))->SetCheck((m_Rec.iRotAngleType == 0));
	((CButton*)GetDlgItem(IDC_RADIO_ANG_PATH))->SetCheck((m_Rec.iRotAngleType == 1));
	((CButton*)GetDlgItem(IDC_RADIO_ANG_USER))->SetCheck((m_Rec.iRotAngleType == 2));
	GetDlgItem(IDC_EDIT_ROTN)->EnableWindow((m_Rec.iRotAngleType == 2));
	
	((CButton*)GetDlgItem(IDC_RADIO_LYR_AUTO))->SetCheck((m_Rec.iLyrType == 1));
	((CButton*)GetDlgItem(IDC_RADIO_LYR_CUR))->SetCheck((m_Rec.iLyrType == 0));
	((CButton*)GetDlgItem(IDC_RADIO_LYR_USER))->SetCheck((m_Rec.iLyrType == 2));
	GetDlgItem(IDC_EDIT_LYR)->EnableWindow((m_Rec.iLyrType == 2));
	
	((CButton*)GetDlgItem(IDC_CHECK_FORCE_ANN))->SetCheck(m_Rec.bForceAnnotation);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnnotationParamDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString strTmp;
	
	GetDlgItem(IDC_EDIT_TXT_HT)->GetWindowText(strTmp);
	m_RecModified.fTxtHeight = atof(strTmp);
	if(m_RecModified.fTxtHeight <= 0.0)
	{
		AfxMessageBox("ERROR: Invalid text size", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT_TXT_HT))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_TXT_HT))->SetSel(0, -1);
		return;
	}
	//Angle......
	if(((CButton*)GetDlgItem(IDC_RADIO_ANG_CNT_LINES))->GetCheck())
		m_RecModified.iRotAngleType = 0;
	else if	(((CButton*)GetDlgItem(IDC_RADIO_ANG_PATH))->GetCheck())
		m_RecModified.iRotAngleType = 1;
	else
	{
		m_RecModified.iRotAngleType = 2;
		//
		GetDlgItem(IDC_EDIT_ROTN)->GetWindowText(strTmp);
		m_RecModified.fRotAngle = atof(strTmp);

		if((m_RecModified.fRotAngle < 0.0) || (m_Rec.fRotAngle > 360.0))
		{
			AfxMessageBox("ERROR: Invalid angle specified", MB_ICONSTOP);
			((CEdit*)GetDlgItem(IDC_EDIT_ROTN))->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_ROTN))->SetSel(0, -1);
			return;
		}
		m_RecModified.fRotAngle = DTOR(m_RecModified.fRotAngle);
	}
	//Lyr......
	strcpy(m_RecModified.szLyr, "Invalid");
	if(((CButton*)GetDlgItem(IDC_RADIO_LYR_AUTO))->GetCheck())
		m_RecModified.iLyrType = 1;
	else if	(((CButton*)GetDlgItem(IDC_RADIO_LYR_CUR))->GetCheck())
		m_RecModified.iLyrType = 0;
	else
	{
		m_RecModified.iLyrType = 2;
		//
		GetDlgItem(IDC_EDIT_LYR)->GetWindowText(strTmp);
		if(!CContourApp::IsValidACADSymName(strTmp))
		{
			AfxMessageBox("ERROR: Invalid layer name specified", MB_ICONSTOP);
			((CEdit*)GetDlgItem(IDC_EDIT_LYR))->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_LYR))->SetSel(0, -1);
			return;
		}
		if(strlen(strTmp) < sizeof(m_RecModified.szLyr))
			strcpy(m_RecModified.szLyr, strTmp);
	}
	m_RecModified.bForceAnnotation = ((CButton*)GetDlgItem(IDC_CHECK_FORCE_ANN))->GetCheck();
	CDialog::OnOK();
}

void CAnnotationParamDlg::OnRadioAngCntLines() 
{
	BOOL bFlag;

	bFlag = ((CButton*)GetDlgItem(IDC_RADIO_ANG_CNT_LINES))->GetCheck();
	
	((CButton*)GetDlgItem(IDC_RADIO_ANG_PATH))->SetCheck(!bFlag);
	((CButton*)GetDlgItem(IDC_RADIO_ANG_USER))->SetCheck(!bFlag);

	GetDlgItem(IDC_EDIT_ROTN)->EnableWindow(!bFlag);
}
void CAnnotationParamDlg::OnRadioAngPath() 
{
	BOOL bFlag;

	bFlag = ((CButton*)GetDlgItem(IDC_RADIO_ANG_PATH))->GetCheck();
	
	((CButton*)GetDlgItem(IDC_RADIO_ANG_CNT_LINES))->SetCheck(!bFlag);
	((CButton*)GetDlgItem(IDC_RADIO_ANG_USER))->SetCheck(!bFlag);

	GetDlgItem(IDC_EDIT_ROTN)->EnableWindow(!bFlag);
}
void CAnnotationParamDlg::OnRadioAngUser() 
{
	BOOL bFlag;

	bFlag = ((CButton*)GetDlgItem(IDC_RADIO_ANG_USER))->GetCheck();
	
	((CButton*)GetDlgItem(IDC_RADIO_ANG_CNT_LINES))->SetCheck(!bFlag);
	((CButton*)GetDlgItem(IDC_RADIO_ANG_PATH))->SetCheck(!bFlag);

	GetDlgItem(IDC_EDIT_ROTN)->EnableWindow(bFlag);
}
void CAnnotationParamDlg::OnRadioLyrAuto() 
{
	BOOL bFlag;

	bFlag = ((CButton*)GetDlgItem(IDC_RADIO_LYR_AUTO))->GetCheck();
	
	((CButton*)GetDlgItem(IDC_RADIO_LYR_CUR))->SetCheck(!bFlag);
	((CButton*)GetDlgItem(IDC_RADIO_LYR_USER))->SetCheck(!bFlag);

	GetDlgItem(IDC_EDIT_LYR)->EnableWindow(!bFlag);
}
void CAnnotationParamDlg::OnRadioLyrCur() 
{
	BOOL bFlag;

	bFlag = ((CButton*)GetDlgItem(IDC_RADIO_LYR_CUR))->GetCheck();
	
	((CButton*)GetDlgItem(IDC_RADIO_LYR_AUTO))->SetCheck(!bFlag);
	((CButton*)GetDlgItem(IDC_RADIO_LYR_USER))->SetCheck(!bFlag);

	GetDlgItem(IDC_EDIT_LYR)->EnableWindow(!bFlag);
}
void CAnnotationParamDlg::OnRadioLyrUser() 
{
	BOOL bFlag;

	bFlag = ((CButton*)GetDlgItem(IDC_RADIO_LYR_USER))->GetCheck();
	
	((CButton*)GetDlgItem(IDC_RADIO_LYR_AUTO))->SetCheck(!bFlag);
	((CButton*)GetDlgItem(IDC_RADIO_LYR_CUR))->SetCheck(!bFlag);

	GetDlgItem(IDC_EDIT_LYR)->EnableWindow(bFlag);
}
///
BOOL GetAnnotationParam(ANNTNPARAMS& Rec)
{
	{//
		CWnd* pAcadWnd;
		CTemporaryResourceOverride ResOverride;
		
		pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());

		CAnnotationParamDlg Dlg(Rec, pAcadWnd);
		if(Dlg.DoModal() != IDOK)
			return FALSE;

		Rec = Dlg.GetAttrib();
	}
	CDwgParamObj::CreateAnnParamObj(&Rec);
	return TRUE;
}
