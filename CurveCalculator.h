//CurveCalculator.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _CURVECALCULATOR_H_
#define _CURVECALCULATOR_H_

#include "Structures.h"
#include "Util.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CCurveCalculator
{
public:
	CCurveCalculator(const AcGePoint3d&, const AcGePoint3d&, const AcGePoint3d&, const double&, const double&);

public:
	static double Rad2DegOfCurve(const double&);
	static double DegOfCurve2Rad(const double&);
	static double CalcDefAng(const AcGePoint3d&, const AcGePoint3d&, const AcGePoint3d&);
	static double CalcIncAng(const AcGePoint3d&, const AcGePoint3d&, const AcGePoint3d&);
	static void	  GetCurveCenter(const AcGePoint3d&, const AcGePoint3d&, const AcGePoint3d&, const double&, const double&, AcGePoint3d&);
	static BOOL	  IsCurveDirClockwise(const AcGePoint3d&, const AcGePoint3d&, const AcGePoint3d&);
	static void	  GetPointOnCurve(const AcGePoint3d&, const AcGePoint3d&, const AcGePoint3d&, const double&, const double&, const double&, AcGePoint3d&);

public:
	BOOL	IsValid() const { return m_bIsCurveValid;}
	CString	GetErrMsg() const { return m_strErrMsg;}

	BOOL	UpdWRTDegOfCurve(const double&);
	BOOL	UpdWRTRad(const double&);
	BOOL	UpdWRTTangentLen(const double&);
	BOOL	UpdWRTXternalLen(const double&);
	BOOL	UpdWRTCurveLen(const double&);

	double	GetDegOfCurve() const { return m_dDegOfCurve;}
	double	GetRad() const { return m_dRadius;}
	double	GetTangentLen() const { return m_dTangentLen;}
	double	GetXternalLen() const { return m_dXternalLen;}
	double	GetCurvleLen() const { return m_dCurveLen;}
	double	GetDefAngle() const { return RTOD(m_dDefAngleInRad);}
	
	BOOL	GetPointsOnCurve(AcGePoint3d&, AcGePoint3d&, AcGePoint3d&) const;

public:
	const static int m_iArcLength;

private:
    const AcGePoint3d&		m_ptPOI;
    const AcGePoint3d&		m_ptLeft;
    const AcGePoint3d&		m_ptRight;
	const double&			m_fFirstTanMaxLen;
	const double&			m_fSecondTanMaxLen;


    BOOL					m_bIsCurveValid;
    CString					m_strErrMsg;
    double					m_dDefAngleInRad;

    double					m_dRadius,
							m_dTangentLen,
							m_dXternalLen,
							m_dDegOfCurve,
							m_dCurveLen;

};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif //_CURVECALCULATOR_H_
