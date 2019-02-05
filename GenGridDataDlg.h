#if !defined(AFX_GENGRIDDATADLG_H__C31101A0_CD91_11D3_A1D3_0080C8047C6F__INCLUDED_)
#define AFX_GENGRIDDATADLG_H__C31101A0_CD91_11D3_A1D3_0080C8047C6F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GenGridDataDlg.h : header file
//
#include "StdAfx.h"
#include "Resource.h"

BOOL GetGenGridData(double&, double&, double&, double&, BOOL&, CString&, CString&);
/////////////////////////////////////////////////////////////////////////////
// CGenGridDataDlg dialog

class CGenGridDataDlg : public CDialog
{
// Construction
public:
	CGenGridDataDlg(const double&, const double&, const double&, const double&, const BOOL&, const CString&, const CString&, CWnd* pParent = NULL);   // standard constructor
	void GetAttrib(double& fWidth, double& fHeight, double& fXROff, double& fYROff, BOOL& bFlag, CString& strPtLyr, CString& strLineLyr) const 
	{ fWidth = m_fWidth; fHeight = m_fHeight; fXROff = m_fXROff; fYROff = m_fYROff; bFlag = m_bDrawGrid; strPtLyr = m_strPtLyr; strLineLyr = m_strLineLyr;}
	static BOOL CheckROffValue(const double&, const double&);

// Dialog Data
	//{{AFX_DATA(CGenGridDataDlg)
	enum { IDD = IDD_TRANSLATE_TO_GRID };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenGridDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGenGridDataDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	double	m_fWidth,
			m_fHeight,
			m_fXROff,
			m_fYROff;
	
	CString m_strPtLyr,
			m_strLineLyr;

	BOOL   m_bDrawGrid;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENGRIDDATADLG_H__C31101A0_CD91_11D3_A1D3_0080C8047C6F__INCLUDED_)
