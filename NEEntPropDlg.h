#if !defined(AFX_NEENTPROPDLG_H__67E75840_CB3B_11D3_A1D3_0080C8047C6F__INCLUDED_)
#define AFX_NEENTPROPDLG_H__67E75840_CB3B_11D3_A1D3_0080C8047C6F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NEEntPropDlg.h : header file
//

#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CNEEntPropDlg dialog

BOOL GetNEEntProperty(double&, double&, short&);

class CNEEntPropDlg : public CDialog
{
// Construction
public:
	CNEEntPropDlg(const double&, const double&, const short&, CWnd* pParent = NULL);   // standard constructor
	void GetAttrib(double&, double&, short&);


// Dialog Data
	//{{AFX_DATA(CNEEntPropDlg)
	enum { IDD = IDD_ANN_ENTITY_PROP_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNEEntPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNEEntPropDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	const double&	m_fConstTxtSize;
	const double&	m_fConstArrSize;
	const short&	m_iConstLuprec;

	double	m_fTxtSize;
	double	m_fArrSize;
	short	m_iLuprec;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEENTPROPDLG_H__67E75840_CB3B_11D3_A1D3_0080C8047C6F__INCLUDED_)
