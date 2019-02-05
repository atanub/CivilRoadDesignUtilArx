
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
#include "HAlignment.h"
#include "Util.h"
#include "HorzCurveDataDlg.h"
#include "CurveCalculator.h"
#include "TINData.h"
#include "XSection.h"
#include "VAlignment.h"

//extern void DeAllocateXSecPtsArray(AcDbVoidPtrArray& Array);

// define a DXF class body for the entity using the
// appropriate ACRX boilerplate macro.  The DXF class name
// *MUST* be all uppercase.
//
// ...
ACRX_DXF_DEFINE_MEMBERS(CHAlignment, AcDbEntity,
AcDbProxyEntity::kNoOperation, CHAlignment, 
"Contour\
|Product Desc: Contour ARX Application\
|Company: Atanu Banik's\
|E-Mail : atanubanik@gmail.com");

AcGePoint3d		CHAlignment::m_TempVertex = AcGePoint3d(0.0, 0.0, 0.0);
BOOL			CHAlignment::m_bDragModeOn = FALSE;

const CString	CHAlignment::m_strCommentPOI = "Point Of Intersection";
const CString	CHAlignment::m_strCommentCurvePoint = "Point On Curve";
const CString	CHAlignment::m_strCommentTC	= "Tangent - Curve";
const CString	CHAlignment::m_strCommentCT	= "Curve - Tangent";

MAKE_ACDBOPENOBJECT_FUNCTION(CHAlignment);

CHAlignment::CHAlignment()
{
	_InitializeData();
}
CHAlignment::CHAlignment(const AcGeVector3d& Normal, const AcGePoint3d& Pt)
: m_Normal(Normal)
{
    m_PointArray.append(Pt);
	m_RadiusArray.append(0.0);//Initialize with Zero Radius
	_InitializeData();
}

CHAlignment::~CHAlignment()
{
	int i;
	
	for(i = 0; i < m_arrayProfileDef.GetSize(); i++)
	{
		delete m_arrayProfileDef[i];
	}
	m_arrayProfileDef.RemoveAll();
	
	for(i = 0; i < m_arrayProfileDefSetting.GetSize(); i++)
	{
		delete m_arrayProfileDef[i];
	}
	m_arrayProfileDefSetting.RemoveAll();
}

void CHAlignment::_InitializeData()
{
    m_fElevAtStartPointVAlignment = 0.0;
    m_fChainageAtStartPointVAlignment = 0.0;

	memset(&m_RecPflData, 0, sizeof(PROFILEDATAEX));
	{//Initialize default data...
		m_RecPflData.iVersion = CVersionOfStructs::m_iVersionProfileData;
		
		m_RecPflData.data.dXScale = 1.0;
		m_RecPflData.data.dYScale = 10.0;

		m_RecPflData.data.dNumAnnSize = 2.0;
		m_RecPflData.data.dRowAnnSize = 3.0;
		m_RecPflData.data.dRowHt = 15.0;
		m_RecPflData.data.dPflLabelSize = 4.0;

		m_RecPflData.data.bDrawNERows = TRUE;

		m_RecPflData.data.dDatum = 0.0;
		m_RecPflData.data.dStartChg = 0.0;
		m_RecPflData.data.dSexonInterval = 5.0;
		m_RecPflData.data.dSexonIntervalMinor = 1.0;
		//
		m_RecPflData.fProfilePathWidth = 3.0 * m_RecPflData.data.dSexonInterval;

		strcpy(m_RecPflData.data.szNorthingTag,	"Northing in Metre");
		strcpy(m_RecPflData.data.szEastingTag,	"Easting in Metre");
		strcpy(m_RecPflData.data.szElevTag,		"R.L. in Metre");
		strcpy(m_RecPflData.data.szDistanceTag,	"Distance in Metre");
		strcpy(m_RecPflData.data.szCumDistanceTag,"Cumulative Distance in Metre");
		strcpy(m_RecPflData.data.szDatumTag,		"Datum = ");
		strcpy(m_RecPflData.data.szProfileTag,	"Profile");

		strcpy(m_RecPflData.data.szLyrAnnotation, "PROFILE_ANNOTATION");
		strcpy(m_RecPflData.data.szLyrTopSurface, "PROFILE_TOP_SURFACE");
		strcpy(m_RecPflData.data.szLyrOrdinate,	"PROFILE_ORDINATE");
		strcpy(m_RecPflData.data.szLyrAxis,		"PROFILE_AXIS");
	}
	memset(&m_RecXSecData, 0, sizeof(XSECDATAEX));
	{//Initialize default data...
		m_RecXSecData.iVersion = CVersionOfStructs::m_iVersionXSecData;
		m_RecXSecData.data.dXScale = 1.0;
		m_RecXSecData.data.dYScale = 10.0;
		m_RecXSecData.data.dNumAnnSize = 2.0;
		m_RecXSecData.data.dRowAnnSize = 3.0;
		m_RecXSecData.data.dCSLabelAnnSize = 4.0;
		m_RecXSecData.data.dRowHt = 18.0;

		m_RecXSecData.data.dStartChg = 0.0;
		m_RecXSecData.data.dCSInterval = 10.0;
		m_RecXSecData.data.dOrdInterval = 2.0;
		m_RecXSecData.data.dMaxCSOffset = 10.0;
		m_RecXSecData.data.dAutoDatumDenominator = 1.0;
		m_RecXSecData.data.bAutoDatumFlag = TRUE;

		strcpy(m_RecXSecData.data.szElevTag, "Elevation");
		strcpy(m_RecXSecData.data.szNorthingTag, "Northing");
		strcpy(m_RecXSecData.data.szEastingTag, "Easting");
		strcpy(m_RecXSecData.data.szOffsetTag, "Offset");
		strcpy(m_RecXSecData.data.szDatumTag, "Datum = ");
		strcpy(m_RecXSecData.data.szXSecLabelTag, "C/S at Ch:");

		strcpy(m_RecXSecData.data.szLyrAnnotation, "XSECN_ANNOTATION");
		strcpy(m_RecXSecData.data.szLyrTopSurface, "XSECN_TOP_SURFACE");
		strcpy(m_RecXSecData.data.szLyrOrdinate,	"XSECN_ORDINATE");
		strcpy(m_RecXSecData.data.szLyrAxis,		"XSECN_AXIS");

		m_RecXSecData.data.dSheetWid = 1000.0;
		m_RecXSecData.data.dSheetHeight = 800.0;
		m_RecXSecData.data.dNPlateWid = 250.0;
		m_RecXSecData.data.dNPlateHeight = 150.0;
		//
		m_RecXSecData.bDrawSectionsAtAllIntersectionPoints	= FALSE;
		m_RecXSecData.bDrawWithNorthingEastingData			= TRUE;
	}
}
Adesk::Boolean CHAlignment::worldDraw(AcGiWorldDraw* wd)
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
AcDbPolyline* CHAlignment::ConvertToPolyLine() const
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

