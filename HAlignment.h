//HAlignment.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HALIGNMENT_H_
#define _HALIGNMENT_H_

#include <dbents.h>
#include <acgi.h>

#include "Structures.h"
#include "ProfileDataDlg.h"
#include "XSecDataDlg.h"
#include "util.h"
#include "PLineDataExtractor.h"
#include "CrossSectionProfileDefinition.h"


typedef struct tagCurveDetData
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
} CURVEDETDATA;
typedef CTypedPtrArray<CPtrArray, CURVEDETDATA*> CCurveDetDataArray;

class CHAlignment : public  AcDbEntity
{
	friend class CHAlignmentJig;
public:
    ACRX_DECLARE_MEMBERS(CHAlignment);
    CHAlignment();
    CHAlignment(const AcGeVector3d&, const AcGePoint3d&);
    virtual ~CHAlignment();

    virtual Adesk::Boolean    worldDraw(AcGiWorldDraw*);
    virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler*);
    virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler*) const;
    virtual Acad::ErrorStatus audit(AcDbAuditInfo*);
	void	saveAs(AcGiWorldDraw*, AcDb::SaveType);

	Acad::ErrorStatus readXSectionProfileDefinition(AcDbDwgFiler* filer);
	Acad::ErrorStatus readXSectionProfileSetting(AcDbDwgFiler* filer);

	Acad::ErrorStatus writeXSectionProfileDefinition(AcDbDwgFiler* filer) const;
	Acad::ErrorStatus writeXSectionProfileSetting(AcDbDwgFiler* filer) const;
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
	PROFILEDATAEX&			GetProfileSetting() { return m_RecPflData;}
	XSECDATAEX&				GetXSecSetting() { return m_RecXSecData;}
	AcGePoint3dArray&		GetDatumRangeArray() { return m_DatumRangeArray;}

	BOOL					DrawProfile(const CTINData*, const BOOL bDrawPlanVw = TRUE) const;
	AcDbPolyline*			ConvertToPolyLine() const;

protected:
    const AcGePoint3dArray&		GetPointData() const { return m_PointArray;} 
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

	BOOL					CreateCurveDataReport(CCurveDetDataArray&) const;

	BOOL					IsValid();
	BOOL					IsValid(const int&, const AcGePoint3d&);

	void					Revert();
	
	BOOL					_CalcPointsOnCurve(const int&, double&, AcGeDoubleArray&, AcGePoint3dArray&, CStringArray&) const;
	void					_InitializeData();
public:
	static BOOL				CalcPointsOnEqInterval(const AcGePoint3d&, const AcGePoint3d&, const double&, const double&, double&, AcGeDoubleArray&, AcGePoint3dArray&, CStringArray&);

protected:
	static double			CalcBF(const AcGePoint3d&, const AcGePoint3d&, const AcGePoint3d&);

private:
    const static CString		m_strCommentPOI;
    const static CString		m_strCommentCurvePoint;
    const static CString		m_strCommentTC;//Tangent - Curve..
    const static CString		m_strCommentCT;//Curve - Tangent
    
	static		 AcGePoint3d	m_TempVertex;
    static		 BOOL			m_bDragModeOn;
// data members
private:
	//For Existing Profile/XSection Data 
    AcGePoint3dArray	m_PointArray;
    AcGeVector3d		m_Normal;
    AcGeDoubleArray		m_RadiusArray;//for each vertex//for 1st & last vertex the radius is = 0.0;

    PROFILEDATAEX		m_RecPflData;
    XSECDATAEX			m_RecXSecData;
	AcGePoint3dArray	m_DatumRangeArray;

	//For Profile/XSection Design Data 
	CCrossSectionProfileDefinitionArray m_arrayProfileDef;//<<Profile Definition Name><Points>>
	CCrossSectionProfileSettingArray	m_arrayProfileDefSetting;//<<Start Chg><End Chg><Profile Definition Name>>

    AcGePoint3dArray	m_PointArrayVAlignment;
    AcGeVector3d		m_NormalVAlignment;
    AcGeDoubleArray		m_RadiusArrayVAlignment;//for each vertex//for 1st & last vertex the radius is = 0.0;
    double				m_fElevAtStartPointVAlignment;
    double				m_fChainageAtStartPointVAlignment;
};

inline double CHAlignment::CalcBF(const AcGePoint3d& ptFirst, const AcGePoint3d& ptSecond, const AcGePoint3d& ptThird)
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

class CHAlignmentJig : public AcEdJig
{
protected:
    CHAlignmentJig(const AcGePoint3d&, const AcGeVector3d&);
public:
    virtual ~CHAlignmentJig();
    void					doIt();
    virtual DragStatus		sampler();
    virtual Adesk::Boolean	update();
    virtual AcDbEntity*		entity() const;
private:
    BOOL					m_bIsEntityAdded;
    CHAlignment*			m_pEntity;
    AcGeVector3d			m_Normal;
    AcGePoint3d				m_PointToAdd;
    DragStatus				m_CurDragStat;
public:
	static void CreateHAlignment();
	static void RevertHAlignment();
	static void EditHAlignmentVertex();
	static CHAlignment* GetHAlignmentEntity(const char* pszPmt);
	static void AnnotateHAlignmentVertex();
protected:
	static void DrawCurveDataTable(const CURVEDETDATA*, const AcGePoint3d&);
	static int GetMaxLengthyStr(const CStringArray&);
	static void DeAllocateCurveDataRecArray(CCurveDetDataArray& arrayCurveDet);
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //_HALIGNMENT_H_
