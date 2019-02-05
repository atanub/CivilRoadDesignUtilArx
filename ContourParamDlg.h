#if !defined(AFX_CONTOURPARAMDLG_H__C9FF8EA0_A7EE_11D3_A1D3_0080C8047C6F__INCLUDED_)
#define AFX_CONTOURPARAMDLG_H__C9FF8EA0_A7EE_11D3_A1D3_0080C8047C6F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Resource.h"
#include "Structures.h"
// ContourParamDlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CContourParamDlg dialog

class CContourParamDlg : public CDialog
{
// Construction
public:
	CContourParamDlg(const CStringArray&, const DWGPARAMS* = 0L, CWnd* pParent = NULL);   // standard constructor
	const DWGPARAMS& GetAttrib() const { return m_DwgParams;}
public:
	static BOOL GetContourParam(DWGPARAMS&);

public:
// Dialog Data
	//{{AFX_DATA(CContourParamDlg)
	enum { IDD = IDD_DIALOG_CONTOUR_PARAM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContourParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CContourParamDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCheckAutoColor();
	afx_msg void OnCheckAutoTxtAnn();
	afx_msg void OnCheckSpLevel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	const CStringArray&	m_BlkNameArray;
	const DWGPARAMS*	m_pConstDwgParams;
	DWGPARAMS			m_DwgParams;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTOURPARAMDLG_H__C9FF8EA0_A7EE_11D3_A1D3_0080C8047C6F__INCLUDED_)
