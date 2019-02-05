#if !defined(AFX_COORDMARKERPROPDLG_H__67E75840_CB3B_11D3_A1D3_0080C8047C6F__INCLUDED_)
#define AFX_COORDMARKERPROPDLG_H__67E75840_CB3B_11D3_A1D3_0080C8047C6F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CoOrdMarkerPropDlg.h : header file
//

#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CCoOrdMarkerPropDlg dialog

BOOL GetCoOrdMarkerProperty(double&, double&, double&, short&);

class CCoOrdMarkerPropDlg : public CDialog
{
// Construction
public:
	CCoOrdMarkerPropDlg(const double&, const double&, const double&, const short&, CWnd* pParent = NULL);   // standard constructor
	void GetAttrib(double&, double&, double&, short&);


// Dialog Data
	//{{AFX_DATA(CCoOrdMarkerPropDlg)
	enum { IDD = IDD_COORD_ANN_ENT_PROP_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCoOrdMarkerPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCoOrdMarkerPropDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	const double&	m_fConstAngleInRad;
	const double&	m_fConstTxtSize;
	const double&	m_fConstMarkerScale;
	const short&	m_iConstLuprec;

	double	m_fAngleInDeg;
	double	m_fTxtSize;
	double	m_fMarkerScale;
	short	m_iLuprec;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COORDMARKERPROPDLG_H__67E75840_CB3B_11D3_A1D3_0080C8047C6F__INCLUDED_)
