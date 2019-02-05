// ResultDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ResultDlg.h"
#include "ResourceHelper.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" HWND adsw_acadMainWnd();
/////////////////////////////////////////////////////////////////////////////
// CResultDlg dialog



CResultDlg::CResultDlg(const CStringArray& strArray, CWnd* pParent /*=NULL*/)
	: CDialog(CResultDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResultDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	{//Copy 'CStringArray' Param
		int i;
		for(i = 0; i < strArray.GetSize(); i++)
			m_ResultStrArray.Add(strArray[i]);
	}
}
void CResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResultDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(CResultDlg, CDialog)
	//{{AFX_MSG_MAP(CResultDlg)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultDlg message handlers

BOOL CResultDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	{//Copy 'CStringArray' Param
		const CString strConstRowSep("\r\n");
		int i;
		CString strToShow;
		for(i = 0; i < m_ResultStrArray.GetSize(); i++)
		{
			if(i == 0)
				strToShow = m_ResultStrArray[i];
			else
				strToShow += strConstRowSep + m_ResultStrArray[i];
		}
		GetDlgItem(IDC_EDIT_RESULT)->SetWindowText(strToShow);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CResultDlg::OnButtonSave() 
{
	CString strMsg, strFile;

	CFileDialog Dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, 0L, this);
	if(Dlg.DoModal() != IDOK)
		return;
	strFile = Dlg.GetPathName();
	{//for ASCII Txt file
		int i;
		CStdioFile FileOut;
		CFileException e;
		const CString strConstRowSep("\n");

		if(!FileOut.Open(strFile, CFile::modeWrite|CFile::typeText|CFile::modeCreate, &e))
		{
			strMsg.Format("File \"%s\" could not be opened (Error# = %d)", strFile, e.m_cause);
			AfxMessageBox(strMsg, MB_ICONSTOP);
			return;
		}
		for(i = 0; i < m_ResultStrArray.GetSize(); i++)
		{
			FileOut.WriteString(m_ResultStrArray[i] + "\n");
		}
		FileOut.Close();
	}
	strMsg.Format("File \"%s\" created successfully", strFile);
	AfxMessageBox(strMsg, MB_ICONINFORMATION);
}

void ShowResultDlg(const CStringArray& Array)
{
	CWnd* pAcadWnd;
	CTemporaryResourceOverride ResOverride;
	
	pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());

	CResultDlg Dlg(Array, pAcadWnd);
	Dlg.DoModal();
}
