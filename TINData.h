// TINData.h
////////////////////////////////////////////////////////////////

#ifndef _TINDATA_H
#define _TINDATA_H

#include <rxobject.h>
#include <rxregsvc.h>
#include <aced.h>
#include <dbsymtb.h>
#include <adslib.h>
#include <actrans.h>
#include "Structures.h"

//
//Forward Declarations....
//

class CTINData : public AcDbObject
{
public:
    ACRX_DECLARE_MEMBERS(CTINData);
    CTINData();
    ~CTINData();


	//Overrides..
    Acad::ErrorStatus	dwgInFields (AcDbDwgFiler*);
    Acad::ErrorStatus	dwgOutFields(AcDbDwgFiler*) const;
	BOOL				GetElevAt(const double&, const double&, double&) const;

public:
	static void	FindElevRange(const AcGePoint3dArray&, double&, double&);
	static void RemoveTINDataObj(AcDbDatabase*);
	static void CreateTINDataObj(const AcGePoint3dArray&, const CTrianglePtrArray&);
	static BOOL GetTINData(AcGePoint3dArray&, CTrianglePtrArray* = 0L);
	static BOOL IsPresentInDatabase();
	static void DeallocateTrgArray(CTrianglePtrArray&);
	static CTINData* GetTINObject(AcDb::OpenMode);

	static void GenTIN();
protected:
	void				ExtractTINData(AcGePoint3dArray&, CTrianglePtrArray* = 0L) const;
	BOOL				SetTINData(const AcGePoint3dArray&, const CTrianglePtrArray&);
	

private:
    AcGePoint3dArray	m_PointArray;
    CTrianglePtrArray	m_TrgPtrArray;
};

#endif //_TINDATA_H