BOOL CHAlignment::IsValid()
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
void CHAlignment::saveAs(AcGiWorldDraw* wd, AcDb::SaveType saveType)
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
Acad::ErrorStatus CHAlignment::dwgInFields(AcDbDwgFiler* filer)
{
    assertWriteEnabled();

    AcDbEntity::dwgInFields(filer);

	Adesk::Int32 i, lCount;

	{//Read
		//all the Points.......
		filer->readInt32(&lCount);
		for(i = 0; i < lCount; i++)
		{
			AcGePoint3d PtTmp;
			
			filer->readPoint3d(&PtTmp);
			m_PointArray.append(PtTmp);
		}
	}
	//Normal
	filer->readItem(&m_Normal);

	//Write all the Radius.......
	filer->readInt32(&lCount);
	for(i = 0; i < lCount; i++)
	{
		double fVal;
		
		filer->readDouble(&fVal);
		m_RadiusArray.append(fVal);
	}
	//Profile related vars
	if(Acad::eOk != filer->readBytes((void*)&(m_RecPflData.data), sizeof(m_RecPflData.data)))
	{
		return Acad::eDwgNeedsRecovery;
	}
	//X-Section related vars
	if(Acad::eOk != filer->readBytes((void*)&(m_RecXSecData.data), sizeof(m_RecXSecData.data)))
	{
		return Acad::eDwgNeedsRecovery;
	}
	//X-Section Datum Range Array
	if(Acad::eOk != filer->readInt32(&lCount))
	{
		return Acad::eDwgNeedsRecovery;
	}
	for(i = 0; i < lCount; i++)
	{
		AcGePoint3d ptTemp;

		if(Acad::eOk != filer->readPoint3d(&ptTemp))
		{
			return Acad::eDwgNeedsRecovery;
		}
		m_DatumRangeArray.append(ptTemp);
	}
	//X-Section Profile Definition Array
	if(Acad::eOk != readXSectionProfileDefinition(filer))
	{
		return Acad::eDwgNeedsRecovery;
	}
	
	//X-Section Profile Setting Array
	if(Acad::eOk != readXSectionProfileSetting(filer))
	{
		return Acad::eDwgNeedsRecovery;
	}

    return filer->filerStatus();
}
Acad::ErrorStatus CHAlignment::readXSectionProfileSetting(AcDbDwgFiler* filer)
{
	CCrossSectionProfileSettingArray	arrayProfileDefSetting;
	long i, iCountDefinitions;
	
	if(Acad::eOk != filer->readInt32(&iCountDefinitions))
	{
		return Acad::eDwgNeedsRecovery;
	}
	for(i = 0; i < iCountDefinitions; i++)
	{
		double fStartChainage, fEndChainage;
		char* pszProfileDefintionName;

		if(Acad::eOk != filer->readDouble(&fStartChainage))
		{
			return Acad::eDwgNeedsRecovery;
		}
		if(Acad::eOk != filer->readDouble(&fEndChainage))
		{
			return Acad::eDwgNeedsRecovery;
		}
		//Read Name of X-Section Profile Definition
		if(Acad::eOk != filer->readString(&pszProfileDefintionName))
		{
			return Acad::eDwgNeedsRecovery;
		}

		CCrossSectionProfileSetting* pRec;
		
		pRec = new CCrossSectionProfileSetting(fStartChainage, fEndChainage, pszProfileDefintionName);
		m_arrayProfileDefSetting.Add(pRec);
		acdbFree(pszProfileDefintionName);
	}
    return filer->filerStatus();
}
Acad::ErrorStatus CHAlignment::readXSectionProfileDefinition(AcDbDwgFiler* filer)
{
	long i, iCountDefinitions;
	char* pszProfileDefintionName;

	if(Acad::eOk != filer->readInt32(&iCountDefinitions))
	{
		return Acad::eDwgNeedsRecovery;
	}
	for(i = 0; i < iCountDefinitions; i++)
	{
		long j, iCountVertices, iIndexOfCenterPoint;
		AcGePoint3d pt3DTemp;
		AcGePoint3dArray arrayPoints;

		//Read all vertices
		if(Acad::eOk != filer->readInt32(&iCountVertices))
		{
			return Acad::eDwgNeedsRecovery;
		}
		for(j = 0; j < iCountVertices; j++)
		{
			if(Acad::eOk != filer->readPoint3d(&pt3DTemp))
			{
				return Acad::eDwgNeedsRecovery;
			}
			arrayPoints.append(pt3DTemp);
		}
		//Read Index Of Center Point
		if(Acad::eOk != filer->readInt32(&iIndexOfCenterPoint))
		{
			return Acad::eDwgNeedsRecovery;
		}
		//Read Name of X-Section Profile Definition
		if(Acad::eOk != filer->readString(&pszProfileDefintionName))
		{
			return Acad::eDwgNeedsRecovery;
		}
		{//
			CCrossSectionProfileDefinition* pDefinition;
			
			pDefinition = new CCrossSectionProfileDefinition(arrayPoints, iIndexOfCenterPoint, pszProfileDefintionName);
			m_arrayProfileDef.Add(pDefinition);

			acdbFree(pszProfileDefintionName);
		}
	}
	CVAlignment::ReadDwgFields(filer, m_PointArrayVAlignment, m_NormalVAlignment, m_RadiusArrayVAlignment, m_fElevAtStartPointVAlignment, m_fChainageAtStartPointVAlignment);
    return filer->filerStatus();
}

