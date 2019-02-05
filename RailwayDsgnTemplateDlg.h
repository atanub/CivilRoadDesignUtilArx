#if !defined(AFX_RAILWAYDSGNTEMPLATEDLG_H__BB792047_284A_42A7_8DE5_1E9C47D0621F__INCLUDED_)
#define AFX_RAILWAYDSGNTEMPLATEDLG_H__BB792047_284A_42A7_8DE5_1E9C47D0621F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RailwayDsgnTemplateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRailwayDsgnTemplateDlg dialog

class CRailwayDsgnTemplateDlg : public CDialog
{
// Construction
public:
	CRailwayDsgnTemplateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRailwayDsgnTemplateDlg)
	enum { IDD = IDD_DIALOG_RAILWAY_DESIGN_TEMPLATE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRailwayDsgnTemplateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRailwayDsgnTemplateDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CGridCtrl m_Grid;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAILWAYDSGNTEMPLATEDLG_H__BB792047_284A_42A7_8DE5_1E9C47D0621F__INCLUDED_)
