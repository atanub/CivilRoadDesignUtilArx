//
// File Name: ContourToolbar.h
//

#ifndef __CONTOURTOOLBAR_H_
#define __CONTOURTOOLBAR_H_

// CContourToolbar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CContourToolbar 

class CContourToolbar : public CToolBar
{
// Construction
public:
	CContourToolbar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContourToolbar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CContourToolbar();
	virtual void Dump( CDumpContext& dc ) const;
	virtual void AssertValid( ) const;



	// Generated message map functions
protected:
	//{{AFX_MSG(CContourToolbar)
	afx_msg void OnUpdAlwaysEnabled(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CTBHandler window

class CTBHandler : public CWnd
{
// Construction
public:
	CTBHandler();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTBHandler)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTBHandler();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTBHandler)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
void CreateToolBar();
/////////////////////////////////////////////////////////////////////////////
#endif //__CONTOURTOOLBAR_H_
