// ProfileDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ContourApp.h"
#include "ResourceHelper.h"
#include "ProfileDataDlg.h"
#include "ContourApp.h"
#include "ContourApp.h"       // the Application
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" HWND adsw_acadMainWnd();
/////////////////////////////////////////////////////////////////////////////
// CProfileDataDlg dialog


CProfileDataDlg::CProfileDataDlg(PROFILEDATAEX& Rec, CWnd* pParent /*=NULL*/)
	: CDialog(CProfileDataDlg::IDD, pParent), m_Rec(Rec)
{
	//{{AFX_DATA_INIT(CProfileDataDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProfileDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProfileDataDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProfileDataDlg, CDialog)
	//{{AFX_MSG_MAP(CProfileDataDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProfileDataDlg message handlers

BOOL CProfileDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	// TODO: Add extra initialization here
	CString strTmp;

	((CButton*)GetDlgItem(IDC_CHECK_NE_TAG))->SetCheck(m_Rec.data.bDrawNERows);

	GetDlgItem(IDC_EDIT_LYR_ANNOTATION)->SetWindowText(m_Rec.data.szLyrAnnotation);
	GetDlgItem(IDC_EDIT_LYR_TOP_SURFACE)->SetWindowText(m_Rec.data.szLyrTopSurface);
	GetDlgItem(IDC_EDIT_LYR_ORDINATE)->SetWindowText(m_Rec.data.szLyrOrdinate);
	GetDlgItem(IDC_EDIT_LYR_AXIS)->SetWindowText(m_Rec.data.szLyrAxis);

	GetDlgItem(IDC_EDIT_NORTHING_TAG)->SetWindowText(m_Rec.data.szNorthingTag);
	GetDlgItem(IDC_EDIT_EASTING_TAG)->SetWindowText(m_Rec.data.szEastingTag);
	GetDlgItem(IDC_EDIT_ELEV_TAG)->SetWindowText(m_Rec.data.szElevTag);
	GetDlgItem(IDC_EDIT_DISTANCE_TAG)->SetWindowText(m_Rec.data.szDistanceTag);
	GetDlgItem(IDC_EDIT_CUM_DISTANCE_TAG)->SetWindowText(m_Rec.data.szCumDistanceTag);
	GetDlgItem(IDC_EDIT_PROFILE_LABEL)->SetWindowText(m_Rec.data.szProfileTag);
	GetDlgItem(IDC_EDIT_DATUM_TAG)->SetWindowText(m_Rec.data.szDatumTag);

	strTmp.Format("%f", m_Rec.data.dXScale);			GetDlgItem(IDC_EDIT_EXAG_HORZ)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dYScale);			GetDlgItem(IDC_EDIT_EXAG_VERT)->SetWindowText(strTmp);

	strTmp.Format("%f", m_Rec.data.dDatum);				GetDlgItem(IDC_EDIT_DATUM)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dStartChg);			GetDlgItem(IDC_EDIT_START_CHG)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dSexonInterval);		GetDlgItem(IDC_EDIT_SECTION_INTERVAL)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dSexonIntervalMinor);	GetDlgItem(IDC_EDIT_SECTION_INTERVAL_MINOR)->SetWindowText(strTmp);

	strTmp.Format("%f", m_Rec.data.dNumAnnSize);		GetDlgItem(IDC_EDIT_NUM_TXT_SIZE)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dRowAnnSize);		GetDlgItem(IDC_EDIT_LAB_TXT_SIZE)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dPflLabelSize);		GetDlgItem(IDC_EDIT_PFL_LABEL_TXT_SIZE)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dRowHt);				GetDlgItem(IDC_EDIT_ROW_HT)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.fProfilePathWidth);		GetDlgItem(IDC_EDIT_PROFILE_PATH_WIDTH)->SetWindowText(strTmp);
	return TRUE;
}

void CProfileDataDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString strTmp;
	
	//
	GetDlgItem(IDC_EDIT_EXAG_VERT)->GetWindowText(strTmp);
	m_Rec.data.dYScale = atof(strTmp);
	if(m_Rec.data.dYScale <= 0.0)
	{
		AfxMessageBox("Invalid vertical exagaration", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_EXAG_VERT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_EXAG_VERT))->SetSel(0, -1);
		return;
	}
	//
	GetDlgItem(IDC_EDIT_EXAG_HORZ)->GetWindowText(strTmp);
	m_Rec.data.dXScale = atof(strTmp);
	if(m_Rec.data.dXScale <= 0.0)
	{
		AfxMessageBox("Invalid horizontal exagaration", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_EXAG_HORZ)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_EXAG_HORZ))->SetSel(0, -1);
		return;
	}
	//
	GetDlgItem(IDC_EDIT_DATUM)->GetWindowText(strTmp);
	m_Rec.data.dDatum = atof(strTmp);
	//
	GetDlgItem(IDC_EDIT_START_CHG)->GetWindowText(strTmp);
	m_Rec.data.dStartChg = atof(strTmp);
	//
	GetDlgItem(IDC_EDIT_SECTION_INTERVAL)->GetWindowText(strTmp);
	m_Rec.data.dSexonInterval = atof(strTmp);
	if(m_Rec.data.dSexonInterval <= 0.0)
	{
		AfxMessageBox("Invalid section interval", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_SECTION_INTERVAL)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_SECTION_INTERVAL))->SetSel(0, -1);
		return;
	}
	//
	GetDlgItem(IDC_EDIT_SECTION_INTERVAL_MINOR)->GetWindowText(strTmp);
	m_Rec.data.dSexonIntervalMinor = atof(strTmp);
	if(m_Rec.data.dSexonIntervalMinor <= 0.0)
	{
		AfxMessageBox("Invalid minor section interval", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_SECTION_INTERVAL_MINOR)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_SECTION_INTERVAL_MINOR))->SetSel(0, -1);
		return;
	}
	/*
	{//Check whether 'section interval' is multiple of 'minor section interval' or not
		double fTmp;
		
		fTmp = fmod(m_Rec.data.dSexonInterval, m_Rec.data.dSexonIntervalMinor);
		if((fTmp != 0.0) || (m_Rec.data.dSexonInterval < m_Rec.data.dSexonIntervalMinor))
		{
			AfxMessageBox("Invalid 'Section Interval'\nThe value should be multiple of 'Minor Section Interval'", MB_ICONSTOP);
			GetDlgItem(IDC_EDIT_SECTION_INTERVAL)->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_SECTION_INTERVAL))->SetSel(0, -1);
			return;
		}
	}
	*/
	{//Validate: 'section interval' is greater than the 'minor section interval'
		if(m_Rec.data.dSexonIntervalMinor > m_Rec.data.dSexonInterval)
		{
			AfxMessageBox("Invalid 'Section Interval'\nThe value should be greater than the 'Minor Section Interval'", MB_ICONSTOP);
			GetDlgItem(IDC_EDIT_SECTION_INTERVAL)->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_SECTION_INTERVAL))->SetSel(0, -1);
			return;
		}
	}

	//
	GetDlgItem(IDC_EDIT_LYR_ANNOTATION)->GetWindowText(strTmp);
	if(!CContourApp::IsValidACADSymName(strTmp))
	{
		AfxMessageBox("Invalid layer name", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_LYR_ANNOTATION)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_LYR_ANNOTATION))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szLyrAnnotation, strTmp);
	//
	GetDlgItem(IDC_EDIT_LYR_TOP_SURFACE)->GetWindowText(strTmp);
	if(!CContourApp::IsValidACADSymName(strTmp))
	{
		AfxMessageBox("Invalid layer name", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_LYR_TOP_SURFACE)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_LYR_TOP_SURFACE))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szLyrTopSurface, strTmp);
	//
	GetDlgItem(IDC_EDIT_LYR_ORDINATE)->GetWindowText(strTmp);
	if(!CContourApp::IsValidACADSymName(strTmp))
	{
		AfxMessageBox("Invalid layer name", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_LYR_ORDINATE)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_LYR_ORDINATE))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szLyrOrdinate, strTmp);
	//
	GetDlgItem(IDC_EDIT_LYR_AXIS)->GetWindowText(strTmp);
	if(!CContourApp::IsValidACADSymName(strTmp))
	{
		AfxMessageBox("Invalid layer name", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_LYR_AXIS)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_LYR_AXIS))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szLyrAxis, strTmp);
	//
	GetDlgItem(IDC_EDIT_PROFILE_LABEL)->GetWindowText(strTmp); strTmp.TrimLeft(); strTmp.TrimRight();
	if((strTmp.Find('%') >= 0) || (strlen(strTmp) <= 0))
	{
		AfxMessageBox("Invalid profile label", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_PROFILE_LABEL)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_PROFILE_LABEL))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szProfileTag, strTmp);
	//
	GetDlgItem(IDC_EDIT_ELEV_TAG)->GetWindowText(strTmp); strTmp.TrimLeft(); strTmp.TrimRight();
	if(strlen(strTmp) <= 0)
	{
		AfxMessageBox("Invalid elevation tag", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_ELEV_TAG)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_ELEV_TAG))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szElevTag, strTmp);
	//
	GetDlgItem(IDC_EDIT_DISTANCE_TAG)->GetWindowText(strTmp); strTmp.TrimLeft(); strTmp.TrimRight();
	if(strlen(strTmp) <= 0)
	{
		AfxMessageBox("Invalid distance tag", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_DISTANCE_TAG)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_DISTANCE_TAG))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szDistanceTag, strTmp);
	//
	GetDlgItem(IDC_EDIT_CUM_DISTANCE_TAG)->GetWindowText(strTmp); strTmp.TrimLeft(); strTmp.TrimRight();
	if(strlen(strTmp) <= 0)
	{
		AfxMessageBox("Invalid cumulative distance tag", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_CUM_DISTANCE_TAG)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_CUM_DISTANCE_TAG))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szCumDistanceTag, strTmp);
	//
	GetDlgItem(IDC_EDIT_DATUM_TAG)->GetWindowText(strTmp); strTmp.TrimLeft(); strTmp.TrimRight();
	if(strlen(strTmp) <= 0)
	{
		AfxMessageBox("Invalid Offset tag", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_DATUM_TAG)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_DATUM_TAG))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szDatumTag, strTmp);

	//
	GetDlgItem(IDC_EDIT_NORTHING_TAG)->GetWindowText(strTmp); strTmp.TrimLeft(); strTmp.TrimRight();
	if(strlen(strTmp) <= 0)
	{
		AfxMessageBox("Invalid northing tag", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_NORTHING_TAG)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_NORTHING_TAG))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szNorthingTag, strTmp);
	//
	GetDlgItem(IDC_EDIT_EASTING_TAG)->GetWindowText(strTmp); strTmp.TrimLeft(); strTmp.TrimRight();
	if(strlen(strTmp) <= 0)
	{
		AfxMessageBox("Invalid northing tag", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_EASTING_TAG)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_EASTING_TAG))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szEastingTag, strTmp);
	//
	GetDlgItem(IDC_EDIT_NUM_TXT_SIZE)->GetWindowText(strTmp);
	m_Rec.data.dNumAnnSize = atof(strTmp);
	if(m_Rec.data.dNumAnnSize <= 0.0)
	{
		AfxMessageBox("Invalid text size", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_NUM_TXT_SIZE)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_NUM_TXT_SIZE))->SetSel(0, -1);
		return;
	}
	//
	GetDlgItem(IDC_EDIT_LAB_TXT_SIZE)->GetWindowText(strTmp);
	m_Rec.data.dRowAnnSize = atof(strTmp);
	if(m_Rec.data.dRowAnnSize <= 0.0)
	{
		AfxMessageBox("Invalid text size", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_LAB_TXT_SIZE)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_LAB_TXT_SIZE))->SetSel(0, -1);
		return;
	}
	//
	GetDlgItem(IDC_EDIT_ROW_HT)->GetWindowText(strTmp);
	m_Rec.data.dRowHt = atof(strTmp);
	if(m_Rec.data.dRowHt <= 0.0)
	{
		AfxMessageBox("Invalid text size", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_ROW_HT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_ROW_HT))->SetSel(0, -1);
		return;
	}
	//
	GetDlgItem(IDC_EDIT_PFL_LABEL_TXT_SIZE)->GetWindowText(strTmp);
	m_Rec.data.dPflLabelSize = atof(strTmp);
	if(m_Rec.data.dPflLabelSize <= 0.0)
	{
		AfxMessageBox("Invalid text size", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_PFL_LABEL_TXT_SIZE)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_PFL_LABEL_TXT_SIZE))->SetSel(0, -1);
		return;
	}
	m_Rec.data.bDrawNERows = ((CButton*)GetDlgItem(IDC_CHECK_NE_TAG))->GetCheck();
	//
	GetDlgItem(IDC_EDIT_PROFILE_PATH_WIDTH)->GetWindowText(strTmp);
	m_Rec.fProfilePathWidth = atof(strTmp);
	if(m_Rec.fProfilePathWidth <= 0.0)
	{
		AfxMessageBox("Invalid Profile Path Width", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_PROFILE_PATH_WIDTH)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_PROFILE_PATH_WIDTH))->SetSel(0, -1);
		return;
	}

	CDialog::OnOK();
}

BOOL CProfileDataDlg::GetProfileData(PROFILEDATAEX& Rec)
{
	CWnd* pAcadWnd;
	CTemporaryResourceOverride ResOverride;
	pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());

	CProfileDataDlg Dlg(Rec, pAcadWnd);
	if(Dlg.DoModal() != IDOK)
		return FALSE;

	return TRUE;
}

//
/*
	PROFILEDATAEX Rec;
	
	GetProfileData(Rec);
	return;
	
*/
//