#ifndef _ABOUTDLG_H
#define _ABOUTDLG_H

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg(const char* pszLicenseInfoForUser, CWnd* pParentWnd);

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	static void ShowAboutDialog();

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeveloper();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnButtonLicenseReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	const char* m_pszLicenseInfoForUser;
};
/////////////////////////////////////////////////////////////////////////////
// CAboutMeDlg dialog
class CAboutMeDlg : public CDialog
{
// Construction
public:
	CAboutMeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAboutMeDlg)
	enum { IDD = IDD_ABOUTME };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutMeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAboutMeDlg)
		// NOTE: the ClassWizard will add member functions here
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif//_ABOUTDLG_H
