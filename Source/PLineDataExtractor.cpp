//PLineDataExtractor.CPP
//
#include "StdAfx.h"
#include <Math.h>


#include "PLineDataExtractor.h"
#include "DrawUtil.h"
#include "Util.h"
#include "TINData.h"
#include "QProfile.h"

//
//CPointOnAlignment
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPointOnAlignment::CPointOnAlignment(const AcGePoint3d& ptParam, const double& fAngleParam, const double& fChainage, const char* pszTag/* = 0L*/, PointType ePointType/* = General = 0L*/) 
: m_ptOnAlignment(ptParam.x, ptParam.y), m_fAngleOfAnnotationLine(fAngleParam), m_fChainage(fChainage), m_strComment((0L == pszTag) ? "":pszTag), m_fElev(0.0), m_eType(ePointType), m_pXData(0L)
{
}
CPointOnAlignment::~CPointOnAlignment()
{
}
void CPointOnAlignment::Annotate(const double& fTextSize, const double& fTickSize, const char* pszLyr)
{
	AcGePoint3d ptIns(m_ptOnAlignment.x, m_ptOnAlignment.y, m_fElev), ptLeft, ptRite;
	//const CString strChg = RTOS(m_fChainage + fStartChg);
	const CString strChg = RTOS(m_fChainage);

	DRAWPOINT(ptIns, pszLyr);
	ads_polar(asDblArray(ptIns), m_fAngleOfAnnotationLine, fTickSize, asDblArray(ptLeft));
	ads_polar(asDblArray(ptIns), m_fAngleOfAnnotationLine, -fTickSize, asDblArray(ptRite));
	LINE(ptLeft, ptRite, pszLyr);
	TEXTLEFT(ptLeft, strChg, m_fAngleOfAnnotationLine, fTextSize, pszLyr);
	TEXTRIGHT(ptRite, strChg, m_fAngleOfAnnotationLine, fTextSize, pszLyr);
}
//
//CThePointsOnAlignment
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CThePointsOnAlignment::~CThePointsOnAlignment()
{
	RemoveAll();//Deallocate if list is not empty
}
BOOL CThePointsOnAlignment::InsertAt(const CPointOnAlignment& Rec, const int iIndex/* = -1*/)
{
	CPointOnAlignment* pRecNew;
	
	if(iIndex < 0)
	{//Just append at the end
		pRecNew = new CPointOnAlignment(Rec);
		m_arrayPts.Add(pRecNew);
		return TRUE;
	}
	if(iIndex > m_arrayPts.GetSize())
	{
		assert(false);
		return FALSE;
	}
	pRecNew = new CPointOnAlignment(Rec);
	m_arrayPts.InsertAt(iIndex, pRecNew);
	return TRUE;
}
BOOL CThePointsOnAlignment::RemoveAt(const int iIndex)
{
	if((iIndex < 0) || (iIndex >= m_arrayPts.GetSize()))
	{//Just append at the end
		assert(false);
		return FALSE;
	}
	CPointOnAlignment* pRec;

	pRec = m_arrayPts.ElementAt(iIndex);
	assert(0L != pRec);
	m_arrayPts.RemoveAt(iIndex);
	delete pRec;
	return TRUE;
}
void CThePointsOnAlignment::RemoveAll()
{
	//Deallocate if list is not empty
	while(m_arrayPts.GetSize() > 0)
	{
		delete m_arrayPts[0];
		m_arrayPts.RemoveAt(0);
	}
}

