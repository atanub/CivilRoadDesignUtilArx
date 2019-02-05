// ModifyAnnotationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ResourceHelper.h"
#include "DrawUtil.h"
#include "Util.h"
#include "ModifyAnnotationDlg.h"


extern "C" HWND adsw_acadMainWnd();
/////////////////////////////////////////////////////////////////////////////
// CModifyAnnotationDlg dialog
const char* CModifyAnnotationDlg::m_pszAnnTags[] = {"Elevation", "Northing", "Easting", "Chainage"};

CModifyAnnotationDlg::CModifyAnnotationDlg(CWnd* pParent /*=NULL*/, const AnnotationType* pObjAnnType /*=NULL*/, const double* pfValue/*= NULL*/)
	: CDialog(CModifyAnnotationDlg::IDD, pParent)
{
	m_ObjAnnType = Elevation;
	if(pObjAnnType != 0L)
		m_ObjAnnType = *pObjAnnType;

	m_fValue = 0.0;
	if(pfValue != 0L)
		m_fValue = *pfValue;

	//{{AFX_DATA_INIT(CModifyAnnotationDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CModifyAnnotationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyAnnotationDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModifyAnnotationDlg, CDialog)
	//{{AFX_MSG_MAP(CModifyAnnotationDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyAnnotationDlg message handlers

BOOL CModifyAnnotationDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int i, iIndex;

	((CComboBox*)GetDlgItem(IDC_COMBO1))->ResetContent();
	for(i = 0; i < sizeof(CModifyAnnotationDlg::m_pszAnnTags) / sizeof(char*); i++)
	{
		iIndex = ((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(CModifyAnnotationDlg::m_pszAnnTags[i]);
		((CComboBox*)GetDlgItem(IDC_COMBO1))->SetItemData(iIndex, i);
	}
	return TRUE;
}

void CModifyAnnotationDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString strTemp;
	int i, iIndex;

	GetDlgItem(IDC_EDIT1)->GetWindowText(strTemp);
	m_fValue = atof(strTemp);
	if(m_fValue <= 0.0)
	{
		AfxMessageBox("Invalid value found", MB_ICONSTOP);
		GetDlgItem(IDC_EDIT1)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT1))->SetSel(0, -1);
		return;
	}
	iIndex = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();
	if(iIndex == CB_ERR)
	{
		AfxMessageBox("Annotation type not chosen", MB_ICONSTOP);
		GetDlgItem(IDC_COMBO1)->SetFocus();
		return;
	}
	i = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetItemData(iIndex);
	m_ObjAnnType = (AnnotationType)i;

	CDialog::OnOK();
}
void CModifyAnnotationDlg::GetAttrib(AnnotationType& objAnnType, double& fVal)
{
	objAnnType = m_ObjAnnType;
	fVal = m_fValue;
}

BOOL GetModAnnotationParam(double& fVal, AnnotationType& objAnnType)
{
	CWnd* pAcadWnd;
	CTemporaryResourceOverride ResOverride;
	pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());

	{//Initialize default data...
		fVal = 0.0;
		objAnnType = Elevation;
	}
	CModifyAnnotationDlg Dlg(pAcadWnd, &objAnnType, &fVal);
	if(Dlg.DoModal() != IDOK)
		return FALSE;


	Dlg.GetAttrib(objAnnType, fVal);
	return TRUE;
}

