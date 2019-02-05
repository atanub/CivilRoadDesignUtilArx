#ifndef _SYSTEMREGISTRYMANAGER_H
#define _SYSTEMREGISTRYMANAGER_H

#include "ProtectionInterface.h"

class CSystemRegistryManager
{
protected:
	CSystemRegistryManager(const CStringArray& arrayRegisteredCmds);
public:
	virtual ~CSystemRegistryManager();
public:
	virtual CApplcationProtector::FaultType	IsCurInstallationOK();
	virtual const char* GetLicenseInfo() const;
	bool				ResetLicenseRelatedData();
	bool				GetResetLicenseFlag() const { return m_bResetLicenseInfo;}

protected:
	CString _MakeRegDBKey();
	BOOL	_IsRegDBOk(const CString& strRegDBKey, CString& strAuthCode);
	BOOL	_IsActivatedBefore(const CString& strRegDBKey, CString& strActCode);
	BOOL	MakeDemandLoadCompatible(const CStringArray& strArrayCmds);
	
protected:
	virtual BOOL GetActivationData();
	virtual void _Activate(const CString& strACode, const CString& strRegDBKey);
protected:
	const CStringArray&		m_arrayRegisteredCmds;
	CString					m_strLicenseInfo;
	bool					m_bResetLicenseInfo;
};

//CSystemRegistryManagerEx
//////////////////////////////////////////////////////////////////////////////
class CSystemRegistryManagerEx : public CSystemRegistryManager
{
public:
	CSystemRegistryManagerEx(const CStringArray& arrayRegisteredCmds);
	virtual ~CSystemRegistryManagerEx();
public:
	virtual CApplcationProtector::FaultType IsCurInstallationOK();
protected:
	CApplcationProtector*	m_pApplcationProtector;
	CProtectionInterface*	m_pProtectionInterface;
};

#endif//_SYSTEMREGISTRYMANAGER_H
