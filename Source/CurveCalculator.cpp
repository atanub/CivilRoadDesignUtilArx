//CurveCalculator.cpp
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include "CurveCalculator.h"
#include "DrawUtil.h"

const int CCurveCalculator::m_iArcLength = 30;
CCurveCalculator::CCurveCalculator(const AcGePoint3d& ptPOI, const AcGePoint3d& ptLeft, const AcGePoint3d& ptRight, const double& fFirstTanMaxLen, const double& fSecondTanMaxLen)
				  : m_ptPOI(ptPOI), m_ptLeft(ptLeft), m_ptRight(ptRight), m_fFirstTanMaxLen(fFirstTanMaxLen), m_fSecondTanMaxLen(fSecondTanMaxLen)
{
	m_bIsCurveValid = FALSE;
	m_strErrMsg = "";
	m_dRadius = m_dTangentLen = m_dXternalLen = m_dCurveLen = 0.0;
	
	m_dDefAngleInRad = CCurveCalculator::CalcDefAng(m_ptPOI, m_ptLeft, m_ptRight);

	//ads_printf("\nCalculated Deflection angle = %f\n", RTOD(m_dDefAngleInRad));
}

double CCurveCalculator::Rad2DegOfCurve(const double& dRad)
{
	return ((360.0 / (2 * PI * dRad)) * CCurveCalculator::m_iArcLength);
}
double CCurveCalculator::DegOfCurve2Rad(const double& dDegOfCUrve)
{
	return ((360.0 / (2 * PI * dDegOfCUrve)) * CCurveCalculator::m_iArcLength);
}

void CCurveCalculator::GetCurveCenter(const AcGePoint3d& ptStart, const AcGePoint3d& ptPOI, const AcGePoint3d& ptEnd, const double& fRad, const double& fTanLen, AcGePoint3d& ptRes)
{
	double fAngle;
	AcGePoint3d ptTC, ptTL, ptTmpA, ptTmpB;
	

	ads_polar(asDblArray(ptPOI), Angle2D(ptPOI, ptStart), fTanLen, asDblArray(ptTC));
	ads_polar(asDblArray(ptPOI), Angle2D(ptPOI, ptEnd), fTanLen, asDblArray(ptTL));

	fAngle = Angle2D(ptStart, ptPOI) + DTOR(90.0);
	ads_polar(asDblArray(ptTC), fAngle, fRad, asDblArray(ptTmpA));

	fAngle = Angle2D(ptEnd, ptPOI) + DTOR(90.0);
	ads_polar(asDblArray(ptTL), fAngle, fRad, asDblArray(ptTmpB));

	GetIntersPt2D(ptTC, ptTmpA, ptTL, ptTmpB, ptRes, TRUE);
}
BOOL CCurveCalculator::IsCurveDirClockwise(const AcGePoint3d& ptStart, const AcGePoint3d& ptPOI, const AcGePoint3d& ptEnd)
{
	return (Angle2D(ptPOI, ptStart) < Angle2D(ptPOI, ptEnd));
}

