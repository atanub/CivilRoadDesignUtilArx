
//NEAnnotationEntity.cpp
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
#include "NEAnnotationEntity.h"
#include "Util.h"
#include "NEEntPropDlg.h"



// define a DXF class body for the entity using the
// appropriate ACRX boilerplate macro.  The DXF class name
// *MUST* be all uppercase.
//
// ...
ACRX_DXF_DEFINE_MEMBERS(NEAnnotationEntity, AcDbEntity,
AcDbProxyEntity::kAllAllowedBits, NEAnnotationEntity, 
"Contour\
|Product Desc: Contour ARX Application\
|Company: Atanu Banik's\
|E-Mail : atanubanik@gmail.com");

const CString	NEAnnotationEntity::m_strAnnFmtNorth = CString("N %s");
const CString	NEAnnotationEntity::m_strAnnFmtEast = CString("E %s");
const CString	NEAnnotationEntity::m_strAnnFmtSouth = CString("S %s");
const CString	NEAnnotationEntity::m_strAnnFmtWest = CString("W %s");

AcGePoint3d	NEAnnotationEntity::m_TempVertex = AcGePoint3d(0.0, 0.0, 0.0);
BOOL		NEAnnotationEntity::m_bDragModeOn = FALSE;

MAKE_ACDBOPENOBJECT_FUNCTION(NEAnnotationEntity);

