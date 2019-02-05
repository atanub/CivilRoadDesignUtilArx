
//HAlignment.cpp
#include "stdafx.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

#include <rxregsvc.h>
#include <rxmfcapi.h>
#include <aced.h>
#include <acdb.h>
#include <dbmain.h>
#include <dbproxy.h>
#include <GEAssign.h>
#include <AdsCodes.h>
#include <Dbsymtb.h>
#include <AcGiUtil.h>

#include "DrawUtil.h"
#include "VAlignment.h"
#include "Util.h"
#include "HorzCurveDataDlg.h"
#include "CurveCalculator.h"
#include "TINData.h"
#include "XSection.h"


// define a DXF class body for the entity using the
// appropriate ACRX boilerplate macro.  The DXF class name
// *MUST* be all uppercase.
//
// ...
ACRX_DXF_DEFINE_MEMBERS(CVAlignment, AcDbEntity,
AcDbProxyEntity::kNoOperation, CVAlignment, 
"Contour\
|Product Desc: Contour ARX Application\
|Company: Atanu Banik's\
|E-Mail : atanubanik@gmail.com");

AcGePoint3d		CVAlignment::m_TempVertex = AcGePoint3d(0.0, 0.0, 0.0);
BOOL			CVAlignment::m_bDragModeOn = FALSE;


MAKE_ACDBOPENOBJECT_FUNCTION(CVAlignment);

CVAlignment::CVAlignment()
{
	m_fElevAtStartPoint = 0.0;
	m_fChainageAtStartPoint = 0.0;
}
CVAlignment::CVAlignment(const AcGeVector3d& Normal, const AcGePoint3d& Pt, const double& fStartElev, const double& fChainageAtStartPoint)
: m_Normal(Normal), m_fElevAtStartPoint(fStartElev), m_fChainageAtStartPoint(fChainageAtStartPoint)
{
    m_PointArray.append(Pt);
	m_RadiusArray.append(0.0);//Initialize with Zero Radius
}

CVAlignment::~CVAlignment()
{
}

Adesk::Boolean CVAlignment::worldDraw(AcGiWorldDraw* wd)
{
	short iSubEntTrait;
	int i;

	for (i = 1, iSubEntTrait = 0; i < m_PointArray.length(); i++)
	{
		AcGePoint3dArray ArrayPtsOnLines;

		if(IsVertexValidForCurveData(i - 1) && (m_RadiusArray[i - 1] > 0.0))
		{
			AcGePoint3d ptFirst, ptSecond, ptThird;
			
			if(GetPointsOnCurve(i - 1, ptFirst, ptSecond, ptThird))
			{
				ArrayPtsOnLines.append(ptThird);
			}
			else
			{
				ArrayPtsOnLines.append(m_PointArray[i - 1]);
			}
		}
		else
		{
			ArrayPtsOnLines.append(m_PointArray[i - 1]);
		}
		if(IsVertexValidForCurveData(i) && (m_RadiusArray[i] > 0.0))
		{//Curve....
			AcGePoint3d ptFirst, ptSecond, ptThird;
			
			if(GetPointsOnCurve(i, ptFirst, ptSecond, ptThird))
			{
				wd->subEntityTraits().setSelectionMarker(iSubEntTrait++);
				wd->geometry().circularArc(ptFirst, ptSecond, ptThird);
				
				ArrayPtsOnLines.append(ptFirst);
			}
			else
			{
				ArrayPtsOnLines.append(m_PointArray[i]);
			}
		}
		else
		{
			ArrayPtsOnLines.append(m_PointArray[i]);
		}
		if(ArrayPtsOnLines.length() == 2)
		{//Straight Line
			wd->subEntityTraits().setSelectionMarker(iSubEntTrait++);
			wd->geometry().polyline(2, ArrayPtsOnLines.asArrayPtr());
		}
	}
    return Adesk::kTrue;
}
AcDbPolyline* CVAlignment::ConvertToPolyLine() const
{
	AcDbPolyline* pEntToRet;
    AcGePoint2d ptVertex;
    double fBF;
	unsigned int i, iVertexCount;

	pEntToRet = new AcDbPolyline();
	for(i = 0, iVertexCount = 0, fBF = 0.0; i < (unsigned int)m_PointArray.length(); i++)
	{
		if(m_RadiusArray[i] == 0.0)
		{
			ptVertex.x = m_PointArray[i].x;
			ptVertex.y = m_PointArray[i].y;
			if(Acad::eOk != pEntToRet->addVertexAt(iVertexCount++, ptVertex, fBF))
			{
				delete pEntToRet;
				return 0L;
			}
			continue;
		}
		//Convert the Curve segment...
		if(IsVertexValidForCurveData(i))
		{//Curve....
			AcGePoint3d ptFirst, ptSecond, ptThird;
			
			if(GetPointsOnCurve(i, ptFirst, ptSecond, ptThird))
			{
				ptVertex.x = ptFirst.x; ptVertex.y = ptFirst.y;
				fBF = CalcBF(ptFirst, ptSecond, ptThird);
				if(Acad::eOk != pEntToRet->addVertexAt(iVertexCount++, ptVertex, fBF))
				{
					delete pEntToRet;
					return 0L;
				}
				fBF = 0.0;
				ptVertex.x = ptThird.x; ptVertex.y = ptThird.y;
				if(Acad::eOk != pEntToRet->addVertexAt(iVertexCount++, ptVertex, 0.0))
				{
					delete pEntToRet;
					return 0L;
				}
			}
			else
			{//
				delete pEntToRet;
				return 0L;
			}
		}
		else
		{
			delete pEntToRet;
			return 0L;
		}
	}

	return pEntToRet;
}