double CCurveCalculator::CalcIncAng(const AcGePoint3d& ptPOI, const AcGePoint3d& ptLeft, const AcGePoint3d& ptRight)
{
	double dAngA, dAngB, dAngToRet;
	
	dAngA = Angle2D(ptPOI, ptLeft);
	dAngB = Angle2D(ptPOI, ptRight);

	if(dAngB > dAngA)
	{
		dAngToRet = (dAngB - dAngA);
	}
	else
	{
		dAngToRet = (dAngA - dAngB);
	}
	if(dAngToRet > PI)
		dAngToRet = 2 * PI - dAngToRet;
	return dAngToRet;
}
double CCurveCalculator::CalcDefAng(const AcGePoint3d& ptPOI, const AcGePoint3d& ptLeft, const AcGePoint3d& ptRight)
{
	double dAngToRet;

	dAngToRet = CCurveCalculator::CalcIncAng(ptPOI, ptLeft, ptRight);
	return (PI - dAngToRet);
}
void CCurveCalculator::GetPointOnCurve(const AcGePoint3d& ptStart, const AcGePoint3d& ptPOI, const AcGePoint3d& ptEnd, const double& fRad, const double& fTanLen, const double& fCurveLenFromTC, AcGePoint3d& ptResult)
{
	AcGePoint3d ptCurveCenter, ptTC, ptTL;
	double fAngle;

	ptResult.x = ptResult.y = ptResult.z = 0.0;
	CCurveCalculator::GetCurveCenter(ptStart, ptPOI, ptEnd, fRad, fTanLen, ptCurveCenter);
	ads_polar(asDblArray(ptPOI), Angle2D(ptPOI, ptStart), fTanLen, asDblArray(ptTC));
	ads_polar(asDblArray(ptPOI), Angle2D(ptPOI, ptEnd), fTanLen, asDblArray(ptTL));

	fAngle = Angle2D(ptCurveCenter, ptTL) - Angle2D(ptCurveCenter, ptTC);
	if(fAngle < 0.0)
		fAngle += 2 * PI;
	if((fAngle > PI) || (fAngle < 0.0))
	{//Clockwise
		fAngle = Angle2D(ptCurveCenter, ptTC) - (fCurveLenFromTC / fRad);
	}
	else
	{//AntiClock
		fAngle = Angle2D(ptCurveCenter, ptTC) + (fCurveLenFromTC / fRad);
	}
	ads_polar(asDblArray(ptCurveCenter), fAngle, fRad, asDblArray(ptResult));
}
BOOL CCurveCalculator::GetPointsOnCurve(AcGePoint3d& ptFirst, AcGePoint3d& ptSecond, AcGePoint3d& ptThird) const
{
	if(!IsValid())
		return FALSE;

	AcGePoint3d ptCurveCenter, ptTmp;

	ptFirst.x = ptFirst.y = ptFirst.z = 0.0;
	CCurveCalculator::GetCurveCenter(m_ptLeft, m_ptPOI, m_ptRight, m_dRadius, m_dTangentLen, ptCurveCenter);
	//
	ads_polar(asDblArray(m_ptPOI), Angle2D(m_ptPOI, m_ptLeft), m_dTangentLen, asDblArray(ptFirst));
	//
	ads_polar(asDblArray(m_ptPOI), Angle2D(m_ptPOI, m_ptRight), m_dTangentLen, asDblArray(ptThird));
	//
	ads_polar(asDblArray(ptCurveCenter), Angle2D(ptCurveCenter, m_ptPOI), m_dRadius, asDblArray(ptSecond));

	return TRUE;
}

BOOL CCurveCalculator::UpdWRTDegOfCurve(const double& dParam)
{
	m_bIsCurveValid = FALSE;
	m_strErrMsg = "";

    m_dDegOfCurve = dParam;
    m_dRadius = CCurveCalculator::DegOfCurve2Rad(m_dDegOfCurve);

	m_dCurveLen = ((2 * PI * m_dRadius) / 360.0) * m_dDefAngleInRad;
	m_dTangentLen = m_dRadius * tan(m_dDefAngleInRad / 2.0);
	
	m_dXternalLen = m_dRadius * ((1.0 / cos(m_dDefAngleInRad / 2.0)) - 1);

	{//Check curve overlapping
		const CString strMsgFmt("%s adjacent curve is overlapping by %f, Allowable tangent length = %f\nPlease try with other value");
		if(m_dTangentLen > m_fFirstTanMaxLen)
		{
			m_strErrMsg.Format(strMsgFmt, "Left", (m_dTangentLen > m_fFirstTanMaxLen), m_fFirstTanMaxLen);
			return FALSE;
		}
		if(m_dTangentLen > m_fSecondTanMaxLen)
		{
			m_strErrMsg.Format(strMsgFmt, "Right", (m_dTangentLen > m_fSecondTanMaxLen), m_fSecondTanMaxLen);
			return FALSE;
		}
	}
	m_bIsCurveValid = TRUE;
	return TRUE;
}