NEAnnotationEntity::NEAnnotationEntity()
{
	m_fArrSize = m_fTxtSize = 0.0;
	m_iLUPREC = LUPREC();
}
NEAnnotationEntity::NEAnnotationEntity(const AcGeVector3d& Normal, const AcGePoint3dArray& PtArray, const AcDbObjectId& ObjIDTxtStyle, const double& fTxtSize, const double& fArrSize, const short& iLuPrec)
: m_DbObjIDTxtStyle(ObjIDTxtStyle), m_Normal(Normal)
{
    m_PointArray.append(PtArray);
	m_fTxtSize = fTxtSize;
	m_fArrSize = fArrSize;
	m_iLUPREC = iLuPrec;
}
NEAnnotationEntity::NEAnnotationEntity(const AcGeVector3d& Normal, const AcGePoint3d& Pt, const AcDbObjectId& ObjIDTxtStyle, const double& fTxtSize, const double& fArrSize, const short& iLuPrec)
: m_DbObjIDTxtStyle(ObjIDTxtStyle), m_Normal(Normal)
{
    m_PointArray.append(Pt);
	m_fTxtSize = fTxtSize;
	m_fArrSize = fArrSize;
	m_iLUPREC = iLuPrec;
}
NEAnnotationEntity::~NEAnnotationEntity()
{
}
void NEAnnotationEntity::MakeStrings(const AcGePoint3d& Pt, CString& strUpDn, CString& strLeftRt, const short& iLUPREC)
{
	char szTxt[_MAX_PATH];

	ads_rtos(fabs(Pt.y), 2, iLUPREC, szTxt);
	if(Pt.y >= 0.0)
	{//Northing
		strUpDn.Format(m_strAnnFmtNorth, szTxt);
	}
	else
	{//Southing
		strUpDn.Format(m_strAnnFmtSouth, szTxt);
	}
	ads_rtos(fabs(Pt.x), 2, iLUPREC, szTxt);
	if(Pt.x >= 0.0)
	{//Easting
		strLeftRt.Format(m_strAnnFmtEast, szTxt);
	}
	else
	{//Westing
		strLeftRt.Format(m_strAnnFmtWest, szTxt);
	}
}
BOOL NEAnnotationEntity::IsTxtOnLHS(const AcGePoint3d& PtLast, const AcGePoint3d& PtPrev)
{
	double dAngle;
	dAngle = Angle2D(PtPrev, PtLast);
	return ((dAngle > (PI / 2.0)) && (dAngle < (3.0 * PI / 2.0)));
}
void NEAnnotationEntity::DrawText(AcGiWorldDraw* wd, const AcGePoint3d& PtFirst, const AcGePoint3d& PtLast, const AcGePoint3d& PtPrev, short& iSubEntTrait, const AcDbHardPointerId& ObjIDTxtStyle, AcDbVoidPtrArray* pEntArray /*= 0L*/) const
{
	const double fTxtWidMF = 1.0;
	CString strUpDn, strLeftRt;
	int iMaxLenOfStr;

	ads_point ptTxtIns, ptXLineStart, ptTxtInsUp, ptTxtInsDn;
    AcGeVector3d dirVec;
	AcGiTextStyle Style;
	
	MakeStrings(PtFirst, strUpDn, strLeftRt, m_iLUPREC);
	iMaxLenOfStr = strlen(strUpDn);
	if(iMaxLenOfStr < (int)strlen(strLeftRt))
		iMaxLenOfStr = strlen(strLeftRt);

	if(!IsTxtOnLHS(PtLast, PtPrev))
	{
		ads_polar(asDblArray(PtLast), 0.0, m_fTxtSize, ptXLineStart);
		ptTxtIns[X] = ptXLineStart[X]; ptTxtIns[Y] = ptXLineStart[Y]; ptTxtIns[Z] = ptXLineStart[Z];
	}
	else
	{
		ads_polar(asDblArray(PtLast), PI, m_fTxtSize, ptXLineStart);
		ads_polar(ptXLineStart, PI, m_fTxtSize * fTxtWidMF * iMaxLenOfStr, ptTxtIns);
	}
	ads_polar(ptTxtIns, (1.0 * PI / 2.0), +m_fTxtSize * 0.5, ptTxtInsUp);
	ads_polar(ptTxtIns, (1.0 * PI / 2.0), -m_fTxtSize * 1.5, ptTxtInsDn);

	//Draw ....
	if(pEntArray == 0L)
	{
		if (ObjIDTxtStyle != NULL)
		{
			dirVec = m_Normal.perpVector();
			dirVec = dirVec.rotateBy(0.0, m_Normal);
			
			if(GetAcGiTextStyle(Style, ObjIDTxtStyle) != Acad::eOk)
			{
				//ObjIDTxtStyle = AcDbObjectId::kNull;
				iSubEntTrait++;
				wd->subEntityTraits().setSelectionMarker(iSubEntTrait);
				wd->geometry().text(asPnt3d(ptTxtInsUp), m_Normal, dirVec, m_fTxtSize, 1, 0, strUpDn);
				iSubEntTrait++;
				wd->subEntityTraits().setSelectionMarker(iSubEntTrait);
				wd->geometry().text(asPnt3d(ptTxtIns), m_Normal, dirVec, m_fTxtSize, 1, 0, strLeftRt);
			}
			else
			{
				Style.setTextSize(m_fTxtSize);
				iSubEntTrait++;
				wd->subEntityTraits().setSelectionMarker(iSubEntTrait);
				wd->geometry().text(asPnt3d(ptTxtInsUp), m_Normal, dirVec, strUpDn, -1, 0, Style);
				iSubEntTrait++;
				wd->subEntityTraits().setSelectionMarker(iSubEntTrait);
				wd->geometry().text(asPnt3d(ptTxtInsDn), m_Normal, dirVec, strLeftRt, -1, 0, Style);
			}
		}
		{//
			AcGePoint3dArray ArrayTmp;
			
			ArrayTmp.append(PtLast);
			ArrayTmp.append(asPnt3d(ptXLineStart));
			iSubEntTrait++;
			wd->subEntityTraits().setSelectionMarker(iSubEntTrait);
			wd->geometry().polyline(ArrayTmp.length(), ArrayTmp.asArrayPtr());
		}
	}
	else
	{
		dirVec = m_Normal.perpVector();
		dirVec = dirVec.rotateBy(0.0, m_Normal);
		if(m_DbObjIDTxtStyle)
		{
			AcDbText* pEnt;
			AcGiTextStyle Style;

			if(GetAcGiTextStyle(Style, m_DbObjIDTxtStyle) != Acad::eOk)
			{
				AcDbObjectId ObjIDTxtStyleTmp;
			
				if(TEXTSTYLE(ObjIDTxtStyleTmp))
				{
					GetAcGiTextStyle(Style, ObjIDTxtStyleTmp);
				}
			}
			Style.setTextSize(m_fTxtSize);

			pEnt = new AcDbText;
			pEnt->setNormal(m_Normal);
			pEnt->setHeight(m_fTxtSize);
			pEnt->setRotation(0.0);
			pEnt->setTextString(strUpDn);
			pEnt->setPosition(asPnt3d(ptTxtInsUp));
			pEnt->setTextStyle(m_DbObjIDTxtStyle);
			pEntArray->append(pEnt);

			pEnt = new AcDbText;
			pEnt->setNormal(m_Normal);
			pEnt->setHeight(m_fTxtSize);
			pEnt->setRotation(0.0);
			pEnt->setTextString(strLeftRt);
			pEnt->setPosition(asPnt3d(ptTxtInsDn));
			pEnt->setTextStyle(m_DbObjIDTxtStyle);
			pEntArray->append(pEnt);
		}
		{//
			AcDbLine* pEnt;

			pEnt = new AcDbLine;
			pEnt->setNormal(m_Normal);
			pEnt->setStartPoint(PtLast);
			pEnt->setEndPoint(asPnt3d(ptXLineStart));
		
			pEntArray->append(pEnt);
		}
	}
}
void NEAnnotationEntity::DrawArrowHead(AcGiWorldDraw* wd, const AcGePoint3d& PtFirst, const AcGePoint3d& PtNext, const double& fArrSize, short& iSubEntTrait, AcDbVoidPtrArray* pEntArray /*= 0L*/) const
{
	double dAngle;
	ads_point ptUp, ptDn;
	AcGePoint3dArray ArrayTmp;

	dAngle = Angle2D(PtFirst, PtNext);

	ads_polar(asDblArray(PtFirst), dAngle + DTOR(15.0), fArrSize * 1.0, ptUp);
	ads_polar(asDblArray(PtFirst), dAngle - DTOR(15.0), fArrSize * 1.0, ptDn);

	
	ArrayTmp.append(PtFirst);
	ArrayTmp.append(asPnt3d(ptUp));
	ArrayTmp.append(asPnt3d(ptDn));
	
	if(pEntArray == 0L)
	{//

		iSubEntTrait++;
		wd->subEntityTraits().setSelectionMarker(iSubEntTrait);
		wd->subEntityTraits().setFillType(kAcGiFillAlways);
		wd->geometry().polygon(ArrayTmp.length(), ArrayTmp.asArrayPtr());
	}
	else
	{
		int i;
		
		for(i = 1; i < ArrayTmp.length(); i++)
		{
			AcDbSolid* pEnt;

			pEnt = new AcDbSolid(PtFirst, asPnt3d(ptUp), asPnt3d(ptDn));
			pEnt->setNormal(m_Normal);
			pEntArray->append(pEnt);
		}

	}
}
Adesk::Boolean NEAnnotationEntity::worldDraw(AcGiWorldDraw* wd)
{
	short iSubEntTrait;
	int i;
	{
		for (i = iSubEntTrait = 1; i < m_PointArray.length(); iSubEntTrait++, i++)
		{
			AcGePoint3dArray ArrayTmp;
			
			ArrayTmp.append(m_PointArray[i - 1]);
			ArrayTmp.append(m_PointArray[i - 0]);
			
			wd->subEntityTraits().setSelectionMarker(iSubEntTrait);
			wd->geometry().polyline(2, ArrayTmp.asArrayPtr());
		}
	}

	if(m_PointArray.length() > 1)
	{
		if(!NEAnnotationEntity::m_bDragModeOn)
			DrawText(wd, m_PointArray.first(), m_PointArray.last(), m_PointArray[m_PointArray.length() - 2], iSubEntTrait, m_DbObjIDTxtStyle);
		else
			DrawText(wd, m_PointArray.first(), NEAnnotationEntity::m_TempVertex, m_PointArray.last(), iSubEntTrait, m_DbObjIDTxtStyle);

		DrawArrowHead(wd, m_PointArray.first(), m_PointArray[1], m_fArrSize, iSubEntTrait);
	}
    return Adesk::kTrue;
}
void NEAnnotationEntity::saveAs(AcGiWorldDraw* wd, AcDb::SaveType saveType)
{
	short iSubEntTrait;
	int i;
	{
		for (i = iSubEntTrait = 1; i < m_PointArray.length(); iSubEntTrait++, i++)
		{
			AcGePoint3dArray ArrayTmp;
			
			ArrayTmp.append(m_PointArray[i - 1]);
			ArrayTmp.append(m_PointArray[i - 0]);
			
			wd->subEntityTraits().setSelectionMarker(iSubEntTrait);
			wd->geometry().polyline(2, ArrayTmp.asArrayPtr());
		}
	}

	if(m_PointArray.length() > 1)
	{
		DrawText(wd, m_PointArray.first(), m_PointArray.last(), m_PointArray[m_PointArray.length() - 2], iSubEntTrait, m_DbObjIDTxtStyle);
		DrawArrowHead(wd, m_PointArray.first(), m_PointArray[1], m_fArrSize, iSubEntTrait);
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
Acad::ErrorStatus NEAnnotationEntity::dwgInFields(AcDbDwgFiler* filer)
{
    assertWriteEnabled();

    AcDbEntity::dwgInFields(filer);

	Adesk::Int32 i, lCount;

	{//Reset
		//Point data...
		if(m_PointArray.length() > 0)
			m_PointArray.removeSubArray(0, m_PointArray.length() - 1);
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
	}
	//
	filer->readDouble(&m_fTxtSize);
	filer->readDouble(&m_fArrSize);
	filer->readItem(&m_iLUPREC);
	filer->readItem(&m_Normal);
	filer->readItem(&m_DbObjIDTxtStyle);
	
    return filer->filerStatus();
}

Acad::ErrorStatus NEAnnotationEntity::dwgOutFields(AcDbDwgFiler* filer) const
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
	filer->writeDouble(m_fTxtSize);
	filer->writeDouble(m_fArrSize);
	filer->writeItem(m_iLUPREC);
	filer->writeItem(m_Normal);
	filer->writeItem(m_DbObjIDTxtStyle);
	return filer->filerStatus();
}
Acad::ErrorStatus NEAnnotationEntity::audit(AcDbAuditInfo* auditor)
{
	return Acad::eOk;
}
Acad::ErrorStatus NEAnnotationEntity::explode(AcDbVoidPtrArray& entities) const
{
    assertReadEnabled();
	
	{//Make Entity first...
		int i;
		
		for(i = 1; i < m_PointArray.length(); i++)
		{
			AcDbLine* pEnt;

			pEnt = new AcDbLine;
			pEnt->setNormal(m_Normal);
			pEnt->setStartPoint(m_PointArray[i - 1]);
			pEnt->setEndPoint(m_PointArray[i - 0]);
		
			entities.append(pEnt);
		}
	}
	short iDummy = -1;
	DrawText(0L, m_PointArray.first(), m_PointArray.last(), m_PointArray[m_PointArray.length() - 2], iDummy, m_DbObjIDTxtStyle, &entities);
	DrawArrowHead(0L, m_PointArray.first(), m_PointArray[1], m_fArrSize, iDummy, &entities);

    return Acad::eOk;
}

void NEAnnotationEntity::list() const
{
    assertReadEnabled();
	
	ads_printf("\nSurveyed Point Annotator");
	ads_printf("\nText Size  = %f", m_fTxtSize);
	ads_printf("\nArrow Size = %f", m_fArrSize);
	ads_printf("\nLinear Unit Precision = %d", m_iLUPREC);

    AcDbEntity::list();
}
BOOL NEAnnotationEntity::AddVertex(const AcGePoint3d& Pt)
{
	int iIndex;
	if(m_PointArray.find(Pt, iIndex) == Adesk::kTrue)
		return FALSE;

	m_PointArray.append(Pt);
	return TRUE;
}
Acad::ErrorStatus NEAnnotationEntity::getGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray& osnapModes, AcDbIntArray& geomIds) const
{
    assertReadEnabled();
    Acad::ErrorStatus ErrStat = Acad::ErrorStatus::eOk;
    
	gripPoints.append(m_PointArray);
    return ErrStat;
}
Acad::ErrorStatus NEAnnotationEntity::moveGripPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
    assertWriteEnabled();
    Acad::ErrorStatus es = Acad::eOk;
	int i;

    for (i = 0; i < indices.length(); i++)
	{
		AcGePoint3d& PtToChange = m_PointArray[indices[i]];
		PtToChange += offset;
    }
    return es;
}
Acad::ErrorStatus NEAnnotationEntity::moveStretchPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
    assertWriteEnabled();
    Acad::ErrorStatus es = Acad::eOk;
	int i;

    for (i = 0; i < indices.length(); i++)
	{
		AcGePoint3d& PtToChange = m_PointArray[indices[i]];

		PtToChange += offset;
    }
    return es;
}
Acad::ErrorStatus NEAnnotationEntity::getStretchPoints(AcGePoint3dArray& stretchPoints) const
{
    assertReadEnabled();

    Acad::ErrorStatus es = Acad::eOk;
	stretchPoints.append(m_PointArray);
    return es;
}

