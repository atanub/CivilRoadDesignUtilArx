#if !defined(AFX_ANNOTATIONPARAMDLG_H__66D718C0_C440_11D3_A1D3_0080C8047C6F__INCLUDED_)
#define AFX_ANNOTATIONPARAMDLG_H__66D718C0_C440_11D3_A1D3_0080C8047C6F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AnnotationParamDlg.h : header file
//
#include "Resource.h"
#include "Structures.h"

/////////////////////////////////////////////////////////////////////////////
// CAnnotationParamDlg dialog

extern "C" BOOL GetAnnotationParam(ANNTNPARAMS&);

class CAnnotationParamDlg : public CDialog
{
// Construction
public:
	CAnnotationParamDlg(const ANNTNPARAMS&, CWnd* pParent = NULL);   // standard constructor
	const ANNTNPARAMS& GetAttrib() const { return m_RecModified;}

// Dialog Data
	//{{AFX_DATA(CAnnotationParamDlg)
	enum { IDD = IDD_DIALOG_ANNOTATE_CONTOUR_PARAM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnnotationParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAnnotationParamDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadioAngCntLines();
	afx_msg void OnRadioAngPath();
	afx_msg void OnRadioAngUser();
	afx_msg void OnRadioLyrAuto();
	afx_msg void OnRadioLyrCur();
	afx_msg void OnRadioLyrUser();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	const ANNTNPARAMS&	m_Rec;
	ANNTNPARAMS			m_RecModified;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANNOTATIONPARAMDLG_H__66D718C0_C440_11D3_A1D3_0080C8047C6F__INCLUDED_)
