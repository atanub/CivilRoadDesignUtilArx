// HorzCurveDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ContourApp.h"
#include "HorzCurveDataDlg.h"
#include "ResourceHelper.h"

extern "C" HWND adsw_acadMainWnd();

/////////////////////////////////////////////////////////////////////////////
// CCurveDataDlg dialog


CCurveDataDlg::CCurveDataDlg(const AcGePoint3d* pPtStart, const AcGePoint3d* pPtPoI, const AcGePoint3d* pPtEnd, const double& fRadius, const double& fFirstTanMaxLen, const double& fSecondTanMaxLen, PROFILEDATAEX* recPfl, XSECDATAEX* recXSec, AcGePoint3dArray* datumRangeArray, CWnd* pParent /*=NULL*/)
	: CDialog(CCurveDataDlg::IDD, pParent), m_pPtStart(pPtStart), m_pPtPoI(pPtPoI), m_pPtEnd(pPtEnd), m_fFirstTanMaxLen(fFirstTanMaxLen), m_fSecondTanMaxLen(fSecondTanMaxLen),
	m_pRecPflData(recPfl), m_pRecXSecData(recXSec), m_pDatumRangeArray(datumRangeArray)
{
	m_fRadius = fRadius;
	if((m_pPtPoI != 0L) || (m_pPtStart != 0L) || (m_pPtEnd != 0L))
	{
		m_pCurveCalculator = new CCurveCalculator(*m_pPtPoI, *m_pPtStart, *m_pPtEnd, m_fFirstTanMaxLen, m_fSecondTanMaxLen);
	}
	else
	{
		m_pCurveCalculator = 0L;
	}
	//{{AFX_DATA_INIT(CCurveDataDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
CCurveDataDlg::~CCurveDataDlg()
{
	if(m_pCurveCalculator != 0L)
		delete m_pCurveCalculator;
	
	m_pCurveCalculator = 0L;
}


void CCurveDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCurveDataDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCurveDataDlg, CDialog)
	//{{AFX_MSG_MAP(CCurveDataDlg)
	ON_BN_CLICKED(IDC_RADIO_CURVE_LENGTH, OnRadioCurveLength)
	ON_BN_CLICKED(IDC_RADIO_DEG_OF_CURVE, OnRadioDegOfCurve)
	ON_BN_CLICKED(IDC_RADIO_RADIUS, OnRadioRadius)
	ON_BN_CLICKED(IDC_RADIO_TANGENT_LENGTH, OnRadioTangentLength)
	ON_BN_CLICKED(IDC_RADIO_XTERNAL_LENGTH, OnRadioXternalLength)
	ON_BN_CLICKED(IDCALC, OnCalc)
	ON_BN_CLICKED(IDPROFILE, OnProfile)
	ON_BN_CLICKED(IDCROSS_SECTION, OnCrossSection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurveDataDlg message handlers

BOOL CCurveDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strValue;

	strValue.Format("%f", m_fRadius);
	((CButton*)GetDlgItem(IDC_RADIO_RADIUS))->SetCheck(1);
	GetDlgItem(IDC_EDIT_RADIUS)->SetWindowText(strValue);

	if(m_pCurveCalculator == 0L)
	{//dialog is only meant for profile/xSec parameter
		GetDlgItem(IDC_EDIT_RADIUS)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TANGENT_LEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_XTERNAL_LEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CURVE_LEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_DEG_OF_CURVE)->EnableWindow(FALSE);

		GetDlgItem(IDC_RADIO_CURVE_LENGTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_DEG_OF_CURVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_RADIUS)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_TANGENT_LENGTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_XTERNAL_LENGTH)->EnableWindow(FALSE);
		GetDlgItem(IDCALC)->EnableWindow(FALSE);
	}
	GetDlgItem(IDPROFILE)->ShowWindow((m_pRecPflData == 0L) ? SW_HIDE:SW_SHOW);
	GetDlgItem(IDCROSS_SECTION)->ShowWindow((m_pRecXSecData == 0L) ? SW_HIDE:SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCurveDataDlg::OnOK() 
{
	if(!_ValidateData())
		return;
	
	CDialog::OnOK();
}

void CCurveDataDlg::OnRadioCurveLength() 
{
	GetDlgItem(IDC_EDIT_RADIUS)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_TANGENT_LEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_XTERNAL_LEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_CURVE_LEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_DEG_OF_CURVE)->EnableWindow(FALSE);
}