Acad::ErrorStatus NEAnnotationEntity::transformBy(const AcGeMatrix3d& xform)
{
    assertWriteEnabled();

 	int i;

    for (i = 0; i < m_PointArray.length(); i++)
	{
		AcGePoint3d& PtToChange = m_PointArray[i];
		PtToChange.transformBy(xform);
    }
    m_fTxtSize *= xform.scale();
    return Acad::eOk;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NEAnnotationEntityJig::NEAnnotationEntityJig(const AcGePoint3d& pt, const AcGeVector3d& Normal, const double& fTxtSize, const double& fArrSize, const short& iLuPrec)
: m_Normal(Normal), m_pEntity(0L), m_PointToAdd(pt), m_bIsEntityAdded(FALSE), m_CurDragStat(DragStatus::kNoChange)
{
	m_fTxtSize = fTxtSize;
	m_fArrSize = fArrSize;
	m_iLUPREC = iLuPrec;
}

NEAnnotationEntityJig::~NEAnnotationEntityJig()
{
	if(!m_bIsEntityAdded)
		if(m_pEntity != 0L)
			delete m_pEntity;
}
// This function creates an AsdkCircle object and gets the
// jig started acquiring the necessary info to properly fill
// it in.
//
void NEAnnotationEntityJig::doIt()
{
    AcDbObjectId TxtStyleObjID;
	
	if(!TEXTSTYLE(TxtStyleObjID))
		return;


    if(m_pEntity == 0L)
	{
	    m_pEntity = new NEAnnotationEntity(m_Normal, m_PointToAdd, TxtStyleObjID, m_fTxtSize, m_fArrSize, m_iLUPREC);
		NEAnnotationEntity::m_TempVertex = m_PointToAdd;
		NEAnnotationEntity::m_bDragModeOn = TRUE;
	}
	else
	{
		AfxMessageBox("ERROR: NEAnnotationEntityJig::doIt()", MB_ICONSTOP);
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
		strMsg.Format("\nEnter Next point <%d> :", iLen);
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
	NEAnnotationEntity::m_TempVertex.x = NEAnnotationEntity::m_TempVertex.y = NEAnnotationEntity::m_TempVertex.z = 0.0;
	NEAnnotationEntity::m_bDragModeOn = FALSE;

    if(m_bIsEntityAdded)
		append();

	m_bIsEntityAdded = TRUE;
}

// This function is called by the drag function in order to
// acquire a sample input
//
AcEdJig::DragStatus NEAnnotationEntityJig::sampler()
{
    if(m_pEntity == 0L)
	{
		AfxMessageBox("ERROR: NEAnnotationEntityJig::doIt()", MB_ICONSTOP);
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
Adesk::Boolean NEAnnotationEntityJig::update()
{
	if(m_CurDragStat == DragStatus::kNormal)
		m_pEntity->UpdLastVertex(m_PointToAdd);
	else
		NEAnnotationEntity::m_TempVertex = m_PointToAdd;
    return Adesk::kTrue;
}
//
AcDbEntity* NEAnnotationEntityJig::entity() const
{
    return m_pEntity;
}
//
void NEAnnotationEntity::CreateNEAnnEntity()
{
	double fTxtSize, fArrSize;
	short iLuprec;
    AcGePoint3d tempPt;
	AcGeVector3d normalVec;

	fTxtSize = TEXTSIZE();
	fArrSize = fTxtSize;
	iLuprec = (short)LUPREC();

	{//Get the starting point
		struct resbuf rbFrom, rbTo;
		const char* pszPmt = "\nEnter point to annotate or Press 'P' to set parameters :";
		int iRet;
	
		ads_initget(RSG_OTHER, "Property _Property");
		iRet = ads_getpoint(0L, pszPmt, asDblArray(tempPt));
		switch (iRet)
		{
		case RTNORM:
			//Point input successful....
			break;
		case RTKWORD:
			//Set Param first ....
			if(!GetNEEntProperty(fTxtSize, fArrSize, iLuprec))
				return;
			//Get Point now....
			if(RTNORM != ads_getpoint(0L, "\nEnter point to annotate :", asDblArray(tempPt)))
			{
				ads_printf("\nERROR: No point input given\n");
				return;
			}
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

	DIMZIN(0);//Don't suppress leading/trailing zeros
    // Create an NEAnnotationEntityJig object passing in the center point
    // just selected by the user and the model space normal vector.
    //
    NEAnnotationEntityJig* pJig;
    
	pJig = new NEAnnotationEntityJig(tempPt, normalVec, fTxtSize, fArrSize, iLuprec);
    pJig->doIt();// Now start up the jig to interactively get the circle's radius
    delete pJig;// Now delete the jig since it's no longer needed
}

void NEAnnotationEntity::EditNEAnnEntity()
{
	const char* pszPmt = "\nSelect NEAnnotationEntity to edit :";
	ads_name entName;
	ads_point ptRes;
	AcDbObject* pObj;
	NEAnnotationEntity* pEnt;
	Acad::ErrorStatus ErrCode;
	AcDbObjectId objId;
	double fTxtSize, fArrSize;
	short iLuprec;

    if(RTNORM != ads_entsel(pszPmt, entName, ptRes))
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
	ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
	if(ErrCode != Acad::eOk)
	{
		ads_printf("\nERROR: AutoCAD System Error\n");
		return;
	}
	if(pObj->isA() != NEAnnotationEntity::desc())
	{
		ads_printf("\nERROR: Entity selected is not a NEAnnotationEntity\n");
		pObj->close();
		return;
	}
	DIMZIN(0);//Don't suppress leading/trailing zeros
	pEnt = (NEAnnotationEntity*)pObj;

	fTxtSize = pEnt->GetTxtSize();
	fArrSize = pEnt->GetArrSize();
	iLuprec = pEnt->GetLUPREC();

	if(!GetNEEntProperty(fTxtSize, fArrSize, iLuprec))
	{
		ads_printf("\nERROR: User Termination\n");
		pObj->close();
		return;
	}
	pEnt->recordGraphicsModified();
	pEnt->SetTxtSize(fTxtSize);
	pEnt->SetArrSize(fArrSize);
	pEnt->SetLUPREC(iLuprec);
	pObj->close();
	{//Update enity graphics
		ads_entupd(entName);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Get an AcGiTextStyle from an AcDbTextStyleTableRecord.
//
Acad::ErrorStatus NEAnnotationEntity::GetAcGiTextStyle(AcGiTextStyle& StyleNew, const AcDbObjectId& StyleId)
{
    AcDbTextStyleTableRecord* pStyleOld;
    Acad::ErrorStatus ErrStat;
    char *tmpStr;

    ErrStat = acdbOpenAcDbObject((AcDbObject *&)pStyleOld, StyleId, AcDb::kForRead);
    if(ErrStat != Acad::eOk)
		return ErrStat;
    
	ErrStat = pStyleOld->fileName(tmpStr);
    if(ErrStat != Acad::eOk)
	{
        pStyleOld->close();
        return ErrStat;
    }
    StyleNew.setFileName(tmpStr);
    delete tmpStr;

	ErrStat = pStyleOld->bigFontFileName(tmpStr);
    if (ErrStat != Acad::eOk)
	{
        pStyleOld->close();
        return ErrStat;
    }
    StyleNew.setBigFontFileName(tmpStr);
    delete tmpStr;

    StyleNew.setTextSize(pStyleOld->textSize());
    StyleNew.setXScale(pStyleOld->xScale());
    StyleNew.setObliquingAngle(pStyleOld->obliquingAngle());

    pStyleOld->close();
    StyleNew.loadStyleRec();
    return ErrStat;
}
