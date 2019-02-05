// XSecDataDlg.cpp : implementation file
//
#include "stdafx.h"
#include "ResourceHelper.h"
#include "XSecDataDlg.h"
#include "ContourApp.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" HWND adsw_acadMainWnd();

static BOOL IsValueDefined(const double&, const AcGePoint3dArray&);
static int GetInsertionIndex(const double&, const AcGePoint3dArray&);
static void SmoothDatumRangeArray(AcGePoint3dArray&);

const char CXSecDataDlg::m_chShAttrSeparetor = '\t';
/////////////////////////////////////////////////////////////////////////////
// CXSecDataDlg dialog

CXSecDataDlg::CXSecDataDlg(XSECDATAEX& Rec, AcGePoint3dArray& Array, CWnd* pParent /*=NULL*/)
	: CDialog(CXSecDataDlg::IDD, pParent), m_Rec(Rec), m_DatumRangeArray(Array)
{
	//{{AFX_DATA_INIT(CXSecDataDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CXSecDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXSecDataDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXSecDataDlg, CDialog)
	//{{AFX_MSG_MAP(CXSecDataDlg)
	ON_BN_CLICKED(IDC_BUTTON_ADD_RANGE, OnButtonAddRange)
	ON_BN_CLICKED(IDC_BUTTON_DEL_RANGE, OnButtonDelRange)
	ON_LBN_DBLCLK(IDC_LIST_LEV_RANGE, OnDblClkListLevRange)
	ON_LBN_SELCHANGE(IDC_LIST_LEV_RANGE, OnSelChangeListLevRange)
	ON_CBN_SELCHANGE(IDC_COMBO_SHEET, OnSelChangeComboSheet)
	ON_BN_CLICKED(IDC_CHECK_AUTO_DATUM, OnCheckAutoDatum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXSecDataDlg message handlers

BOOL CXSecDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strTmp;

	{//Set Font...
		HFONT hFont;
		CListBox* pCtrl;

		pCtrl = (CListBox*)GetDlgItem(IDC_LIST_LEV_RANGE);
		hFont = (HFONT)GetStockObject(ANSI_FIXED_FONT);
		ASSERT(hFont != 0L);
		pCtrl->SendMessage(WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
	}
	{//Set Font...
		HFONT hFont;
		CComboBox* pCtrl;

		pCtrl = (CComboBox*)GetDlgItem(IDC_COMBO_SHEET);
		hFont = (HFONT)GetStockObject(ANSI_FIXED_FONT);
		ASSERT(hFont != 0L);
		pCtrl->SendMessage(WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

		CXSecDataDlg::InitSheetArray(m_SheetSizeArray);
		CXSecDataDlg::FillCombo(pCtrl, m_SheetSizeArray);
	}
	GetDlgItem(IDC_EDIT_LYR_ANNOTATION)->SetWindowText(m_Rec.data.szLyrAnnotation);
	GetDlgItem(IDC_EDIT_LYR_TOP_SURFACE)->SetWindowText(m_Rec.data.szLyrTopSurface);
	GetDlgItem(IDC_EDIT_LYR_ORDINATE)->SetWindowText(m_Rec.data.szLyrOrdinate);
	GetDlgItem(IDC_EDIT_LYR_AXIS)->SetWindowText(m_Rec.data.szLyrAxis);

	GetDlgItem(IDC_EDIT_CS_LABEL_TAG)->SetWindowText(m_Rec.data.szXSecLabelTag);
	
	GetDlgItem(IDC_EDIT_NORTHING_TAG)->SetWindowText(m_Rec.data.szNorthingTag);
	GetDlgItem(IDC_EDIT_EASTING_TAG)->SetWindowText(m_Rec.data.szEastingTag);
	GetDlgItem(IDC_EDIT_ELEV_TAG)->SetWindowText(m_Rec.data.szElevTag);
	GetDlgItem(IDC_EDIT_OFFSET_TAG)->SetWindowText(m_Rec.data.szOffsetTag);
	GetDlgItem(IDC_EDIT_DATUM_TAG)->SetWindowText(m_Rec.data.szDatumTag);
	
	strTmp.Format("%f", m_Rec.data.dXScale); GetDlgItem(IDC_EDIT_EXAG_HORZ)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dYScale); GetDlgItem(IDC_EDIT_EXAG_VERT)->SetWindowText(strTmp);

	strTmp.Format("%f", m_Rec.data.dNumAnnSize);		GetDlgItem(IDC_EDIT_NUM_TXT_SIZE)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dRowAnnSize);		GetDlgItem(IDC_EDIT_LAB_TXT_SIZE)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dRowHt);			GetDlgItem(IDC_EDIT_ROW_HT)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dCSLabelAnnSize); GetDlgItem(IDC_EDIT_CS_LABEL_TXT)->SetWindowText(strTmp);
	
	((CButton*)GetDlgItem(IDC_CHECK_AUTO_DATUM))->SetCheck(m_Rec.data.bAutoDatumFlag);
	GetDlgItem(IDC_EDIT_DATUM_DENOMINATOR)->EnableWindow(m_Rec.data.bAutoDatumFlag);
	GetDlgItem(IDC_EDIT_MARGIN)->EnableWindow(m_Rec.data.bAutoDatumFlag);
	
	GetDlgItem(IDC_BUTTON_ADD_RANGE)->EnableWindow(!m_Rec.data.bAutoDatumFlag);
	GetDlgItem(IDC_BUTTON_DEL_RANGE)->EnableWindow(!m_Rec.data.bAutoDatumFlag);
	GetDlgItem(IDC_EDIT_DATUM_START)->EnableWindow(!m_Rec.data.bAutoDatumFlag);
	GetDlgItem(IDC_EDIT_DATUM_END)->EnableWindow(!m_Rec.data.bAutoDatumFlag);
	GetDlgItem(IDC_EDIT_DATUM_ELEV)->EnableWindow(!m_Rec.data.bAutoDatumFlag);
	GetDlgItem(IDC_LIST_LEV_RANGE)->EnableWindow(!m_Rec.data.bAutoDatumFlag);

	strTmp.Format("%f", 0.0); GetDlgItem(IDC_EDIT_DATUM_START)->SetWindowText(strTmp);
	strTmp.Format("%f", 0.0); GetDlgItem(IDC_EDIT_DATUM_END)->SetWindowText(strTmp);
	strTmp.Format("%f", 0.0); GetDlgItem(IDC_EDIT_DATUM_ELEV)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dAutoDatumDenominator);		GetDlgItem(IDC_EDIT_DATUM_DENOMINATOR)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.dAutoDatumMargin);				GetDlgItem(IDC_EDIT_MARGIN)->SetWindowText(strTmp);

	strTmp.Format("%f", m_Rec.data.dSheetWid);		GetDlgItem(IDC_EDIT_SHEET_WIDTH)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dSheetHeight);	GetDlgItem(IDC_EDIT_SHEET_HEIGHT)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dNPlateWid);		GetDlgItem(IDC_EDIT_NAMEPLATE_WIDTH)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dNPlateHeight);	GetDlgItem(IDC_EDIT_NAMEPLATE_HEIGHT)->SetWindowText(strTmp);

	strTmp.Format("%f", m_Rec.data.dStartChg);		GetDlgItem(IDC_EDIT_START_CHG)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dCSInterval);	GetDlgItem(IDC_EDIT_CS_INTERVAL)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dOrdInterval);	GetDlgItem(IDC_EDIT_ORD_INTERVAL)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.data.dMaxCSOffset);	GetDlgItem(IDC_EDIT_MAX_OFFSET)->SetWindowText(strTmp);
	strTmp.Format("%f", m_Rec.dCSIntervalMinor);	GetDlgItem(IDC_EDIT_CS_INTERVAL_MINOR)->SetWindowText(strTmp);

	((CButton*)GetDlgItem(IDC_CHECK_DRAW_NE))->SetCheck(m_Rec.bDrawWithNorthingEastingData);
	((CButton*)GetDlgItem(IDC_CHECK_DRAW_SECTIONS_ON_POI))->SetCheck(m_Rec.bDrawSectionsAtAllIntersectionPoints);
	
	_FillDatumRangeList();

	/*
	{//
		((CComboBox*)GetDlgItem(IDC_COMBO_SHEET))->SetCurSel(0);
		OnSelChangeComboSheet();
	}
	*/
	return FALSE;
}

void CXSecDataDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString strTmp;

	m_Rec.data.bAutoDatumFlag = ((CButton*)GetDlgItem(IDC_CHECK_AUTO_DATUM))->GetCheck();
	//
	GetDlgItem(IDC_EDIT_NORTHING_TAG)->GetWindowText(strTmp);
	if(strlen(strTmp) <= 0)
	{
		AfxMessageBox("Invalid Northing tag", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_NORTHING_TAG)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_NORTHING_TAG))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szNorthingTag, strTmp);
	//
	GetDlgItem(IDC_EDIT_EASTING_TAG)->GetWindowText(strTmp);
	if(strlen(strTmp) <= 0)
	{
		AfxMessageBox("Invalid Easting tag", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_EASTING_TAG)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_EASTING_TAG))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szEastingTag, strTmp);
	//
	GetDlgItem(IDC_EDIT_ELEV_TAG)->GetWindowText(strTmp);
	if(strlen(strTmp) <= 0)
	{
		AfxMessageBox("Invalid Elevation tag", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_ELEV_TAG)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_ELEV_TAG))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szElevTag, strTmp);
	//
	GetDlgItem(IDC_EDIT_OFFSET_TAG)->GetWindowText(strTmp);
	if(strlen(strTmp) <= 0)
	{
		AfxMessageBox("Invalid Offset tag", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_OFFSET_TAG)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_OFFSET_TAG))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szOffsetTag, strTmp);
	//
	GetDlgItem(IDC_EDIT_DATUM_TAG)->GetWindowText(strTmp);
	if(strlen(strTmp) <= 0)
	{
		AfxMessageBox("Invalid Offset tag", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_DATUM_TAG)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_DATUM_TAG))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szDatumTag, strTmp);
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
	GetDlgItem(IDC_EDIT_CS_LABEL_TXT)->GetWindowText(strTmp);
	m_Rec.data.dCSLabelAnnSize = atof(strTmp);
	if(m_Rec.data.dCSLabelAnnSize <= 0.0)
	{
		AfxMessageBox("Invalid text size", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_CS_LABEL_TXT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_CS_LABEL_TXT))->SetSel(0, -1);
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
	if(!m_Rec.data.bAutoDatumFlag)
	{
		if(m_DatumRangeArray.length() <= 0)
		{
			AfxMessageBox("Datum range not defined", MB_ICONSTOP);
			GetDlgItem(IDC_LIST_LEV_RANGE)->SetFocus();
			return;
		}
	}
	else
	{
		GetDlgItem(IDC_EDIT_DATUM_DENOMINATOR)->GetWindowText(strTmp);
		m_Rec.data.dAutoDatumDenominator = atof(strTmp);
		if(m_Rec.data.dAutoDatumDenominator <= 0.0)
		{
			AfxMessageBox("Invalid datum Denominator", MB_ICONSTOP);
			((CEdit*)GetDlgItem(IDC_EDIT_DATUM_DENOMINATOR))->SetSel(0, -1);
			GetDlgItem(IDC_EDIT_DATUM_DENOMINATOR)->SetFocus();
			return;
		}
		//
		GetDlgItem(IDC_EDIT_MARGIN)->GetWindowText(strTmp);
		m_Rec.dAutoDatumMargin = atof(strTmp);
		if(m_Rec.dAutoDatumMargin <= 0.0)
		{
			AfxMessageBox("Invalid datum margin", MB_ICONSTOP);
			((CEdit*)GetDlgItem(IDC_EDIT_MARGIN))->SetSel(0, -1);
			GetDlgItem(IDC_EDIT_MARGIN)->SetFocus();
			return; 
		}
	}
	m_Rec.bDrawWithNorthingEastingData = ((CButton*)GetDlgItem(IDC_CHECK_DRAW_NE))->GetCheck();
	m_Rec.bDrawSectionsAtAllIntersectionPoints = ((CButton*)GetDlgItem(IDC_CHECK_DRAW_SECTIONS_ON_POI))->GetCheck();

	//Layers....
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
	GetDlgItem(IDC_EDIT_CS_LABEL_TAG)->GetWindowText(strTmp);
	if(strTmp.Find('%') >= 0)
	{
		AfxMessageBox("Invalid cross section label prefix", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_CS_LABEL_TAG)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_CS_LABEL_TAG))->SetSel(0, -1);
		return;
	}
	strcpy(m_Rec.data.szXSecLabelTag, strTmp);
	//Sheet Detail
	//
	GetDlgItem(IDC_EDIT_SHEET_WIDTH)->GetWindowText(strTmp);
	m_Rec.data.dSheetWid = atof(strTmp);
	if(m_Rec.data.dSheetWid <= 0.0)
	{
		AfxMessageBox("Invalid sheet width", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_SHEET_WIDTH)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_SHEET_WIDTH))->SetSel(0, -1);
		return;
	}
	//
	GetDlgItem(IDC_EDIT_SHEET_HEIGHT)->GetWindowText(strTmp);
	m_Rec.data.dSheetHeight = atof(strTmp);
	if(m_Rec.data.dSheetHeight <= 0.0)
	{
		AfxMessageBox("Invalid sheet height", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_SHEET_HEIGHT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_SHEET_HEIGHT))->SetSel(0, -1);
		return;
	}
	//
	GetDlgItem(IDC_EDIT_NAMEPLATE_WIDTH)->GetWindowText(strTmp);
	m_Rec.data.dNPlateWid = atof(strTmp);
	if(m_Rec.data.dNPlateWid <= 0.0)
	{
		AfxMessageBox("Invalid name plate width", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_NAMEPLATE_WIDTH)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_NAMEPLATE_WIDTH))->SetSel(0, -1);
		return;
	}
	//
	GetDlgItem(IDC_EDIT_NAMEPLATE_HEIGHT)->GetWindowText(strTmp);
	m_Rec.data.dNPlateHeight = atof(strTmp);
	if(m_Rec.data.dNPlateHeight <= 0.0)
	{
		AfxMessageBox("Invalid name plate height", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_NAMEPLATE_HEIGHT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_NAMEPLATE_HEIGHT))->SetSel(0, -1);
		return;
	}
	//
	if(m_Rec.data.dNPlateHeight >= m_Rec.data.dSheetHeight)
	{
		AfxMessageBox("Invalid name plate height", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_NAMEPLATE_HEIGHT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_NAMEPLATE_HEIGHT))->SetSel(0, -1);
		return;
	}
	if(m_Rec.data.dNPlateWid >= m_Rec.data.dSheetWid)
	{
		AfxMessageBox("Invalid name plate width", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_NAMEPLATE_WIDTH)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_NAMEPLATE_WIDTH))->SetSel(0, -1);
		return;
	}
	//
	GetDlgItem(IDC_EDIT_START_CHG)->GetWindowText(strTmp);
	m_Rec.data.dStartChg = atof(strTmp);
	//
	GetDlgItem(IDC_EDIT_CS_INTERVAL)->GetWindowText(strTmp);
	m_Rec.data.dCSInterval = atof(strTmp);
	if(m_Rec.data.dCSInterval <= 0.0)
	{
		AfxMessageBox("Invalid C/S interval", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_CS_INTERVAL)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_CS_INTERVAL))->SetSel(0, -1);
		return;
	}
	//
	GetDlgItem(IDC_EDIT_ORD_INTERVAL)->GetWindowText(strTmp);
	m_Rec.data.dOrdInterval = atof(strTmp);
	if(m_Rec.data.dOrdInterval <= 0.0)
	{
		AfxMessageBox("Invalid ordinate interval", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_ORD_INTERVAL)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_ORD_INTERVAL))->SetSel(0, -1);
		return;
	}
	//
	GetDlgItem(IDC_EDIT_MAX_OFFSET)->GetWindowText(strTmp);
	m_Rec.data.dMaxCSOffset = atof(strTmp);
	if(m_Rec.data.dMaxCSOffset <= 0.0)
	{
		AfxMessageBox("Invalid ordinate interval", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_MAX_OFFSET)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_MAX_OFFSET))->SetSel(0, -1);
		return;
	}
	//
	if(m_Rec.data.bAutoDatumFlag)
	{
		if(m_DatumRangeArray.length() > 0)
		{
			m_DatumRangeArray.removeSubArray(0, m_DatumRangeArray.length() - 1);
			SmoothDatumRangeArray(m_DatumRangeArray);
		}
	}
	//
	GetDlgItem(IDC_EDIT_CS_INTERVAL_MINOR)->GetWindowText(strTmp);
	m_Rec.dCSIntervalMinor = atof(strTmp);
	if(m_Rec.dCSIntervalMinor <= 0.0)
	{
		AfxMessageBox("Invalid minor interval", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_CS_INTERVAL_MINOR)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_CS_INTERVAL_MINOR))->SetSel(0, -1);
		return;
	}

	CDialog::OnOK();
}
void CXSecDataDlg::OnCheckAutoDatum() 
{
	m_Rec.data.bAutoDatumFlag = ((CButton*)GetDlgItem(IDC_CHECK_AUTO_DATUM))->GetCheck();
	GetDlgItem(IDC_EDIT_DATUM_DENOMINATOR)->EnableWindow(m_Rec.data.bAutoDatumFlag);
	GetDlgItem(IDC_EDIT_MARGIN)->EnableWindow(m_Rec.data.bAutoDatumFlag);
	
	GetDlgItem(IDC_BUTTON_ADD_RANGE)->EnableWindow(!m_Rec.data.bAutoDatumFlag);
	GetDlgItem(IDC_BUTTON_DEL_RANGE)->EnableWindow(!m_Rec.data.bAutoDatumFlag);
	GetDlgItem(IDC_EDIT_DATUM_START)->EnableWindow(!m_Rec.data.bAutoDatumFlag);
	GetDlgItem(IDC_EDIT_DATUM_END)->EnableWindow(!m_Rec.data.bAutoDatumFlag);
	GetDlgItem(IDC_EDIT_DATUM_ELEV)->EnableWindow(!m_Rec.data.bAutoDatumFlag);
	GetDlgItem(IDC_LIST_LEV_RANGE)->EnableWindow(!m_Rec.data.bAutoDatumFlag);
}

