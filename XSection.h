// XSection.h : header file
//
#if !defined(AFX_XSECTION_H__850D34C1_F800_11D3_86CC_C97D650903E4__INCLUDED_)
#define AFX_XSECTION_H__850D34C1_F800_11D3_86CC_C97D650903E4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "TINData.h"
#include "HAlignment.h"


class CXSection
{
private:
	CXSection() {;}
	~CXSection() {;}
public:
	static void DrawXSection();
	static BOOL GetXSecPoints(const AcGePoint3dArray& ContourPtsArray, const CTrianglePtrArray& ArrayTrgs, const AcGePoint3d& PtMidOfXec, const double& fAngle, const XSECDATAEX& RecParam, AcGePoint3dArray& PtsAlongXSecArray);

protected:
	static double	CalculateLeftNRiteAngles(const AcGePoint3d& ptFirst, const AcGePoint3d& ptMid, const AcGePoint3d& ptThird, double& fAngleLeft, double& fAngleRite);
	static BOOL		GetXSecPointsInSeg(const AcGePoint3dArray&, const CTrianglePtrArray&, const AcGePoint3d*, const double, const XSECDATAEX&, AcGePoint3dArray*&);
	static BOOL		GetAllXSecPoints(const AcGePoint3dArray& ContourPtsArray, const CTrianglePtrArray& ArrayTrgs, CThePointsOnAlignment&, const XSECDATAEX& RecParam);
	static void		DeAllocateXSecPtsArray(AcDbVoidPtrArray& Array);
	static void		DrawPointsInPlanView(const CThePointsOnAlignment& XSecPtsArray, const XSECDATAEX& Rec, const BOOL bDeallocate = FALSE);
};


#endif // !defined(AFX_XSECTION_H__850D34C1_F800_11D3_86CC_C97D650903E4__INCLUDED_)
