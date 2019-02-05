// SubRoutines.cpp - SurvUtilARX Call this functions using ADSRX Interface
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"           // MFC stuff

#include "acdb.h"             // acdb definitions
#include "adslib.h"           // ads defs
#include "aced.h"             // aced stuff
#include "dbmain.h"           // object and entity reactors
#include "dbdict.h"           // dictionaries
#include "actrans.h"          // transaction manager
#include "resourcehelper.h"   // temporary resource class
#include "rxmfcapi.h"         // ACAD MFC stuff
#include "rxdefs.h"
#include "SubRoutines.h"
#include "CoOrdMarker.h"
#include "DrawUtil.h"
#include "CoOrdMarkerPropDlg.h"

#define ELEMENTS(array) (sizeof(array)/sizeof((array)[0]))
static struct func_entry func_table[] = {
											{"TestSubR",		CExportedSubRoutines::TestSubR},
											{"InsCoOrdMarker",	CExportedSubRoutines::InsCoOrdMarkerSubR}
                                        };

int CExportedSubRoutines::Initialize()
{
    int i;

    for (i = 0; i < ELEMENTS(func_table); i++)
	{
		if (RTNORM != ads_defun(func_table[i].pszFuncName, i))
		{
			ads_printf("\n\nFATAL ERROR: Function registration(A) failed!! @ SubRoutines.cpp \n\n");
			return RTERROR;
		}
		if (RTNORM != ads_regfunc(func_table[i].func, i))
		{
			ads_printf("\n\nFATAL ERROR: Function registration(B) failed!! @ SubRoutines.cpp \n\n");
			return RTERROR;
		}
	}
	#ifdef _DEBUG
		ads_printf("\nTRACE: Function registration successful!!\n");
	#endif//_DEBUG
    return RTNORM;
}
#ifdef _DEBUG
	CString CExportedSubRoutines::DebugPrintRB(const resbuf *pRB)
	{
		if(pRB == 0L)
		{
			return CString("0L");
		}
		switch(pRB->restype)
		{
		case RTNONE:
			return CString("RTNONE");
		case RTREAL:
			return CString("RTREAL");
		case RTPOINT:
			return CString("RTPOINT");
		case RTSHORT:
			return CString("RTSHORT");
		case RTANG:
			return CString("RTANG");
		case RTSTR:
			return CString("RTSTR");
		case RTENAME:
			return CString("RTENAME");
		case RTPICKS:
			return CString("RTPICKS");
		case RTORINT:
			return CString("RTORINT");
		case RT3DPOINT:
			return CString("RT3DPOINT");
		case RTLONG:
			return CString("RTLONG");
		case RTVOID:
			return CString("RTVOID");
		case RTLB:
			return CString("RTLB");
		case RTLE:
			return CString("RTLE");
		case RTDOTE:
			return CString("RTDOTE");
		case RTNIL:
			return CString("RTNIL");
		case RTDXF0:
			return CString("RTDXF0");
		case RTT:
			return CString("RTT");
		case RTRESBUF:
			return CString("");
		default:
			break;
		}
		return CString("UNKNOWN");
	}
#endif//ifdef _DEBUG
int CExportedSubRoutines::TestSubR()
{
    resbuf *pRB;
    
	pRB = ads_getargs();
    if(pRB == 0L)
	{
        ads_printf("\nERROR: No Argument received!");
		ads_retint(0);//return the value to AutoLISP
        return RTERROR;
	}
#ifdef _DEBUG
	CString strTmp;
	
	strTmp = DebugPrintRB(pRB);
    ads_printf("\nTRACE: Argument of type %s received!", (LPCSTR)strTmp);
#endif//ifdef _DEBUG
	
    ads_retint(1);//return the value to AutoLISP
    return RTNORM;
}
int CExportedSubRoutines::InsCoOrdMarkerSubR()
{
	resbuf*  pTemp;
	int i;
	AcGePoint3d ptOfIns;
	double fTxtSize, fMarkerScale, fAngleInRad;
	double* pfPtrs[]  = {&fTxtSize, &fMarkerScale, &fAngleInRad};
	short iLuprec;
 	CString strLyr;
   resbuf *pRB;
    
	pRB = ads_getargs();
    if(pRB == 0L)
	{
        ads_printf("\nERROR: No Parameter received!");
		ads_retint(0);//return the value to AutoLISP ->>Failure
        return RTERROR;
	}
#ifdef _DEBUG
	for(pTemp = pRB, i = 0; pTemp != 0L; pTemp = pTemp->rbnext, i++)
	{
        ads_printf("\nParam#%d : %s", i, (LPCSTR)DebugPrintRB(pTemp));
	}
    //ads_retint(1);//return the value to AutoLISP ->>Success
    //return RTNORM;
#endif//ifdef _DEBUG

	for(pTemp = pRB, i = 0; (pTemp != 0L) && (i < (sizeof(pfPtrs)/sizeof(double*))); pTemp = pTemp->rbnext, i++)
	{
		if(pTemp->restype == RTREAL)
		{
	        *(pfPtrs[i]) = pTemp->resval.rreal;
		}
		else
		{
			ads_printf("\nERROR: Invalid Parameter received! <<Element# %d>>", i);
			ads_retint(0);//return the value to AutoLISP ->>Failure
			return RTERROR;
		}
	}
	if(0L != pTemp)
	{//LUPREC
		if(pTemp->restype == RTSHORT)
		{
	        iLuprec = pTemp->resval.rint;
		}
		else
		{
			ads_printf("\nERROR: Invalid Parameter <LUPREC> received!");
			ads_retint(0);//return the value to AutoLISP ->>Failure
			return RTERROR;
		}
	}
	pTemp = pTemp->rbnext;
	if(0L != pTemp)
	{//Insertion Point
		if(pTemp->restype == RT3DPOINT)
		{
	        ptOfIns.x = pTemp->resval.rpoint[0];
	        ptOfIns.y = pTemp->resval.rpoint[1];
	        ptOfIns.z = pTemp->resval.rpoint[2];
		}
		else
		{
			ads_printf("\nERROR: Invalid Parameter <Insertion Point> received!");
			ads_retint(0);//return the value to AutoLISP ->>Failure
			return RTERROR;
		}
	}
	pTemp = pTemp->rbnext;
	if(0L != pTemp)
	{//Layer Name
		if(pTemp->restype == RTSTR)
		{
	        strLyr.Format("%s", pTemp->resval.rstring);
		}
		else
		{
			ads_printf("\nERROR: Invalid Parameter <Layer Name> received!");
			ads_retint(0);//return the value to AutoLISP ->>Failure
			return RTERROR;
		}
	}
	CCoOrdMarker::CoOrdMarker(ptOfIns, fTxtSize, fMarkerScale, fAngleInRad, iLuprec, strLyr);
	ads_retint(1);//return the value to AutoLISP ->>Success
    return RTNORM;
}