BOOL CThePointsOnAlignment::DrawPoints(const double& fTextSize, const double& fTickSize, const char* pszLyr) const 
{
	int i;

	if(m_arrayPts.GetSize() <= 0)
	{
		ads_printf("\nFATAL ERROR: Failed to draw profile, no vertices generated yet![CThePointsOnAlignment::DrawProfile()]\n");
		return FALSE;
	}
	LAYER(pszLyr, 1);
	for(i = 0; i < m_arrayPts.GetSize(); i++)
	{
		const CPointOnAlignment::PointType ePointType =	m_arrayPts[i]->GetType();
		
		if((ePointType & CPointOnAlignment::PointType::AtIntersection) && ((i + 1) != m_arrayPts.GetSize()))
			continue;
		
		m_arrayPts[i]->Annotate(fTextSize, fTickSize, pszLyr);
	}
	return TRUE;
}
//
//1) Calculates Z-Values for each of the vertices ...
//2) Draws the profile...
//
BOOL CThePointsOnAlignment::DrawProfile(const PROFILEDATAEX& objDwgAttrs, const CTINData* pTinDataObject) const
{
	int i;

	if(m_arrayPts.GetSize() <= 0)
	{
		ads_printf("\nFATAL ERROR: Failed to draw profile, no vertices generated yet![CThePointsOnAlignment::DrawProfile()]\n");
		return FALSE;
	}
	//Calculates Z-Values for each of the vertices
	for(i = 0; i < m_arrayPts.GetSize(); i++)
	{
		AcGePoint3d pt3D;
		
		pt3D = m_arrayPts[i]->GetPoint();
		if(!pTinDataObject->GetElevAt(pt3D.x, pt3D.y, pt3D.z))
		{
			m_arrayPts[i]->SetComment("Failed to Calculate Z value");
		}
		m_arrayPts[i]->SetElev(pt3D.z);
	}
	{//
		AcGePoint3d PtOrg;

		//Draw Profile now ....
		if(RTNORM != ads_getpoint(0L, "\nEnter lower left corner of profile :", asDblArray(PtOrg)))
		{
			ads_printf("\nERROR: No point picked\n");
			return FALSE;
		}
		DrawQProfile(PtOrg, this, objDwgAttrs);
	}

	return TRUE;
}
//
//PLineDataExtractor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* CPLineDataExtractor::m_pszStartOfTangent	= "Start of Tangent";
const char* CPLineDataExtractor::m_pszStartOfCurve	= "Start of Curve";
const char* CPLineDataExtractor::m_pszEndOfCurve		= "End Of Curve";

