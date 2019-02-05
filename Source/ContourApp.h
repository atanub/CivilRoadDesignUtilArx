// ContourApp.h : header file
//

#ifndef __CONTOURAPP_H_
#define __CONTOURAPP_H_

#include "ContourToolbar.h"
#include "ResourceHelper.h"
#include "Resource.h"

class CSystemRegistryManager;
/////////////////////////////////////////////////////////////////////////////
// CContourToolbar 
class CContourApp
{
// Construction
protected:
	CContourApp(HINSTANCE hInstance);
	~CContourApp();
public:
	static CContourApp*			Instance() { return m_pTheOnlyInstance;}
	static CContourApp*			Initialize(HINSTANCE hInstance);
	static void					UnInitialize();
	static BOOL					HasValidLicense();
	static const char*			GetLicenseInfo();
	static BOOL					ResetLicenseInfo();
	static BOOL					GetResetLicenseFlag();

	//
	static void					RegisterCmd(const char*);
	static void					RegisterCmdGroup(const char*);
	//Utility Func.
	static BOOL					IsValidACADSymName(const char*);
	static CString				GetThisAppPath();
	static void					SendCmdToAutoCAD(CString& strCmd);
	
protected:
	void CreateToolBar();

// Attributes
protected:
	CContourToolbar*			m_pToolBar;
	CTBHandler*					m_pToolBarHandler;
	CSystemRegistryManager*		m_pSystemRegistryManager;
	//
	BOOL						m_bHasValidLicense;
	CString						m_strLicenseInfo;
	HINSTANCE					m_hInstance;
protected:
	static CContourApp*			m_pTheOnlyInstance;
public:
	static const CString gStrDictionaryName;
	static const CString gStrTINDataSexn;
	static const CString gStrDwgParamSexn;
	static const CString m_strConstThisARXName;
	//
	static CStringArray			m_arrayRegisteredCmds;
	static CStringArray			m_arrayRegisteredCmdGroups;
};
/////////////////////////////////////////////////////////////////////////////
#endif //__CONTOURAPP_H_