BOOL CVAlignment::IsValid()
{
	int i;
	for (i = 1; i < m_PointArray.length(); i++)
	{
		if(IsVertexValidForCurveData(i) && (m_RadiusArray[i] > 0.0))
		{//Curve....
			AcGePoint3d ptFirst, ptSecond, ptThird;
			
			if(!GetPointsOnCurve(i, ptFirst, ptSecond, ptThird))
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}
void CVAlignment::saveAs(AcGiWorldDraw* wd, AcDb::SaveType saveType)
{
	this->worldDraw(wd);
    // saveType == AcDb::kR12Save for saveasr12
    // saveType == AcDb::kR13Save for proxy graphics save
    //
    if (saveType == AcDb::kR12Save)
	{
    }
	else if (saveType == AcDb::kR13Save)
	{
    }
}
Acad::ErrorStatus CVAlignment::dwgInFields(AcDbDwgFiler* filer)
{
    assertWriteEnabled();

    AcDbEntity::dwgInFields(filer);

	ReadDwgFields(filer, m_PointArray, m_Normal, m_RadiusArray, m_fElevAtStartPoint, m_fChainageAtStartPoint);
    return filer->filerStatus();
}
Acad::ErrorStatus CVAlignment::ReadDwgFields(AcDbDwgFiler* filer, AcGePoint3dArray&	arrayVertices, AcGeVector3d& vecNormal, AcGeDoubleArray& arrayRadius, double& fElevAtStartPoint, double& fChainageAtStartPoint)
{
	Adesk::Int32 i, lCount;

	{//Reset
		//Point data...
		if(arrayVertices.length() > 0)
			arrayVertices.removeSubArray(0, arrayVertices.length() - 1);
	}
	{//Read
		//all the Points.......
		filer->readInt32(&lCount);
		for(i = 0; i < lCount; i++)
		{
			AcGePoint3d PtTmp;
			
			filer->readPoint3d(&PtTmp);
			arrayVertices.append(PtTmp);
		}
	}
	//Normal
	filer->readItem(&vecNormal);

	//Write all the Radius.......
	filer->readInt32(&lCount);
	for(i = 0; i < lCount; i++)
	{
		double fVal;
		
		filer->readDouble(&fVal);
		arrayRadius.append(fVal);
	}
	filer->readDouble(&fElevAtStartPoint);//Write Elev. At Start Point.......
	filer->readDouble(&fChainageAtStartPoint);//Write Chainage At Start Point.......

    return filer->filerStatus();
}

Acad::ErrorStatus CVAlignment::dwgOutFields(AcDbDwgFiler* filer) const
{
    assertReadEnabled();

    AcDbEntity::dwgOutFields(filer);

	WriteDwgFields(filer, m_PointArray, m_Normal, m_RadiusArray, m_fElevAtStartPoint, m_fChainageAtStartPoint);
	
	return filer->filerStatus();
}
Acad::ErrorStatus CVAlignment::WriteDwgFields(AcDbDwgFiler* filer, const AcGePoint3dArray& arrayVertices, const AcGeVector3d& vecNormal, const AcGeDoubleArray& arrayRadius, const double& fElevAtStartPoint, const double& fChainageAtStartPoint)
{
	//Write all the Points.......
	Adesk::Int32 i, lCount;
	
	lCount = arrayVertices.length();
	filer->writeInt32(lCount);
	for(i = 0; i < lCount; i++)
	{
		filer->writePoint3d(arrayVertices.at(i));
	}
	//Normal
	filer->writeItem(vecNormal);

	//Write all the Radius.......
	lCount = arrayRadius.length();
	filer->writeInt32(lCount);
	for(i = 0; i < lCount; i++)
	{
		filer->writeDouble(arrayRadius.at(i));
	}

	filer->writeDouble(fElevAtStartPoint);//Write Elev. At Start Point.......
	filer->writeDouble(fChainageAtStartPoint);//Write Chainage At Start Point.......

	return filer->filerStatus();
}
Acad::ErrorStatus CVAlignment::audit(AcDbAuditInfo* /*auditor*/)
{
	return Acad::eOk;
}
Acad::ErrorStatus CVAlignment::explode(AcDbVoidPtrArray& entities) const
{
    assertReadEnabled();

	{//Create 2D Polyline
		AcDbPolyline* pEnt;
		
		pEnt = this->ConvertToPolyLine();
		if(0L != pEnt)
			entities.append(pEnt);

		return Acad::eOk;
	}
    return Acad::eOk;
}
void CVAlignment::Revert()
{
    assertWriteEnabled();
	m_PointArray.reverse();
	m_RadiusArray.reverse();
}

void CVAlignment::list() const
{
    assertReadEnabled();
 	int i;

	ads_printf("\nVerticalAlignment Entity\nStart Elevation: %s\nStart Chainage: %s\n# Of Vertices : %d", RTOS(m_fElevAtStartPoint), RTOS(m_fChainageAtStartPoint), m_PointArray.length());
    for (i = 0; i < m_PointArray.length(); i++)
	{
		ads_printf("\nVertex #%d, Chainage: = %s, Elevation = %s", (i+1), RTOS(m_PointArray[i].x + m_fChainageAtStartPoint), RTOS(m_PointArray[i].y + m_fElevAtStartPoint));
		if(m_RadiusArray[i] > 0.0)
		{
			ads_printf(" Curve Radius : %s", RTOS(m_RadiusArray[i]));
		}
	}
	{//
		AcDbPolyline* pEnt;
		
		pEnt = ConvertToPolyLine();
		if(pEnt == 0L)
		{
			ads_printf("\nFATAL ERROR: Failed to calculate curve attributes\nPossible cause: This entity may be corrupted\n");
			return;
		}
		ads_printf("\nSegments........");
		{//Report segment Lengths
			const unsigned int iVerticesCount = pEnt->numVerts();
			unsigned int i;

			for (i = 1; i < iVerticesCount; i++)
			{
				AcGeCircArc2d segArc;
				AcGePoint2d ptVertex;

				if(Acad::eOk != pEnt->getPointAt(i - 1, ptVertex))
				{
					delete pEnt;
					return;
				}
				const AcGePoint3d ptStart(ptVertex.x, ptVertex.y, 0.0);
				//
				if(Acad::eOk != pEnt->getPointAt(i - 0, ptVertex))
				{
					delete pEnt;
					return;
				}
				const AcGePoint3d ptEnd(ptVertex.x, ptVertex.y, 0.0);

				if(AcDbPolyline::kArc == pEnt->segType(i - 1))
				{//Curve Segment Starts .....
					if(Acad::eOk != pEnt->getArcSegAt(i - 1, segArc))
					{
						delete pEnt;
						return;
					}
					const AcGePoint3d ptCen(segArc.center().x, segArc.center().y, 0.0);
					const AcGePoint3d ptStartCurve(segArc.startPoint().x, segArc.startPoint().y, 0.0);
					const AcGePoint3d ptEndCurve(segArc.endPoint().x, segArc.endPoint().y, 0.0);

					ads_printf("\nSegment #%d, Type: Curve, Start Point: X = %s, Z = %s, End Point: X = %s, Z = %s", i, RTOS(ptStartCurve.x), RTOS(ptStartCurve.y), RTOS(ptEndCurve.x), RTOS(ptEndCurve.y));
					ads_printf("\nSegment Length: %s", RTOS(segArc.radius() * fabs(segArc.endAng() - segArc.startAng())));
				}
				else
				{
					ads_printf("\nSegment #%d, Type: Line, Start Point: X = %s, Z = %s, End Point: X = %s, Z = %s", i, RTOS(ptStart.x), RTOS(ptStart.y), RTOS(ptEnd.x), RTOS(ptEnd.y));
					ads_printf("\nSegment Length: %s", RTOS(Dist2D(ptStart, ptEnd)));
				}
			}
		}
		delete pEnt;
	}
}
BOOL CVAlignment::AddVertex(const AcGePoint3d& Pt)
{
	int iIndex;
	AcGePoint3d PtTmp;

	PtTmp = Pt;
	PtTmp.z = 0.0;//Make sure the Z-Value is zero....

	if(m_PointArray.find(PtTmp, iIndex) == Adesk::kTrue)
		return FALSE;

	m_RadiusArray.append(0.0);//Initialize with Zero Radius
	m_PointArray.append(PtTmp);
	return TRUE;
}
Acad::ErrorStatus CVAlignment::getGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray& /*osnapModes*/, AcDbIntArray& /*geomIds*/) const
{
    assertReadEnabled();
    Acad::ErrorStatus ErrStat = Acad::ErrorStatus::eOk;
    
	gripPoints.append(m_PointArray);
    return ErrStat;
}
BOOL CVAlignment::IsValid(const int& iVertexIndex, const AcGePoint3d& ptToTest)
{
	const AcGePoint3d ptVertexOrg = m_PointArray[iVertexIndex];

	{//Try with new setting
		AcGePoint3d& PtVertex = m_PointArray[iVertexIndex];
		PtVertex = ptToTest;
		if(IsValid())
		{
			return TRUE;
		}
	}
	{//Bak prev setting
		AcGePoint3d& PtVertex = m_PointArray[iVertexIndex];
		PtVertex = ptVertexOrg;
	}
	return FALSE;
}

Acad::ErrorStatus CVAlignment::moveGripPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
    assertWriteEnabled();
    Acad::ErrorStatus es = Acad::eOk;
	int i;

    for (i = 0; i < indices.length(); i++)
	{
		AcGePoint3d PtToChange = m_PointArray[indices[i]];
		PtToChange += offset;
		if(!IsValid(indices[i], PtToChange))
		{
			return Acad::eInvalidInput;
		}
    }
    return es;
}
Acad::ErrorStatus CVAlignment::moveStretchPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
    assertWriteEnabled();
    Acad::ErrorStatus es = Acad::eOk;
	int i;

    for (i = 0; i < indices.length(); i++)
	{
		AcGePoint3d PtToChange = m_PointArray[indices[i]];
		PtToChange += offset;
		if(!IsValid(indices[i], PtToChange))
		{
			return Acad::eInvalidInput;
		}
    }
    return es;
}
Acad::ErrorStatus CVAlignment::getStretchPoints(AcGePoint3dArray& stretchPoints) const
{
    assertReadEnabled();

    Acad::ErrorStatus es = Acad::eOk;
	stretchPoints.append(m_PointArray);
    return es;
}

