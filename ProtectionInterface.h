#if !defined(_PROTECTIONINTERFACEBASE_H)
#define _PROTECTIONINTERFACEBASE_H


//
// CProtectionInterface
/////////////////////////////////////////////////////////////////////////////
class CProtectionInterface : public CProtectionInterfaceBase
{
public:
	CProtectionInterface() {;}
	~CProtectionInterface(){;}
public:
	bool GetLicenseKeyFromUser(char*&) const;

};


#endif // !defined(_PROTECTIONINTERFACEBASE_H)