CPLineDataExtractor::~CPLineDataExtractor()
{
}
BOOL CPLineDataExtractor::Draw(const double& fInterval, const double& fIntervalMinor, const double& fTextSize, const double& fTickSize, const double& fStartChg, const char* pszLyr, const BOOL bMustReCalc/* = FALSE*/)
{
	if(0L == m_pPlineToExamine)
	{
		assert(0L != m_pPlineToExamine);
		return FALSE;
	}
	m_pPlineToExamine->assertReadEnabled();
	//
	if(!DivideIntoSegments(fInterval, fIntervalMinor, fStartChg, bMustReCalc))
	{
		assert(false);
		return FALSE;
	}
	m_ptsOnAlignment.DrawPoints(fTextSize, fTickSize, pszLyr);// Draw generated points
	return TRUE;
}
//Returns 
void CPLineDataExtractor::DivideLineSegment(const AcGePoint3d& ptStart, const AcGePoint3d& ptEnd, const double& fInterval, const double& fFirstStationOffset, CThePointsOnAlignment& arrayResult, const double& fStartChgForEntirePath)
{
	const double fAngle = Angle2D(ptStart, ptEnd);
	const double fLen = Dist2D(ptStart, ptEnd);
	double fCumDist, fStartChgOfThisSeg;
	
	fCumDist = fFirstStationOffset;

	//determine start chainage of this segment....
	fStartChgOfThisSeg = fStartChgForEntirePath;
	if(arrayResult.GetCount() > 0)
	{
		const CPointOnAlignment* pLastRec =	arrayResult.GetAt(arrayResult.GetCount() - 1);
		fStartChgOfThisSeg = pLastRec->GetChainage();//Considering the start chainage (applicable for all the segments)
	}

	while(TRUE)
	{
		AcGePoint3d ptResult;

		if((fCumDist > fLen) && (arrayResult.GetCount() > 0))
		{//Last point
			const CPointOnAlignment::PointType ePointType = (CPointOnAlignment::PointType)(CPointOnAlignment::PointType::AtMajorInterval | CPointOnAlignment::PointType::AtIntersection);

			arrayResult.InsertAt(CPointOnAlignment(ptEnd, (fAngle + PI / 2.0), (fStartChgOfThisSeg + fLen), 0L, ePointType));
			return;
		}
		if(arrayResult.GetCount() == 1)
		{//If the chainage is not multiple of interval.....insert an extra point 
			const CPointOnAlignment* pLastRec =	arrayResult.GetAt(arrayResult.GetCount() - 1);
			double fTmpChg, fTmpIncrement;

			fTmpChg = pLastRec->GetChainage();
			fTmpIncrement = (fInterval - fmod(fTmpChg, fInterval));
			fTmpChg += fTmpIncrement;
			//
			fCumDist -= fInterval;
			fCumDist += fTmpIncrement;
			//
			ads_polar(asDblArray(ptStart), fAngle, fCumDist, asDblArray(ptResult));
			const CPointOnAlignment::PointType ePointType = CPointOnAlignment::PointType::AtMajorInterval;
			arrayResult.InsertAt(CPointOnAlignment(ptResult, (fAngle + PI / 2.0), fTmpChg, 0L, ePointType));
	
			fCumDist += fInterval;
			continue;
		}
		if(fCumDist <= 0.0)
		{
			ptResult = ptStart;
		}
		else
		{
			ads_polar(asDblArray(ptStart), fAngle, fCumDist, asDblArray(ptResult));
		}
		const CPointOnAlignment::PointType ePointType = CPointOnAlignment::PointType::AtMajorInterval;
		//
		arrayResult.InsertAt(CPointOnAlignment(ptResult, (fAngle + PI / 2.0), (fStartChgOfThisSeg + fCumDist), 0L, ePointType));
		fCumDist += fInterval;
	}
}
void CPLineDataExtractor::DivideCurveSegment(const BOOL bIsCW, const AcGePoint3d& ptCentre, const double& fRaddi, const AcGePoint3d& ptStartCurve, const AcGePoint3d& ptEndCurve, const double& fInterval, const double& fFirstStationOffset, CThePointsOnAlignment& arrayResult, const double& fStartChgForEntirePath)
{
	double fIncAngle, fStartAngleCalc, fEndAngleCalc;

	fStartAngleCalc = Angle2D(ptCentre, ptStartCurve);
	fEndAngleCalc = Angle2D(ptCentre, ptEndCurve);

	if(bIsCW)
	{//CW
		if(fStartAngleCalc < fEndAngleCalc)
			fStartAngleCalc += 2.0 * PI;
		
		fIncAngle = fStartAngleCalc - fEndAngleCalc;
	}
	else
	{//CCW
		if(fEndAngleCalc < fStartAngleCalc)
			fEndAngleCalc += 2.0 * PI;
		
		fIncAngle = fEndAngleCalc - fStartAngleCalc;
	}
	const double fCurveLen = fRaddi * fIncAngle;
	double fCumDist, fStartChgOfThisSeg;

	fCumDist = fFirstStationOffset;
	//determine start chainage of this segment....
	fStartChgOfThisSeg = fStartChgForEntirePath;
	if(arrayResult.GetCount() > 0)
	{
		const CPointOnAlignment* pLastRec =	arrayResult.GetAt(arrayResult.GetCount() - 1);
		fStartChgOfThisSeg = pLastRec->GetChainage();//Considering the start chainage (applicable for all the segments)
	}

	while(TRUE)
	{
		if(fCumDist > fCurveLen)
		{//Insert the last point...
			const CPointOnAlignment::PointType ePointType = (CPointOnAlignment::PointType)(CPointOnAlignment::PointType::AtMajorInterval | CPointOnAlignment::PointType::AtIntersection);
			
			arrayResult.InsertAt(CPointOnAlignment(ptEndCurve, fEndAngleCalc, (fCurveLen + fStartChgOfThisSeg), m_pszEndOfCurve, ePointType));
			return;
		}
		//
		AcGePoint3d ptResult;
		const double fCumIncAng = fCumDist / fRaddi;
		const double fAngle = fStartAngleCalc - ((bIsCW) ? fCumIncAng:-fCumIncAng);
		const double fAngleAlongAlgn =  fAngle + ((!bIsCW) ? (PI / 2.0):-(PI / 2.0));
		
		if(arrayResult.GetCount() == 1)
		{//Applicable for the 1st point on profile - If the chainage is not multiple of interval.....insert an extra point 
			const CPointOnAlignment* pLastRec =	arrayResult.GetAt(arrayResult.GetCount() - 1);
			double fTmpChg, fTmpIncrement;

			fTmpChg = pLastRec->GetChainage();
			fTmpIncrement = (fInterval - fmod(fTmpChg, fInterval));
			fTmpChg += fTmpIncrement;
			//
			fCumDist -= fInterval;
			fCumDist += fTmpIncrement;
			//
			const double fCumIncAngTmp = fCumDist / fRaddi;
			const double fAngleTmp = fStartAngleCalc - ((bIsCW) ? fCumIncAngTmp:-fCumIncAngTmp);
			const double fAngleAlongAlgnTmp =  fAngleTmp + ((!bIsCW) ? (PI / 2.0):-(PI / 2.0));

			ads_polar(asDblArray(ptCentre), fAngleTmp, fRaddi, asDblArray(ptResult));
			const CPointOnAlignment::PointType ePointType = CPointOnAlignment::PointType::AtMinorInterval;
			arrayResult.InsertAt(CPointOnAlignment(ptResult, (fAngleAlongAlgnTmp + PI / 2.0), fTmpChg, 0L, ePointType));
	
			fCumDist += fInterval;
			continue;
		}
		ads_polar(asDblArray(ptCentre), fAngle, fRaddi, asDblArray(ptResult));
		//
		if((fCumDist == fFirstStationOffset) && (arrayResult.GetCount() > 0))
		{
			CPointOnAlignment* pLastRec;
			
			pLastRec = arrayResult.GetAt(arrayResult.GetCount() - 1);
			pLastRec->SetComment(m_pszStartOfCurve);
		}
		
		const CPointOnAlignment::PointType ePointType = CPointOnAlignment::PointType::AtMinorInterval;
		arrayResult.InsertAt(CPointOnAlignment(ptResult, (fAngleAlongAlgn + PI / 2.0), (fCumDist + fStartChgOfThisSeg), 0L, ePointType));
		fCumDist += fInterval;
	}
}
BOOL CPLineDataExtractor::DivideIntoSegments(const double& fInterval, const double& fIntervalMinor, const double& fStartChgForEntirePath, const BOOL bMustReCalc/* = FALSE*/)
{
	const unsigned int iVerticesCount = m_pPlineToExamine->numVerts();
	unsigned int i;
	double fCumChg, fLastInterval;

	if(!bMustReCalc && (m_ptsOnAlignment.GetCount() > 0))
	{
		return TRUE;//Already calculated!!
	}
	//Deallocate if list is not empty
	m_ptsOnAlignment.RemoveAll();
	for (i = 1, fCumChg = 0.0, fLastInterval = -1.0; i < iVerticesCount; i++)
	{
		AcGeCircArc2d segArc;
		AcGePoint2d ptVertex;
		double fFirstStationOffset;

		//Statr point of segment
		if(Acad::eOk != m_pPlineToExamine->getPointAt(i - 1, ptVertex))
			return FALSE;
		const AcGePoint3d ptStart(ptVertex.x, ptVertex.y, 0.0);
		
		//End point of segment
		if(Acad::eOk != m_pPlineToExamine->getPointAt(i - 0, ptVertex))
			return FALSE;
		const AcGePoint3d ptEnd(ptVertex.x, ptVertex.y, 0.0);

		if(AcDbPolyline::kArc == m_pPlineToExamine->segType(i - 1))
		{//Curve Segment Starts .....
			if(Acad::eOk != m_pPlineToExamine->getArcSegAt(i - 1, segArc))
			{
				return FALSE;
			}
			const AcGePoint3d ptCen(segArc.center().x, segArc.center().y, 0.0);
			const AcGePoint3d ptStartCurve(segArc.startPoint().x, segArc.startPoint().y, 0.0);
			const AcGePoint3d ptEndCurve(segArc.endPoint().x, segArc.endPoint().y, 0.0);
			
			fCumChg = 0.0;
			fFirstStationOffset = 0.0;
			if(m_ptsOnAlignment.GetCount() > 0)
			{
				const CPointOnAlignment* pLastRec =	m_ptsOnAlignment.GetAt(m_ptsOnAlignment.GetCount() - 1);

				fCumChg = pLastRec->GetChainage();
				fFirstStationOffset = fIntervalMinor - fmod(fCumChg, fIntervalMinor);
				fCumChg = fCumChg + fFirstStationOffset;
			}

			DivideCurveSegment((Adesk::kTrue == segArc.isClockWise()) ? TRUE:FALSE, ptCen, segArc.radius(), ptStartCurve, ptEndCurve, fIntervalMinor, fFirstStationOffset, m_ptsOnAlignment, fStartChgForEntirePath);
			fLastInterval = fIntervalMinor;
		}
		else
		{
			fCumChg = 0.0;
			fFirstStationOffset = 0.0;
			if(m_ptsOnAlignment.GetCount() > 0)
			{
				const CPointOnAlignment* pLastRec =	m_ptsOnAlignment.GetAt(m_ptsOnAlignment.GetCount() - 1);

				fCumChg = pLastRec->GetChainage();
				fFirstStationOffset = fInterval - fmod(fCumChg, fInterval);
				fCumChg = fCumChg + fFirstStationOffset;
			}
			DivideLineSegment(ptStart, ptEnd, fInterval, fFirstStationOffset, m_ptsOnAlignment, fStartChgForEntirePath);
			fLastInterval = fInterval;
		}
	}
	return TRUE;
}
AcDbPolyline* CPLineDataExtractor::Revert2DPLine(const AcDbPolyline* pEntParam)
{
	const unsigned int iVerticesCount = pEntParam->numVerts();
	int i, iNewIndex;
	AcDbPolyline* pEntToRet;
	AcGePoint2d ptVertex;
	double fBFac;

	pEntToRet = new AcDbPolyline();
	for (i = (iVerticesCount - 1), iNewIndex = 0; i >= 0; i--, iNewIndex++)
	{
		if(Acad::eOk != pEntParam->getPointAt(i, ptVertex))
		{
			delete pEntToRet;
			return 0L;
		}
		if(Acad::eOk != pEntToRet->addVertexAt(iNewIndex, ptVertex))
		{
			delete pEntToRet;
			return 0L;
		}
		//Bulge Factor Setting......
		fBFac = 0.0;
		if(Acad::eOk != pEntParam->getBulgeAt(i, fBFac))
		{
			delete pEntToRet;
			return 0L;
		}
		if(iNewIndex >= 1)
		{//
			if(Acad::eOk != pEntToRet->setBulgeAt(iNewIndex - 1, -fBFac))
			{
				delete pEntToRet;
				return 0L;
			}
		}
	}
	return pEntToRet;
}
void CPLineDataExtractor::RevertPLine()
{
	const char* pszPrompt = "\nSelect Polyline to revert :";
	AcDbObject* pObj;

	if(!GetPLineSelection(pszPrompt, pObj))
	{
		return;
	}
	
	if(pObj->isA() == AcDb3dPolyline::desc())
	{//3D Poly
		AcGePoint3dArray arrayVertices;

		if(AcDb::Poly3dType::k3dSimplePoly != ((AcDb3dPolyline*)pObj)->polyType())
		{
			ads_printf("\nERROR: Selected 3D Polyline contains curved segments !\n");
			pObj->close();
			return;
		}
		if(Extract3DPLineVertices(((AcDb3dPolyline*)pObj), arrayVertices))
		{
			arrayVertices.reverse();
			POLYLINE3D(arrayVertices, (LPCSTR)GetEntLayer((AcDbEntity*)pObj));
		}
		else
		{
			ads_printf("\nERROR: Failed to extract 3D Polyline vertices!\n");
			pObj->close();
			return;
		}
	}
	else if(pObj->isA() == AcDbPolyline::desc())
	{//2D Poly
		AcDbPolyline* pNewEnt;
		
		pNewEnt = CPLineDataExtractor::Revert2DPLine((AcDbPolyline*)pObj);
		assert(0L != pNewEnt);

		pNewEnt->setLayer((LPCSTR)GetEntLayer((AcDbEntity*)pObj));
		AddToACDBase(pNewEnt);
	}
	else
	{
		assert(false);
	}
	{//Delete the old entity
		ads_name entName;
		
		if(Acad::eOk == acdbGetAdsName(entName, pObj->objectId()))
		{//Delete the entity.....
			pObj->close();
			if(ads_entdel(entName) != RTNORM)
			{
				ads_printf("\nERROR: AutoCAD System error !\nFailed to delete entity!\n");
				return;//ERROR
			}
			return;
		}
		pObj->close();
	}
}
void CPLineDataExtractor::DividePLine()
{
	AcDbObject* pObj;
	//Drawing Attributes....
	double fInterval, fIntervalMinor, fTextSize, fTickSize, fStartChg;
	CString strCurLayer;
	const char* pszPrompt = "\nSelect Polyline to divide :";

	if(!GetPLineSelection(pszPrompt, pObj))
	{
		return;
	}
	//Get Drawing Attributes....
	if(!GetDividePLineParams(fInterval, fIntervalMinor, fTextSize, fTickSize, fStartChg, strCurLayer))
	{
		ads_printf("\nUser break encountered\n");
		pObj->close();
		return;
	}
	if(pObj->isA() == AcDbPolyline::desc())
	{//2D Polylines with/without curved segments 
		CPLineDataExtractor* pDataExtractor;
		
		#ifdef _DEBUG
		{//For DEBUG only
			AcDbPolyline* pRevertedEnt;
			pDataExtractor = new CPLineDataExtractorEx((AcDbPolyline*)pObj);
			
			//Test Revert function
			pRevertedEnt = CPLineDataExtractorEx::Revert2DPLine((AcDbPolyline*)pObj);
			assert(0L != pRevertedEnt);
			pRevertedEnt->setLayer((LPCSTR)GetEntLayer((AcDbEntity*)pObj));
			AddToACDBase(pRevertedEnt);
		}
		#else
			pDataExtractor = new CPLineDataExtractor((AcDbPolyline*)pObj);
		#endif

		//Draw now
		pDataExtractor->Draw(fInterval, fIntervalMinor, fTextSize, fTickSize, fStartChg, (LPCSTR)strCurLayer);
		pObj->close();
		delete pDataExtractor;
		return;
	}
	if(pObj->isA() == AcDb3dPolyline::desc())
	{//for 3D Polylines
		if(AcDb::Poly3dType::k3dSimplePoly != ((AcDb3dPolyline*)pObj)->polyType())
		{
			ads_printf("\nERROR: Selected 3D Polyline contains curved segments !\n");
			pObj->close();
			return;
		}
		{//Extract vertices & then convert to 2D Polyline
			AcGePoint3dArray arrayVertices;

			if(Extract3DPLineVertices(((AcDb3dPolyline*)pObj), arrayVertices))
			{
				AcDbPolyline objPLine;
				int i;

				for(i = 0; i < arrayVertices.length(); i++)
				{
					if(Acad::eOk != objPLine.addVertexAt(i, AcGePoint2d(arrayVertices[i].x, arrayVertices[i].y)))
					{
						pObj->close();
						return;
					}
				}
				//Draw now.....
				CPLineDataExtractor objDataExtractor(&objPLine);
				objDataExtractor.Draw(fInterval, fIntervalMinor, fTextSize, fTickSize, fStartChg, (LPCSTR)strCurLayer);
			}
		}
		pObj->close();
		return;
	}
}
//
//
BOOL CPLineDataExtractor::GetDividePLineParams(double& fInterval, double& fIntervalMinor, double& fTextSize, double& fTickSize, double& fStartChg, CString& strCurLayer)
{
	fInterval = 0.0;
	fIntervalMinor = 0.0;
	fTextSize = 0.0;
	fTickSize = 0.0;
	fStartChg = 0.0;
	CLAYER(strCurLayer);

	//fInterval = 0.2;
	if(ads_getdist(0L, "\nEnter segment length :", &fInterval) != RTNORM)
	{
		ads_printf("\nUser break encountered\n");
		return FALSE;
	}
	if(fInterval <= 0.0)
	{
		ads_printf("\nERROR: Invalid segment length input\n");
		return FALSE;
	}
	//fTickSize = 4.0 * fTextSize;
	if(ads_getdist(0L, "\nEnter tick size :", &fTickSize) != RTNORM)
	{
		ads_printf("\nUser break encountered\n");
		return FALSE;
	}
	if(fTickSize <= 0.0)
	{
		ads_printf("\nERROR: Invalid tick size input\n");
		return FALSE;
	}
	//start chainage;
	if(ads_getreal("\nEnter start chainage :", &fStartChg) != RTNORM)
	{
		ads_printf("\nUser break encountered\n");
		return FALSE;
	}
	if(fStartChg < 0.0)
	{
		ads_printf("\nERROR: Invalid start chainage input\n");
		return FALSE;
	}
	//Drawing Attributes
	fIntervalMinor = fInterval;
	fTextSize = TEXTSIZE();
	CLAYER(strCurLayer);
		
	return TRUE;
}
BOOL CPLineDataExtractor::GetPLineSelection(const char* pszPrompt, AcDbObject*& pObj)
{
	AcGePoint3d ptTmp;
	ads_name entName;
	AcDbObjectId objId;

	if(RTNORM != ads_entsel(pszPrompt, entName, asDblArray(ptTmp)))
	{
		ads_printf("\nERROR: No entity selected\n");
		return FALSE;
	}
	if(Acad::eOk != acdbGetObjectId(objId, entName))
	{
		return FALSE;
	}
	if(Acad::eOk != acdbOpenObject(pObj, objId, AcDb::kForRead))
	{
		return FALSE;
	}
	if((pObj->isA() != AcDbPolyline::desc()) && (pObj->isA() != AcDb3dPolyline::desc()))
	{
		ads_printf("\nERROR: Entity selected is \"%s\", not a polyline\n", pObj->isA()->dxfName());
		pObj->close();
		return FALSE;
	}
	return TRUE;
}

//
//CPLineDataExtractorEx - For Profile drawing using contour Data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CPLineDataExtractorEx::DivideIntoSegments(const double& fInterval, const double& fIntervalMinor, const double& fStartChgForEntirePath, const BOOL bMustReCalc/* = FALSE*/)
{
	return CPLineDataExtractor::DivideIntoSegments(fInterval, fIntervalMinor, fStartChgForEntirePath, bMustReCalc);
}
CPLineDataExtractorEx::~CPLineDataExtractorEx()
{
}