Acad::ErrorStatus CHAlignment::dwgOutFields(AcDbDwgFiler* filer) const
{
    assertReadEnabled();

    AcDbEntity::dwgOutFields(filer);
	
	//Write all the Points.......
	Adesk::Int32 i, lCount;
	
	lCount = m_PointArray.length();
	filer->writeInt32(lCount);
	for(i = 0; i < lCount; i++)
	{
		filer->writePoint3d(m_PointArray.at(i));
	}
	//Normal
	filer->writeItem(m_Normal);

	//Write all the Radius.......
	lCount = m_RadiusArray.length();
	filer->writeInt32(lCount);

	for(i = 0; i < lCount; i++)
	{
		filer->writeDouble(m_RadiusArray.at(i));
	}
	//Profile related vars
	filer->writeBytes((void*)(&m_RecPflData.data), sizeof(m_RecPflData.data));
	//X-Section related vars
	filer->writeBytes((void*)(&m_RecXSecData.data), sizeof(m_RecXSecData.data));
	//X-Section Datum Range Array
	lCount = m_DatumRangeArray.length();
	filer->writeInt32(lCount);
	for(i = 0; i < lCount; i++)
	{
		filer->writePoint3d(m_DatumRangeArray.at(i));
	}
	//X-Section Profile Definition Array
	if(Acad::eOk != writeXSectionProfileDefinition(filer))
	{
		return Acad::eDwgNeedsRecovery;
	}
	
	//X-Section Profile Setting Array
	if(Acad::eOk != writeXSectionProfileSetting(filer))
	{
		return Acad::eDwgNeedsRecovery;
	}
	CVAlignment::WriteDwgFields(filer, m_PointArrayVAlignment, m_NormalVAlignment, m_RadiusArrayVAlignment, m_fElevAtStartPointVAlignment, m_fChainageAtStartPointVAlignment);

	return filer->filerStatus();
}
Acad::ErrorStatus CHAlignment::writeXSectionProfileDefinition(AcDbDwgFiler* filer) const
{
	Adesk::Int32 i, lCountDefinitions;
	
	lCountDefinitions = m_arrayProfileDef.GetSize();
	filer->writeInt32(lCountDefinitions);
	for(i = 0; i < lCountDefinitions; i++)
	{
		const CCrossSectionProfileDefinition* pRec = m_arrayProfileDef[i];
		Adesk::Int32 j, lCount;
		const AcGePoint3dArray& arrayPoints = pRec->getVertices();

		//Write all vertices
		lCount = arrayPoints.length();
		filer->writeInt32(lCount);
		for(j = 0; j < lCount; j++)
		{
			filer->writePoint3d(arrayPoints[j]);
		}
		//Write Index Of Center Point
		filer->writeInt32((Adesk::Int32)(pRec->getIndexOfCenterPoint()));
		//Write Name of X-Section Profile Definition
		filer->writeString(pRec->getName());
	}
    return filer->filerStatus();
}
Acad::ErrorStatus CHAlignment::writeXSectionProfileSetting(AcDbDwgFiler* filer) const
{
	Adesk::Int32 i, lCountDefSetting;
	
	lCountDefSetting = m_arrayProfileDefSetting.GetSize();
	filer->writeInt32(lCountDefSetting);
	for(i = 0; i < lCountDefSetting; i++)
	{
		const CCrossSectionProfileSetting* pRec = m_arrayProfileDefSetting[i];

		filer->writeDouble(pRec->getStartChainage());
		filer->writeDouble(pRec->getEndChainage());
		filer->writeString(pRec->getProfileDefintionName());
	}
    return filer->filerStatus();
}

Acad::ErrorStatus CHAlignment::audit(AcDbAuditInfo* /*auditor*/)
{
	return Acad::eOk;
}
Acad::ErrorStatus CHAlignment::explode(AcDbVoidPtrArray& entities) const
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
void CHAlignment::Revert()
{
    assertWriteEnabled();
	m_PointArray.reverse();
	m_RadiusArray.reverse();
}