void CXSecDataDlg::OnSelChangeComboSheet() 
{
	CComboBox* pCtrl;
	int iData, iIndex;
	CString strTmp;
	double fWid, fHt;

	pCtrl = (CComboBox*)GetDlgItem(IDC_COMBO_SHEET);
	iIndex = pCtrl->GetCurSel();
	if(iIndex == CB_ERR)
		return;
	iData = pCtrl->GetItemData(iIndex);
	strTmp = m_SheetSizeArray[iData];
	CXSecDataDlg::ParseSheetSizeDataRec(strTmp, fWid, fHt);

	strTmp.Format("%f", fWid);	GetDlgItem(IDC_EDIT_SHEET_WIDTH)->SetWindowText(strTmp);
	strTmp.Format("%f", fHt);	GetDlgItem(IDC_EDIT_SHEET_HEIGHT)->SetWindowText(strTmp);
}
void CXSecDataDlg::_FillDatumRangeList() 
{
	const CString strRangeDispFmt = "%12.3f - %12.3f";
	CString strTmp;
	CListBox* pCtrl;
	int i, iIndex;

	pCtrl = (CListBox*)GetDlgItem(IDC_LIST_LEV_RANGE);
	pCtrl->ResetContent();
	for(i = 0; i < m_DatumRangeArray.length(); i++)
	{
		strTmp.Format(strRangeDispFmt, m_DatumRangeArray[i].x, m_DatumRangeArray[i].y);
		iIndex = pCtrl->AddString(strTmp);
		pCtrl->SetItemData(iIndex, i);
	}
}

