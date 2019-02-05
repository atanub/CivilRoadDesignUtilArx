//NEAnnotationEntity.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _NEANNOTATIONENTITY_H_
#define _NEANNOTATIONENTITY_H_

#include <dbents.h>
#include <acgi.h>

#include "Structures.h"

class NEAnnotationEntity : public  AcDbEntity
{
public:
    ACRX_DECLARE_MEMBERS(NEAnnotationEntity);
    NEAnnotationEntity();
    NEAnnotationEntity(const AcGeVector3d&, const AcGePoint3dArray&, const AcDbObjectId& ObjIDTxtStyle, const double&, const double&, const short&);
    NEAnnotationEntity(const AcGeVector3d&, const AcGePoint3d&, const AcDbObjectId& ObjIDTxtStyle, const double&, const double&, const short&);
    virtual ~NEAnnotationEntity();

    virtual Adesk::Boolean    worldDraw(AcGiWorldDraw*);
    virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler*);
    virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler*) const;
    virtual Acad::ErrorStatus audit(AcDbAuditInfo*);
	void	saveAs(AcGiWorldDraw*, AcDb::SaveType);

    // AcDbEntity overrides
    //
    virtual Acad::ErrorStatus explode(AcDbVoidPtrArray&) const;
    virtual void              list() const;

	Acad::ErrorStatus getGripPoints(AcGePoint3dArray&, AcDbIntArray&, AcDbIntArray&) const;
	Acad::ErrorStatus getStretchPoints(AcGePoint3dArray&) const;

	Acad::ErrorStatus moveStretchPointsAt(const AcDbIntArray&, const AcGeVector3d&);
	Acad::ErrorStatus moveGripPointsAt(const AcDbIntArray&, const AcGeVector3d&);
	Acad::ErrorStatus transformBy(const AcGeMatrix3d&);

    const AcGePoint3dArray&		GetPointData() const { return m_PointArray;} 
    BOOL						AddVertex(const AcGePoint3d&);
    const AcGePoint3d			GetLastVertex() const {return m_PointArray.last();}
	void						UpdLastVertex(const AcGePoint3d& pt) { AcGePoint3d& ptLast = m_PointArray.last(); ptLast = pt;}

public:
	static void CreateNEAnnEntity();
	static void EditNEAnnEntity();

private:
	static void MakeStrings(const AcGePoint3d&, CString&, CString&, const short&);
	static BOOL IsTxtOnLHS(const AcGePoint3d&, const AcGePoint3d&);
	static Acad::ErrorStatus GetAcGiTextStyle(AcGiTextStyle&, const AcDbObjectId&);

private:
	void DrawText(AcGiWorldDraw*, const AcGePoint3d&, const AcGePoint3d&, const AcGePoint3d&, short&, const AcDbHardPointerId&, AcDbVoidPtrArray* = 0L) const;
	void DrawArrowHead(AcGiWorldDraw*, const AcGePoint3d&, const AcGePoint3d&, const double&, short&, AcDbVoidPtrArray* = 0L) const;
public:
    static		 AcGePoint3d	m_TempVertex;
    static		 BOOL			m_bDragModeOn;

    static const CString	m_strAnnFmtNorth;
	static const CString	m_strAnnFmtEast;
    static const CString	m_strAnnFmtSouth;
	static const CString	m_strAnnFmtWest;

// data members
public:
	double	GetTxtSize() const { return m_fTxtSize;}
	double	GetArrSize() const { return m_fArrSize;}
	short	GetLUPREC()  const { return m_iLUPREC;}

	void SetTxtSize(const double& Val) { m_fTxtSize = Val;}
	void SetArrSize(const double& Val) { m_fArrSize = Val;}
	void SetLUPREC(const short& Val) { m_iLUPREC = Val;}
private:
    AcGePoint3dArray	m_PointArray;
    AcDbHardPointerId	m_DbObjIDTxtStyle;
    short				m_iLUPREC;
    double				m_fTxtSize,
						m_fArrSize;
    AcGeVector3d		m_Normal;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class NEAnnotationEntityJig : public AcEdJig
{
public:
    NEAnnotationEntityJig(const AcGePoint3d&, const AcGeVector3d&, const double&, const double&, const short&);
    virtual ~NEAnnotationEntityJig();
    void					doIt();
    virtual DragStatus		sampler();
    virtual Adesk::Boolean	update();
    virtual AcDbEntity*		entity() const;
private:
    BOOL					m_bIsEntityAdded;
    NEAnnotationEntity*		m_pEntity;
    short					m_iLUPREC;
    double					m_fTxtSize,
							m_fArrSize;
    AcGeVector3d			m_Normal;
    AcGePoint3d				m_PointToAdd;
    DragStatus				m_CurDragStat;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif //_NEANNOTATIONENTITY_H_
