#if !defined(AFX_ACTIVATEDLG_H__8B4E0FA1_120D_11D3_A760_4C4807C1440E__INCLUDED_)
#define AFX_ACTIVATEDLG_H__8B4E0FA1_120D_11D3_A760_4C4807C1440E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ActivateDlg.h : header file
//
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CActivateDlg dialog

class CActivateDlg : public CDialog
{
// Construction
public:
	CActivateDlg(const CString&, const CString&, CWnd* pParent = NULL);   // standard constructor
public:
	static BOOL GetActivationData();

public:
// Dialog Data
	//{{AFX_DATA(CActivateDlg)
	enum { IDD = IDD_DIALOG_ACTIVATION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActivateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CActivateDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	const CString m_ConstStrMsg;
	const CString m_ConstActCode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIVATEDLG_H__8B4E0FA1_120D_11D3_A760_4C4807C1440E__INCLUDED_)