void CCurveDataDlg::OnRadioDegOfCurve() 
{
	GetDlgItem(IDC_EDIT_RADIUS)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_TANGENT_LEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_XTERNAL_LEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_CURVE_LEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_DEG_OF_CURVE)->EnableWindow(TRUE);
}

void CCurveDataDlg::OnRadioRadius() 
{
	GetDlgItem(IDC_EDIT_RADIUS)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_TANGENT_LEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_XTERNAL_LEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_CURVE_LEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_DEG_OF_CURVE)->EnableWindow(FALSE);
}

void CCurveDataDlg::OnRadioTangentLength() 
{
	GetDlgItem(IDC_EDIT_RADIUS)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_TANGENT_LEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_XTERNAL_LEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_CURVE_LEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_DEG_OF_CURVE)->EnableWindow(FALSE);
}

void CCurveDataDlg::OnRadioXternalLength() 
{
	GetDlgItem(IDC_EDIT_RADIUS)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_TANGENT_LEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_XTERNAL_LEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_CURVE_LEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_DEG_OF_CURVE)->EnableWindow(FALSE);
}

BOOL CCurveDataDlg::_ValidateData() 
{
	double fVal;
	CString strTmp;
	if(m_pCurveCalculator == 0L)
		return TRUE;//No need for checking....dialog is only meant for profile/xSec parameter
	

	if(((CButton*)GetDlgItem(IDC_RADIO_DEG_OF_CURVE))->GetCheck() == 1)
	{
		GetDlgItem(IDC_EDIT_DEG_OF_CURVE)->GetWindowText(strTmp);
		fVal = atof(strTmp);

		if(fVal <= 0.0)
		{
			AfxMessageBox("Invalid value for calculation", MB_ICONSTOP);
			GetDlgItem(IDC_EDIT_DEG_OF_CURVE)->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_DEG_OF_CURVE))->SetSel(0, -1);
			return FALSE;
		}
		if(!m_pCurveCalculator->UpdWRTDegOfCurve(fVal) || (fVal <= 0.0))
		{
			AfxMessageBox(m_pCurveCalculator->GetErrMsg(), MB_ICONSTOP);
			GetDlgItem(IDC_EDIT_DEG_OF_CURVE)->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_DEG_OF_CURVE))->SetSel(0, -1);
			return FALSE;
		}
	}
	if(((CButton*)GetDlgItem(IDC_RADIO_CURVE_LENGTH))->GetCheck() == 1)
	{
		GetDlgItem(IDC_EDIT_CURVE_LEN)->GetWindowText(strTmp);
		fVal = atof(strTmp);
		if(fVal <= 0.0)
		{
			AfxMessageBox("Invalid value for calculation", MB_ICONSTOP);
			GetDlgItem(IDC_EDIT_CURVE_LEN)->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_CURVE_LEN))->SetSel(0, -1);
			return FALSE;
		}
		if(!m_pCurveCalculator->UpdWRTCurveLen(fVal) || (fVal <= 0.0))
		{
			AfxMessageBox(m_pCurveCalculator->GetErrMsg(), MB_ICONSTOP);
			GetDlgItem(IDC_EDIT_CURVE_LEN)->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_CURVE_LEN))->SetSel(0, -1);
			return FALSE;
		}
	}
	if(((CButton*)GetDlgItem(IDC_RADIO_RADIUS))->GetCheck() == 1)
	{
		GetDlgItem(IDC_EDIT_RADIUS)->GetWindowText(strTmp);
		fVal = atof(strTmp);
		if(fVal <= 0.0)
		{
			AfxMessageBox("Invalid value for calculation", MB_ICONSTOP);
			GetDlgItem(IDC_EDIT_RADIUS)->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_RADIUS))->SetSel(0, -1);
			return FALSE;
		}
		if(!m_pCurveCalculator->UpdWRTRad(fVal) || (fVal <= 0.0))
		{
			AfxMessageBox(m_pCurveCalculator->GetErrMsg(), MB_ICONSTOP);
			GetDlgItem(IDC_EDIT_RADIUS)->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_RADIUS))->SetSel(0, -1);
			return FALSE;
		}
	}
	if(((CButton*)GetDlgItem(IDC_RADIO_TANGENT_LENGTH))->GetCheck() == 1)
	{
		GetDlgItem(IDC_EDIT_TANGENT_LEN)->GetWindowText(strTmp);
		fVal = atof(strTmp);
		if(fVal <= 0.0)
		{
			AfxMessageBox("Invalid value for calculation", MB_ICONSTOP);
			GetDlgItem(IDC_EDIT_TANGENT_LEN)->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_TANGENT_LEN))->SetSel(0, -1);
			return FALSE;
		}
		if(!m_pCurveCalculator->UpdWRTTangentLen(fVal) || (fVal <= 0.0))
		{
			AfxMessageBox(m_pCurveCalculator->GetErrMsg(), MB_ICONSTOP);
			GetDlgItem(IDC_EDIT_TANGENT_LEN)->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_TANGENT_LEN))->SetSel(0, -1);
			return FALSE;
		}
	}
	if(((CButton*)GetDlgItem(IDC_RADIO_XTERNAL_LENGTH))->GetCheck() == 1)
	{
		GetDlgItem(IDC_EDIT_XTERNAL_LEN)->GetWindowText(strTmp);
		fVal = atof(strTmp);
		if(fVal <= 0.0)
		{
			AfxMessageBox("Invalid value for calculation", MB_ICONSTOP);
			GetDlgItem(IDC_EDIT_XTERNAL_LEN)->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_XTERNAL_LEN))->SetSel(0, -1);
			return FALSE;
		}
		if(!m_pCurveCalculator->UpdWRTXternalLen(fVal) || (fVal <= 0.0))
		{
			AfxMessageBox(m_pCurveCalculator->GetErrMsg(), MB_ICONSTOP);
			GetDlgItem(IDC_EDIT_XTERNAL_LEN)->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_XTERNAL_LEN))->SetSel(0, -1);
			return FALSE;
		}
	}
	m_fRadius = m_pCurveCalculator->GetRad();
	return TRUE;
}
void CCurveDataDlg::OnCalc() 
{
	CString strValue;

	if(m_pCurveCalculator == 0L)
		return;//No need for checking....dialog is only meant for profile/xSec parameter

	if(!_ValidateData())
	{
		const CString strUndefinedMsg("- Calculation failed -");
		if(((CButton*)GetDlgItem(IDC_RADIO_DEG_OF_CURVE))->GetCheck() == 1)
		{
			//GetDlgItem(IDC_EDIT_DEG_OF_CURVE)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_CURVE_LEN)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_RADIUS)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_TANGENT_LEN)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_XTERNAL_LEN)->SetWindowText(strUndefinedMsg);
		}
		if(((CButton*)GetDlgItem(IDC_RADIO_CURVE_LENGTH))->GetCheck() == 1)
		{
			GetDlgItem(IDC_EDIT_DEG_OF_CURVE)->SetWindowText(strUndefinedMsg);
			//GetDlgItem(IDC_EDIT_CURVE_LEN)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_RADIUS)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_TANGENT_LEN)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_XTERNAL_LEN)->SetWindowText(strUndefinedMsg);
		}
		if(((CButton*)GetDlgItem(IDC_RADIO_RADIUS))->GetCheck() == 1)
		{
			GetDlgItem(IDC_EDIT_DEG_OF_CURVE)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_CURVE_LEN)->SetWindowText(strUndefinedMsg);
			//GetDlgItem(IDC_EDIT_RADIUS)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_TANGENT_LEN)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_XTERNAL_LEN)->SetWindowText(strUndefinedMsg);
		}
		if(((CButton*)GetDlgItem(IDC_RADIO_TANGENT_LENGTH))->GetCheck() == 1)
		{
			GetDlgItem(IDC_EDIT_DEG_OF_CURVE)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_CURVE_LEN)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_RADIUS)->SetWindowText(strUndefinedMsg);
			//GetDlgItem(IDC_EDIT_TANGENT_LEN)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_XTERNAL_LEN)->SetWindowText(strUndefinedMsg);
		}
		if(((CButton*)GetDlgItem(IDC_RADIO_XTERNAL_LENGTH))->GetCheck() == 1)
		{
			GetDlgItem(IDC_EDIT_DEG_OF_CURVE)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_CURVE_LEN)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_RADIUS)->SetWindowText(strUndefinedMsg);
			GetDlgItem(IDC_EDIT_TANGENT_LEN)->SetWindowText(strUndefinedMsg);
			//GetDlgItem(IDC_EDIT_XTERNAL_LEN)->SetWindowText(strUndefinedMsg);
		}
		return;
	}
	strValue.Format("%f", m_pCurveCalculator->GetDegOfCurve()); GetDlgItem(IDC_EDIT_DEG_OF_CURVE)->SetWindowText(strValue);
	strValue.Format("%f", m_pCurveCalculator->GetRad());		GetDlgItem(IDC_EDIT_RADIUS)->SetWindowText(strValue);
	strValue.Format("%f", m_pCurveCalculator->GetTangentLen()); GetDlgItem(IDC_EDIT_TANGENT_LEN)->SetWindowText(strValue);
	strValue.Format("%f", m_pCurveCalculator->GetXternalLen()); GetDlgItem(IDC_EDIT_XTERNAL_LEN)->SetWindowText(strValue);
	strValue.Format("%f", m_pCurveCalculator->GetCurvleLen());	GetDlgItem(IDC_EDIT_CURVE_LEN)->SetWindowText(strValue);
}
void CCurveDataDlg::OnProfile() 
{
	CProfileDataDlg::GetProfileData(*m_pRecPflData);
}