void CXSecDataDlg::OnButtonAddRange() 
{
	const CString strRangeDispFmt = "%12.3f - %12.3f";
	CString strTmp;
	double fStart, fEnd, fElev;
	CListBox* pCtrl;
	int iIndex, iIndexNew;

	GetDlgItem(IDC_EDIT_DATUM_START)->GetWindowText(strTmp);
	fStart = atof(strTmp);
	GetDlgItem(IDC_EDIT_DATUM_END)->GetWindowText(strTmp);
	fEnd = atof(strTmp);
	GetDlgItem(IDC_EDIT_DATUM_ELEV)->GetWindowText(strTmp);
	fElev = atof(strTmp);

	if(fStart >= fEnd)
	{
		AfxMessageBox("ERROR: Invalid range data", MB_ICONSTOP);
		return;
	}
	pCtrl = (CListBox*)GetDlgItem(IDC_LIST_LEV_RANGE);
	if(m_DatumRangeArray.length() <= 0)
	{
		if(pCtrl->GetCount() != 0)
		{
			AfxMessageBox("FATAL ERROR: Contact Developer!!", MB_ICONSTOP);
			return;
		}
		strTmp.Format(strRangeDispFmt, fStart, fEnd);
		iIndex = pCtrl->AddString(strTmp);
		m_DatumRangeArray.append(AcGePoint3d(fStart, fEnd, fElev));
		pCtrl->SetItemData(iIndex, m_DatumRangeArray.length() - 1);

		return;
	}
	if(IsValueDefined(fStart, m_DatumRangeArray) || IsValueDefined(fEnd, m_DatumRangeArray))
	{
		AfxMessageBox("ERROR: Range already defined !", MB_ICONSTOP);
		return;
	}

	iIndexNew = GetInsertionIndex(fStart, m_DatumRangeArray);

	strTmp.Format(strRangeDispFmt, fStart, fEnd);
	iIndex = pCtrl->InsertString(iIndexNew, strTmp);
    m_DatumRangeArray.insertAt(iIndexNew, AcGePoint3d(fStart, fEnd, fElev));
	pCtrl->SetItemData(iIndex, iIndexNew);
}


