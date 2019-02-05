
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

#include "TINEntity.h"
#include "DrawUtil.h"
#include "Util.h"

static void CalculateZValue(const AcGePoint3d&, const AcGePoint3d&, AcGePoint3d&);
static double GetZValueOfPointWithinTrg(const AcGePoint3d&, const AcGePoint3d&, const AcGePoint3d&, const AcGePoint3d&);

static void PrintPt3DArray(const AcGePoint3dArray& Array, const char* pszPmt, CStringArray* pstrArrayRep = 0L)
{
	int i;
	CString strMsg;
	

	if(pstrArrayRep != 0L)
	{
		strMsg.Format("****** %s ******", pszPmt);
		pstrArrayRep->Add(strMsg);
	}
	else
	{
		strMsg.Format("\n****** %s ****** ", pszPmt);
		ads_printf(strMsg);
	}

	for(i = 0; i < Array.length(); i++)
	{
		if(pstrArrayRep != 0L)
		{
			strMsg.Format("%f %f %f (#%d/%d)", Array[i].x, Array[i].y, Array[i].z, i+1, Array.length());
			pstrArrayRep->Add(strMsg);
		}
		else
		{
			strMsg.Format("\n%f,%f,%f (#%d/%d)", Array[i].x, Array[i].y, Array[i].z, i+1, Array.length());
			ads_printf(strMsg);
		}
	}
	if(pstrArrayRep != 0L)
	{
		strMsg.Format("****** ****** ****** ******", pszPmt);
		pstrArrayRep->Add(strMsg);
	}
	else
	{
		strMsg.Format("\n****** ****** ****** ******\n", pszPmt);
		ads_printf(strMsg);
	}
}

// define a DXF class body for the entity using the
// appropriate ACRX boilerplate macro.  The DXF class name
// *MUST* be all uppercase.
//
// ...
ACRX_DXF_DEFINE_MEMBERS(TINEntity, AcDbEntity,
AcDbProxyEntity::kAllAllowedBits, TINEntity, 
"Contour\
|Product Desc:     Contour ARX Application\
|Company: Atanu Banik's\
|E-Mail : atanubanik@gmail.com");