BOOL CCurveCalculator::UpdWRTRad(const double& dParam)
{
	m_bIsCurveValid = FALSE;
	m_strErrMsg = "";

    m_dRadius = dParam;
    m_dDegOfCurve = CCurveCalculator::Rad2DegOfCurve(m_dRadius);
    
	m_dCurveLen = ((2 * PI * m_dRadius) / 360.0) * RTOD(m_dDefAngleInRad);
	m_dTangentLen = m_dRadius * tan(m_dDefAngleInRad / 2.0);
	
	m_dXternalLen = m_dRadius * ((1.0 / cos(m_dDefAngleInRad / 2.0)) - 1);

	{//Check curve overlapping
		const CString strMsgFmt("%s adjacent curve is overlapping by %f, Allowable tangent length = %f\nPlease try with other value");
		if(m_dTangentLen > m_fFirstTanMaxLen)
		{
			m_strErrMsg.Format(strMsgFmt, "Left", (m_dTangentLen - m_fFirstTanMaxLen), m_fFirstTanMaxLen);
			return FALSE;
		}
		if(m_dTangentLen > m_fSecondTanMaxLen)
		{
			m_strErrMsg.Format(strMsgFmt, "Right", (m_dTangentLen - m_fSecondTanMaxLen), m_fSecondTanMaxLen);
			return FALSE;
		}
	}
	m_bIsCurveValid = TRUE;
	return TRUE;
}
BOOL CCurveCalculator::UpdWRTTangentLen(const double& dParam)
{
	m_bIsCurveValid = FALSE;
	m_strErrMsg = "";

	m_dTangentLen = dParam;
	m_dRadius = m_dTangentLen / tan(m_dDefAngleInRad / 2.0);
    m_dDegOfCurve = CCurveCalculator::Rad2DegOfCurve(m_dRadius);
	m_dCurveLen = ((2 * PI * m_dRadius) / 360.0) * RTOD(m_dDefAngleInRad);
	m_dXternalLen = m_dRadius * ((1.0 / cos(m_dDefAngleInRad / 2.0)) - 1);

	{//Check curve overlapping
		const CString strMsgFmt("%s adjacent curve is overlapping by %f, Allowable tangent length = %f\nPlease try with other value");
		if(m_dTangentLen > m_fFirstTanMaxLen)
		{
			m_strErrMsg.Format(strMsgFmt, "Left", (m_dTangentLen - m_fFirstTanMaxLen), m_fFirstTanMaxLen);
			return FALSE;
		}
		if(m_dTangentLen > m_fSecondTanMaxLen)
		{
			m_strErrMsg.Format(strMsgFmt, "Right", (m_dTangentLen - m_fSecondTanMaxLen), m_fSecondTanMaxLen);
			return FALSE;
		}
	}
	m_bIsCurveValid = TRUE;
	return TRUE;
}
BOOL CCurveCalculator::UpdWRTXternalLen(const double& dParam)
{
	m_bIsCurveValid = FALSE;
	m_strErrMsg = "";

	m_dXternalLen = dParam;
	m_dRadius = m_dXternalLen / ((1.0 / cos(m_dDefAngleInRad / 2.0)) - 1);
    m_dDegOfCurve = CCurveCalculator::Rad2DegOfCurve(m_dRadius);
	m_dTangentLen = m_dRadius * tan(m_dDefAngleInRad / 2.0);
	m_dCurveLen = ((2 * PI * m_dRadius) / 360.0) * RTOD(m_dDefAngleInRad);
	
	{//Check curve overlapping
		const CString strMsgFmt("%s adjacent curve is overlapping by %f, Allowable tangent length = %f\nPlease try with other value");
		if(m_dTangentLen > m_fFirstTanMaxLen)
		{
			m_strErrMsg.Format(strMsgFmt, "Left", (m_dTangentLen - m_fFirstTanMaxLen), m_fFirstTanMaxLen);
			return FALSE;
		}
		if(m_dTangentLen > m_fSecondTanMaxLen)
		{
			m_strErrMsg.Format(strMsgFmt, "Right", (m_dTangentLen - m_fSecondTanMaxLen), m_fSecondTanMaxLen);
			return FALSE;
		}
	}
	m_bIsCurveValid = TRUE;
	return TRUE;
}
BOOL CCurveCalculator::UpdWRTCurveLen(const double& dParam)
{
	m_bIsCurveValid = FALSE;
	m_strErrMsg = "";

	m_dCurveLen = dParam;
	m_dRadius = m_dCurveLen * 360.0 / (2 * PI * RTOD(m_dDefAngleInRad));
    m_dDegOfCurve = CCurveCalculator::Rad2DegOfCurve(m_dRadius);
	m_dTangentLen = m_dRadius * tan(m_dDefAngleInRad / 2.0);
	m_dXternalLen = m_dRadius * ((1.0 / cos(m_dDefAngleInRad / 2.0)) - 1);

	{//Check curve overlapping
		const CString strMsgFmt("%s adjacent curve is overlapping by %f, Allowable tangent length = %f\nPlease try with other value");
		if(m_dTangentLen > m_fFirstTanMaxLen)
		{
			m_strErrMsg.Format(strMsgFmt, "Left", (m_dTangentLen - m_fFirstTanMaxLen), m_fFirstTanMaxLen);
			return FALSE;
		}
		if(m_dTangentLen > m_fSecondTanMaxLen)
		{
			m_strErrMsg.Format(strMsgFmt, "Right", (m_dTangentLen - m_fSecondTanMaxLen), m_fSecondTanMaxLen);
			return FALSE;
		}
	}

	m_bIsCurveValid = TRUE;
	return TRUE;
}
