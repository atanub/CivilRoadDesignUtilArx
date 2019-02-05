// XSecDataDlg.h : header file
//
#if !defined(AFX_XSECDATADLG_H__143076E1_F45E_11D3_86CC_B92A2A4E01FB__INCLUDED_)
#define AFX_XSECDATADLG_H__143076E1_F45E_11D3_86CC_B92A2A4E01FB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "Resource.h"
#include <acgi.h>

/////////////////////////////////////////////////////////////////////////////
// CXSecDataDlg dialog


BOOL GetXSectionData(XSECDATAEX&, AcGePoint3dArray&);

class CXSecDataDlg : public CDialog
{
// Construction
public:
	CXSecDataDlg(XSECDATAEX&, AcGePoint3dArray&, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CXSecDataDlg)
	enum { IDD = IDD_XSEC_DATA };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXSecDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	static void		FillCombo(CComboBox*, const CStringArray&);
	static void		InitSheetArray(CStringArray&);
	static void		ParseSheetSizeDataRec(const CString&, double&, double&);
protected:
	void _FillDatumRangeList();
	static CString	ConvertInternalFmt2GUIFmt(const CString&);
protected:
	const static char	m_chShAttrSeparetor;
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CXSecDataDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonAddRange();
	afx_msg void OnButtonDelRange();
	afx_msg void OnDblClkListLevRange();
	afx_msg void OnSelChangeListLevRange();
	afx_msg void OnSelChangeComboSheet();
	afx_msg void OnCheckAutoDatum();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	XSECDATAEX&				m_Rec;
	AcGePoint3dArray&		m_DatumRangeArray;
	CStringArray			m_SheetSizeArray;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XSECDATADLG_H__143076E1_F45E_11D3_86CC_B92A2A4E01FB__INCLUDED_)
