#if !defined(AFX_MODIFYANNOTATIONDLG_H__F8755FE2_383D_11D5_86CD_ACE1112B239F__INCLUDED_)
#define AFX_MODIFYANNOTATIONDLG_H__F8755FE2_383D_11D5_86CD_ACE1112B239F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ModifyAnnotationDlg.h : header file
//
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CModifyAnnotationDlg dialog
typedef enum {Elevation = 0, Northing = 1, Easting = 2, Chainage = 3} AnnotationType;
BOOL GetModAnnotationParam(double&, AnnotationType&);

class CModifyAnnotationDlg : public CDialog
{
// Construction
public:
	CModifyAnnotationDlg(CWnd* pParent = NULL, const AnnotationType* = NULL, const double* = NULL);   // standard constructor
	void GetAttrib(AnnotationType&, double&);

// Dialog Data
	//{{AFX_DATA(CModifyAnnotationDlg)
	enum { IDD = IDD_MODIFY_SURV_TAGS_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifyAnnotationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModifyAnnotationDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	AnnotationType	m_ObjAnnType;
	double 			m_fValue;
	static const char*	m_pszAnnTags[];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFYANNOTATIONDLG_H__F8755FE2_383D_11D5_86CD_ACE1112B239F__INCLUDED_)
