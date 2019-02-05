#if !defined(AFX_ROADDESIGNDATADLG_H__334EEF69_83EA_4768_B9A5_F1E5EB09C474__INCLUDED_)
#define AFX_ROADDESIGNDATADLG_H__334EEF69_83EA_4768_B9A5_F1E5EB09C474__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RoadDesignDataDlg.h : header file
//
#include "CrossSectionProfileDefinition.h"
#include "VAlignment.h"

/////////////////////////////////////////////////////////////////////////////
// CRoadDesignDataDlg dialog

class CRoadDesignDataDlg : public CDialog
{
// Construction
public:
	CRoadDesignDataDlg(CWnd* pParent = NULL);   // standard constructor
	~CRoadDesignDataDlg();

// Dialog Data
	//{{AFX_DATA(CRoadDesignDataDlg)
	enum { IDD = IDD_PROFILE_DESIGN_DATA };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
public:
	static void GetData();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoadDesignDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRoadDesignDataDlg)
	virtual void OnOK();
	afx_msg void OnButtonPickValignment();
	afx_msg void OnButtonPickPline();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CGridCtrl* m_pGridCtrlSectionDefinition;
	CGridCtrl* m_pGridCtrlSectionSetting;
	CGridCtrl* m_pGridCtrlVAlignmentData;
protected:
	CCrossSectionProfileDefinitionArray m_arrayOfSectionDefinition;
	CCrossSectionProfileSettingArray m_arrayOfSectionSettings;
	CVAlignment* m_pVAlignment;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROADDESIGNDATADLG_H__334EEF69_83EA_4768_B9A5_F1E5EB09C474__INCLUDED_)