void CHAlignment::list() const
{
    assertReadEnabled();
 	int i;

	ads_printf("\nHorizontal Alignment Entity\n# Of Vertices : %d", m_PointArray.length());
    for (i = 0; i < m_PointArray.length(); i++)
	{
		ads_printf("\nVertex #%d, X = %s, Y = %s", (i+1), RTOS(m_PointArray[i].x), RTOS(m_PointArray[i].y));
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

					ads_printf("\nSegment #%d, Type: Curve, Start Point: X = %s, Y = %s, End Point: X = %s, Y = %s", i, RTOS(ptStartCurve.x), RTOS(ptStartCurve.y), RTOS(ptEndCurve.x), RTOS(ptEndCurve.y));
					ads_printf("\nSegment Length: %s", RTOS(segArc.radius() * fabs(segArc.endAng() - segArc.startAng())));
				}
				else
				{
					ads_printf("\nSegment #%d, Type: Line, Start Point: X = %s, Y = %s, End Point: X = %s, Y = %s", i, RTOS(ptStart.x), RTOS(ptStart.y), RTOS(ptEnd.x), RTOS(ptEnd.y));
					ads_printf("\nSegment Length: %s", RTOS(Dist2D(ptStart, ptEnd)));
				}
			}
		}
		delete pEnt;
	}
}
BOOL CHAlignment::AddVertex(const AcGePoint3d& Pt)
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
Acad::ErrorStatus CHAlignment::getGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray& /*osnapModes*/, AcDbIntArray& /*geomIds*/) const
{
    assertReadEnabled();
    Acad::ErrorStatus ErrStat = Acad::ErrorStatus::eOk;
    
	gripPoints.append(m_PointArray);
    return ErrStat;
}
BOOL CHAlignment::IsValid(const int& iVertexIndex, const AcGePoint3d& ptToTest)
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

Acad::ErrorStatus CHAlignment::moveGripPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
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
Acad::ErrorStatus CHAlignment::moveStretchPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
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
Acad::ErrorStatus CHAlignment::getStretchPoints(AcGePoint3dArray& stretchPoints) const
{
    assertReadEnabled();

    Acad::ErrorStatus es = Acad::eOk;
	stretchPoints.append(m_PointArray);
    return es;
}

Acad::ErrorStatus CHAlignment::transformBy(const AcGeMatrix3d& xform)
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
int CHAlignment::GetNearestVertex(const AcGePoint3d& ptParam) const
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
BOOL CHAlignment::SetCurveRadius(const int& iVertex, const double& fRad)
{
    assertWriteEnabled();

	if(!IsVertexValidForCurveData(iVertex))
		return FALSE;
	m_RadiusArray.setAt(iVertex, fRad);
	return TRUE;
}

BOOL CHAlignment::IsVertexValidForCurveData(const int& iVertex) const
{
    assertReadEnabled();

	if((iVertex == 0) || (iVertex == (m_PointArray.length() - 1)))
		return FALSE;
	return TRUE;
}
BOOL CHAlignment::IsVertexValidForCurveData(const int& iVertex, AcGePoint3d& ptLeft, AcGePoint3d& ptPOI, AcGePoint3d& ptRite, double& fRad) const
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
BOOL CHAlignment::GetTangentLengthLimits(const int& iVertex, double& fFirstTanMaxLen, double& fSecondTanMaxLen) const
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