Acad::ErrorStatus CVAlignment::transformBy(const AcGeMatrix3d& xform)
{
    assertWriteEnabled();

 	int i;

    for (i = 0; i < m_PointArray.length(); i++)
	{
		AcGePoint3d PtToChange = m_PointArray[i];
		PtToChange.transformBy(xform);
		if(!IsValid(i, PtToChange))
		{
			return Acad::eInvalidInput;
		}
    }
    return Acad::eOk;
}
int CVAlignment::GetNearestVertex(const AcGePoint3d& ptParam) const
{
    assertReadEnabled();

 	int i, iIndex;
 	double fDist;

    for (i = iIndex = 0, fDist = 0.0; i < m_PointArray.length(); i++)
	{
 		double fDistTmp;
		const AcGePoint3d& PtVertex= m_PointArray[i];

		if(i == 0)
		{
			fDist = Dist2D(PtVertex, ptParam);
			iIndex = i;
		}
		else
		{
			fDistTmp = Dist2D(PtVertex, ptParam);
			if(fDistTmp < fDist)
			{
				fDist = fDistTmp;
				iIndex = i;
			}
		}
	}
	return iIndex;
}
BOOL CVAlignment::SetCurveRadius(const int& iVertex, const double& fRad)
{
    assertWriteEnabled();

	if(!IsVertexValidForCurveData(iVertex))
		return FALSE;
	m_RadiusArray.setAt(iVertex, fRad);
	return TRUE;
}

