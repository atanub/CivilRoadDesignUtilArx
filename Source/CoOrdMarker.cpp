
//CoOrdMarker.cpp
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
#include "CoOrdMarker.h"
#include "Util.h"
#include "CoOrdMarkerPropDlg.h"


// define a DXF class body for the entity using the
// appropriate ACRX boilerplate macro.  The DXF class name
// *MUST* be all uppercase.
//
// ...
ACRX_DXF_DEFINE_MEMBERS(CCoOrdMarker, AcDbEntity,
AcDbProxyEntity::kAllAllowedBits, CCoOrdMarker, 
"Contour\
|Product Desc: Contour ARX Application\
|Company: Atanu Banik's\
|E-Mail : atanubanik@gmail.com");


MAKE_ACDBOPENOBJECT_FUNCTION(CCoOrdMarker);

const double CCoOrdMarker::m_fMinmMarkerSizeWRTRad = 0.15;

CCoOrdMarker::CCoOrdMarker()
{
	m_fMarkerScale = m_fTxtSize = m_fAngleInRad = 0.0;
	m_iLUPREC = (short)LUPREC();
}

CCoOrdMarker::CCoOrdMarker(const AcGeVector3d& Normal, const AcGePoint3d& PtParam, const AcDbObjectId& ObjIDTxtStyle, const double& fTxtSize, const double& fMarkerScale, const double& fAngleInRad, const short& iLuPrec)
: m_DbObjIDTxtStyle(ObjIDTxtStyle), m_Normal(Normal)
{
	m_Point = PtParam;
	m_fAngleInRad = fAngleInRad;
	m_fTxtSize = fTxtSize;
	m_fMarkerScale = fMarkerScale;
	m_iLUPREC = iLuPrec;
}
CCoOrdMarker::~CCoOrdMarker()
{
}
void CCoOrdMarker::MakeStrings(const double& fZValue, const short& iLuPrec, CString& strLeft, CString& strRite)
{
	char szTmp[_MAX_PATH];
	const char chDot = '.';
	CString strTmp;
	int iIndex;
	
	{//Get Left String....
		ads_rtos(fZValue, 2, iLuPrec, szTmp);
		strTmp = szTmp;
		iIndex = strTmp.Find(chDot);
		if(iIndex < 0)
			strLeft = szTmp;
		else if (iIndex == 0)
			strLeft = "0";
		else
			strLeft = strTmp.Mid(0, iIndex);
	}
	{//Get Right String....
		ads_rtos(fZValue, 2, iLuPrec, szTmp);
		strTmp = szTmp;
		iIndex = strTmp.Find(chDot);
		
		if(iIndex < 0)
			strTmp = "0";
		else
			strTmp = strTmp.Mid(iIndex + 1);
		
		strRite = strTmp;
		//Remove Trailing Zeros....
		//for(i = strlen(strTmp) - 1; i >= 0; i--)
		//{
		//	if(strTmp[i] != '0')
		//	{
		//		strRite = strTmp.Mid(0, i+1);
		//		break;
		//	}
		//}
		//if(i < 0)
		//{
		//	strRite = "0";
		//}
	}
}
void CCoOrdMarker::DrawText(AcGiWorldDraw* wd, const AcGePoint3d& PtParam, const AcDbHardPointerId& ObjIDTxtStyle, AcDbVoidPtrArray* pEntArray /*= 0L*/) const
{
	const double fTxtWidMF = 1.0;
	double fLeftTxtStrLenInDwgUnit = 0.0;
	CString strLeft, strRite;
	short iSubEntTrait;

	ads_point ptLeft, ptRite;
    AcGeVector3d dirVec;
	AcGiTextStyle Style;
	
	CCoOrdMarker::MakeStrings(PtParam.z, m_iLUPREC, strLeft, strRite);

	{//Calculate Text Extent of Left String
		AcGePoint2d ptExtent;

		if(Acad::eOk != fromAcDbTextStyle(Style, ObjIDTxtStyle))
		{
			if(GetAcGiTextStyle(Style, ObjIDTxtStyle) != Acad::eOk)
			{
				AcDbObjectId ObjIDTxtStyleTmp;
			
				if(TEXTSTYLE(ObjIDTxtStyleTmp))
				{
					GetAcGiTextStyle(Style, ObjIDTxtStyleTmp);
				}

			}
		}
		//
		Style.setTextSize(m_fTxtSize);
		ptExtent = Style.extents(strLeft, Adesk::kFalse, strlen(strLeft), Adesk::kTrue);
		fLeftTxtStrLenInDwgUnit = ptExtent.x;
	}
	{//Calculate Left 'n Rite point
		ads_point ptTemp;

		//Get Right hand text Insertion point
		ads_polar(asDblArray(PtParam), m_fAngleInRad, (m_fTxtSize / 2.0), ptTemp);
		ads_polar(ptTemp, (m_fAngleInRad - (PI / 2.0)), (m_fTxtSize / 2.0), ptRite);

		//Get Left hand text Insertion point
		ads_polar(asDblArray(PtParam), (m_fAngleInRad + PI), ((m_fTxtSize / 2.0) + fLeftTxtStrLenInDwgUnit), ptTemp);
		ads_polar(ptTemp, ((m_fAngleInRad + PI) + (PI / 2.0)), (m_fTxtSize / 2.0), ptLeft);
	}

	//Draw ....
	if(pEntArray == 0L)
	{//Drawing purpose only.......................
		if (ObjIDTxtStyle != NULL)
		{
			dirVec = m_Normal.perpVector();
			dirVec = dirVec.rotateBy(m_fAngleInRad, m_Normal);
			iSubEntTrait = 0;

			if(GetAcGiTextStyle(Style, ObjIDTxtStyle) != Acad::eOk)
			{
				AcDbObjectId ObjIDTxtStyleTmp;
			
				if(TEXTSTYLE(ObjIDTxtStyleTmp))
				{
					GetAcGiTextStyle(Style, ObjIDTxtStyleTmp);
				}
				Style.setTextSize(m_fTxtSize);

				iSubEntTrait++;//1
				wd->subEntityTraits().setSelectionMarker(iSubEntTrait);
				wd->geometry().text(asPnt3d(ptRite), m_Normal, dirVec, strRite, -1, 0, Style);
				
				iSubEntTrait++;//2
				wd->subEntityTraits().setSelectionMarker(iSubEntTrait);
				wd->geometry().text(asPnt3d(ptLeft), m_Normal, dirVec, strLeft, -1, 0, Style);
			}
			else
			{
				Style.setTextSize(m_fTxtSize);
				iSubEntTrait++;//1
				wd->subEntityTraits().setSelectionMarker(iSubEntTrait);
				wd->geometry().text(asPnt3d(ptRite), m_Normal, dirVec, strRite, -1, 0, Style);
				iSubEntTrait++;//2
				wd->subEntityTraits().setSelectionMarker(iSubEntTrait);
				wd->geometry().text(asPnt3d(ptLeft), m_Normal, dirVec, strLeft, -1, 0, Style);
			}
			if((m_fMarkerScale > 0.0) || (m_fMarkerScale <= 1.0))
			{
				iSubEntTrait++;//3
				wd->subEntityTraits().setFillType(kAcGiFillAlways);
				wd->subEntityTraits().setSelectionMarker(iSubEntTrait);
				wd->geometry().circle(PtParam, (m_fMarkerScale * m_fTxtSize / 2.0), m_Normal);
			}
		}
	}
	else
	{//For Explosion
		dirVec = m_Normal.perpVector();
		dirVec = dirVec.rotateBy(0.0, m_Normal);

		{//Calculate Left 'n Rite point
			//Get Right hand text Insertion point
			ads_polar(asDblArray(PtParam), m_fAngleInRad, (m_fTxtSize / 2.0), ptRite);
			//Get Left hand text Insertion point
			ads_polar(asDblArray(PtParam), (m_fAngleInRad + PI), (m_fTxtSize / 2.0), ptLeft);
		}

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

			//
			pEnt = new AcDbText();
			pEnt->setNormal(m_Normal);
			pEnt->setHeight(m_fTxtSize);
			pEnt->setRotation(m_fAngleInRad);

			pEnt->setTextString(strRite);
			pEnt->setTextStyle(m_DbObjIDTxtStyle);
			pEnt->setHorizontalMode(AcDb::TextHorzMode::kTextLeft);
			pEnt->setVerticalMode(AcDb::TextVertMode::kTextVertMid);
			pEnt->setAlignmentPoint(asPnt3d(ptRite));
			
			pEntArray->append(pEnt);

			//
			pEnt = new AcDbText();
			pEnt->setNormal(m_Normal);
			pEnt->setHeight(m_fTxtSize);
			pEnt->setRotation(m_fAngleInRad);
			pEnt->setTextString(strLeft);

			pEnt->setTextStyle(m_DbObjIDTxtStyle);
			pEnt->setHorizontalMode(AcDb::TextHorzMode::kTextRight);
			pEnt->setVerticalMode(AcDb::TextVertMode::kTextVertMid);
			pEnt->setAlignmentPoint(asPnt3d(ptLeft));

			pEntArray->append(pEnt);

			if(m_fMarkerScale > 0.0)
			{
				AcDbPolyline* pNewEnt;
				AcGePoint2d ptTmp;
				const double fOutsideRad = m_fMarkerScale * m_fTxtSize / 2.0;
				const double fInsideRad = 0.0;

				pNewEnt = new AcDbPolyline();
				//Initialize data..
				pNewEnt->setElevation(PtParam.z);
				pNewEnt->setClosed(Adesk::kTrue);
				//Initialize data..
				ptTmp.x = PtParam.x - (fInsideRad + (fOutsideRad - fInsideRad) / 2.0); 
				ptTmp.y = PtParam.y;
				pNewEnt->addVertexAt(0, ptTmp, tan((PI - 0.0) / 4.0), (fOutsideRad - fInsideRad), (fOutsideRad - fInsideRad));

				ptTmp.x = PtParam.x + (fInsideRad + (fOutsideRad - fInsideRad) / 2.0); 
				ptTmp.y = PtParam.y;
				pNewEnt->addVertexAt(0, ptTmp, tan((PI - 0.0) / 4.0), (fOutsideRad - fInsideRad), (fOutsideRad - fInsideRad));

				pEntArray->append(pNewEnt);
			}
		}
	}
}
Adesk::Boolean CCoOrdMarker::worldDraw(AcGiWorldDraw* wd)
{
	DrawText(wd, m_Point, m_DbObjIDTxtStyle);
    return Adesk::kTrue;
}
void CCoOrdMarker::saveAs(AcGiWorldDraw* wd, AcDb::SaveType saveType)
{
	DrawText(wd, m_Point, m_DbObjIDTxtStyle);
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
Acad::ErrorStatus CCoOrdMarker::dwgInFields(AcDbDwgFiler* filer)
{
    assertWriteEnabled();

    AcDbEntity::dwgInFields(filer);

	{//Read
		//all the Points.......
		filer->readPoint3d(&m_Point);
	}
	//
	filer->readDouble(&m_fTxtSize);
	filer->readDouble(&m_fMarkerScale);
	filer->readItem(&m_iLUPREC);
	filer->readItem(&m_fAngleInRad);
	filer->readItem(&m_Normal);
	filer->readItem(&m_DbObjIDTxtStyle);
	
    return filer->filerStatus();
}

Acad::ErrorStatus CCoOrdMarker::dwgOutFields(AcDbDwgFiler* filer) const
{
    assertReadEnabled();

    AcDbEntity::dwgOutFields(filer);
	
	//Write the Point.......
	filer->writePoint3d(m_Point);

	filer->writeDouble(m_fTxtSize);
	filer->writeDouble(m_fMarkerScale);
	filer->writeItem(m_iLUPREC);
	filer->writeItem(m_fAngleInRad);
	filer->writeItem(m_Normal);
	filer->writeItem(m_DbObjIDTxtStyle);
	return filer->filerStatus();
}
Acad::ErrorStatus CCoOrdMarker::audit(AcDbAuditInfo* /*auditor*/)
{
	return Acad::eOk;
}
Acad::ErrorStatus CCoOrdMarker::explode(AcDbVoidPtrArray& entities) const
{
    assertReadEnabled();
	
	DrawText(0L, m_Point, m_DbObjIDTxtStyle, &entities);
    return Acad::eOk;
}

void CCoOrdMarker::list() const
{
    assertReadEnabled();
	
	ads_printf("\nSurveyed Point");
	ads_printf("\nX = %f, Y = %f, Z = %f", m_Point.x, m_Point.y, m_Point.z);
	ads_printf("\nText Size             = %f", m_fTxtSize);
	ads_printf("\nMarker Relative Scale = %f", m_fMarkerScale);
	ads_printf("\nRotation Angle        = %f Degrees", RTOD(m_fAngleInRad));
	ads_printf("\nLinear Unit Precision = %d", m_iLUPREC);

    AcDbEntity::list();
}
Acad::ErrorStatus CCoOrdMarker::getOsnapPoints(AcDb::OsnapMode osMode, int gsSelectionMark, const AcGePoint3d& /*pickPoint*/, const AcGePoint3d&, const AcGeMatrix3d& /*viewXform*/, AcGePoint3dArray& snapPoints, AcDbIntArray&) const
{
	switch(osMode)
	{
	case AcDb::OsnapMode::kOsModeMid:
	case AcDb::OsnapMode::kOsModeNear:
	case AcDb::OsnapMode::kOsModeNode:
	case AcDb::OsnapMode::kOsModeCen:
	case AcDb::OsnapMode::kOsModeIns:
		snapPoints.append(m_Point);
		break;
	default:
		return Acad::eNotImplemented;
	}
    return Acad::eOk;
}

Acad::ErrorStatus CCoOrdMarker::getGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray& osnapModes, AcDbIntArray& geomIds) const
{
    assertReadEnabled();
    Acad::ErrorStatus ErrStat = Acad::ErrorStatus::eOk;
    
	gripPoints.append(m_Point);
    return ErrStat;
}
Acad::ErrorStatus CCoOrdMarker::moveGripPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
    assertWriteEnabled();
    Acad::ErrorStatus es = Acad::eOk;

	m_Point += offset;
    return es;
}
Acad::ErrorStatus CCoOrdMarker::moveStretchPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
    assertWriteEnabled();
	return Acad::eNotImplemented;
    /*
	Acad::ErrorStatus es = Acad::eOk;

	m_Point += offset;

    return es;
	*/
}
Acad::ErrorStatus CCoOrdMarker::getStretchPoints(AcGePoint3dArray& stretchPoints) const
{
    assertReadEnabled();
	return Acad::eNotImplemented;

    /*
    Acad::ErrorStatus es = Acad::eOk;
	stretchPoints.append(m_Point);
    return es;
	*/
}

