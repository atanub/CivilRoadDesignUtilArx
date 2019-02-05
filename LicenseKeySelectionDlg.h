#if !defined(_LICENSEKEYSELECTIONDLG_H_)
#define _LICENSEKEYSELECTIONDLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LicenseKeySelectionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLicenseKeySelectionDlg dialog

class CLicenseKeySelectionDlg : public CDialog
{
// Construction
public:
	CLicenseKeySelectionDlg(CWnd* pParent = NULL);   // standard constructor
public:
	const char* GetLicenseKey() const { return m_strLicenseKey;}
public:
	static const char* GetKey();

// Dialog Data
	//{{AFX_DATA(CLicenseKeySelectionDlg)
	enum { IDD = IDD_LICENSE_KEY_SELECTION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLicenseKeySelectionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLicenseKeySelectionDlg)
	virtual void OnOK();
	afx_msg void OnButtonFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CString m_strLicenseKey;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_LICENSEKEYSELECTIONDLG_H_)
