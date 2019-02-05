// DwgParamObj.h
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

#ifndef _DWGPARAMOBJ_H
#define _DWGPARAMOBJ_H

#include <rxobject.h>
#include <rxregsvc.h>
#include <aced.h>
#include <dbsymtb.h>
#include <adslib.h>
#include <actrans.h>
#include "Structures.h"


class CDwgParamObj : public AcDbObject
{
public:
    ACRX_DECLARE_MEMBERS(CDwgParamObj);
    CDwgParamObj();
	~CDwgParamObj();

	void				GetParamData(ANNTNPARAMS*) const;
	void				GetParamData(DWGPARAMS*) const;
	BOOL				SetParamData(const DWGPARAMS*);
	BOOL				SetParamData(const ANNTNPARAMS*);

	BOOL				GetBoundaryParamData(BOUNDARYDEF*) const;
	BOOL				SetBoundaryParamData(const BOUNDARYDEF*);
	//Overrides..
    Acad::ErrorStatus dwgInFields (AcDbDwgFiler*);
    Acad::ErrorStatus dwgOutFields(AcDbDwgFiler*) const;
public:
	static BOOL			CreateDwgParamObj(const DWGPARAMS* pRec = 0L);
	static BOOL			CreateAnnParamObj(const ANNTNPARAMS* pRec = 0L);
	static BOOL			RemoveDwgParamObj(AcDbDatabase* pCurDwg);
	static BOOL			GetDwgParamData(DWGPARAMS* pRec = 0L);
	static BOOL			GetAnnParamData(ANNTNPARAMS* pRec = 0L);
	static BOOL			CreateBoundaryParamObj(const BOUNDARYDEF* pRec);
	static BOOL			GetBoundaryParamData(BOUNDARYDEF* pRec = 0L);
private:
    DWGPARAMS		m_DwgParam;
    ANNTNPARAMS		m_AnnParam;
    BOUNDARYDEF		m_BoundaryDef;
};


#endif //_DWGPARAMOBJ_H
