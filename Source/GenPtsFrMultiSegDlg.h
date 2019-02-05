#if !defined(AFX_GENPTSFRMULTISEGDLG_H__A1383F81_F38E_11D3_86CC_F6172AE25A7A__INCLUDED_)
#define AFX_GENPTSFRMULTISEGDLG_H__A1383F81_F38E_11D3_86CC_F6172AE25A7A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GenPtsFrMultiSegDlg.h : header file
//
#include "Resource.h"
#include "DrawUtil.h"
#include "Util.h"

/////////////////////////////////////////////////////////////////////////////
// CGenPtsFrMultiSegDlg dialog
BOOL GetGenPtsFrMultiSegParam(AcGePoint3dArray&, AcDbIntArray&);

class CGenPtsFrMultiSegDlg : public CDialog
{
// Construction
public:
	CGenPtsFrMultiSegDlg(AcGePoint3dArray&, AcDbIntArray&, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGenPtsFrMultiSegDlg)
	enum { IDD = IDD_GEN_POINTS_MULTI_SEG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenPtsFrMultiSegDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGenPtsFrMultiSegDlg)
	virtual void OnOK();
	afx_msg void OnSelChangeComboSegment();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocusEditNoOfPts();
	afx_msg void OnButtonSetAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	int					m_iCurSegValue;
	AcGePoint3dArray&	m_ArrayPts;
	AcDbIntArray&		m_NoOfPtsArray;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENPTSFRMULTISEGDLG_H__A1383F81_F38E_11D3_86CC_F6172AE25A7A__INCLUDED_)
