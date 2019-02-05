//TINEntity.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _TINENTITY_H_
#define _TINENTITY_H_

#include <dbents.h>
#include <acgi.h>

#include "Structures.h"

class TINEntity : public  AcDbEntity
{
public:
    ACRX_DECLARE_MEMBERS(TINEntity);
    TINEntity();
    TINEntity(const AcGePoint3dArray&, const CTrianglePtrArray&);
    virtual ~TINEntity();

    virtual Adesk::Boolean    worldDraw(AcGiWorldDraw*);
    virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler*);
    virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler*) const;
    virtual Acad::ErrorStatus audit(AcDbAuditInfo*);

    // AcDbEntity overrides
    //
    virtual Acad::ErrorStatus	explode(AcDbVoidPtrArray&) const;
    virtual void				list() const;
    virtual void				saveAs(AcGiWorldDraw* mode, AcDb::SaveType saveType);
    virtual Acad::ErrorStatus   getOsnapPoints(AcDb::OsnapMode, int, const AcGePoint3d&, const AcGePoint3d&, const AcGeMatrix3d&, AcGePoint3dArray&, AcDbIntArray&) const;
	virtual Acad::ErrorStatus	intersectWith(const AcDbEntity*, AcDb::Intersect, AcGePoint3dArray&, int = 0, int = 0) const;

    const AcGePoint3dArray&		GetPointData() const { return m_PointArray;} 
    const CTrianglePtrArray&	GetTrgData() const { return m_TrgPtrArray;} 
    
	static BOOL					GetIntersectionPts(const AcGePoint3dArray&, const CTrianglePtrArray&, const AcGePoint3d&, const AcGePoint3d&, AcGePoint3dArray&);
	static void					GetExtents(const AcGePoint3dArray&, double&, double&, double&, double&, const double& = 0.0, const double& = 0.0);
	static BOOL					CalculateVolume(const AcGePoint3dArray&, const CTrianglePtrArray&, const AcGePoint3dArray&, const double&, const double&, double&, CStringArray* = 0L);
	static BOOL					IsPointWithinBoundary(const AcGePoint3dArray&, const CTrianglePtrArray&, const AcGePoint3d&, double* pfZValue = 0L);

	BOOL						GetProperty(double&, double&) const;
	// data members
private:
    AcGePoint3dArray	m_PointArray;
    CTrianglePtrArray	m_TrgPtrArray;
};

MAKE_ACDBOPENOBJECT_FUNCTION(TINEntity);


#endif //_TINENTITY_H_