double CHAlignment::GetCurveLength(const int& iVertex) const
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
BOOL CHAlignment::GetTangentLength(const int& iVertex, double& fMaxTangentLen) const
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
BOOL CHAlignment::GetPointsOnCurve(const int& iVertex, AcGePoint3d& ptFirst, AcGePoint3d& ptSecond, AcGePoint3d& ptThird, AcGePoint3d* pPtCentre /*= 0L*/) const
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
BOOL CHAlignment::CreateCurveDataReport(CCurveDetDataArray& arrayRecs) const
{
    assertReadEnabled();
	if(arrayRecs.GetSize() > 0)
		return FALSE;

	int i;
	CURVEDETDATA* pRec;

	for(i = 0; i < m_PointArray.length(); i++)
	{
		if((i == 0) || (i == (m_PointArray.length() - 1)))
		{//Start/End Line...
			pRec = new CURVEDETDATA;

			pRec->fRad = 0.0;
			pRec->fTangentLength = 0.0;
			pRec->fXternalLength = 0.0;
			pRec->fCurveLen = 0.0;
			pRec->fDegOfCurve = 0.0;
			pRec->fDefAngle = 0.0;
			pRec->ptPOI = m_PointArray[i];
			//pRec->ptTC = 0.0;
			//pRec->ptCT = 0.0;
			if(i == 0)
				pRec->strComment = "Start of Alignment";
			else
				pRec->strComment = "End of Alignment";
			
			arrayRecs.Add(pRec);
		}//Start/End Line...
		else
		{//Intermediate Line...
			if(m_RadiusArray[i] == 0.0)
			{
				pRec = new CURVEDETDATA;

				pRec->fRad = 0.0;
				pRec->fTangentLength = 0.0;
				pRec->fXternalLength = 0.0;
				pRec->fCurveLen = 0.0;
				pRec->fDegOfCurve = 0.0;
				pRec->ptPOI = m_PointArray[i];
				//pRec->ptTC = 0.0;
				//pRec->ptCT = 0.0;
				pRec->strComment = "Intermediate Point of Intersection";
				arrayRecs.Add(pRec);
			}
			else
			{
				double fFirstTanMaxLen, fSecondTanMaxLen;

				fFirstTanMaxLen = Dist2D(m_PointArray[i], m_PointArray[i - 1]);
				fSecondTanMaxLen = Dist2D(m_PointArray[i], m_PointArray[i + 1]);

				CCurveCalculator CalcObj(m_PointArray[i], m_PointArray[i - 1], m_PointArray[i + 1], fFirstTanMaxLen, fSecondTanMaxLen);

				if(!CalcObj.UpdWRTRad(m_RadiusArray[i]))
				{
					ads_printf("\nERROR: Invalid curve radius found on vertex #%d\n", i);
					return FALSE;
				}
				if(!CalcObj.IsValid())
				{
					ads_printf("\nERROR: Invalid curve radius found on vertex #%d\n", i);
					return FALSE;
				}
				//
				pRec = new CURVEDETDATA;

				pRec->fRad = m_RadiusArray[i];
				pRec->fTangentLength = CalcObj.GetTangentLen();
				pRec->fXternalLength = CalcObj.GetXternalLen();
				pRec->fCurveLen = CalcObj.GetCurvleLen();
				pRec->fDegOfCurve = CalcObj.GetDegOfCurve();
				pRec->ptPOI = m_PointArray[i];
				pRec->fDefAngle = CalcObj.GetDefAngle();

				ads_polar(asDblArray(pRec->ptPOI), Angle2D(m_PointArray[i], m_PointArray[i-1]), pRec->fTangentLength, asDblArray(pRec->ptTC));
				ads_polar(asDblArray(pRec->ptPOI), Angle2D(m_PointArray[i], m_PointArray[i+1]), pRec->fTangentLength, asDblArray(pRec->ptCT));

				pRec->strComment = "Intermediate Point of Intersection";
				arrayRecs.Add(pRec);
			}

		}//Intermediate Line...
	}
	return (arrayRecs.GetSize() > 0);
}
BOOL CHAlignment::DrawProfile(const CTINData* pTinDataObject, const BOOL bDrawPlanVw/* = TRUE*/) const
{
    assertReadEnabled();
	{//
		AcDbPolyline* pObj;
		BOOL bRetValue;
		
		bRetValue = TRUE;
		pObj = ConvertToPolyLine();
		if(0L == pObj)
		{
			assert(false);
			return FALSE;//ERROR
		}
		{//
			CPLineDataExtractorEx objDataExtractor(pObj);

			if(objDataExtractor.DivideIntoSegments(m_RecPflData.data.dSexonInterval, m_RecPflData.data.dSexonIntervalMinor, m_RecPflData.data.dStartChg))
			{
				const char* pszLyr = m_RecPflData.data.szLyrAnnotation;
				const CThePointsOnAlignment& arrayGenPoints = objDataExtractor.GetPointData();
				
				bRetValue = arrayGenPoints.DrawProfile(m_RecPflData, pTinDataObject);
				if(!bRetValue)
				{
					ads_printf("ERROR: Failed to draw Profile. User break encounterd!");
					bRetValue = FALSE;
				}
				if(bRetValue && bDrawPlanVw)
				{
					bRetValue = arrayGenPoints.DrawPoints(m_RecPflData.data.dNumAnnSize, (m_RecPflData.fProfilePathWidth / 2.0), m_RecPflData.data.szLyrAnnotation);
				}
			}
			/*
			if(bRetValue)
			{//Temporary Solution : 23May2005 -- Draw points for C/S drawing.............
				const CThePointsOnAlignment& objPtsOnAlignment = objDataExtractor.GetPointData();
				const char* pszLyr = "TEST_LYR";
				int i, j;
				AcGePoint3dArray ContourPtsArray;
				CTrianglePtrArray objTrgArray;

				pTinDataObject->GetTINData(ContourPtsArray, &objTrgArray);

				LAYER(pszLyr, 1);
				for(i = 0; i < objPtsOnAlignment.GetCount(); i++)
				{//Generate X Sectional points on each generated stations....
					const CPointOnAlignment* pRec =	objPtsOnAlignment.GetAt(i);
					AcGePoint3dArray PtsAlongXSecArray;

					if(GetXSecPoints(ContourPtsArray, objTrgArray, pRec->GetPoint(), (pRec->GetAngleOfAnnotationLine() + (PI / 2.0)), m_RecXSecData, PtsAlongXSecArray))
					{
						for(j = 0; j < PtsAlongXSecArray.length(); j++)
						{
							DRAWPOINT(PtsAlongXSecArray.at(j), pszLyr);
						}
					}
				}
				//
				DeallocateTrgArray(objTrgArray);//Deallocate Triangle data...
			}
			*/
		}
		delete pObj;
		return bRetValue;//ERROR
	}
}
BOOL CHAlignment::_CalcPointsOnCurve(const int& iVertex, double& fCumChainage, AcGeDoubleArray& fChgArray, AcGePoint3dArray& PflPtsArray, CStringArray& arrayOrdComment) const
{
	if((PflPtsArray.length() != arrayOrdComment.GetSize()) || (arrayOrdComment.GetSize() != fChgArray.length()) || (arrayOrdComment.GetSize() != 0))
	{
		return FALSE;
	}
	{//Calculate Curve Points
		const double  fStartChainage = fCumChainage;
		double fCurveLen, fShift, fTanLen;
		int iCount;
		AcGePoint3d ptRes;
		
		if(!GetTangentLength(iVertex, fTanLen))
			return FALSE;

		fCurveLen = GetCurveLength(iVertex);
		if(fmod(fCumChainage, m_RecPflData.data.dSexonIntervalMinor) == 0.0)
		{
			
			fShift = 0.0;
		}
		else
		{
			fShift = m_RecPflData.data.dSexonIntervalMinor - fmod(fCumChainage, m_RecPflData.data.dSexonIntervalMinor);

		}
		for(iCount = 0; ; iCount++)
		{//Calculate all the points on curve
			if(iCount == 0)
			{
				CCurveCalculator::GetPointOnCurve(m_PointArray[iVertex - 1], m_PointArray[iVertex], m_PointArray[iVertex + 1], m_RadiusArray[iVertex], fTanLen, 0.0, ptRes);
				PflPtsArray.append(ptRes);
				arrayOrdComment.Add(m_strCommentTC);//"Tangent To Curve"
				fChgArray.append(fCumChainage);
				fCumChainage += fShift;
			}
			else
			{
				CCurveCalculator::GetPointOnCurve(m_PointArray[iVertex - 1], m_PointArray[iVertex], m_PointArray[iVertex + 1], m_RadiusArray[iVertex], fTanLen, fShift, ptRes);
				arrayOrdComment.Add(m_strCommentCurvePoint);//"Point On Curve"
				PflPtsArray.append(ptRes);
				fChgArray.append(fCumChainage);

				fShift += m_RecPflData.data.dSexonIntervalMinor;
				fCumChainage += m_RecPflData.data.dSexonIntervalMinor;

				if(fShift >= fCurveLen)
				{
					if(fShift > fCurveLen)
					{
						//CCurveCalculator::GetPointOnCurve(m_PointArray[iVertex - 1], m_PointArray[iVertex], m_PointArray[iVertex + 1], m_RadiusArray[iVertex], fTanLen, fCurveLen, ptRes);
						
						//PflPtsArray.append(ptRes);
						//arrayOrdComment.Add(m_strCommentCT);//"Curve To Tangent"
						//fCumChainage = fStartChainage + fCurveLen;
						//fChgArray.append(fCumChainage);
					
						fCumChainage = fStartChainage + fCurveLen;
					}
					return TRUE;
				}
			}
		}
	}
}