void CXSecDataDlg::OnButtonDelRange() 
{
	CListBox* pCtrl;
	int iIndex, iArrayIndex;

	pCtrl = (CListBox*)GetDlgItem(IDC_LIST_LEV_RANGE);
	iIndex = pCtrl->GetCurSel();
	if(iIndex == CB_ERR)
	{
		AfxMessageBox("ERROR: Nothing selected for deletion", MB_ICONSTOP);
		return;
	}
	iArrayIndex = pCtrl->GetItemData(iIndex);
	iIndex = pCtrl->DeleteString(iIndex);

    m_DatumRangeArray.removeAt(iArrayIndex);
}
void CXSecDataDlg::OnDblClkListLevRange() 
{
	OnButtonDelRange();
}
void CXSecDataDlg::OnSelChangeListLevRange() 
{
	CString strTmp;
	CListBox* pCtrl;
	int iIndex, iArrayIndex;

	pCtrl = (CListBox*)GetDlgItem(IDC_LIST_LEV_RANGE);
	iIndex = pCtrl->GetCurSel();
	if(iIndex == CB_ERR)
	{
		return;
	}
	iArrayIndex = pCtrl->GetItemData(iIndex);

	strTmp.Format("%f", m_DatumRangeArray[iArrayIndex].x); GetDlgItem(IDC_EDIT_DATUM_START)->SetWindowText(strTmp);
	strTmp.Format("%f", m_DatumRangeArray[iArrayIndex].y); GetDlgItem(IDC_EDIT_DATUM_END)->SetWindowText(strTmp);
	strTmp.Format("%f", m_DatumRangeArray[iArrayIndex].z); GetDlgItem(IDC_EDIT_DATUM_ELEV)->SetWindowText(strTmp);
}

