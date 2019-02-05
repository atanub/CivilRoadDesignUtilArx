// ContourParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ContourApp.h"
#include "ContourParamDlg.h"
#include "ResourceHelper.h"
#include "DrawUtil.h"
#include "Util.h"
#include <ads.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" HWND adsw_acadMainWnd();

/////////////////////////////////////////////////////////////////////////////
// CContourParamDlg dialog


CContourParamDlg::CContourParamDlg(const CStringArray& BlkNameArray, const DWGPARAMS* pRec/* = 0L*/, CWnd* pParent /*=NULL*/)
	: CDialog(CContourParamDlg::IDD, pParent), m_pConstDwgParams(pRec), m_BlkNameArray(BlkNameArray)
{
	if(m_pConstDwgParams != 0L)
	{
		m_DwgParams = *m_pConstDwgParams;
	}
	else
	{
		memset(&m_DwgParams, 0, sizeof(m_DwgParams));
		m_DwgParams.fContourInterval = 5;
		m_DwgParams.iContourColorBase = 0;
		m_DwgParams.fTxtHeight	= 0;
		m_DwgParams.fTxtSpaMF = 0;

		strcpy(m_DwgParams.szPointXDFlag, "POINT_CONTOUR");
		strcpy(m_DwgParams.szAppName, "ATANU_BANIK");
		strcpy(m_DwgParams.szPointLyr, "POINT");
		strcpy(m_DwgParams.szTriangleLyr, "TRIANGLE");
		strcpy(m_DwgParams.szElevLyrPrefix, "CONTOUR_");
		m_DwgParams.chDotSeparator = '-';
	}
	//{{AFX_DATA_INIT(CContourParamDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CContourParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CContourParamDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CContourParamDlg, CDialog)
	//{{AFX_MSG_MAP(CContourParamDlg)
	ON_BN_CLICKED(IDC_CHECK1, OnCheckAutoColor)
	ON_BN_CLICKED(IDC_CHECK_AUTO_TXT_ANN, OnCheckAutoTxtAnn)
	ON_BN_CLICKED(IDC_CHECK_SP_LEVEL, OnCheckSpLevel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CContourParamDlg message handlers

BOOL CContourParamDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strTmp;
	BOOL bFlag;

	strTmp.Format("%f", m_DwgParams.fContourInterval);
	GetDlgItem(IDC_EDIT_INTERVEL)->SetWindowText(strTmp);

	//Version which is not registered 
	GetDlgItem(IDC_EDIT_INTERVEL)->EnableWindow(m_pConstDwgParams->bIsRegistered);

	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(m_DwgParams.iContourColorBase > 0);
	GetDlgItem(IDC_EDIT_BASE_CLR)->EnableWindow(m_DwgParams.iContourColorBase > 0);

	((CButton*)GetDlgItem(IDC_CHECK_AUTO_TXT_ANN))->SetCheck(m_DwgParams.fTxtHeight > 0.0);
	GetDlgItem(IDC_EDIT_TXT_SPA)->EnableWindow(m_DwgParams.fTxtHeight > 0.0);
	GetDlgItem(IDC_EDIT_TXT_HT)->EnableWindow(m_DwgParams.fTxtHeight > 0.0);

	strTmp.Format("%d", m_DwgParams.iContourColorBase); GetDlgItem(IDC_EDIT_BASE_CLR)->SetWindowText(strTmp);
	strTmp.Format("%f", m_DwgParams.fTxtSpaMF); 		GetDlgItem(IDC_EDIT_TXT_SPA)->SetWindowText(strTmp);
	strTmp.Format("%f", m_DwgParams.fTxtHeight);		GetDlgItem(IDC_EDIT_TXT_HT)->SetWindowText(strTmp);
	
	GetDlgItem(IDC_EDIT_POINT_LYR)->SetWindowText(m_DwgParams.szPointLyr);
	GetDlgItem(IDC_EDIT_TRG_LYR)->SetWindowText(m_DwgParams.szTriangleLyr);
	GetDlgItem(IDC_EDIT_CONTOUR_LYR_PREFIX)->SetWindowText(m_DwgParams.szElevLyrPrefix);

	//Spot Level Related.....
	bFlag = FALSE;
	if(m_DwgParams.fSpotLevTxtHt > 0.0)
		bFlag = TRUE;

	((CButton*)GetDlgItem(IDC_CHECK_SP_LEVEL))->SetCheck(bFlag);
	GetDlgItem(IDC_EDIT_LYR_SP_LEVEL)->EnableWindow(bFlag);
	GetDlgItem(IDC_EDIT_SP_LEV_TXT_HT)->EnableWindow(bFlag);
	GetDlgItem(IDC_EDIT_SP_LEV_ROT_ANGLE)->EnableWindow(bFlag);
	GetDlgItem(IDC_COMBO_BLK_NAME)->EnableWindow(bFlag);
	GetDlgItem(IDC_EDIT_BLK_SCALE)->EnableWindow(bFlag);
	{//Block Name Combo...
		int i;
		for(i = 0; i < m_BlkNameArray.GetSize(); i++)
		{
			((CComboBox*)GetDlgItem(IDC_COMBO_BLK_NAME))->AddString(m_BlkNameArray[i]);
		}
	}

	strTmp.Format("%f", m_DwgParams.fSpotLevTxtHt); GetDlgItem(IDC_EDIT_SP_LEV_TXT_HT)->SetWindowText(strTmp);
	strTmp.Format("%f", RTOD(m_DwgParams.fSpotLevRotAngle)); GetDlgItem(IDC_EDIT_SP_LEV_ROT_ANGLE)->SetWindowText(strTmp);
	GetDlgItem(IDC_EDIT_LYR_SP_LEVEL)->SetWindowText(m_DwgParams.szSpotLevLyr);
	if(strlen(m_DwgParams.szSpotLevBlock) > 0)
	{
		int iIndex;
		iIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_BLK_NAME))->FindStringExact(-1, m_DwgParams.szSpotLevBlock);
		if(iIndex != CB_ERR)
		{
			iIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_BLK_NAME))->SetCurSel(iIndex);
		}
	}
	strTmp.Format("%f", m_DwgParams.fSpotLevBlkRelScale); GetDlgItem(IDC_EDIT_BLK_SCALE)->SetWindowText(strTmp);
	return TRUE;
}
void CContourParamDlg::OnCheckAutoColor() 
{
	GetDlgItem(IDC_EDIT_BASE_CLR)->EnableWindow(((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck());
}
void CContourParamDlg::OnCheckAutoTxtAnn() 
{
	GetDlgItem(IDC_EDIT_TXT_SPA)->EnableWindow(((CButton*)GetDlgItem(IDC_CHECK_AUTO_TXT_ANN))->GetCheck());
	GetDlgItem(IDC_EDIT_TXT_HT)->EnableWindow(((CButton*)GetDlgItem(IDC_CHECK_AUTO_TXT_ANN))->GetCheck());
}
void CContourParamDlg::OnCheckSpLevel() 
{
	BOOL bFlag;
	
	bFlag = ((CButton*)GetDlgItem(IDC_CHECK_SP_LEVEL))->GetCheck();
	GetDlgItem(IDC_EDIT_LYR_SP_LEVEL)->EnableWindow(bFlag);
	GetDlgItem(IDC_COMBO_BLK_NAME)->EnableWindow(bFlag);
	GetDlgItem(IDC_EDIT_SP_LEV_TXT_HT)->EnableWindow(bFlag);
	GetDlgItem(IDC_EDIT_SP_LEV_ROT_ANGLE)->EnableWindow(bFlag);
	GetDlgItem(IDC_EDIT_BLK_SCALE)->EnableWindow(bFlag);
}

void CContourParamDlg::OnOK() 
{
	CString strTmp;

	GetDlgItem(IDC_EDIT_INTERVEL)->GetWindowText(strTmp);
	m_DwgParams.fContourInterval = atof(strTmp);
	if(m_DwgParams.fContourInterval <= 0.0)
	{
		AfxMessageBox("ERROR: Invalid contour interval", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT_INTERVEL))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_INTERVEL))->SetSel(0, -1);
		return;
	}
	
	
	if(TRUE == ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck())
	{
		GetDlgItem(IDC_EDIT_BASE_CLR)->GetWindowText(strTmp);
		m_DwgParams.iContourColorBase = atoi(strTmp);
		if((m_DwgParams.iContourColorBase <= 0) || (m_DwgParams.iContourColorBase >= 255))
		{
			AfxMessageBox("ERROR: Invalid base color code", MB_ICONSTOP);
			((CEdit*)GetDlgItem(IDC_EDIT_BASE_CLR))->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_BASE_CLR))->SetSel(0, -1);
			return;
		}
	}
	else
	{
		m_DwgParams.iContourColorBase = -1;
	}

	if(TRUE == ((CButton*)GetDlgItem(IDC_CHECK_AUTO_TXT_ANN))->GetCheck())
	{
		GetDlgItem(IDC_EDIT_TXT_HT)->GetWindowText(strTmp);
		m_DwgParams.fTxtHeight = atof(strTmp);
		if(m_DwgParams.fTxtHeight <= 0.0)
		{
			AfxMessageBox("ERROR: Invalid text size", MB_ICONSTOP);
			((CEdit*)GetDlgItem(IDC_EDIT_TXT_HT))->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_TXT_HT))->SetSel(0, -1);
			return;
		}
		GetDlgItem(IDC_EDIT_TXT_SPA)->GetWindowText(strTmp);
		m_DwgParams.fTxtSpaMF = atof(strTmp);
		if(m_DwgParams.fTxtSpaMF <= 1.0)
		{
			strTmp.Format("ERROR: Invalid text spacing multiplication factor");
			AfxMessageBox(strTmp, MB_ICONSTOP);
			((CEdit*)GetDlgItem(IDC_EDIT_TXT_SPA))->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_TXT_SPA))->SetSel(0, -1);
			return;
		}
	}
	else
	{
		m_DwgParams.fTxtHeight = -1.0;
		m_DwgParams.fTxtSpaMF = -1.0;
	}

	GetDlgItem(IDC_EDIT_POINT_LYR)->GetWindowText(strTmp);
	if(!CContourApp::IsValidACADSymName(strTmp))
	{
		AfxMessageBox("ERROR: Invalid layer name", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT_POINT_LYR))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_POINT_LYR))->SetSel(0, -1);
		return;
	}
	strcpy(m_DwgParams.szPointLyr, strTmp);

	GetDlgItem(IDC_EDIT_TRG_LYR)->GetWindowText(strTmp);
	if(!CContourApp::IsValidACADSymName(strTmp))
	{
		AfxMessageBox("ERROR: Invalid layer name", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT_TRG_LYR))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_TRG_LYR))->SetSel(0, -1);
		return;
	}
	strcpy(m_DwgParams.szTriangleLyr, strTmp);

	GetDlgItem(IDC_EDIT_CONTOUR_LYR_PREFIX)->GetWindowText(strTmp);
	if(!CContourApp::IsValidACADSymName(strTmp))
	{
		AfxMessageBox("ERROR: Invalid layer name", MB_ICONSTOP);
		((CEdit*)GetDlgItem(IDC_EDIT_CONTOUR_LYR_PREFIX))->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_CONTOUR_LYR_PREFIX))->SetSel(0, -1);
		return;
	}
	strcpy(m_DwgParams.szElevLyrPrefix, strTmp);

	//Spot Level Related.....
	if(TRUE == ((CButton*)GetDlgItem(IDC_CHECK_SP_LEVEL))->GetCheck())
	{
		int iIndex;

		GetDlgItem(IDC_EDIT_LYR_SP_LEVEL)->GetWindowText(strTmp);
		if(!CContourApp::IsValidACADSymName(strTmp))
		{
			AfxMessageBox("ERROR: Invalid layer name", MB_ICONSTOP);
			((CEdit*)GetDlgItem(IDC_EDIT_LYR_SP_LEVEL))->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_LYR_SP_LEVEL))->SetSel(0, -1);
			return;
		}
		strcpy(m_DwgParams.szSpotLevLyr, strTmp);

		iIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_BLK_NAME))->GetCurSel();
		if(iIndex == CB_ERR)
		{
			AfxMessageBox("No block selected", MB_ICONSTOP);
			GetDlgItem(IDC_COMBO_BLK_NAME)->SetFocus();
			return;
		}
		((CComboBox*)GetDlgItem(IDC_COMBO_BLK_NAME))->GetLBText(iIndex, strTmp);
		strcpy(m_DwgParams.szSpotLevBlock, strTmp);

		GetDlgItem(IDC_EDIT_SP_LEV_TXT_HT)->GetWindowText(strTmp);
		m_DwgParams.fSpotLevTxtHt = atof(strTmp);
		if(m_DwgParams.fSpotLevTxtHt <= 0.0)
		{
			strTmp.Format("ERROR: Invalid Spot Level text height");
			AfxMessageBox(strTmp, MB_ICONSTOP);
			((CEdit*)GetDlgItem(IDC_EDIT_SP_LEV_TXT_HT))->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_SP_LEV_TXT_HT))->SetSel(0, -1);
			return;
		}

		GetDlgItem(IDC_EDIT_SP_LEV_ROT_ANGLE)->GetWindowText(strTmp);
		m_DwgParams.fSpotLevRotAngle = DTOR(atof(strTmp));
		if((m_DwgParams.fSpotLevRotAngle < 0.0) || (m_DwgParams.fSpotLevRotAngle > (2 * PI)))
		{
			strTmp.Format("ERROR: Invalid Spot Level rotation angle");
			AfxMessageBox(strTmp, MB_ICONSTOP);
			((CEdit*)GetDlgItem(IDC_EDIT_SP_LEV_ROT_ANGLE))->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_SP_LEV_ROT_ANGLE))->SetSel(0, -1);
			return;
		}
		GetDlgItem(IDC_EDIT_BLK_SCALE)->GetWindowText(strTmp);
		m_DwgParams.fSpotLevBlkRelScale = atof(strTmp);
		if((m_DwgParams.fSpotLevBlkRelScale <= 0.0) || (m_DwgParams.fSpotLevBlkRelScale > 1.0))
		{
			strTmp.Format("ERROR: Invalid relative block scale");
			AfxMessageBox(strTmp, MB_ICONSTOP);
			((CEdit*)GetDlgItem(IDC_EDIT_BLK_SCALE))->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_BLK_SCALE))->SetSel(0, -1);
			return;
		}
	}
	else
	{
		m_DwgParams.fSpotLevTxtHt = -1.0;
		m_DwgParams.fSpotLevBlkRelScale = -1.0;
		m_DwgParams.fSpotLevRotAngle = 0.0;
	}

	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// 
BOOL CContourParamDlg::GetContourParam(DWGPARAMS& DwgParams)
{
	CWnd* pAcadWnd;
	CStringArray BlkNameArray;

	GetBlockNames(BlkNameArray);
	pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());
	CTemporaryResourceOverride Res;

	CContourParamDlg Dlg(BlkNameArray, &DwgParams, pAcadWnd);
	if(Dlg.DoModal() != IDOK)
	{
		return FALSE;
	}
	DwgParams = Dlg.GetAttrib();
	return TRUE;
}
