// SubRoutines.h
/////////////////////////////////////////////////////////

#ifndef _SUBROUTINES_H
#define _SUBROUTINES_H

#include  "adslib.h"
#include  "rxdefs.h"

struct func_entry
{
	char *pszFuncName;
	int (*func) (void);
};


class CExportedSubRoutines
{
private:
	CExportedSubRoutines() {}
	~CExportedSubRoutines() {}

public:
	static int Initialize();
public:
	//Utility(s)
	static int TestSubR();
	static int InsCoOrdMarkerSubR();

private:
	#ifdef _DEBUG
		static CString DebugPrintRB(const resbuf *pRB);
	#endif//_DEBUG
};

#endif	//!_SUBROUTINES_H
