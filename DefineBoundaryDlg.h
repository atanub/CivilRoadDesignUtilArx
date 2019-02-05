#if !defined(AFX_DEFINEBOUNDARYDLG_H__16ABE8C1_3EFD_11D4_86CC_E82E4EC4A17F__INCLUDED_)
#define AFX_DEFINEBOUNDARYDLG_H__16ABE8C1_3EFD_11D4_86CC_E82E4EC4A17F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DefineBoundaryDlg.h : header file
//
#include "Resource.h"
#include "Structures.h"

/////////////////////////////////////////////////////////////////////////////
// CDefineBoundaryDlg dialog

class CDefineBoundaryDlg : public CDialog
{
// Construction
public:
	CDefineBoundaryDlg(BOUNDARYDEF*, CWnd* pParent = NULL);
public:
	static void DefineBoundary();

public:
// Dialog Data
	//{{AFX_DATA(CDefineBoundaryDlg)
	enum { IDD = IDD_DIALOG_CONTOUR_BOUNDARY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefineBoundaryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefineBoundaryDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonNewHole();
	afx_msg void OnButtonDelHole();
	afx_msg void OnButtonDelAllHoles();
	afx_msg void OnButtonSelBoundary();
	afx_msg void OnCheckDefineBoundary();
	afx_msg void OnCheckConsiderAllPts();
	virtual void OnOK();
	afx_msg void OnButtonViewBoundary();
	afx_msg void OnButtonViewHole();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	BOUNDARYDEF* m_pBoundaryDef;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFINEBOUNDARYDLG_H__16ABE8C1_3EFD_11D4_86CC_E82E4EC4A17F__INCLUDED_)