BOOL CHAlignment::CalcPointsOnEqInterval(const AcGePoint3d& PtStart, const AcGePoint3d& PtEnd, const double& fMaxLineLen, const double& fStnInterval, double& fCumChgParam, AcGeDoubleArray& fChgArray, AcGePoint3dArray& PflPtsArray, CStringArray& strArrayComments)
{
	double fAngle, fChgAdjustment, fCumChgTmp;
	AcGePoint3d ptOnPflPath, PtStartWRTChg;

	if((PflPtsArray.length() > 0) || (fStnInterval <= 0.0) || (strArrayComments.GetSize() > 0) || (fChgArray.length() > 0))
		return FALSE;
	
	if(fMaxLineLen > Dist2D(PtStart, PtEnd))
		return FALSE;

	fAngle = Angle2D(PtStart, PtEnd);
	
	ptOnPflPath = PtStart;
	PflPtsArray.append(ptOnPflPath);
	fChgArray.append(fCumChgParam);
	strArrayComments.Add(m_strCommentPOI);//"Point of Intersection"
	
	if(fStnInterval >= fMaxLineLen)
	{
		ads_polar(asDblArray(PtStart), fAngle, fMaxLineLen, asDblArray(ptOnPflPath));
		PflPtsArray.append(ptOnPflPath);
		fCumChgParam += fMaxLineLen;
		fChgArray.append(fCumChgParam);
		strArrayComments.Add("");
		return TRUE;
	}
	{//Recalc Cum Chainage.....
		fChgAdjustment = fmod(fCumChgParam, fStnInterval);
		if(fChgAdjustment > 0.0)
		{
			fChgAdjustment = (fStnInterval - fChgAdjustment);
			fCumChgParam += fChgAdjustment;
			ads_polar(asDblArray(PtStart), fAngle, fChgAdjustment, asDblArray(PtStartWRTChg));

			fChgArray.append(fCumChgParam);
			PflPtsArray.append(PtStartWRTChg);
			strArrayComments.Add("");
		}
		else
		{
			PtStartWRTChg = PtStart;
		}
	}

	for(fCumChgTmp = fStnInterval; fCumChgTmp <= (fMaxLineLen - fChgAdjustment); fCumChgTmp += fStnInterval)
	{
		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break encountered\n");
			return FALSE;
		}
		if(fCumChgTmp <= (fMaxLineLen - fChgAdjustment))
		{
			ads_polar(asDblArray(PtStartWRTChg), fAngle, fCumChgTmp, asDblArray(ptOnPflPath));
			fCumChgParam += fStnInterval;
			fChgArray.append(fCumChgParam);
			PflPtsArray.append(ptOnPflPath);
			strArrayComments.Add("");
		}
	}
	//Don't consider the last point, only consider increment the chainage
	fCumChgParam += fmod((fMaxLineLen - fChgAdjustment), fStnInterval);
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHAlignmentJig::CHAlignmentJig(const AcGePoint3d& pt, const AcGeVector3d& Normal)
: m_Normal(Normal), m_pEntity(0L), m_PointToAdd(pt), m_bIsEntityAdded(FALSE), m_CurDragStat(DragStatus::kNoChange)
{
}

