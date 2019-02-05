// ProfileDataDlg.h : header file
//
#if !defined(AFX_PROFILEDATADLG_H__850D34C1_F800_11D3_86CC_C97D650903E4__INCLUDED_)
#define AFX_PROFILEDATADLG_H__850D34C1_F800_11D3_86CC_C97D650903E4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Resource.h"
#include "Structures.h"


/////////////////////////////////////////////////////////////////////////////
// CProfileDataDlg dialog

class CProfileDataDlg : public CDialog
{
// Construction
public:
	CProfileDataDlg(PROFILEDATAEX&, CWnd* pParent = NULL);   // standard constructor
public:
	static BOOL GetProfileData(PROFILEDATAEX&);

// Dialog Data
	//{{AFX_DATA(CProfileDataDlg)
	enum { IDD = IDD_PROFILE_CHG_Z };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProfileDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProfileDataDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	PROFILEDATAEX& m_Rec;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROFILEDATADLG_H__850D34C1_F800_11D3_86CC_C97D650903E4__INCLUDED_)