void CCurveDataDlg::OnCrossSection() 
{
	GetXSectionData(*m_pRecXSecData, *m_pDatumRangeArray);
}

///
BOOL CCurveDataDlg::GetCurveRadius(const AcGePoint3d* pPtStart, const AcGePoint3d* pPtPoI, const AcGePoint3d* pPtEnd, double& fRadius, const double& fFirstTanMaxLen, const double& fSecondTanMaxLen, PROFILEDATAEX* pRecPfl, XSECDATAEX* pRecXSec, AcGePoint3dArray* pDatumRangeArray)
{
	CWnd* pAcadWnd;
	
	pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());
	{//
		CTemporaryResourceOverride ResOverride;
	
		CCurveDataDlg Dlg(pPtStart, pPtPoI, pPtEnd, fRadius, fFirstTanMaxLen, fSecondTanMaxLen, pRecPfl, pRecXSec, pDatumRangeArray, pAcadWnd);
		if(IDOK != Dlg.DoModal())
			return FALSE;
	
		fRadius = Dlg.GetRadius();
	}
	return TRUE;
}

BOOL CCurveDataDlg::GetOnlyPflAndXSecParam(PROFILEDATAEX& recPfl, XSECDATAEX& recXSec, AcGePoint3dArray& datumRangeArray)
{
	CWnd* pAcadWnd;
	
	pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());
	{//
		CTemporaryResourceOverride ResOverride;
	
		CCurveDataDlg Dlg(0L, 0L, 0L, 0.0, 0.0, 0.0, &recPfl, &recXSec, &datumRangeArray, pAcadWnd);

		if(IDOK != Dlg.DoModal())
			return FALSE;
	}
	return TRUE;
}
