//PLineDataExtractor.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _PLINEDATAEXTRACTOR_H_
#define _PLINEDATAEXTRACTOR_H_

#include <DbPl.h>
#include "Structures.h"

//
//Forward Declarations....
//
class CTINData;

//
//CPointOnAlignment
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CPointOnAlignment
{
public:
	typedef enum {General = 1, AtMajorInterval = 2, AtMinorInterval = 4, AtIntersection = 8} PointType;
public:
	CPointOnAlignment(const AcGePoint3d&, const double&, const double&, const char* = 0L, PointType = General);
	virtual ~CPointOnAlignment();
public:
	AcGePoint3d		GetPoint() const { return AcGePoint3d(m_ptOnAlignment.x, m_ptOnAlignment.y, m_fElev);}
	const double	GetChainage() const { return m_fChainage;}
	//
	const char*		GetComment() const { return (LPCSTR)m_strComment;}
	void			SetComment(const char* pszValue) { assert(0L != pszValue); if(0L != pszValue) m_strComment = pszValue;}
	//
	const double	GetElev() const { return m_fElev;}
	void			SetElev(const double& fElev) { m_fElev = fElev;}
	//
	const PointType	GetType() const { return m_eType;}
	void			SetType(const PointType eType) { m_eType = eType;}

	const double	GetAngleOfAnnotationLine() const { return m_fAngleOfAnnotationLine;}
	
	const void		SetItemDataPtr(const void* pData) { m_pXData = pData;}
	const void*		GetItemDataPtr() const { return m_pXData;}
public:
	virtual void Annotate(const double&, const double&, const char*);
protected:
	const void*			m_pXData;
	const AcGePoint2d	m_ptOnAlignment;
	const double		m_fAngleOfAnnotationLine;
	const double		m_fChainage;
	double				m_fElev;
	CString				m_strComment;
	PointType			m_eType;//0 = General point; 2 = @ Major Interval; 4 = @ Minor Interval; 8 = @ Point of Intersection
};
//
//CThePointsOnAlignment
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CThePointsOnAlignment
{
public:
	CThePointsOnAlignment()
	{
	}
	virtual ~CThePointsOnAlignment();
public:
	const CPointOnAlignment*	GetAt(const int iIndex) const { assert(iIndex < m_arrayPts.GetSize()); return m_arrayPts[iIndex];}
	CPointOnAlignment*			GetAt(const int iIndex) { assert(iIndex < m_arrayPts.GetSize()); return m_arrayPts[iIndex];}
	//
	BOOL	InsertAt(const CPointOnAlignment&, const int iIndex = -1);
	BOOL	RemoveAt(const int iIndex);
	void	RemoveAll();
	int		GetCount() const { return m_arrayPts.GetSize();}
public:
	BOOL	DrawPoints(const double&, const double&, const char*) const;
	BOOL	DrawProfile(const PROFILEDATAEX&, const CTINData*) const;
protected:
	CTypedPtrArray<CPtrArray, CPointOnAlignment*>	m_arrayPts;
};

//
//PLineDataExtractor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CPLineDataExtractor
{
public:
	CPLineDataExtractor(const AcDbPolyline* pEnt) : m_pPlineToExamine(pEnt)
	{
		assert(0L != m_pPlineToExamine);
		m_pPlineToExamine->assertReadEnabled();
	}
	virtual ~CPLineDataExtractor();

public:
	const CThePointsOnAlignment& GetPointData() const { return m_ptsOnAlignment;}
	CThePointsOnAlignment& GetPointData() { return m_ptsOnAlignment;}

public:
	virtual BOOL Draw(const double&, const double&, const double&, const double&, const double&, const char*, const BOOL bMustReCalc = FALSE);
	virtual BOOL DivideIntoSegments(const double&, const double&, const double& fStartChgForEntirePath, const BOOL bMustReCalc = FALSE);

public:
	static AcDbPolyline*	Revert2DPLine(const AcDbPolyline*);
	static 	void			DividePLine();
	static 	void			RevertPLine();


protected:
	static void DivideCurveSegment(const BOOL, const AcGePoint3d&, const double&, const AcGePoint3d&, const AcGePoint3d&, const double&, const double&, CThePointsOnAlignment&, const double&);
	static void DivideLineSegment(const AcGePoint3d&, const AcGePoint3d&, const double&, const double&, CThePointsOnAlignment&, const double&);
	static BOOL GetPLineSelection(const char* pszPrompt, AcDbObject*& pObj);
	static BOOL GetDividePLineParams(double& fInterval, double& fIntervalMinor, double& fTextSize, double& fTickSize, double& fStartChg, CString& strCurLayer);

protected:
	const AcDbPolyline*							m_pPlineToExamine;
	CThePointsOnAlignment						m_ptsOnAlignment;
protected:
	const static char* m_pszStartOfTangent;
	const static char* m_pszStartOfCurve;
	const static char* m_pszEndOfCurve;
};

//
//CPLineDataExtractorEx
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CPLineDataExtractorEx : public CPLineDataExtractor 
{
public:
	CPLineDataExtractorEx(const AcDbPolyline* pEnt) : CPLineDataExtractor(pEnt)
	{
	}
	virtual ~CPLineDataExtractorEx();
public:
	virtual BOOL DivideIntoSegments(const double&, const double&, const double&, const BOOL bMustReCalc = FALSE);
};



#endif //_PLINEDATAEXTRACTOR_H_