CHAlignmentJig::~CHAlignmentJig()
{
	if(!m_bIsEntityAdded)
		if(m_pEntity != 0L)
			delete m_pEntity;
}
// This function creates an AsdkCircle object and gets the
// jig started acquiring the necessary info to properly fill
// it in.
//
void CHAlignmentJig::doIt()
{
    if(m_pEntity == 0L)
	{
	    m_pEntity = new CHAlignment(m_Normal, m_PointToAdd);
		CHAlignment::m_TempVertex = m_PointToAdd;
		CHAlignment::m_bDragModeOn = TRUE;
	}
	else
	{
		AfxMessageBox("ERROR: CHAlignmentJig::doIt()", MB_ICONSTOP);
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
	CHAlignment::m_TempVertex.x = CHAlignment::m_TempVertex.y = CHAlignment::m_TempVertex.z = 0.0;
	CHAlignment::m_bDragModeOn = FALSE;
    const AcGePoint3dArray&	ptArray = m_pEntity->GetPointData();
    if(ptArray.length() <= 1)
	{
		ads_printf("\nERROR: Invalid # of vertex input for Horizontal Alignment Entity\n");
		m_bIsEntityAdded = FALSE;
	}

    if(m_bIsEntityAdded)
		append();

	m_bIsEntityAdded = TRUE;
}

// This function is called by the drag function in order to
// acquire a sample input
//
AcEdJig::DragStatus CHAlignmentJig::sampler()
{
    if(m_pEntity == 0L)
	{
		AfxMessageBox("ERROR: CHAlignmentJig::doIt()", MB_ICONSTOP);
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
Adesk::Boolean CHAlignmentJig::update()
{
	if(m_CurDragStat == DragStatus::kNormal)
		m_pEntity->UpdLastVertex(m_PointToAdd);
	else
		CHAlignment::m_TempVertex = m_PointToAdd;
    return Adesk::kTrue;
}


//
AcDbEntity* CHAlignmentJig::entity() const
{
    return m_pEntity;
}
void CHAlignmentJig::CreateHAlignment()
{
    AcGePoint3d tempPt;
	AcGeVector3d normalVec;

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

    // Create an CHAlignmentJig object passing in the center point
    // just selected by the user and the model space normal vector.
    //
    CHAlignmentJig* pJig;
    
	pJig = new CHAlignmentJig(tempPt, normalVec);
    pJig->doIt();// Now start up the jig 
    delete pJig;// Now delete the jig since it's no longer needed
}
void CHAlignmentJig::RevertHAlignment()
{
	const char* pszPmt = "\nSelect Horizontal Alignment to revert : ";
	ads_name entName;
	ads_point ptRes;
	AcDbObject* pObj;
	CHAlignment* pEnt;
	Acad::ErrorStatus ErrCode;
	AcDbObjectId objId;
	AcGePoint3d ptTmp;
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
	if(pObj->isA() != CHAlignment::desc())
	{
		ads_printf("\nERROR: Entity selected is not a Horizontal Alignment Entity\n");
		pObj->close();
		return;
	}
	pEnt = (CHAlignment*)pObj;
	pEnt->Revert();
	ads_printf("\nEntity direction reversed successfully\n");
	pObj->close();
}
void CHAlignmentJig::AnnotateHAlignmentVertex()
{
	const char* pszPmt = "\nSelect Horizontal Alignment Vertex to annotate: ";
	ads_name entName;
	ads_point ptRes;
	AcDbObject* pObj;
	CHAlignment* pEnt;
	Acad::ErrorStatus ErrCode;
	AcDbObjectId objId;
	AcGePoint3d ptTmp;
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
	if(pObj->isA() != CHAlignment::desc())
	{
		ads_printf("\nERROR: Entity selected is not a Horizontal Alignment Entity\n");
		pObj->close();
		return;
	}
	pEnt = (CHAlignment*)pObj;

	DIMZIN(0);//Don't suppress leading/trailing zeros
	{//
		CCurveDetDataArray arrayDataRecs;
		int iVertex;
	
		iVertex = pEnt->GetNearestVertex(asPnt3d(ptRes));
		if(!pEnt->CreateCurveDataReport(arrayDataRecs))
		{
			ads_printf("\nERROR: Failed to calculate Curve data\n");
			pObj->close();
			DeAllocateCurveDataRecArray(arrayDataRecs);
			return;
		}
		{//
			int iRet;
	
			iRet = ads_getpoint(asDblArray(arrayDataRecs[iVertex]->ptPOI), "\nSelect top-left corner of table :", asDblArray(ptTmp));
			if(iRet != RTNORM)
			{
				ads_printf("\nERROR: Not a valid point picked\n");
				pObj->close();
				DeAllocateCurveDataRecArray(arrayDataRecs);
				return;
			}
		}
		//
		DrawCurveDataTable(arrayDataRecs[iVertex], ptTmp);
		DeAllocateCurveDataRecArray(arrayDataRecs);
		pObj->close();
	}
}

void CHAlignmentJig::EditHAlignmentVertex()
{
	const char* pszPmt = "\nSelect Point of Intersection to edit: ";
	ads_name entName;
	ads_point ptRes;
	AcDbObject* pObj;
	CHAlignment* pEnt;
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
	if(pObj->isA() != CHAlignment::desc())
	{
		ads_printf("\nERROR: Entity selected is not a Horizontal Alignment Entity\n");
		pObj->close();
		return;
	}
	pEnt = (CHAlignment*)pObj;

	{//
		AcGePoint3d ptLeft, ptPOI, ptRite;
		double fRadius, fFirstTanMaxLen, fSecondTanMaxLen;

		iVertex = pEnt->GetNearestVertex(asPnt3d(ptRes));
		if(!pEnt->IsVertexValidForCurveData(iVertex, ptLeft, ptPOI, ptRite, fRadius))
		{
			ads_printf("\nWARNING: Curve data is not applicable for vertex selected\n");
			if(CCurveDataDlg::GetOnlyPflAndXSecParam(pEnt->GetProfileSetting(), pEnt->GetXSecSetting(), pEnt->GetDatumRangeArray()))
			{
			}
			else
			{
				ads_printf("\nERROR: User termination\n");
			}
		}
		else
		{//Call dlg interface & get Curve data
			pEnt->GetTangentLengthLimits(iVertex, fFirstTanMaxLen, fSecondTanMaxLen);

			if(CCurveDataDlg::GetCurveRadius(&ptLeft, &ptPOI, &ptRite, fRadius, fFirstTanMaxLen, fSecondTanMaxLen, &(pEnt->GetProfileSetting()), &(pEnt->GetXSecSetting()), &(pEnt->GetDatumRangeArray())))
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

//Returns 'CHAlignment' obj pointer opened in readable state
//Caller func. is responsible for proper close() call on valid 'CHAlignment' obj pointer 
CHAlignment* CHAlignmentJig::GetHAlignmentEntity(const char* pszPmt)
{
	ads_name entName;
	ads_point ptRes;
	AcDbObject* pObj;
	CHAlignment* pEnt;
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
	if(pObj->isA() != CHAlignment::desc())
	{
		ads_printf("\nERROR: Entity selected is not a Horizontal Alignment Entity\n");
		pObj->close();
		return 0L;
	}
	pEnt = (CHAlignment*)pObj;
	return pEnt;
}

void CHAlignmentJig::DrawCurveDataTable(const CURVEDETDATA* pRec, const AcGePoint3d& ptTopLeft)
{
	const CString strLyr("CURVE_TABLE");
	int i;
	double fLabColWid, fValColWid, fTextSize;
	CStringArray strArrayLabels, strArrayVals;
	CString strTmp;
	AcGePoint3d ptTmpA, ptTmpB, ptTmpC;

	strTmp.Format("\nEnter text size <%f>:", TEXTSIZE());
 	if(RTNORM != ads_getdist(asDblArray(ptTopLeft), strTmp, &fTextSize))
		fTextSize = TEXTSIZE();

	const double fRowHt = fTextSize * 2.0;

	//Form LabelArray
	strArrayLabels.Add("Description");
	strArrayLabels.Add("Comment");
	strArrayLabels.Add("Curve Radius");
	if(pRec->fRad > 0.0)
	{
		strArrayLabels.Add("Tangent Length");
		strArrayLabels.Add("Apex Distance");
		strArrayLabels.Add("Curve Length");
		strArrayLabels.Add("Degree of Curvature");
		strArrayLabels.Add("Deflection Angle");
		strArrayLabels.Add("Point of Intesection");
		strArrayLabels.Add("Start of Curve");
		strArrayLabels.Add("End of Curve");
	}

	strTmp.Format("%s", "Value");
	strArrayVals.Add(strTmp);
	strTmp.Format("%s", pRec->strComment);
	strArrayVals.Add(strTmp);
	strTmp = RTOS(pRec->fRad);
	strArrayVals.Add(strTmp);
	if(pRec->fRad > 0.0)
	{
		strTmp = RTOS(pRec->fTangentLength);
		strArrayVals.Add(strTmp);
		strTmp = RTOS(pRec->fXternalLength);
		strArrayVals.Add(strTmp);
		strTmp = RTOS(pRec->fCurveLen);
		strArrayVals.Add(strTmp);
		strTmp = RTOS(pRec->fDegOfCurve);
		strArrayVals.Add(strTmp);
		strTmp = RTOS(pRec->fDefAngle);
		strArrayVals.Add(strTmp);
		strTmp.Format("N=%s, E=%s", RTOS(pRec->ptPOI.y), RTOS(pRec->ptPOI.x));
		strArrayVals.Add(strTmp);
		strTmp.Format("N=%s, E=%s", RTOS(pRec->ptTC.y), RTOS(pRec->ptTC.x));
		strArrayVals.Add(strTmp);
		strTmp.Format("N=%s, E=%s", RTOS(pRec->ptCT.y), RTOS(pRec->ptCT.x));
		strArrayVals.Add(strTmp);
	}
	//
	if(strArrayLabels.GetSize() != strArrayVals.GetSize())
	{
		ads_printf("\nERROR: Failed to draw Curve data chart\n");
		return;
	}
	//
	fLabColWid = GetMaxLengthyStr(strArrayLabels) * fTextSize;
	fValColWid = GetMaxLengthyStr(strArrayVals) * fTextSize;

	//Draw Rows & Text
	ptTmpA = ptTmpB = ptTopLeft;
	ptTmpB.x += (fLabColWid + fValColWid);
	LAYER(strLyr, 1);
	for(i = 0; i < strArrayLabels.GetSize(); i++)
	{
		LINE(ptTmpA, ptTmpB, strLyr);
		//Labels
		ptTmpC.x = ptTopLeft.x + fTextSize * 0.1;//
		ptTmpC.y = ptTmpA.y - fRowHt / 2.0;
		ptTmpC.z = 0.0;
		TEXTLEFT(ptTmpC, strArrayLabels[i], 0.0, fTextSize, strLyr);
		//Vals
		ptTmpC.x = ptTopLeft.x + fLabColWid + fTextSize * 0.1;//
		ptTmpC.y = ptTmpA.y - fRowHt / 2.0;
		ptTmpC.z = 0.0;
		TEXTLEFT(ptTmpC, strArrayVals[i], 0.0, fTextSize, strLyr);

		ptTmpA.y -= fRowHt;
		ptTmpB.y -= fRowHt;
		if(i == (strArrayLabels.GetSize() - 1))
			LINE(ptTmpA, ptTmpB, strLyr);
	}
	//Draw Cols only
	ptTmpA = ptTopLeft;
	ptTmpB = ptTopLeft;
	ptTmpB.y -= (strArrayLabels.GetSize() * fRowHt);
	LINE(ptTmpA, ptTmpB, strLyr);

	ptTmpA.x = ptTmpB.x = ptTopLeft.x + fLabColWid;
	LINE(ptTmpA, ptTmpB, strLyr);

	ptTmpA.x = ptTmpB.x = ptTopLeft.x + fLabColWid + fValColWid;
	LINE(ptTmpA, ptTmpB, strLyr);
}
int CHAlignmentJig::GetMaxLengthyStr(const CStringArray& array)
{
	int i, iMaxLen;
	CString strTmp;

	for(i = 0, iMaxLen = -1; i < array.GetSize(); i++)
	{
		strTmp = array[i];
		if(strTmp.GetLength() > iMaxLen)
		{
			iMaxLen = strlen(array[i]);
		}
	}
	return iMaxLen;
}

void CHAlignmentJig::DeAllocateCurveDataRecArray(CCurveDetDataArray& arrayCurveDet)
{
	int i;
	for(i = 0; i < arrayCurveDet.GetSize(); i++)
	{
		CURVEDETDATA* pRec;
		pRec = arrayCurveDet[i];
		delete pRec;
	}
	if(arrayCurveDet.GetSize() > 0)
		arrayCurveDet.RemoveAll();
}