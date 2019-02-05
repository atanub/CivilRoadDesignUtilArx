// Profile.cpp : implementation file
//

#include "StdAfx.h"
#include <Adscodes.h>
#include <DbEnts.h>
#include <GeAssign.h>
#include <Math.h>

#include "StdAfx.h"
#include "ContourApp.h"
#include "Profile.h"
#include "DwgParamObj.h"
#include "DrawUtil.h"
#include "Util.h"
#include "TINEntity.h"
#include "TINData.h"
#include "QProfile.h"
#include "HAlignment.h"
#include "ResultDlg.h"

//static void ShowStackReport(const AcGePoint3dArray&, const CStringArray&, const AcGeDoubleArray&, const CCurveDetDataArray&);

void DrawProfile()
{
	DWGPARAMS DwgParams;
	AcGePoint3dArray PflPtsArray;
	CStringArray arrayOrdComment;
	AcGePoint3d PtOrg;
	AcGeDoubleArray fChgArray;
	CCurveDetDataArray arrayCurveDet;
	CTINData* pTinDataObject;

	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		AfxMessageBox("ERROR: Contour drawing parameters not set yet", MB_ICONSTOP);
		return;
	}
	pTinDataObject = CTINData::GetTINObject(AcDb::kForRead);
	if(0L == pTinDataObject)
	{//
		CString strMsg;
		strMsg.Format("ERROR: Triangulation not done yet\nPlease Run Triangulation & then try to draw profile");
		AfxMessageBox(strMsg, MB_ICONSTOP);
		return;
	}
	{//
		CHAlignment* pEnt;
		
		DIMZIN(0);//Don't suppress leading/trailing zeros
		pEnt = CHAlignmentJig::GetHAlignmentEntity("\nSelect Horizontal Alignment Entity :");
		if(pEnt == 0L)
		{
			pTinDataObject->close();
			return;
		}
		if(!pEnt->DrawProfile(pTinDataObject))
		{
			pTinDataObject->close();
			pEnt->close();
			return;
		}
		/*
		if(!pEnt->CreateCurveDataReport(arrayCurveDet))
		{
			pTinDataObject->close();
			pEnt->close();
			return;
		}
		*/
		pTinDataObject->close();
		pEnt->close();
	}

	//////////////////////////////////////////////////////////////////////
	/*
	if(!GetAllProfilePoints(ContourPtsArray, ArrayTrgs, PflPtsArray, arrayOrdComment))
		return;
	//
	ShowStackReport(PflPtsArray, arrayOrdComment, fChgArray, arrayCurveDet);

	//Draw Profile now ....
	if(RTNORM != ads_getpoint(0L, "\nEnter lower left corner of profile :", asDblArray(PtOrg)))
		return;
	
	RecPflData.dSexonInterval = 0.0;//for drawing all the ordinates.....
	DrawQProfile(PtOrg, PflPtsArray, arrayOrdComment, fChgArray, RecPflData);
	*/
}


static void ShowStackReport(const AcGePoint3dArray& PflPtsArray, const CStringArray& arrayOrdComment, const AcGeDoubleArray& fChgArray, const CCurveDetDataArray& arrayCurveDet)
{
	const CString strFormat("%s\t%s\t%s\t%s\t%s");
	const CString strHeading("Chainage\tNorthing\tEasting\tElevation\tRemarks");
	CString strLine;
	int i;
	CStringArray arrayAllLines;

	arrayAllLines.Add(strHeading);
	for(i = 0; i < PflPtsArray.length(); i++)
	{
		strLine.Format(strFormat, RTOS(fChgArray[i]), RTOS(PflPtsArray[i].y), RTOS(PflPtsArray[i].x), RTOS(PflPtsArray[i].z), arrayOrdComment[i]);
		arrayAllLines.Add(strLine);
	}
	arrayAllLines.Add("=== Curve Detail ======================================================");
	for(i = 0; i < arrayCurveDet.GetSize(); i++)
	{
		const CURVEDETDATA* pRec = arrayCurveDet[i];

		strLine.Format("Detail of Point of Intersection #%d.....", i+1);
		arrayAllLines.Add(strLine);
		if(pRec->fRad == 0.0)
		{
			strLine.Format("Comment\t: %s", pRec->strComment);
			arrayAllLines.Add(strLine);
			strLine.Format("Curve Radius\t: %f", pRec->fRad);
			arrayAllLines.Add(strLine);
		}
		else
		{
			if(strlen(pRec->strComment) > 0)
			{
				strLine.Format("Comment\t: %s", pRec->strComment);
				arrayAllLines.Add(strLine);
			}
			strLine.Format("Curve Radius\t: %s", RTOS(pRec->fRad));
			arrayAllLines.Add(strLine);
			strLine.Format("Tangent Length\t: %s", RTOS(pRec->fTangentLength));
			arrayAllLines.Add(strLine);
			strLine.Format("Apex Distance\t: %s", RTOS(pRec->fXternalLength));
			arrayAllLines.Add(strLine);
			strLine.Format("Curve Length\t: %s", RTOS(pRec->fCurveLen));
			arrayAllLines.Add(strLine);
			strLine.Format("Degree of Curvature\t: %s", RTOS(pRec->fDegOfCurve));
			arrayAllLines.Add(strLine);
			strLine.Format("Deflection Angle\t: %s", RTOS(pRec->fDefAngle));
			arrayAllLines.Add(strLine);
			strLine.Format("Point of Intesection\t: N=%s, E=%s", RTOS(pRec->ptPOI.y), RTOS(pRec->ptPOI.x));
			arrayAllLines.Add(strLine);
			strLine.Format("Start of Curve\t: N=%s, E=%s", RTOS(pRec->ptTC.y), RTOS(pRec->ptTC.x));
			arrayAllLines.Add(strLine);
			strLine.Format("End of Curve\t: N=%s, E=%s", RTOS(pRec->ptCT.y), RTOS(pRec->ptCT.x));
			arrayAllLines.Add(strLine);
		}
		strLine.Format("======================================================");
		arrayAllLines.Add(strLine);
	}
	ShowResultDlg(arrayAllLines);
}
