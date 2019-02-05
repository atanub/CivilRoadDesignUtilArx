#if !defined(AFX_GENPTSDLG_H__7A05BBE2_C1D4_11D3_A1D3_0080C8047C6F__INCLUDED_)
#define AFX_GENPTSDLG_H__7A05BBE2_C1D4_11D3_A1D3_0080C8047C6F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GenPtsDlg.h : header file
//
#include "stdafx.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CGenPtsDlg dialog

class CGenPtsDlg : public CDialog
{
// Construction
public:
	CGenPtsDlg(const BOOL&, const double&, const double&, CWnd* pParent = NULL);   // standard constructor
	int GetAttrib(double* pf = 0L) const {if(pf != 0L) *pf = m_fElevInput; return m_iNoOfPts;}

// Dialog Data
	//{{AFX_DATA(CGenPtsDlg)
	enum { IDD = IDD_GEN_POINTS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenPtsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGenPtsDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	const BOOL&	  m_bIsForInterpolation; 
	const double& m_fDist; 
	const double& m_fElev; 
	int			  m_iNoOfPts; 
	double		  m_fElevInput; 
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENPTSDLG_H__7A05BBE2_C1D4_11D3_A1D3_0080C8047C6F__INCLUDED_)