static BOOL IsValueDefined(const double& fVal, const AcGePoint3dArray& Array)
{
	int i;
	BOOL bIsDefined;

	for(i = 0; i < Array.length(); i++)
	{
		bIsDefined = (fVal > Array[i].x) && (fVal < Array[i].y);
		if(bIsDefined)
			return TRUE;
		if((fVal == Array[i].x) && (fVal == Array[i].y))
			return TRUE;
	}
	return FALSE;
	//Old
	//	if((fVal <= Array[i].y) && (fVal >= Array[i].x))
	//		return TRUE;
}
static int GetInsertionIndex(const double& fVal, const AcGePoint3dArray& Array)
{
	int i;

	for(i = Array.length() - 1; i >= 0; i--)
	{
		if((i == Array.length() - 1) && (fVal >= Array[i].y))
			return Array.length();
		
		if((fVal > Array[i].x) && (fVal >= Array[i].y))
		{
			return i+1;
		}
	}
	return 0;
}
static void SmoothDatumRangeArray(AcGePoint3dArray& Array)
{
	int i;

	for(i = 1; i < Array.length(); i++)
	{
		if(Array[i].x != Array[i-1].y)
		{
			AcGePoint3d& Pt = Array[i];
			Pt.x = Array[i-1].y;
		}
	}
}
void CXSecDataDlg::FillCombo(CComboBox* pCtrl, const CStringArray& SheetSizeArray)
{
	int i, iIndex;
	CString strGUIFmt;
	
	pCtrl->ResetContent();
	for(i = 0; i < SheetSizeArray.GetSize(); i++)
	{
		strGUIFmt = ConvertInternalFmt2GUIFmt(SheetSizeArray[i]);
		iIndex = pCtrl->AddString(strGUIFmt);
		pCtrl->SetItemData(iIndex, i);
	}
}
void CXSecDataDlg::InitSheetArray(CStringArray& SheetSizeArray)
{
	if(SheetSizeArray.GetSize() != 0)
	{
		AfxMessageBox("Some error occured...Control should not reach here !", MB_ICONSTOP);
		return;
	}
	CString strTmp;

	strTmp.Format("%s%c%-4.1f%c%-4.1f", "A0", m_chShAttrSeparetor, 1189.0, m_chShAttrSeparetor, 841.0);SheetSizeArray.Add(strTmp);
	strTmp.Format("%s%c%-4.1f%c%-4.1f", "A1", m_chShAttrSeparetor,  841.0, m_chShAttrSeparetor, 595.0);SheetSizeArray.Add(strTmp);
	strTmp.Format("%s%c%-4.1f%c%-4.1f", "A2", m_chShAttrSeparetor,  595.0, m_chShAttrSeparetor, 421.0);SheetSizeArray.Add(strTmp);
	strTmp.Format("%s%c%-4.1f%c%-4.1f", "A3", m_chShAttrSeparetor,  421.0, m_chShAttrSeparetor, 297.0);SheetSizeArray.Add(strTmp);
	strTmp.Format("%s%c%-4.1f%c%-4.1f", "A4", m_chShAttrSeparetor,  297.0, m_chShAttrSeparetor, 421.0);SheetSizeArray.Add(strTmp);
}
void CXSecDataDlg::ParseSheetSizeDataRec(const CString& strData, double& fWid, double& fHt)
{
	CStringArray strArray;

	fWid = fHt = 0.0;
	if(!XTractWordsFromLine(strData, strArray))
		return;
	
	if(strArray.GetSize() != 3)
		return;

	fWid = atof(strArray[1]);
	fHt = atof(strArray[2]);
}
CString CXSecDataDlg::ConvertInternalFmt2GUIFmt(const CString& strParam)
{
	CString strToRet;
	int i, iSepCount;

	for(i = 0, iSepCount = 0; i < (int)strlen(strParam); i++)
	{
		if(strParam[i] == m_chShAttrSeparetor)
		{
			switch(iSepCount)
			{
			case 0:
				strToRet += "  ";
				break;
			case 1:
				strToRet += " X ";
				break;
			default:
				AfxMessageBox("Some error occured...Control should not reach here !", MB_ICONSTOP);
				return strToRet;
			}
			iSepCount++;
		}
		else
		{
			strToRet += strParam[i];
		}
	}
	return strToRet;
}

//////////////////////////////////////////////////
BOOL GetXSectionData(XSECDATAEX& Rec, AcGePoint3dArray& Array)
{
	CWnd* pAcadWnd;
	CTemporaryResourceOverride ResOverride;
	pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());

	CXSecDataDlg Dlg(Rec, Array, pAcadWnd);
	if(Dlg.DoModal() != IDOK)
		return FALSE;

	return TRUE;
}


