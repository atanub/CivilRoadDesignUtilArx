// ResultDlg.h : header file
//
#if !defined(AFX_RESULTDLG_H__12FB0481_DBE5_11D2_A760_9C1A04C13107__INCLUDED_)
#define AFX_RESULTDLG_H__12FB0481_DBE5_11D2_A760_9C1A04C13107__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CResultDlg dialog

void ShowResultDlg(const CStringArray&);

class CResultDlg : public CDialog
{
// Construction
public:
	CResultDlg(const CStringArray&, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CResultDlg)
	enum { IDD = IDD_DIALOG_RESULT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResultDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResultDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CStringArray m_ResultStrArray;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESULTDLG_H__12FB0481_DBE5_11D2_A760_9C1A04C13107__INCLUDED_)