TINEntity::TINEntity()
{
}
TINEntity::TINEntity(const AcGePoint3dArray& PtArray, const CTrianglePtrArray& TrgPtrArray)
{
    m_PointArray.append(PtArray);

	Adesk::Int32 i;
	TRIANGLE* pRec;
	TRIANGLE* pNewRec;
	for(i = 0; i < TrgPtrArray.GetSize(); i++)
	{
		pRec = TrgPtrArray[i];
		pNewRec = new TRIANGLE;
		*pNewRec = *pRec;
		m_TrgPtrArray.Add(pNewRec);
	}
}
TINEntity::~TINEntity()
{
	{////Reset Triangle data...
		int i;
		TRIANGLE* pRec;
		for(i = 0; i < m_TrgPtrArray.GetSize(); i++)
		{
			pRec = m_TrgPtrArray[i];
			delete pRec;
		}
	}
}
Adesk::Boolean TINEntity::worldDraw(AcGiWorldDraw* wd)
{
    int i, iSelMarker;
    
	for(i = 0, iSelMarker = 0; i < m_TrgPtrArray.GetSize(); i++)
	{
		TRIANGLE* pRec;

		pRec = m_TrgPtrArray[i];
		if((pRec->p1 >= m_PointArray.length()) || (pRec->p2 >= m_PointArray.length()) || (pRec->p3 >= m_PointArray.length()))
			return Adesk::kFalse;//ERROR

		{//Draw Line #1....
			AcGePoint3dArray PtArray;
			
			PtArray.append(m_PointArray.at(pRec->p1));
			PtArray.append(m_PointArray.at(pRec->p2));

			wd->subEntityTraits().setSelectionMarker(iSelMarker);
			wd->geometry().polyline(2, PtArray.asArrayPtr());
			iSelMarker++;
		}
		{//Draw Line #2....
			AcGePoint3dArray PtArray;
			
			PtArray.append(m_PointArray.at(pRec->p2));
			PtArray.append(m_PointArray.at(pRec->p3));

			wd->subEntityTraits().setSelectionMarker(iSelMarker);
			wd->geometry().polyline(2, PtArray.asArrayPtr());
			iSelMarker++;
		}
		{//Draw Line #3....
			AcGePoint3dArray PtArray;
			
			PtArray.append(m_PointArray.at(pRec->p3));
			PtArray.append(m_PointArray.at(pRec->p1));

			wd->subEntityTraits().setSelectionMarker(iSelMarker);
			wd->geometry().polyline(2, PtArray.asArrayPtr());
			iSelMarker++;
		}
	}
    return Adesk::kTrue;
}
Acad::ErrorStatus TINEntity::getOsnapPoints(AcDb::OsnapMode osMode, int gsSelectionMark, const AcGePoint3d& pickPoint, const AcGePoint3d&, const AcGeMatrix3d& viewXform, AcGePoint3dArray& snapPoints, AcDbIntArray&) const
{
	int iTrgIndex, iLineIndex;
	TRIANGLE* pRec;
	AcGePoint3d PtStart, PtEnd;

	iLineIndex = gsSelectionMark % 3; 
	iTrgIndex = (gsSelectionMark - iLineIndex) / 3; 

	pRec = m_TrgPtrArray[iTrgIndex];

	switch(iLineIndex)
	{
	case 0:
		PtStart = m_PointArray.at(pRec->p1);
		PtEnd = m_PointArray.at(pRec->p2);
		break;
	case 1:
		PtStart = m_PointArray.at(pRec->p2);
		PtEnd = m_PointArray.at(pRec->p3);
		break;
	case 2:
		PtStart = m_PointArray.at(pRec->p3);
		PtEnd = m_PointArray.at(pRec->p1);
		break;
	default:
		return Acad::eNotImplemented;
	}

	switch(osMode)
	{
	case AcDb::OsnapMode::kOsModeEnd:
		if(Dist2D(pickPoint, PtStart) < Dist2D(pickPoint, PtEnd))
		{
			snapPoints.append(PtStart);
		}
		else
		{
			snapPoints.append(PtEnd);
		}
		break;
	case AcDb::OsnapMode::kOsModeMid:
		{//Find Mid Point...
			double dDist, dAngle, dElev;
			
			dDist = Dist2D(PtStart, PtEnd);
			dAngle = Angle2D(PtStart, PtEnd);
			dElev = PtEnd.z - PtStart.z;
			ads_polar(asDblArray(PtStart), dAngle, (dDist / 2.0), asDblArray(PtEnd));
			PtEnd.z = PtStart.z + (dElev / 2.0);
			snapPoints.append(PtEnd);
		}
		break;
	case AcDb::OsnapMode::kOsModeNear:
		{
			AcGeLineSeg3d LineSeg(PtStart, PtEnd);
			AcGeVector3d viewDir(viewXform(Z, 0), viewXform(Z, 1), viewXform(Z, 2));

			PtEnd = LineSeg.projClosestPointTo(pickPoint, viewDir);
			snapPoints.append(PtEnd);
		}
		break;
	default:
		return Acad::eNotImplemented;
	}
    return Acad::eOk;
}
Acad::ErrorStatus TINEntity::intersectWith(const AcDbEntity* ent, AcDb::Intersect intType, AcGePoint3dArray& points, int thisGsMarker, int) const
{
	int iTrgIndex, iLineIndex;
	TRIANGLE* pRec;
	AcGePoint3d PtStart, PtEnd;

	intType = AcDb::kOnBothOperands;
	iLineIndex = thisGsMarker % 3; 
	iTrgIndex = (thisGsMarker - iLineIndex) / 3; 

	pRec = m_TrgPtrArray[iTrgIndex];
	switch(iLineIndex)
	{
	case 0:
		PtStart = m_PointArray.at(pRec->p1);
		PtEnd = m_PointArray.at(pRec->p2);
		break;
	case 1:
		PtStart = m_PointArray.at(pRec->p2);
		PtEnd = m_PointArray.at(pRec->p3);
		break;
	case 2:
		PtStart = m_PointArray.at(pRec->p3);
		PtEnd = m_PointArray.at(pRec->p1);
		break;
	default:
		return Acad::eNotImplemented;
	}
    if (ent->isKindOf(AcDbLine::desc()))
	{
		AcGePoint2d intPt;
		AcGeLineSeg2d LineSegThis(asPnt2d(asDblArray(PtStart)), asPnt2d(asDblArray(PtEnd)));
		AcGeLineSeg2d LineSegOther(asPnt2d(asDblArray(AcDbLine::cast(ent)->startPoint())), asPnt2d(asDblArray(AcDbLine::cast(ent)->endPoint())));
    
		if(Adesk::kTrue == LineSegThis.intersectWith (LineSegOther, intPt))
		{
			AcGePoint3d Pt(intPt.x, intPt.y, 0.0);
			double dDistA, dDistB, dElev;
			
			dDistA = Dist2D(PtStart, PtEnd);
			dDistB = Dist2D(PtStart, Pt);
			dElev = PtEnd.z - PtStart.z;
			
			Pt.z = PtStart.z + dDistB * (dElev /dDistA);
			points.append(Pt);
			return Acad::eOk;
		}
	}
    return Acad::eNotApplicable;
}
Acad::ErrorStatus TINEntity::dwgInFields(AcDbDwgFiler* filer)
{
    assertWriteEnabled();

    AcDbEntity::dwgInFields(filer);

	Adesk::Int32 i, lCount;

	{//Reset
		//Point data...
		if(m_PointArray.length() > 0)
			m_PointArray.removeSubArray(0, m_PointArray.length() - 1);

		{//Triangle data...
			int i;
			TRIANGLE* pRec;
			for(i = 0; i < m_TrgPtrArray.GetSize(); i++)
			{
				pRec = m_TrgPtrArray[i];
				delete pRec;
			}
			if(m_TrgPtrArray.GetSize() > 0)
				m_TrgPtrArray.RemoveAll();
		}
	}
	{//Read
		//all the Points.......
		filer->readInt32(&lCount);
		for(i = 0; i < lCount; i++)
		{
			AcGePoint3d PtTmp;
			
			filer->readPoint3d(&PtTmp);
			m_PointArray.append(PtTmp);
		}
		//all the Triangles.......
		filer->readInt32(&lCount);
		for(i = 0; i < lCount; i++)
		{
			TRIANGLE Rec;
			TRIANGLE* pRec;
			
			filer->readInt32(&(Rec.p1));
			filer->readInt32(&(Rec.p2));
			filer->readInt32(&(Rec.p3));
			
			pRec = new TRIANGLE;
			*pRec = Rec;
			m_TrgPtrArray.Add(pRec);
		}
	}
    return filer->filerStatus();
}

