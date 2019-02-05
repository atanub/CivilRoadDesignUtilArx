#if !defined(AFX_PROTECTAPP_H__A40DFD0F_DC9E_11D3_8811_0080C8047EF6__INCLUDED_)
#define AFX_PROTECTAPP_H__A40DFD0F_DC9E_11D3_8811_0080C8047EF6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ProtectApp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProtectApp window

class CProtectApp  
{
// Construction
public:
	CProtectApp();
	BOOL IsFileExist(){return m_bFileExist;};
	CString GetFileName(){return m_StrFlname;}
	BOOL CreateFile();


	BOOL Initialize(const BOOL& bIsFirstRun = FALSE);

public:

// Operations
public:


// Implementation
public:
	virtual ~CProtectApp();

	// Generated message map functions
protected:
	CString m_StrFlname;
	BOOL m_bFileExist;

private :
	static const CString m_strFileName;
	BOOL				 m_bCanRunApp;
	static const int	 m_iNoOfAllowableAxessDays;
	COleDateTime		 m_InstallationDate,
						 m_LastAccessDate;
	int				     m_iNoOfDaysAccessed;


};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROTECTAPP_H__A40DFD0F_DC9E_11D3_8811_0080C8047EF6__INCLUDED_)