BOOL CVAlignment::IsVertexValidForCurveData(const int& iVertex) const
{
    assertReadEnabled();

	if((iVertex == 0) || (iVertex == (m_PointArray.length() - 1)))
		return FALSE;
	return TRUE;
}
BOOL CVAlignment::IsVertexValidForCurveData(const int& iVertex, AcGePoint3d& ptLeft, AcGePoint3d& ptPOI, AcGePoint3d& ptRite, double& fRad) const
{
    assertReadEnabled();

	if((iVertex == 0) || (iVertex == (m_PointArray.length() - 1)))
		return FALSE;
	
	ptLeft = m_PointArray[iVertex - 1];
	ptPOI = m_PointArray[iVertex - 0];
	ptRite = m_PointArray[iVertex + 1];
	fRad = m_RadiusArray[iVertex];


	return TRUE;
}
BOOL CVAlignment::GetTangentLengthLimits(const int& iVertex, double& fFirstTanMaxLen, double& fSecondTanMaxLen) const
{
	double fPrevTanLen, fNextTanLen;

	fFirstTanMaxLen = fSecondTanMaxLen = 0.0;
	if(!IsVertexValidForCurveData(iVertex))
		return FALSE;

	if(iVertex == 1)
	{//First POI
		fFirstTanMaxLen = Dist2D(m_PointArray[iVertex], m_PointArray[iVertex - 1]);
		fSecondTanMaxLen = Dist2D(m_PointArray[iVertex], m_PointArray[iVertex + 1]);
		if((m_PointArray.length() > 3) && (m_RadiusArray[iVertex + 1] > 0.0))
		{
			if(!GetTangentLength((iVertex + 1), fNextTanLen))
			{
				ads_printf("\nERROR: Tangent length calculation failed on vertex #%d\n", (iVertex - 1));
				return FALSE;
			}
			fSecondTanMaxLen = fSecondTanMaxLen - fNextTanLen;
		}
		return TRUE;
	}
	if(iVertex == (m_PointArray.length() - 2))
	{//Last POI
		if(m_RadiusArray[iVertex - 1] > 0.0)
		{
			if(!GetTangentLength((iVertex - 1), fPrevTanLen))
			{
				ads_printf("\nERROR: Tangent length calculation failed on vertex #%d\n", (iVertex - 1));
				return FALSE;
			}
		}
		else
		{
			fPrevTanLen = 0.0;
		}
		fFirstTanMaxLen = Dist2D(m_PointArray[iVertex], m_PointArray[iVertex - 1]);
		fFirstTanMaxLen = fFirstTanMaxLen - fPrevTanLen;
		fSecondTanMaxLen = Dist2D(m_PointArray[iVertex], m_PointArray[iVertex + 1]);
		return TRUE;
	}
	//intermediate POI
	if(!GetTangentLength((iVertex - 1), fPrevTanLen))
	{
		ads_printf("\nERROR: Tangent length calculation failed on vertex #%d\n", (iVertex - 1));
		return FALSE;
	}
	if(!GetTangentLength((iVertex + 1), fNextTanLen))
	{
		ads_printf("\nERROR: Tangent length calculation failed on vertex #%d\n", (iVertex + 1));
		return FALSE;
	}
	fFirstTanMaxLen = Dist2D(m_PointArray[iVertex], m_PointArray[iVertex - 1]);//To be changed
	fFirstTanMaxLen = fFirstTanMaxLen - fPrevTanLen;

	fSecondTanMaxLen = Dist2D(m_PointArray[iVertex], m_PointArray[iVertex + 1]);
	fSecondTanMaxLen = fSecondTanMaxLen - fNextTanLen;

	return TRUE;
}

