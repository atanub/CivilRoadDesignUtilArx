// GenPtsFrMultiSegDlg.cpp : implementation file
//

#include "Stdafx.h"
#include "ContourApp.h"
#include "GenPtsFrMultiSegDlg.h"
#include "ResourceHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" HWND adsw_acadMainWnd();

/////////////////////////////////////////////////////////////////////////////
// CGenPtsFrMultiSegDlg dialog


CGenPtsFrMultiSegDlg::CGenPtsFrMultiSegDlg(AcGePoint3dArray& ArrayPts, AcDbIntArray& NoOfPtsArray, CWnd* pParent /*=NULL*/)
	: CDialog(CGenPtsFrMultiSegDlg::IDD, pParent), m_ArrayPts(ArrayPts), m_NoOfPtsArray(NoOfPtsArray), m_iCurSegValue(-1)
{
	//{{AFX_DATA_INIT(CGenPtsFrMultiSegDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGenPtsFrMultiSegDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenPtsFrMultiSegDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenPtsFrMultiSegDlg, CDialog)
	//{{AFX_MSG_MAP(CGenPtsFrMultiSegDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_SEGMENT, OnSelChangeComboSegment)
	ON_EN_KILLFOCUS(IDC_EDIT_NO_OF_PTS, OnKillFocusEditNoOfPts)
	ON_BN_CLICKED(IDC_BUTTON_SET_ALL, OnButtonSetAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenPtsFrMultiSegDlg message handlers

void CGenPtsFrMultiSegDlg::OnOK() 
{
	// TODO: Add extra validation here
	CComboBox* pCtrl;
	int i, iNoOfPts;
	
	OnSelChangeComboSegment();
	pCtrl = (CComboBox*)GetDlgItem(IDC_COMBO_SEGMENT);
	for(i = 0; i < pCtrl->GetCount(); i++)
	{
		iNoOfPts = pCtrl->GetItemData(i);
		m_NoOfPtsArray[i] = iNoOfPts;
	}
	
	CDialog::OnOK();
}
void CGenPtsFrMultiSegDlg::OnKillFocusEditNoOfPts() 
{
	CString strTmp;
	int iNoOfPts;

	GetDlgItem(IDC_EDIT_NO_OF_PTS)->GetWindowText(strTmp);
	iNoOfPts = atoi(strTmp);
	if(iNoOfPts <= 0)
	{
		AfxMessageBox("Invalid no of points specified", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_NO_OF_PTS)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_NO_OF_PTS))->SetSel(0, -1);
	}
}

void CGenPtsFrMultiSegDlg::OnSelChangeComboSegment() 
{
	CComboBox* pCtrl;
	int iIndex, iIndexPt;
	const CString strPtFmt = "X=%f, Y=%f, Z=%f";
	CString strTmp;
	
	pCtrl = (CComboBox*)GetDlgItem(IDC_COMBO_SEGMENT);
	if(m_iCurSegValue >= 0)
	{//Previous selection present 
		int iNoOfPts;

		GetDlgItem(IDC_EDIT_NO_OF_PTS)->GetWindowText(strTmp);
		iNoOfPts = atoi(strTmp);
		if(iNoOfPts <= 0)
			return;//ERROR

		m_NoOfPtsArray[m_iCurSegValue] = iNoOfPts;
		pCtrl->SetItemData(m_iCurSegValue, iNoOfPts);
	}

	iIndex = pCtrl->GetCurSel();
	if(iIndex == CB_ERR)
		return;
	m_iCurSegValue = iIndex;

	iIndexPt = pCtrl->GetItemData(iIndex);
	strTmp.Format("%d", iIndexPt);
	GetDlgItem(IDC_EDIT_NO_OF_PTS)->SetWindowText(strTmp);
	
	//
	strTmp.Format(strPtFmt, m_ArrayPts[iIndex].x, m_ArrayPts[iIndex].y, m_ArrayPts[iIndex].z);
	GetDlgItem(IDC_EDIT_START)->SetWindowText(strTmp);
	strTmp.Format(strPtFmt, m_ArrayPts[iIndex+1].x, m_ArrayPts[iIndex+1].y, m_ArrayPts[iIndex+1].z);
	GetDlgItem(IDC_EDIT_END)->SetWindowText(strTmp);
	strTmp.Format("%f", (m_ArrayPts[iIndex+1].z - m_ArrayPts[iIndex].z));
	GetDlgItem(IDC_EDIT_ELEV_DIFF)->SetWindowText(strTmp);
}

BOOL CGenPtsFrMultiSegDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int i, iIndex;
	CString strTmp;

	if(m_ArrayPts.length() != m_NoOfPtsArray.length())
	{
		AfxMessageBox("FATAL ERROR: Contact Developer!!");
		EndDialog(IDCANCEL);
		return TRUE;  // return TRUE unless you set the focus to a control
	}
	{//
		HFONT hFont;
		CComboBox* pCtrl;
		
		pCtrl = (CComboBox*)GetDlgItem(IDC_COMBO_SEGMENT);
		hFont = (HFONT)GetStockObject(ANSI_FIXED_FONT);
		ASSERT(hFont != 0L);
		pCtrl->SendMessage(WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
	}
	for(i = 1; i < m_ArrayPts.length(); i++)
	{
		strTmp.Format("Vertex %4d -> %4d", i+1-1, i+1);
		iIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_SEGMENT))->AddString(strTmp);
		if(iIndex != i - 1)
		{
			AfxMessageBox("FATAL ERROR: Contact Developer!!");
		}
		((CComboBox*)GetDlgItem(IDC_COMBO_SEGMENT))->SetItemData(iIndex, m_NoOfPtsArray[iIndex]);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///
BOOL GetGenPtsFrMultiSegParam(AcGePoint3dArray& ArrayPts, AcDbIntArray& NoOfPtsArray)
{
	CWnd* pAcadWnd;
	CTemporaryResourceOverride ResOverride;
	
	pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());

	CGenPtsFrMultiSegDlg Dlg(ArrayPts, NoOfPtsArray, pAcadWnd);
	if(Dlg.DoModal() != IDOK)
		return FALSE;

	return TRUE;
}


void CGenPtsFrMultiSegDlg::OnButtonSetAll() 
{
	if(IDYES != AfxMessageBox("Are you sure to set #of point data for each segment ?", MB_ICONQUESTION|MB_YESNO))
		return;

	CComboBox* pCtrl;
	CString strTmp;
	int i, iNoOfPts;
	
	GetDlgItem(IDC_EDIT_NO_OF_PTS)->GetWindowText(strTmp);
	iNoOfPts = atoi(strTmp);
	if(iNoOfPts <= 0)
	{
		AfxMessageBox("Invalid no of points specified", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT_NO_OF_PTS)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_NO_OF_PTS))->SetSel(0, -1);
	}

	pCtrl = (CComboBox*)GetDlgItem(IDC_COMBO_SEGMENT);
	for(i = 0; i < pCtrl->GetCount(); i++)
	{
		pCtrl->SetItemData(i, iNoOfPts);
	}
}