Acad::ErrorStatus TINEntity::dwgOutFields(AcDbDwgFiler* filer) const
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
	//Write all the Triangles.......
	lCount = m_TrgPtrArray.GetSize();
	filer->writeInt32(lCount);
	for(i = 0; i < lCount; i++)
	{
		TRIANGLE* pRec;
		
		pRec = m_TrgPtrArray[i];
		
		filer->writeInt32(pRec->p1);
		filer->writeInt32(pRec->p2);
		filer->writeInt32(pRec->p3);
	}
	return filer->filerStatus();
}
Acad::ErrorStatus TINEntity::audit(AcDbAuditInfo*)
{
	return Acad::eOk;
}

// For explode,  create the objects that will be the result of the
// explode,  append them to the "entities" array,  then return.
// AutoCAD will take care of the rest.
//
Acad::ErrorStatus TINEntity::explode(AcDbVoidPtrArray& entities) const
{
    assertReadEnabled();
    int i;
    
	for(i = 0; i < m_TrgPtrArray.GetSize(); i++)
	{
		TRIANGLE* pRec;

		pRec = m_TrgPtrArray[i];
		if((pRec->p1 >= m_PointArray.length()) || (pRec->p2 >= m_PointArray.length()) || (pRec->p3 >= m_PointArray.length()))
			return Acad::eAmbiguousOutput;//ERROR

		AcDbFace* pEntity = new AcDbFace(m_PointArray.at(pRec->p1), m_PointArray.at(pRec->p2), m_PointArray.at(pRec->p3));
		entities.append(pEntity);
	}
    return Acad::eOk;
}

