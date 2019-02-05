//CoOrdMarker.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _COORDMARKER_H_
#define _COORDMARKER_H_

#include <dbents.h>
#include <acgi.h>

#include "Structures.h"


class CCoOrdMarker : public  AcDbEntity
{
public:
    ACRX_DECLARE_MEMBERS(CCoOrdMarker);
    CCoOrdMarker();
    CCoOrdMarker(const AcGeVector3d&, const AcGePoint3d&, const AcDbObjectId& ObjIDTxtStyle, const double&, const double&, const double&, const short&);

    virtual ~CCoOrdMarker();

    virtual Adesk::Boolean    worldDraw(AcGiWorldDraw*);
    virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler*);
    virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler*) const;
    virtual Acad::ErrorStatus audit(AcDbAuditInfo*);
	void	saveAs(AcGiWorldDraw*, AcDb::SaveType);

    // AcDbEntity overrides
    //
    virtual Acad::ErrorStatus explode(AcDbVoidPtrArray&) const;
    virtual void              list() const;

	Acad::ErrorStatus getOsnapPoints(AcDb::OsnapMode, int, const AcGePoint3d&, const AcGePoint3d&, const AcGeMatrix3d&, AcGePoint3dArray&, AcDbIntArray&) const;
	Acad::ErrorStatus getGripPoints(AcGePoint3dArray&, AcDbIntArray&, AcDbIntArray&) const;
	Acad::ErrorStatus getStretchPoints(AcGePoint3dArray&) const;

	Acad::ErrorStatus moveStretchPointsAt(const AcDbIntArray&, const AcGeVector3d&);
	Acad::ErrorStatus moveGripPointsAt(const AcDbIntArray&, const AcGeVector3d&);
	Acad::ErrorStatus transformBy(const AcGeMatrix3d&);

public:
	static void CoOrdMarker();
	static void EditCoOrdMarker();
	static void InsCoOrdMarker();
	static void CoOrdMarker(const AcGePoint3d&, const double&, const double&, const double&, const short&, const char* =0L);

// data members
public:
	double		GetAngle()		const { return m_fAngleInRad;}
	double		GetTxtSize()	const { return m_fTxtSize;}
	double		GetMarkerSize() const { return m_fMarkerScale;}
	short		GetLUPREC()		const { return m_iLUPREC;}
    AcGePoint3d GetLocation()	const { return m_Point;} 

	void SetAngle(const double& Val)				{ m_fAngleInRad = Val;}
	void SetTxtSize(const double& Val)				{ m_fTxtSize = Val;}
	void SetMarkerSize(const double& Val)			{ m_fMarkerScale = Val;}
	void SetLUPREC(const short& Val)				{ m_iLUPREC = Val;}
    void SetLocation(const AcGePoint3d& ptParam)	{ m_Point = ptParam;} 

private:
	void DrawText(AcGiWorldDraw*, const AcGePoint3d&, const AcDbHardPointerId&, AcDbVoidPtrArray* = 0L) const;

private:
	static void MakeStrings(const double&, const short&, CString&, CString&);
	static BOOL GetCoOrdMarkerEnt(AcDbObjectIdArray&);
	static BOOL GetCoOrdMarkerEntProperty(const AcDbObjectId&, double&, double&, double&, short&);

	static Acad::ErrorStatus GetAcGiTextStyle(AcGiTextStyle&, const AcDbObjectId&);

private:
	const static double	m_fMinmMarkerSizeWRTRad;
    AcGePoint3d			m_Point;
    AcDbHardPointerId	m_DbObjIDTxtStyle;

	short				m_iLUPREC;
    double				m_fTxtSize,
						m_fAngleInRad,
						m_fMarkerScale;
    AcGeVector3d		m_Normal;
};

#endif //_COORDMARKER_H_