double CVAlignment::GetCurveLength(const int& iVertex) const
{
	if(!IsVertexValidForCurveData(iVertex))
		return 0.0;
	double fFirstTanMaxLen, fSecondTanMaxLen;

	fFirstTanMaxLen = Dist2D(m_PointArray[iVertex], m_PointArray[iVertex - 1]);
	fSecondTanMaxLen = Dist2D(m_PointArray[iVertex], m_PointArray[iVertex + 1]);

	if(m_RadiusArray[iVertex] == 0.0)
	{//No Curve Defined
		return 0.0;
	}

	CCurveCalculator CalcObj(m_PointArray[iVertex], m_PointArray[iVertex - 1], m_PointArray[iVertex + 1], fFirstTanMaxLen, fSecondTanMaxLen);

	if(!CalcObj.UpdWRTRad(m_RadiusArray[iVertex]))
	{
		ads_printf("\nERROR: Invalid curve radius found on vertex #%d\n", iVertex);
		return 0.0;
	}
	if(!CalcObj.IsValid())
	{
		ads_printf("\nERROR: Invalid curve radius found on vertex #%d\n", iVertex);
		return 0.0;
	}
	return CalcObj.GetCurvleLen();
}
BOOL CVAlignment::GetTangentLength(const int& iVertex, double& fMaxTangentLen) const
{
	if(!IsVertexValidForCurveData(iVertex))
		return FALSE;
	double fFirstTanMaxLen, fSecondTanMaxLen;

	fFirstTanMaxLen = Dist2D(m_PointArray[iVertex], m_PointArray[iVertex - 1]);
	fSecondTanMaxLen = Dist2D(m_PointArray[iVertex], m_PointArray[iVertex + 1]);

	if(m_RadiusArray[iVertex] == 0.0)
	{//No Curve Defined
		fMaxTangentLen = 0.0;
		return TRUE;
	}

	CCurveCalculator CalcObj(m_PointArray[iVertex], m_PointArray[iVertex - 1], m_PointArray[iVertex + 1], fFirstTanMaxLen, fSecondTanMaxLen);

	if(!CalcObj.UpdWRTRad(m_RadiusArray[iVertex]))
	{
		ads_printf("\nERROR: Invalid curve radius found on vertex #%d\n", iVertex);
		return FALSE;
	}
	if(!CalcObj.IsValid())
	{
		ads_printf("\nERROR: Invalid curve radius found on vertex #%d\n", iVertex);
		return FALSE;
	}
	fMaxTangentLen = CalcObj.GetTangentLen();

	return TRUE;
}
BOOL CVAlignment::GetPointsOnCurve(const int& iVertex, AcGePoint3d& ptFirst, AcGePoint3d& ptSecond, AcGePoint3d& ptThird, AcGePoint3d* pPtCentre /*= 0L*/) const
{
	if(!IsVertexValidForCurveData(iVertex))
		return FALSE;
	{//Curve....
		double fFirstTanMaxLen, fSecondTanMaxLen;

		GetTangentLengthLimits(iVertex, fFirstTanMaxLen, fSecondTanMaxLen);
		CCurveCalculator CalcObj(m_PointArray[iVertex], m_PointArray[iVertex - 1], m_PointArray[iVertex + 1], fFirstTanMaxLen, fSecondTanMaxLen);

		if(!CalcObj.UpdWRTRad(m_RadiusArray[iVertex]))
		{
			ads_printf("\nERROR: Invalid curve radius found on vertex #%d\n", iVertex);
			return FALSE;
		}
		if(!CalcObj.IsValid())
		{
			ads_printf("\nERROR: Invalid curve radius found on vertex #%d\n", iVertex);
			return FALSE;
		}
		if(!CalcObj.GetPointsOnCurve(ptFirst, ptSecond, ptThird))
		{
			ads_printf("\nERROR: Failed to calculate curve points on vertex #%d\n", iVertex);
			return FALSE;
		}
		if(pPtCentre != 0L)
		{
			CCurveCalculator::GetCurveCenter(m_PointArray[iVertex - 1], m_PointArray[iVertex], m_PointArray[iVertex + 1], m_RadiusArray[iVertex], CalcObj.GetTangentLen(), *pPtCentre);
		}
	}
	return TRUE;
}
double CVAlignment::getElevAt(const double& fChainage) const
{
	double fElev, fChainageAbsolute;
	
	if((fChainage - m_fChainageAtStartPoint) < 0)
	{
		ads_printf("\nERROR: Chainage \"%s\" is out of range! Failed to calculate elevation!", RTOS(fChainage));
		return 0.0;
	}

	fChainageAbsolute = m_PointArray[0].x + (fChainage - m_fChainageAtStartPoint);
	AcDbPolyline* pPLine = ConvertToPolyLine();
	AcDbLine objLine(AcGePoint3d(fChainageAbsolute, 0.0, 0.0), AcGePoint3d(fChainageAbsolute, 1.0, 0.0));
	AcGePoint3dArray arrayIntersectionPoints;

	fElev = 0.0;
	if(Acad::eOk == objLine.intersectWith(pPLine, AcDb::kExtendThis, arrayIntersectionPoints))
	{
		if(arrayIntersectionPoints.length() > 0)
		{
			fElev = m_fElevAtStartPoint + arrayIntersectionPoints[0].y - m_PointArray[0].y;
#ifdef _DEBUG
			DRAWPOINT(arrayIntersectionPoints[0], "0");
#endif//_DEBUG
		}
	}
	else
	{
		ads_printf("\nERROR: Failed to calculate elevation at chainage: %s", RTOS(fChainage));
	}
	delete pPLine;

	return fElev;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CVAlignmentJig::CVAlignmentJig(const AcGePoint3d& pt, const AcGeVector3d& Normal, const double& fStartElev, const double& fChainageAtStartPoint)
: m_Normal(Normal), m_pEntity(0L), m_PointToAdd(pt), m_bIsEntityAdded(FALSE), m_CurDragStat(DragStatus::kNoChange), m_fElevAtStartPoint(fStartElev), m_fChainageAtStartPoint(fChainageAtStartPoint)
{
}

CVAlignmentJig::~CVAlignmentJig()
{
	if(!m_bIsEntityAdded)
		if(m_pEntity != 0L)
			delete m_pEntity;
}
// This function creates an AsdkCircle object and gets the
// jig started acquiring the necessary info to properly fill
// it in.
//
void CVAlignmentJig::doIt()
{
    if(m_pEntity == 0L)
	{
	    m_pEntity = new CVAlignment(m_Normal, m_PointToAdd, m_fElevAtStartPoint, m_fChainageAtStartPoint);
		CVAlignment::m_TempVertex = m_PointToAdd;
		CVAlignment::m_bDragModeOn = TRUE;
	}
	else
	{
		AfxMessageBox("ERROR: CVAlignmentJig::doIt()", MB_ICONSTOP);
		return;
	}

    //
    DragStatus Stat;
    Stat = DragStatus::kNormal;
	m_bIsEntityAdded = TRUE;
	for(;Stat == kNormal;) 
	{
		int iLen;
		CString strMsg;

		iLen = m_pEntity->GetPointData().length();
		strMsg.Format("\nEnter next vertex <%d> :", iLen);
	    setDispPrompt(strMsg);
		
		Stat = drag();
		if(Stat == kNormal) 
		{
			m_pEntity->AddVertex(m_PointToAdd);
		}
		if(Stat == kCancel) 
		{
			m_bIsEntityAdded = FALSE;
			break;
		}
	}
	CVAlignment::m_TempVertex.x = CVAlignment::m_TempVertex.y = CVAlignment::m_TempVertex.z = 0.0;
	CVAlignment::m_bDragModeOn = FALSE;
    const AcGePoint3dArray&	ptArray = m_pEntity->GetPointData();
    if(ptArray.length() <= 1)
	{
		ads_printf("\nERROR: Invalid # of vertex input for VerticalAlignment Entity\n");
		m_bIsEntityAdded = FALSE;
	}

    if(m_bIsEntityAdded)
		append();

	m_bIsEntityAdded = TRUE;
}

// This function is called by the drag function in order to
// acquire a sample input
//
AcEdJig::DragStatus CVAlignmentJig::sampler()
{
    if(m_pEntity == 0L)
	{
		AfxMessageBox("ERROR: CVAlignmentJig::doIt()", MB_ICONSTOP);
		return AcEdJig::DragStatus::kCancel;
	}
    static AcGePoint3d	Pt3D(0, 0, 0);
    AcGePoint3d	Pt3DBase;
    DragStatus Stat;

    setUserInputControls((UserInputControls)(AcEdJig::kAccept3dCoordinates|AcEdJig::kNullResponseAccepted));
 	Pt3DBase = m_pEntity->GetLastVertex();
	Stat = acquirePoint(Pt3D, Pt3DBase);
	switch(Stat)
	{
	case DragStatus::kNormal:
		{//
			if(m_PointToAdd != Pt3D)
				m_PointToAdd = Pt3D;
			else
			{
				m_CurDragStat = DragStatus::kNoChange;
				return DragStatus::kNoChange;
			}
		}
		return Stat;
	default:
		return Stat;
	}
    return Stat;
}
// This function is called to update the entity based on the
// input values
//
Adesk::Boolean CVAlignmentJig::update()
{
	if(m_CurDragStat == DragStatus::kNormal)
		m_pEntity->UpdLastVertex(m_PointToAdd);
	else
		CVAlignment::m_TempVertex = m_PointToAdd;
    return Adesk::kTrue;
}


//
AcDbEntity* CVAlignmentJig::entity() const
{
    return m_pEntity;
}
void CVAlignmentJig::CreateVAlignment()
{
    AcGePoint3d tempPt;
	AcGeVector3d normalVec;
	double fStartElev, fChainageAtStartPoint;

	{//Get the Start Elevation
		const char* pszPmt = "\nEnter start elevation: ";
		int iRet;
	
		iRet = ads_getreal(pszPmt, &fStartElev);
		if(iRet != RTNORM)
		{
			ads_printf("\nERROR: No start elevation specified\n");
			return;
		}
	}
	{//Get the Start Chainage
		const char* pszPmt = "\nEnter start chainage: ";
		int iRet;
	
		iRet = ads_getreal(pszPmt, &fChainageAtStartPoint);
		if(iRet != RTNORM)
		{
			ads_printf("\nERROR: No start chainage specified\n");
			return;
		}
	}
	{//Get the starting point
		struct resbuf rbFrom, rbTo;
		const char* pszPmt = "\nEnter start point :";
		int iRet;
	
		iRet = ads_getpoint(0L, pszPmt, asDblArray(tempPt));
		switch (iRet)
		{
		case RTNORM:
			//Point input successful....
			break;
		default:
			ads_printf("\nERROR: No point input given\n");
			return;
		}
		rbFrom.restype		= RTSHORT;
		rbFrom.resval.rint	= 1;// from UCS
		rbTo.restype		= RTSHORT;
		rbTo.resval.rint	= 0;// to WCS

		ads_trans(asDblArray(tempPt), &rbFrom, &rbTo, Adesk::kFalse, asDblArray(tempPt));
	}
	{//Get the Vector....
		AcGeVector3d xVec3D, yVec3D;

		xVec3D = acdbCurDwg()->ucsxdir();
		yVec3D = acdbCurDwg()->ucsydir();
		
		normalVec = xVec3D.crossProduct(yVec3D);
		normalVec.normalize();
	}

    // Create an CVAlignmentJig object passing in the center point
    // just selected by the user and the model space normal vector.
    //
    CVAlignmentJig* pJig;
    
	pJig = new CVAlignmentJig(tempPt, normalVec, fStartElev, fChainageAtStartPoint);
    pJig->doIt();// Now start up the jig 
    delete pJig;// Now delete the jig since it's no longer needed
}
void CVAlignmentJig::EditVAlignmentVertex()
{
	const char* pszPmt = "\nSelect Point of Intersection to edit: ";
	ads_name entName;
	ads_point ptRes;
	AcDbObject* pObj;
	CVAlignment* pEnt;
	Acad::ErrorStatus ErrCode;
	AcDbObjectId objId;
	int iVertex;
	int iReturn;

    iReturn = ads_entsel(pszPmt, entName, ptRes);
    if(iReturn != RTNORM)
	{
		ads_printf("\nERROR: No Entity selected\n");
		return;
	}

	ErrCode = acdbGetObjectId(objId, entName);
	if(ErrCode != Acad::eOk)
	{
		ads_printf("\nERROR: AutoCAD System Error\n");
		return;
	}
	ErrCode = acdbOpenObject(pObj, objId, AcDb::kForWrite);
	if(ErrCode != Acad::eOk)
	{
		ads_printf("\nERROR: AutoCAD System Error\n");
		return;
	}
	if(pObj->isA() != CVAlignment::desc())
	{
		ads_printf("\nERROR: Entity selected is not a Vertical Alignment Entity\n");
		pObj->close();
		return;
	}
	pEnt = (CVAlignment*)pObj;

	{//
		AcGePoint3d ptLeft, ptPOI, ptRite;
		double fRadius, fFirstTanMaxLen, fSecondTanMaxLen;

		iVertex = pEnt->GetNearestVertex(asPnt3d(ptRes));
		if(!pEnt->IsVertexValidForCurveData(iVertex, ptLeft, ptPOI, ptRite, fRadius))
		{
			ads_printf("\nWARNING: Curve data is not applicable for vertex selected\n");
		}
		else
		{//Call dlg interface & get Curve data
			pEnt->GetTangentLengthLimits(iVertex, fFirstTanMaxLen, fSecondTanMaxLen);

			if(CCurveDataDlg::GetCurveRadius(&ptLeft, &ptPOI, &ptRite, fRadius, fFirstTanMaxLen, fSecondTanMaxLen, 0L, 0L, 0L))
			{
				pEnt->SetCurveRadius(iVertex, fRadius);
			}
			else
			{
				ads_printf("\nERROR: User termination\n");
			}
		}
		pObj->close();
	}
}

//Returns 'CVAlignment' obj pointer opened in readable state
//Caller func. is responsible for proper close() call on valid 'CVAlignment' obj pointer 
CVAlignment* CVAlignmentJig::GetVAlignmentEntity(const char* pszPmt)
{
	ads_name entName;
	ads_point ptRes;
	AcDbObject* pObj;
	CVAlignment* pEnt;
	Acad::ErrorStatus ErrCode;
	AcDbObjectId objId;

    if(ads_entsel(pszPmt, entName, ptRes) != RTNORM)
	{
		ads_printf("\nERROR: No Entity selected\n");
		return 0L;
	}

	ErrCode = acdbGetObjectId(objId, entName);
	if(ErrCode != Acad::eOk)
	{
		ads_printf("\nERROR: AutoCAD System Error\n");
		return 0L;
	}
	ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
	if(ErrCode != Acad::eOk)
	{
		ads_printf("\nERROR: AutoCAD System Error\n");
		return 0L;
	} 
	if(pObj->isA() != CVAlignment::desc())
	{
		ads_printf("\nERROR: Entity selected is not a Vertical Alignment Entity\n");
		pObj->close();
		return 0L;
	}
	pEnt = (CVAlignment*)pObj;
	return pEnt;
}

