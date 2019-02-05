//VAlignment.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _VALIGNMENT_H_
#define _VALIGNMENT_H_

#include <dbents.h>
#include <acgi.h>

#include "Structures.h"
#include "ProfileDataDlg.h"
#include "XSecDataDlg.h"
#include "util.h"
#include "PLineDataExtractor.h"


typedef struct tagVCurveDetData
{
	double	fRad;
	double	fTangentLength;
	double	fXternalLength;
	double	fCurveLen;
	double	fDegOfCurve;
	double	fDefAngle;

	AcGePoint3d	ptPOI;
	AcGePoint3d	ptTC;
	AcGePoint3d	ptCT;
	
	CString		strComment;
} VCURVEDETDATA;
typedef CTypedPtrArray<CPtrArray, VCURVEDETDATA*> CVCurveDetDataArray;


class CVAlignment : public  AcDbEntity
{
	friend class CVAlignmentJig;
public:
    ACRX_DECLARE_MEMBERS(CVAlignment);
    CVAlignment();
    CVAlignment(const AcGeVector3d&, const AcGePoint3d&, const double& fStartElev, const double& fChainageAtStartPoint);
    virtual ~CVAlignment();

    virtual Adesk::Boolean    worldDraw(AcGiWorldDraw*);
    
	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler*);
	static Acad::ErrorStatus  ReadDwgFields(AcDbDwgFiler* filer, AcGePoint3dArray&	arrayVertices, AcGeVector3d& vecNormal, AcGeDoubleArray& arrayRadius, double& fElevAtStartPoint, double& fChainageAtStartPoint);

    virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler*) const;
	static Acad::ErrorStatus  WriteDwgFields(AcDbDwgFiler* filer, const AcGePoint3dArray& arrayVertices, const AcGeVector3d& vecNormal, const AcGeDoubleArray& arrayRadius, const double& fElevAtStartPoint, const double& fChainageAtStartPoint);
    
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
public:
	double getElevAt(const double& fChainage) const;

public:
    const AcGePoint3dArray&		GetPointData() const { return m_PointArray;} 
    const AcGeDoubleArray&		GetRadius() const { return m_RadiusArray;} 
protected:
    BOOL						AddVertex(const AcGePoint3d&);
    const AcGePoint3d			GetLastVertex() const {return m_PointArray.last();}
	void						UpdLastVertex(const AcGePoint3d& pt) { AcGePoint3d& ptLast = m_PointArray.last(); ptLast = pt;}
    
	int	 GetNearestVertex(const AcGePoint3d&) const;
	BOOL IsVertexValidForCurveData(const int&) const;
	BOOL IsVertexValidForCurveData(const int&, AcGePoint3d&, AcGePoint3d&, AcGePoint3d&, double&) const;
	BOOL SetCurveRadius(const int&, const double&);

	BOOL	GetTangentLengthLimits(const int&, double&, double&) const;
	double	GetCurveLength(const int&) const;

	BOOL GetTangentLength(const int&, double&) const;
	BOOL GetPointsOnCurve(const int&, AcGePoint3d&, AcGePoint3d&, AcGePoint3d&, AcGePoint3d* = 0L) const;

	BOOL					IsValid();
	BOOL					IsValid(const int&, const AcGePoint3d&);

	void					Revert();
	
	static double			CalcBF(const AcGePoint3d&, const AcGePoint3d&, const AcGePoint3d&);
	AcDbPolyline*			ConvertToPolyLine() const;

protected:
    static		 AcGePoint3d	m_TempVertex;
    static		 BOOL			m_bDragModeOn;

// data members
protected:
    AcGePoint3dArray	m_PointArray;
    AcGeVector3d		m_Normal;
    AcGeDoubleArray		m_RadiusArray;//for each vertex//for 1st & last vertex the radius is = 0.0;
    double				m_fElevAtStartPoint;
    double				m_fChainageAtStartPoint;
};

inline double CVAlignment::CalcBF(const AcGePoint3d& ptFirst, const AcGePoint3d& ptSecond, const AcGePoint3d& ptThird)
{
	const double fPrecision = 1.0E-9;
	const double fDist = Dist2D(ptFirst, ptThird) / 2.0;
	const double fAngle = Angle2D(ptFirst, ptThird);
	AcGePoint3d ptMid;
	double fAngleTmp;

	ads_polar(asDblArray(ptFirst), fAngle, fDist, asDblArray(ptMid));
	const double fHt = Dist2D(ptMid, ptSecond);
	if(fabs(fHt) <= fPrecision)
	{
		return 0.0;
	}
	{//Direction....CW/CCW
		AcGeCircArc2d segArc(AcGePoint2d(ptFirst.x, ptFirst.y), AcGePoint2d(ptSecond.x, ptSecond.y), AcGePoint2d(ptThird.x, ptThird.y));

		fAngleTmp = Angle2D(ptMid, ptThird) - Angle2D(ptMid, ptFirst);
		if(fAngleTmp < 0.0)
			fAngleTmp += 2 * PI;

		if(Adesk::kTrue== segArc.isClockWise())
		{//CW
			return -(fHt / fDist);//-VE for CW
		}
		return (fHt / fDist);//+VE for CCW
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CVAlignmentJig : public AcEdJig
{
protected:
    CVAlignmentJig(const AcGePoint3d&, const AcGeVector3d&, const double& fStartElev, const double& fChainageAtStartPoint);
public:
    virtual ~CVAlignmentJig();
    void					doIt();
    virtual DragStatus		sampler();
    virtual Adesk::Boolean	update();
    virtual AcDbEntity*		entity() const;
private:
    BOOL					m_bIsEntityAdded;
    CVAlignment*			m_pEntity;
    AcGeVector3d			m_Normal;
    AcGePoint3d				m_PointToAdd;
    DragStatus				m_CurDragStat;
    double					m_fElevAtStartPoint;
    double					m_fChainageAtStartPoint;
public:
	static void EditVAlignmentVertex();
	static void CreateVAlignment();
	static CVAlignment* GetVAlignmentEntity(const char*);
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif //_VALIGNMENT_H_
