#if !defined(AFX_HORZCURVEDATADLG_H__7A06FBE1_52C5_11D4_86CC_A8FB4588151C__INCLUDED_)
#define AFX_HORZCURVEDATADLG_H__7A06FBE1_52C5_11D4_86CC_A8FB4588151C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// HorzCurveDataDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCurveDataDlg dialog
#include "Resource.h"
#include "Structures.h"
#include "CurveCalculator.h"
#include "ProfileDataDlg.h"
#include "XSecDataDlg.h"


class CCurveDataDlg : public CDialog
{
// Construction
public:
	CCurveDataDlg(const AcGePoint3d*, const AcGePoint3d*, const AcGePoint3d*, const double&, const double&, const double&, PROFILEDATAEX*, XSECDATAEX*, AcGePoint3dArray*, CWnd* pParent = NULL);   // standard constructor
	~CCurveDataDlg();
	double GetRadius() const { return m_fRadius;}

// Dialog Data
	//{{AFX_DATA(CCurveDataDlg)
	enum { IDD = IDD_DIALOG_HORZ_PI_DATA };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCurveDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	//for intermediate vertexes
	static BOOL GetCurveRadius(const AcGePoint3d*, const AcGePoint3d*, const AcGePoint3d*, double&, const double&, const double&, PROFILEDATAEX*, XSECDATAEX*, AcGePoint3dArray*);
	//for start/end vertexes
	static BOOL GetOnlyPflAndXSecParam(PROFILEDATAEX&, XSECDATAEX&, AcGePoint3dArray&);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCurveDataDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadioCurveLength();
	afx_msg void OnRadioDegOfCurve();
	afx_msg void OnRadioRadius();
	afx_msg void OnRadioTangentLength();
	afx_msg void OnRadioXternalLength();
	afx_msg void OnCalc();
	afx_msg void OnProfile();
	afx_msg void OnCrossSection();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	const AcGePoint3d* m_pPtStart;
	const AcGePoint3d* m_pPtPoI;
	const AcGePoint3d* m_pPtEnd;

	const double&			m_fFirstTanMaxLen;
	const double&			m_fSecondTanMaxLen;
	
	double					m_fRadius;

    PROFILEDATAEX*		m_pRecPflData;
    XSECDATAEX*			m_pRecXSecData;
	AcGePoint3dArray*	m_pDatumRangeArray;

protected:
	BOOL _ValidateData();
protected:
	CCurveCalculator* m_pCurveCalculator;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HORZCURVEDATADLG_H__7A06FBE1_52C5_11D4_86CC_A8FB4588151C__INCLUDED_)