void TINEntity::list() const
{
    assertReadEnabled();

    AcDbEntity::list();

	double fVolume, fArea;
	if(!GetProperty(fVolume, fArea))
	{
	    ads_printf("\nERROR : Volume & area calculation failed\n");
	    return;
	}
	ads_printf("\nGenerated by ContourARX Application, Don't try to edit this entity\n\
				  Total no. of Points = %d\n\
		          Total no. of Triangles = %d\n\
				  Area = %f\n\
				  Volume = %f\n",
				  m_PointArray.length(), m_TrgPtrArray.GetSize(), fArea, fVolume
			  );
}
// This function is called whenever a saveasr12 command is executed
// in AutoCAD or a DWG save is occurring that requires proxy graphics
// to be saved.  Overriding this function allows control of the graphics
// that will be stored in these two cases.
//
void TINEntity::saveAs(AcGiWorldDraw* wd, AcDb::SaveType saveType)
{
    int i, iSelMarker;
    
	for(i = 0, iSelMarker = 0; i < m_TrgPtrArray.GetSize(); i++)
	{
		TRIANGLE* pRec;

		pRec = m_TrgPtrArray[i];
		if((pRec->p1 >= m_PointArray.length()) || (pRec->p2 >= m_PointArray.length()) || (pRec->p3 >= m_PointArray.length()))
			return;//ERROR

		{//Draw Line #1....
			AcGePoint3dArray PtArray;
			
			PtArray.append(m_PointArray.at(pRec->p1));
			PtArray.append(m_PointArray.at(pRec->p2));

			wd->subEntityTraits().setSelectionMarker(iSelMarker);
			wd->geometry().polyline(2, PtArray.asArrayPtr());
			iSelMarker++;
		}
		{//Draw Line #2....
			AcGePoint3dArray PtArray;
			
			PtArray.append(m_PointArray.at(pRec->p2));
			PtArray.append(m_PointArray.at(pRec->p3));

			wd->subEntityTraits().setSelectionMarker(iSelMarker);
			wd->geometry().polyline(2, PtArray.asArrayPtr());
			iSelMarker++;
		}
		{//Draw Line #3....
			AcGePoint3dArray PtArray;
			
			PtArray.append(m_PointArray.at(pRec->p3));
			PtArray.append(m_PointArray.at(pRec->p1));

			wd->subEntityTraits().setSelectionMarker(iSelMarker);
			wd->geometry().polyline(2, PtArray.asArrayPtr());
			iSelMarker++;
		}
	}
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

BOOL TINEntity::GetIntersectionPts(const AcGePoint3dArray& ContPtsArray, const CTrianglePtrArray& TrgArray, const AcGePoint3d& PtParam01, const AcGePoint3d& PtParam02, AcGePoint3dArray& PtArray)
{
    int i;
    
	for(i = 0; i < TrgArray.GetSize(); i++)
	{
		TRIANGLE* pRec;

		pRec = TrgArray[i];
		if((pRec->p1 >= ContPtsArray.length()) || (pRec->p2 >= ContPtsArray.length()) || (pRec->p3 >= ContPtsArray.length()))
			return FALSE;//ERROR

		if(ads_usrbrk() == 1)
		{
			ads_printf("\nUser break\n");
			return FALSE;
		}

		const AcGePoint3d& Pt01 = ContPtsArray[pRec->p1];
		const AcGePoint3d& Pt02 = ContPtsArray[pRec->p2];
		const AcGePoint3d& Pt03 = ContPtsArray[pRec->p3];
		AcGePoint3d PtResult;

		//First line.....
		if(GetIntersPt2D(Pt01, Pt02, PtParam01, PtParam02, PtResult))
		{
			CalculateZValue(Pt01, Pt02, PtResult);
			if(PtArray.contains(PtResult) != Adesk::kTrue)
				PtArray.append(PtResult);
		}
		else
		{//Chk if the line is parallel & coinsides 
			//if(IsLinesColinear(Pt01, Pt02, PtParam01, PtParam02))
			//{
			//	if(PtArray.contains(Pt01) != Adesk::kTrue)
			//		PtArray.append(Pt01);
			//	if(PtArray.contains(Pt02) != Adesk::kTrue)
			//		PtArray.append(Pt02);
			//}
		}
		
		//Second line.....
		if(GetIntersPt2D(Pt02, Pt03, PtParam01, PtParam02, PtResult))
		{
			CalculateZValue(Pt02, Pt03, PtResult);
			if(PtArray.contains(PtResult) != Adesk::kTrue)
				PtArray.append(PtResult);
		}
		else
		{//Chk if the line is parallel & coinsides 
			//if(IsLinesColinear(Pt02, Pt03, PtParam01, PtParam02))
			//{
			//	if(PtArray.contains(Pt02) != Adesk::kTrue)
			//		PtArray.append(Pt02);
			//	if(PtArray.contains(Pt03) != Adesk::kTrue)
			//		PtArray.append(Pt03);
			//}
		}

		//Third line.....
		if(GetIntersPt2D(Pt03, Pt01, PtParam01, PtParam02, PtResult))
		{
			CalculateZValue(Pt03, Pt01, PtResult);
			if(PtArray.contains(PtResult) != Adesk::kTrue)
				PtArray.append(PtResult);
		}
		else
		{//Chk if the line is parallel & coinsides 
			//if(IsLinesColinear(Pt03, Pt01, PtParam01, PtParam02))
			//{
			//	if(PtArray.contains(Pt03) != Adesk::kTrue)
			//		PtArray.append(Pt03);
			//	if(PtArray.contains(Pt01) != Adesk::kTrue)
			//		PtArray.append(Pt01);
			//}
		}
	}
	return (PtArray.length() > 0);
}
void TINEntity::GetExtents(const AcGePoint3dArray& ContPtsArray, double& fMinX, double& fMinY, double& fMaxX, double& fMaxY, const double& fROffOrdX/*= 0.0*/, const double& fROffOrdY/*= 0.0*/)
{
    int i;

	fMinX = fMaxX = fMinY = fMaxY = 0.0;
	if(ContPtsArray.length() <= 0)
		return;
	
	fMinX = fMaxX = ContPtsArray[0].x;
	fMinY = fMaxY = ContPtsArray[0].y;

	for(i = 1; i < ContPtsArray.length(); i++)
	{
		const AcGePoint3d& Point = ContPtsArray[i];
		
		if(Point.x < fMinX)
			fMinX = Point.x;
		if(Point.x > fMaxX)
			fMaxX = Point.x;

		if(Point.y < fMinY)
			fMinY = Point.y;
		if(Point.y > fMaxY)
			fMaxY = Point.y;
	}
	if(fROffOrdX != 0.0)
	{
		double fRem, fInteger;

		fRem = fmod(fMinX, fROffOrdX);
		fInteger = (fMinX - fRem) / fROffOrdX;
		fMinX = (fInteger + 1) * fROffOrdX;
	}
	if(fROffOrdY != 0.0)
	{
		double fRem, fInteger;

		fRem = fmod(fMinY, fROffOrdY);
		fInteger = (fMinY - fRem) / fROffOrdY;
		fMinY = (fInteger + 1) * fROffOrdY;
	}
}
BOOL TINEntity::GetProperty(double& fVolume, double& fArea) const
{
    assertReadEnabled();
    int i;

	fArea = fVolume = 0.0;
	if(m_PointArray.length() <= 0)
		return FALSE;//ERROR
	
	for(i = 0; i < m_TrgPtrArray.GetSize(); i++)
	{
		TRIANGLE* pRec;
		double fAreaTmp, fVolTmp;

		pRec = m_TrgPtrArray[i];
		if((pRec->p1 >= m_PointArray.length()) || (pRec->p2 >= m_PointArray.length()) || (pRec->p3 >= m_PointArray.length()))
			return FALSE;//ERROR

		const AcGePoint3d& Pt01 = m_PointArray[pRec->p1];
		const AcGePoint3d& Pt02 = m_PointArray[pRec->p2];
		const AcGePoint3d& Pt03 = m_PointArray[pRec->p3];

		if(!CalculateTrgArea(Pt01, Pt02, Pt03, fAreaTmp))
			return FALSE;//ERROR
		fArea += fAreaTmp;
		
		if(!CalculateTrgVolume(Pt01, Pt02, Pt03, fVolTmp))
			return FALSE;//ERROR
		fVolume += fVolTmp;
	}
	return TRUE;
}
BOOL TINEntity::IsPointWithinBoundary(const AcGePoint3dArray& ContPtsArray, const CTrianglePtrArray& TrgArray, const AcGePoint3d& PtParam, double* pfZValue/* = 0L*/)
{
    int i;

	if(ContPtsArray.length() <= 0)
		return FALSE;//ERROR
	
	for(i = 0; i < TrgArray.GetSize(); i++)
	{
		TRIANGLE* pRec;

		pRec = TrgArray[i];
		if((pRec->p1 >= ContPtsArray.length()) || (pRec->p2 >= ContPtsArray.length()) || (pRec->p3 >= ContPtsArray.length()))
			return FALSE;//ERROR

		const AcGePoint3d& Pt01 = ContPtsArray[pRec->p1];
		const AcGePoint3d& Pt02 = ContPtsArray[pRec->p2];
		const AcGePoint3d& Pt03 = ContPtsArray[pRec->p3];

		if(IsPointWithinTrg(PtParam, Pt01, Pt02, Pt03))
		{
			if(pfZValue != 0L)
			{//Calculate Z-Value...
				if((Pt01.x == PtParam.x) && (Pt01.y == PtParam.y))
				{
					*pfZValue = Pt01.z;
					return TRUE;
				}
				if((Pt02.x == PtParam.x) && (Pt02.y == PtParam.y))
				{
					*pfZValue = Pt02.z;
					return TRUE;
				}
				if((Pt03.x == PtParam.x) && (Pt03.y == PtParam.y))
				{
					*pfZValue = Pt03.z;
					return TRUE;
				}

				*pfZValue = GetZValueOfPointWithinTrg(PtParam, Pt01, Pt02, Pt03) + GetZValueOfPointWithinTrg(PtParam, Pt02, Pt03, Pt01) + GetZValueOfPointWithinTrg(PtParam, Pt03, Pt02, Pt01);
				*pfZValue = *pfZValue / 3.0;
			}
			return TRUE;
		}
		else
		{//It can be on a edge....
		}
	}
	return FALSE;
}

BOOL TINEntity::CalculateVolume(const AcGePoint3dArray& ContPtsArray, const CTrianglePtrArray& TrgArray, const AcGePoint3dArray& PtArray, const double& fArea, const double& fFormationLevel, double& fVolm, CStringArray* pstrArrayRep/* = 0L*/)
{
	double fZValue, fZValueCum;
	int j, i;
	AcGePoint3dArray VertexArray, ArrayAllPts;
	CString strMsg;

	VertexArray.append(PtArray);

	fVolm = 0.0;
	for(i = 0; i < VertexArray.length(); i++)
	{
		if(!IsPointWithinBoundary(ContPtsArray, TrgArray, VertexArray[i], &fZValue))
		{
			if(pstrArrayRep != 0L)
			{
				strMsg.Format("\nERROR: Vertex #%d <x = %f, y = %f, z = %f> is not within TINEntity boundary\nVolume calculation failed\n", i+1, VertexArray[i].x, VertexArray[i].y, VertexArray[i].z);
				ads_printf(strMsg);
				strMsg.Format("ERROR: Vertex #%d <x = %f, y = %f, z = %f> is not within TINEntity boundary...Volume calculation failed", i+1, VertexArray[i].x, VertexArray[i].y, VertexArray[i].z);
				pstrArrayRep->Add(strMsg);
			}
			return FALSE;
		}
		else
		{
			VertexArray[i].z = fZValue;
		}
	}
	//All vertexes are within TINEntity boundary .. so calculation is possible;
	for(i = 1; i < VertexArray.length(); i++)
	{
		AcGePoint3dArray PtArrayTmp;

		if(GetIntersectionPts(ContPtsArray, TrgArray, VertexArray[i - 1], VertexArray[i], PtArrayTmp))
		{
			SortPointsOnLine(VertexArray[i - 1], PtArrayTmp);
		}
		else
		{
			if(pstrArrayRep != 0L)
			{
				strMsg.Format("No Intersection point found in between vertex [%d - %d]", i - 1 + 1, i + 1);
				pstrArrayRep->Add(strMsg);
			}
		}
		if(Adesk::kTrue != PtArrayTmp.contains(VertexArray[i - 1]))
		{//add the start point at the begining
			PtArrayTmp.insertAt(0, VertexArray[i - 1]);
		}
		//Store ....
		for(j = 0; j < PtArrayTmp.length(); j++)
		{
			if(Adesk::kTrue != ArrayAllPts.contains(PtArrayTmp[j]))
			{
				ArrayAllPts.append(PtArrayTmp[j]);
			}
		}
		if(pstrArrayRep != 0L)
		{
			if(PtArrayTmp.length() > 0)
			{
				strMsg.Format("%d# of Intersection point(s) found in between vertex [%d - %d]", PtArrayTmp.length(), i - 1 + 1, i + 1);
				PrintPt3DArray(PtArrayTmp, strMsg, pstrArrayRep);
			}
		}
	}
	//Get all the points within boundary....
	for(i = 0; i < ContPtsArray.length(); i++)
	{
		if(::IsPointWithinBoundary(ContPtsArray[i], PtArray))
		{	
			//Store ....
			if(Adesk::kTrue != ArrayAllPts.contains(ContPtsArray[i]))
			{
				ArrayAllPts.append(ContPtsArray[i]);
			}
		}
	}
	if(pstrArrayRep != 0L)
	{
		if(ArrayAllPts.length() > 0)
		{
			PrintPt3DArray(ArrayAllPts, "Following are the points considered in volume calculation", pstrArrayRep);
		}
	}

	//Calculate Volume now.....
	for(i = 0, fZValueCum= 0.0; i < ArrayAllPts.length(); i++)
	{
		fZValueCum += ArrayAllPts[i].z;
	}
	fZValueCum = fZValueCum / ArrayAllPts.length();
	fVolm = fArea * fZValueCum;
	
	//Show Output....
	ads_printf("\n********************************\n");
	ads_printf("\nVolume calculation detail ......");
	ads_printf("\nTotal %d #of points considered in volume calculation", ArrayAllPts.length());
	ads_printf("\nArea = %f\tAverage Z-Value = %f\nVolume = %f", fArea, fZValueCum, fVolm);
	if(fFormationLevel > fZValueCum)
	{//Filling
		ads_printf("\nConsidering Formation Level %f, the Total Filling = %f", fFormationLevel, (fFormationLevel - fZValueCum) * fArea);
	}
	else
	{//Cutting
		ads_printf("\nConsidering Formation Level %f, the Total Cutting = %f", fFormationLevel, (fZValueCum - fFormationLevel) * fArea);
	}
	ads_printf("\n********************************\n");
	if(pstrArrayRep != 0L)
	{
		strMsg.Format("********************************");	pstrArrayRep->Add(strMsg);
		strMsg.Format("*Volume calculation detail ......");	pstrArrayRep->Add(strMsg);
		strMsg.Format("*Total %d #of points considered in volume calculation", ArrayAllPts.length());	pstrArrayRep->Add(strMsg);
		strMsg.Format("*Area = %f Average Z-Value = %f Volume = %f", fArea, fZValueCum, fVolm);	pstrArrayRep->Add(strMsg);
		if(fFormationLevel > fZValueCum)
		{//Filling
			strMsg.Format("Considering Formation Level %f, the Total Filling = %f", fFormationLevel, (fFormationLevel - fZValueCum) * fArea);	pstrArrayRep->Add(strMsg);
		}
		else
		{//Cutting
			strMsg.Format("Considering Formation Level %f, the Total Cutting = %f", fFormationLevel, (fZValueCum - fFormationLevel) * fArea);	pstrArrayRep->Add(strMsg);
		}
		strMsg.Format("********************************");	pstrArrayRep->Add(strMsg);
	}
	return TRUE;
}
static double GetZValueOfPointWithinTrg(const AcGePoint3d& Pt, const AcGePoint3d& Pt01, const AcGePoint3d& Pt02, const AcGePoint3d& Pt03)
{
	AcGePoint3d ptResult;
	double fZValue;

	if(!GetIntersPt2D(Pt01, Pt, Pt02, Pt03, ptResult, TRUE))
	{
		return 0.0;
	}
	//Interpolate (Pt02 & Pt03) & Get the Z-Value of 'ptResult'
	if(Interpolate(&Pt02, &Pt03, &ptResult, fZValue) != 1)
	{
		ads_printf("\nERROR: Interpolation failed!!!!!\n");
		return 0.0;
	}
	ptResult.z = fZValue;
	if(Interpolate(&Pt01, &ptResult, &Pt, fZValue) != 1)
	{
		ads_printf("\nERROR: Interpolation failed!!!!!\n");
		return 0.0;
	}
	return fZValue;
}

static void CalculateZValue(const AcGePoint3d& PtParam01, const AcGePoint3d& PtParam02, AcGePoint3d& PtParamInBetween)
{
	double dElev, dDist, dTmp;
	
	dDist = Dist2D(PtParam01, PtParam02);
	if(dDist == 0.0)
	{

		ads_printf("\nFATAL ERROR: in 'CalculateZValue'\n");
		return;
	}
	dTmp = Dist2D(PtParam01, PtParamInBetween);
	
	dElev = PtParam02.z - PtParam01.z;
	PtParamInBetween.z = PtParam01.z + dTmp * (dElev / dDist);
}