Acad::ErrorStatus CCoOrdMarker::transformBy(const AcGeMatrix3d& xform)
{
    assertWriteEnabled();
	
	m_Point.transformBy(xform);
    m_fTxtSize *= xform.scale();
    m_fMarkerScale *= xform.scale();

	//m_fAngleInRad = ??;
    return Acad::eOk;
}

void CCoOrdMarker::CoOrdMarker(const AcGePoint3d& thePoint, const double& fTxtSize, const double& fMarkerScale, const double& fAngleInRad, const short& iLuprec, const char* pszLyr/*=0L*/)
{
	AcGeVector3d normalVec;

	{//Get the Vector....
		AcGeVector3d xVec3D, yVec3D;

		xVec3D = acdbCurDwg()->ucsxdir();
		yVec3D = acdbCurDwg()->ucsydir();
		
		normalVec = xVec3D.crossProduct(yVec3D);
		normalVec.normalize();
	}
	if(0L != pszLyr)
	{
		LAYER(pszLyr);
	}
    {// Create the Entity
		AcDbObjectId ObjIDTxtStyle;
	
		if(!TEXTSTYLE(ObjIDTxtStyle))
			return;
		AcDbDatabase* pDb;
		AcDbBlockTable* pBtbl;
		AcDbBlockTableRecord* pBtblRcd;
		CCoOrdMarker* pEntity;

		pDb = acdbCurDwg();
		//Initialize Line data..
		pEntity = new CCoOrdMarker(normalVec, thePoint, ObjIDTxtStyle, fTxtSize, fMarkerScale, fAngleInRad, iLuprec);
		if(0L != pszLyr)
		{
			pEntity->setLayer(pszLyr);
		}

		pDb->getBlockTable(pBtbl, AcDb::kForRead);

		pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
		pBtbl->close();

		pBtblRcd->appendAcDbEntity(pEntity);
		pEntity->close();
		pBtblRcd->close();
	}
}
void CCoOrdMarker::CoOrdMarker()
{
	double fTxtSize, fMarkerScale, fAngleInRad;
	short iLuprec;
    AcGePoint3d thePoint;

	DIMZIN(0);//Don't suppress leading/trailing zeros
	fAngleInRad = PI/6.0;
	fTxtSize = TEXTSIZE();
	fMarkerScale = 0.5;
	iLuprec = (short)LUPREC();

	{//Get the starting point
		struct resbuf rbFrom, rbTo;
		const char* pszPmt = "\nEnter point to mark or Press 'P' to set parameters :";
		int iRet;
		BOOL bFlag;
	
		bFlag = TRUE;
		while(bFlag)
		{
			ads_initget(RSG_OTHER, "Property _Property");
			iRet = ads_getpoint(0L, pszPmt, asDblArray(thePoint));
			switch (iRet)
			{
			case RTNORM:
				//Point input successful....
				bFlag = FALSE;
				break;
			case RTKWORD:
				//Set Param first here....
				GetCoOrdMarkerProperty(fTxtSize, fMarkerScale, fAngleInRad, iLuprec);
				break;
			default:
				ads_printf("\nERROR: No valid input given\n");
				return;
			}
		}
		rbFrom.restype		= RTSHORT;
		rbFrom.resval.rint	= 1;// from UCS
		rbTo.restype		= RTSHORT;
		rbTo.resval.rint	= 0;// to WCS

		ads_trans(asDblArray(thePoint), &rbFrom, &rbTo, Adesk::kFalse, asDblArray(thePoint));
	}
	CoOrdMarker(thePoint, fTxtSize, fMarkerScale, fAngleInRad, iLuprec);
}
void CCoOrdMarker::InsCoOrdMarker()
{
	ads_name SelSet, Entity;
    int i, iRet;
    long iNoOfEnt;
	double fTxtSize, fMarkerScale, fAngleInRad;
	short iLuprec;

	fAngleInRad = PI/6.0;
	fTxtSize = TEXTSIZE();
	fMarkerScale = 0.5;
	iLuprec = (short)LUPREC();
	DIMZIN(0);//Don't suppress leading/trailing zeros
	{//Get user selected  points....
		struct  resbuf  Rb01;
		const char szEntName[] = "POINT";
		const char* szprompts[] = {"Select Points on which to insert CoOrdMarker :", "Remove Points :"};
		// Entity type
		Rb01.restype = 0;
		Rb01.resval.rstring = (char*)szEntName;
		Rb01.rbnext = 0L;
		
		iRet = ads_ssget(":$", szprompts, 0L, &Rb01, SelSet);
		if(iRet != RTNORM)
		{
			ads_printf("\nERROR: No point entity selected\n");
			return;
		}
		if(ads_sslength(SelSet, &iNoOfEnt) != RTNORM)
		{
			ads_ssfree(SelSet);//Free Selset
			ads_printf("\nERROR: No point entity selected\n");
			return;//ERROR
		}
	}
	if(!GetCoOrdMarkerProperty(fTxtSize, fMarkerScale, fAngleInRad, iLuprec))
	{
		ads_printf("\nERROR: Operation cancelled");
		return;
	}

	for(i = 0; i < iNoOfEnt; i++)
	{
		if(ads_ssname(SelSet, i, Entity) != RTNORM)
		{
			ads_printf("\nERROR: AutoCAD System error !\n");
			ads_ssfree(SelSet);//Free Selset
			return;
		}
		{//Xtract Point...
			AcDbObjectId objId;
			AcDbObject* pObj;
			Acad::ErrorStatus ErrCode;

			ErrCode = acdbGetObjectId(objId, Entity);
			if(ErrCode != Acad::eOk)
			{
				ads_ssfree(SelSet);//Free Selset
				ads_printf("\nERROR: AutoCAD System error !\n");
				return;
			}
			ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
			if(ErrCode != Acad::eOk)
			{
				ads_ssfree(SelSet);//Free Selset
				ads_printf("\nERROR: AutoCAD System error !\n");
				return;
			}
			CoOrdMarker(((AcDbPoint*)pObj)->position(), fTxtSize, fMarkerScale, fAngleInRad, iLuprec);
			pObj->close();
		}
		ads_printf("\rTotal %d # of points annotated", i+1);
	}
}
BOOL CCoOrdMarker::GetCoOrdMarkerEntProperty(const AcDbObjectId& ObjID, double& fTxtSize, double& fMarkerScale, double& fAngleInRad, short& iLuprec)
{
	Acad::ErrorStatus ErrCode;
	AcDbObject* pObj;

	ErrCode = acdbOpenObject(pObj, ObjID, AcDb::kForRead);
	if(ErrCode != Acad::eOk)
	{
		return FALSE;
	}
	if(pObj->isA() == CCoOrdMarker::desc())
	{
		CCoOrdMarker* pEntity = (CCoOrdMarker*)pObj;

		fTxtSize = pEntity->GetTxtSize();
		fAngleInRad = pEntity->GetAngle();
		fMarkerScale = pEntity->GetMarkerSize();
		iLuprec = pEntity->GetLUPREC();
	}
	pObj->close();
	return TRUE;
}
BOOL CCoOrdMarker::GetCoOrdMarkerEnt(AcDbObjectIdArray& Array)
{
	ads_name SelSet;
	long iNoOfEnt;
	if(Array.length() > 0)
	{
		return FALSE;
	}
	{//Get all the ents....
		const char* szprompts[] = {"Select CCoOrdMarker objects :", "Remove CCoOrdMarker objects :"};
		
		if(ads_ssget(":$", szprompts, 0L, 0L, SelSet) != RTNORM)
		{
			return FALSE;
		}
		if(ads_sslength(SelSet, &iNoOfEnt) != RTNORM)
		{
			return FALSE;
		}
	}
	{//Extract Data ....
		long i;
		ads_name Entity;
		Acad::ErrorStatus ErrCode;
		AcDbObjectId objId;
		AcDbObject* pObj;

		ads_printf("\n");
		for(i = 0; i < iNoOfEnt; i++)
		{
			if(ads_usrbrk() == 1)
			{
				ads_printf("\nUser break encountered\n");
				ads_ssfree(SelSet);//Free Selset
				return FALSE;
			}
			if(ads_ssname(SelSet, i, Entity) != RTNORM)
			{
				ads_printf("\nERROR: AutoCAD System error !\n");
				ads_ssfree(SelSet);//Free Selset
				return FALSE;
			}
			ErrCode = acdbGetObjectId(objId, Entity);
			if(ErrCode != Acad::eOk)
			{
				ads_ssfree(SelSet);//Free Selset
				return FALSE;
			}
			ErrCode = acdbOpenObject(pObj, objId, AcDb::kForRead);
			if(ErrCode != Acad::eOk)
			{
				ads_ssfree(SelSet);//Free Selset
				return FALSE;
			}
			if(pObj->isA() == CCoOrdMarker::desc())
			{
				Array.append(objId);
				ads_printf("\rPlease wait...Found entity #%d of %d", Array.length(), iNoOfEnt);
			}
			pObj->close();
		}
	}
	ads_ssfree(SelSet);//Free Selset
	return TRUE;
}
void CCoOrdMarker::EditCoOrdMarker()
{
	AcDbObjectIdArray Array;
	Acad::ErrorStatus ErrCode;
	AcDbObject* pObj;
	double fTxtSize, fMarkerScale, fAngleInRad;
	short iLuprec;
	int i;
	
	if(!GetCoOrdMarkerEnt(Array) || (Array.length() < 1))
	{
		ads_printf("\nERROR: No valid entity selected for editing");
		return;
	}
	DIMZIN(0);//Don't suppress leading/trailing zeros
	{//GUI 
		fAngleInRad = PI/6.0;
		fTxtSize = TEXTSIZE();
		fMarkerScale = 0.5;
		iLuprec = (short)LUPREC();
		if(!GetCoOrdMarkerEntProperty(Array[0], fTxtSize, fMarkerScale, fAngleInRad, iLuprec))
		{
			ads_printf("\nERROR: No valid entity selected for editing");
			return;
		}

		if(!GetCoOrdMarkerProperty(fTxtSize, fMarkerScale, fAngleInRad, iLuprec))
		{
			ads_printf("\nERROR: Operation cancelled");
			return;
		}
	}

	for(i = 0; i < Array.length(); i++)
	{
		ErrCode = acdbOpenObject(pObj, Array[i], AcDb::kForWrite);
		if(ErrCode != Acad::eOk)
		{
			ads_printf("\nInternal error occured !");
			return;
		}
		if(pObj->isA() != CCoOrdMarker::desc())
		{
			ads_printf("\nInternal error occured !");
			pObj->close();
			return;
		}
		{//
			CCoOrdMarker* pEnt;
			
			pEnt = (CCoOrdMarker*)pObj;

			pEnt->SetAngle(fAngleInRad);
			pEnt->SetTxtSize(fTxtSize);
			pEnt->SetMarkerSize(fMarkerScale);
			pEnt->SetLUPREC(iLuprec);
			pEnt->recordGraphicsModified();
		}
		pObj->close();
		//{//Update enity graphics
		//	ads_name  objName;
		//
		//	if(acdbGetAdsName(objName, Array[i]) == Acad::eOk)
		//		ads_entupd(objName);
		//}
	}
	ads_printf("\nTotal %d no. of entity modified", Array.length());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Get an AcGiTextStyle from an AcDbTextStyleTableRecord.
//
Acad::ErrorStatus CCoOrdMarker::GetAcGiTextStyle(AcGiTextStyle& StyleNew, const AcDbObjectId& StyleId)
{
	return fromAcDbTextStyle(StyleNew, StyleId);
	/*
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
	*/
}
