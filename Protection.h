// Protection.h : header file
////////////////////////////////////////////////////////////////
#ifndef _PROTECTION_H_
#define _PROTECTION_H_


typedef struct tagPROTECTIONDATA
{
	double	fInstallDate,
			fLastAccessDate;
	int		iNoOfDaysAccessed;
} PROTECTIONDATA;

class CProtection  
{
public:
	CProtection();
	~CProtection();

	BOOL Initialize(const BOOL& bIsFirstRun = FALSE);
	CString GetDispStr();
private:
	static BOOL		_IsFileExists(const CString&);
	static BOOL		_GetFileName(CString&);

private:
	BOOL _ReadFile(const CString&);
	BOOL _WriteFile(const CString&, const BOOL& = FALSE);

private:
	static const CString m_strFileName;
	static const int	 m_iNoOfAllowableAxessDays;
	
	BOOL				 m_bCanRunApp;
	
	COleDateTime		 m_InstallationDate,
						 m_LastAccessDate;
	int				     m_iNoOfDaysAccessed;
};

/////////////////////////////////////////////////////////////////////////////
#endif //_PROTECTION_H_

