// ContourArx.cpp
/////////////////////////////////////////////////////////

#include "stdafx.h"           // MFC stuff

#include "resourcehelper.h"   // temporary resource class
#include "ContourApp.h"       // the Application
// TODO: add your own includes here or in ContourArx.h
#include "ContourParamDlg.h"
#include "TINEntity.h"
#include "ImportPts.h"
#include "DwgParamObj.h"
#include "DrawContourLines.h"
#include "AboutDlg.h"
#include "ContourAnnotation.h"
#include "ActivateDlg.h"
#include "GenGridData.h"
#include "Volume.h"
#include "Line2PLine.h"
#include "NEAnnotationEntity.h"
#include "Protection.h"
#include "Purge.h"
#include "TINData.h"
#include "Profile.h"
#include "XSection.h"
#include "CoOrdMarker.h"
#include "SubRoutines.h"
#include "RailwayDsgnTemplateDlg.h"
#include "VAlignment.h"

#ifdef _DEBUG
	#include "CutFillCalculatorForXSection.h"
	#include "CrossSectionProfileDefinition.h"
	#include "RoadDesignDataDlg.h"
#endif//_DEBUG

// entry point for this application
extern "C" AcRx::AppRetCode acrxEntryPoint( AcRx::AppMsgCode msg, void* );
// message handlers
static void OnkLoadDwgMsg  (void);
static void OnkPreQuitMsg  (void);

// helper functions
static BOOL initApp  (void);
static void unloadApp(void);

// MFC support
extern "C" HINSTANCE _hdllInstance;
extern "C" HWND adsw_acadMainWnd();

BOOL InitModule(HINSTANCE, DWORD, LPVOID);


void TestFunc()
{
#ifdef _DEBUG
	if(1 == 2)
	{//
		CWnd* pAcadWnd;
		CTemporaryResourceOverride ResOverride;
		
		pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());
		CRailwayDsgnTemplateDlg Dlg(pAcadWnd);
		Dlg.DoModal();
	}
	if(1 == 2)
	{//
		CCutFillCalculatorForXSection::Test();
		CVAlignment* pEntity = CVAlignmentJig::GetVAlignmentEntity("Select Entity:");
		if(pEntity != 0L)
		{
			double fElev;
			double fChainage = 20;
			fElev = pEntity->getElevAt(fChainage);
			pEntity->close();
		}
	}
	if(1 == 1)
	{//
		CRoadDesignDataDlg::GetData();
		return;

		CCrossSectionProfileDefinition* pRec = CCrossSectionProfileDefinition::Define();
		if(pRec != 0L)
		{
			delete pRec;
		}
	}
#endif//_DEBUG
}
/////////////////////////////////////////////////////////////////////
// acrxEntryPoint(internal)
// This function is the entry point for your application.
/////////////////////////////////////////////////////////////////////
AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt)
{
  switch (msg)
  {
	case AcRx::kInitAppMsg:
		acrxDynamicLinker->unlockApplication(pkt);
		ads_printf("\nLoading ARX application: %s", CContourApp::m_strConstThisARXName);
		if(!initApp())
		{
			//ads_printf("... Load failed!\nPossible cause: Invalid license information!\n");
			//MessageBox(adsw_acadMainWnd(), "Failed to load application\nPossible cause: Invalid license information!", CContourApp::m_strConstThisARXName, MB_ICONSTOP);
			return AcRx::kRetError;
		}
		break;
	case AcRx::kUnloadAppMsg:
		unloadApp();
		break;
	case AcRx::kLoadDwgMsg:
		OnkLoadDwgMsg();
		break;
	case AcRx::kPreQuitMsg:
		OnkPreQuitMsg();
		break;
	case AcRx::kInvkSubrMsg:
	case AcRx::kUnloadDwgMsg:
	case AcRx::kEndMsg:
	case AcRx::kQuitMsg:
	case AcRx::kSaveMsg:
	case AcRx::kCfgMsg:
	case AcRx::kDependencyMsg:
	case AcRx::kNoDependencyMsg:
	default:
		break;
	}
	return AcRx::kRetOK;
}
static void OnkLoadDwgMsg(void)
{
	DWGPARAMS DwgParams;

	if(!CDwgParamObj::GetDwgParamData(&DwgParams))
	{
		ads_printf("\nSetting Contour Parameter(s) first time...");
		if(!CDwgParamObj::CreateDwgParamObj())
		{
			ads_printf("\nFATAL ERROR: Failed to attach drawing parameter data. Drawing file may be corrupted !");
			return;
		}
	}
	PROXYNOTICE(0);
	//..............................................................................
	CDwgParamObj::GetDwgParamData(&DwgParams);
	DwgParams.bIsRegistered = TRUE;

	if(!CDwgParamObj::CreateDwgParamObj(&DwgParams))
	{
		ads_printf("\nFATAL ERROR: Failed to attach drawing parameter data. Drawing file may be corrupted !");
		return;
	}
	ads_printf("\nThank you for using Contour ARX Application\n\n");
}
static void OnkPreQuitMsg(void)
{
	AcDbDatabase*	pDb;

	pDb = acdbCurDwg();
	if(pDb == 0L)
		return;
	pDb->setSaveproxygraphics(1);
}

static BOOL initApp(void)
{
	InitModule(_hdllInstance, DLL_PROCESS_ATTACH, NULL);// MFC Initialization
	
	//Contour Related ....
	CContourApp::RegisterCmdGroup("CONTOUR");
	acedRegCmds->addCommand("CONTOUR", "AboutContour",			"AboutContour",		ACRX_CMD_MODAL|ACRX_CMD_TRANSPARENT|ACRX_CMD_NOMULTIPLE, CAboutDlg::ShowAboutDialog);	CContourApp::RegisterCmd("AboutContour");
	
	acedRegCmds->addCommand("CONTOUR", "ChkContourPoints",		"ChkContourPoints",	ACRX_CMD_MODAL|ACRX_CMD_NOMULTIPLE, CImportPtsUtil::ChkContourPoints);		CContourApp::RegisterCmd("ChkContourPoints");
	acedRegCmds->addCommand("CONTOUR", "ImportData",			"ImportData",		ACRX_CMD_MODAL|ACRX_CMD_NOMULTIPLE, CImportPtsUtil::ContourImportData);						CContourApp::RegisterCmd("ImportData");
	acedRegCmds->addCommand("CONTOUR", "GenPoints",				"GenPoints",		ACRX_CMD_MODAL, CImportPtsUtil::GenPoints);									CContourApp::RegisterCmd("GenPoints");
	acedRegCmds->addCommand("CONTOUR", "GenPtsFrMultiSeg",		"GenPtsFrMultiSeg",	ACRX_CMD_MODAL|ACRX_CMD_NOMULTIPLE, CImportPtsUtil::GenPtsFrMultiSeg);		CContourApp::RegisterCmd("GenPtsFrMultiSeg");

	acedRegCmds->addCommand("CONTOUR", "AddPoints",				"AddPoints",		ACRX_CMD_MODAL|ACRX_CMD_NOMULTIPLE, CImportPtsUtil::AddPoints);				CContourApp::RegisterCmd("AddPoints");
	acedRegCmds->addCommand("CONTOUR", "GenHardBreakLine",		"GenHardBreakLine",	ACRX_CMD_MODAL|ACRX_CMD_NOMULTIPLE, CImportPtsUtil::GenHardBreakLine);		CContourApp::RegisterCmd("GenHardBreakLine");
	acedRegCmds->addCommand("CONTOUR", "GenGridPoints",			"GenGridPoints",	ACRX_CMD_MODAL|ACRX_CMD_NOMULTIPLE, CGenGridPoints::GenGridPoints);			CContourApp::RegisterCmd("GenGridPoints");
	
	acedRegCmds->addCommand("CONTOUR", "InstantAnnotatation",	"InstantAnnotatation",	ACRX_CMD_MODAL,						CContourAnnotation::InstantAnnotatation);			CContourApp::RegisterCmd("InstantAnnotatation");
	acedRegCmds->addCommand("CONTOUR", "AnnotateAllContours",	"AnnotateAllContours",	ACRX_CMD_MODAL|ACRX_CMD_NOMULTIPLE, CContourAnnotation::AnnotateAllContours);			CContourApp::RegisterCmd("AnnotateAllContours");
	acedRegCmds->addCommand("CONTOUR", "AnnotateSelContours",	"AnnotateSelContours",	ACRX_CMD_MODAL,						CContourAnnotation::AnnotateSelContours);			CContourApp::RegisterCmd("AnnotateSelContours");
	acedRegCmds->addCommand("CONTOUR", "ConvertSelPLinesAsContour",	"ConvertSelPLinesAsContour",	ACRX_CMD_MODAL,			CContourAnnotation::ConvertSelPLinesAsContour);		CContourApp::RegisterCmd("ConvertSelPLinesAsContour");

	acedRegCmds->addCommand("CONTOUR", "SetAnnotationParam",	"SetAnnotationParam",	ACRX_CMD_MODAL, CContourAnnotation::SetContourAnnotationParam);					CContourApp::RegisterCmd("SetAnnotationParam");

	acedRegCmds->addCommand("CONTOUR", "DrawSpotLevOnSelPts",	"DrawSpotLevOnSelPts",	ACRX_CMD_MODAL, CImportPtsUtil::DrawSpotLevOnSelPts);						CContourApp::RegisterCmd("DrawSpotLevOnSelPts");
	 
	acedRegCmds->addCommand("CONTOUR", "SetParam",				"SetParam",				ACRX_CMD_MODAL|ACRX_CMD_NOMULTIPLE, CDrawContourLines::ContourSetParam);			CContourApp::RegisterCmd("SetParam");
	acedRegCmds->addCommand("CONTOUR", "GenTIN",				"GenTIN",				ACRX_CMD_MODAL|ACRX_CMD_NOMULTIPLE, CTINData::GenTIN);				CContourApp::RegisterCmd("GenTIN");
	acedRegCmds->addCommand("CONTOUR", "DrawContour",			"DrawContour",			ACRX_CMD_MODAL|ACRX_CMD_NOMULTIPLE, CDrawContourLines::DrawContour);				CContourApp::RegisterCmd("DrawContour");
	acedRegCmds->addCommand("CONTOUR", "DrawUserSpecContour",	"DrawUserSpecContour",	ACRX_CMD_MODAL|ACRX_CMD_NOMULTIPLE, CDrawContourLines::DrawUserSpecContour);		CContourApp::RegisterCmd("DrawUserSpecContour");
	acedRegCmds->addCommand("CONTOUR", "DelAllContours",		"DelAllContours",		ACRX_CMD_MODAL|ACRX_CMD_NOMULTIPLE, CDrawContourLines::DelAllContours);				CContourApp::RegisterCmd("DelAllContours");
	acedRegCmds->addCommand("CONTOUR", "CompactContourLines",	"CompactContourLines",	ACRX_CMD_MODAL|ACRX_CMD_NOMULTIPLE, CDrawContourLines::CompactContourLines);		CContourApp::RegisterCmd("CompactContourLines");
	acedRegCmds->addCommand("CONTOUR", "CalcVolume",			"CalcVolume",			ACRX_CMD_MODAL,						CalculateVolume);			CContourApp::RegisterCmd("CalcVolume");

	acedRegCmds->addCommand("CONTOUR", "DrawProfile",			"DrawProfile",			ACRX_CMD_MODAL,						DrawProfile);				CContourApp::RegisterCmd("DrawProfile");
	acedRegCmds->addCommand("CONTOUR", "DrawXSection",			"DrawXSection",			ACRX_CMD_MODAL,						CXSection::DrawXSection);	CContourApp::RegisterCmd("DrawXSection");
	acedRegCmds->addCommand("CONTOUR", "PurgeAll",				"PurgeAll",				ACRX_CMD_MODAL|ACRX_CMD_NOMULTIPLE,	PurgeAll);					CContourApp::RegisterCmd("PurgeAll");

	acedRegCmds->addCommand("CONTOUR",	"HAlignment",					"HAlignment",					ACRX_CMD_MODAL,		CHAlignmentJig::CreateHAlignment);			CContourApp::RegisterCmd("HAlignment");
	acedRegCmds->addCommand("CONTOUR",	"EditHAlignment",				"EditHAlignment",				ACRX_CMD_MODAL,		CHAlignmentJig::EditHAlignmentVertex);		CContourApp::RegisterCmd("EditHAlignment");
	acedRegCmds->addCommand("CONTOUR",	"AnnotateHAlignmentVertex",		"AnnotateHAlignmentVertex",		ACRX_CMD_MODAL,		CHAlignmentJig::AnnotateHAlignmentVertex);	CContourApp::RegisterCmd("AnnotateHAlignmentVertex");
	acedRegCmds->addCommand("CONTOUR",	"RevertHAlignment",				"RevertHAlignment",				ACRX_CMD_MODAL,		CHAlignmentJig::RevertHAlignment);			CContourApp::RegisterCmd("RevertHAlignment");

	acedRegCmds->addCommand("CONTOUR",	"VAlignment",					"VAlignment",					ACRX_CMD_MODAL,		CVAlignmentJig::CreateVAlignment);			CContourApp::RegisterCmd("VAlignment");
	acedRegCmds->addCommand("CONTOUR",	"EditVAlignment",				"EditVAlignment",				ACRX_CMD_MODAL,		CVAlignmentJig::EditVAlignmentVertex);		CContourApp::RegisterCmd("EditVAlignment");

	//Annotaion Related ....
	CContourApp::RegisterCmdGroup("ANNOTATION");
	acedRegCmds->addCommand("ANNOTATION", "NEAnnotation",		"NEAnnotation",			ACRX_CMD_MODAL, NEAnnotationEntity::CreateNEAnnEntity);			CContourApp::RegisterCmd("NEAnnotation");
	acedRegCmds->addCommand("ANNOTATION", "EditNEAnnotation",	"EditNEAnnotation",		ACRX_CMD_MODAL, NEAnnotationEntity::EditNEAnnEntity);			CContourApp::RegisterCmd("EditNEAnnotation");

	acedRegCmds->addCommand("ANNOTATION", "DrawCoOrdMarker",	"DrawCoOrdMarker",		ACRX_CMD_MODAL,	CCoOrdMarker::CoOrdMarker);				CContourApp::RegisterCmd("DrawCoOrdMarker");
	acedRegCmds->addCommand("ANNOTATION", "EditCoOrdMarker",	"EditCoOrdMarker",		ACRX_CMD_MODAL,	CCoOrdMarker::EditCoOrdMarker);			CContourApp::RegisterCmd("EditCoOrdMarker");
	acedRegCmds->addCommand("ANNOTATION", "InsCoOrdMarker",		"InsCoOrdMarker",		ACRX_CMD_MODAL,	CCoOrdMarker::InsCoOrdMarker);			CContourApp::RegisterCmd("InsCoOrdMarker");

	acedRegCmds->addCommand("ANNOTATION", "RevertPLine",		"RevertPLine",			ACRX_CMD_MODAL, CPLineDataExtractor::RevertPLine);		CContourApp::RegisterCmd("RevertPLine");
	acedRegCmds->addCommand("ANNOTATION", "DividePLine",		"DividePLine",			ACRX_CMD_MODAL, CPLineDataExtractor::DividePLine);		CContourApp::RegisterCmd("DividePLine");

#ifdef _DEBUG
	////////////////////////////////////////////////////////////////////////////////////////////////
	//for Testing Purpose Only.....
	////////////////////////////////////////////////////////////////////////////////////////////////
	CContourApp::RegisterCmdGroup("TEST");
	acedRegCmds->addCommand("TEST",		"TDC",						"TDC",				ACRX_CMD_MODAL,	CDrawContourLines::TestDrawContourInTriangle);	CContourApp::RegisterCmd("TDC");
	acedRegCmds->addCommand("TEST",		"T",						"T",				ACRX_CMD_MODAL,	TestFunc);										CContourApp::RegisterCmd("TestFunc");
	////////////////////////////////////////////////////////////////////////////////////////////////
#endif //_DEBUG
	
	// TODO: init your application
	CTINData::rxInit();
	TINEntity::rxInit();
	CHAlignment::rxInit();
	CVAlignment::rxInit();
    CDwgParamObj::rxInit();
	NEAnnotationEntity::rxInit();
	CCoOrdMarker::rxInit();

    acrxBuildClassHierarchy();
	return (0L != CContourApp::Initialize(_hdllInstance));
}

static void unloadApp(void)
{
	CContourApp::UnInitialize();
	
	//TODO: clean up your application
    deleteAcRxClass(CTINData::desc());
    deleteAcRxClass(TINEntity::desc());
    deleteAcRxClass(CHAlignment::desc());
    deleteAcRxClass(CDwgParamObj::desc());
    deleteAcRxClass(NEAnnotationEntity::desc());
    deleteAcRxClass(CCoOrdMarker::desc());

	// END unloadApp()
	InitModule(_hdllInstance, DLL_PROCESS_DETACH, NULL);
}
static AFX_EXTENSION_MODULE arxmfcDLL;

BOOL InitModule(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if(!AfxInitExtensionModule(arxmfcDLL, hInstance))
			return 0;

		CTemporaryResourceOverride::SetDefaultResource(_hdllInstance);
		// TODO: add other initialization tasks here

		new CDynLinkLibrary(arxmfcDLL);
		// end of initialization
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(arxmfcDLL);
		// TODO: perform other cleanup tasks here

		// end of cleanup
	}
	return 1;
}

//Testing Routines............
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//For bypassing 'ssget' func.
//Access the database directly...
//
int TESTDBase(const DWGPARAMS& DwgParams)
{
	//..............................................................................
    AcDbDatabase* pDb;
    AcDbBlockTable* pBtbl;
	AcDbBlockTableRecord* pBtblRcd;
	int iCountTrg, iCountPts;

	pDb = acdbCurDwg();
    pDb->getBlockTable(pBtbl, AcDb::kForRead);

    pBtbl->getAt(ACDB_MODEL_SPACE, pBtblRcd, AcDb::kForWrite);
    pBtbl->close();
	
	iCountTrg = iCountPts = 0;
	{
		AcDbBlockTableRecordIterator *pBlkTblRcdItr;
		pBtblRcd->newIterator(pBlkTblRcdItr);
		for (pBlkTblRcdItr->start(); !pBlkTblRcdItr->done(); pBlkTblRcdItr->step())
		{
			AcDbEntity *pEnt;
			resbuf*  pRB;
			resbuf*  pTemp;

			pBlkTblRcdItr->getEntity(pEnt, AcDb::kForRead);
			pRB = pEnt->xData(DwgParams.szAppName);
			if(pRB != 0L)
			{
				BOOL bIsNative;
				
				bIsNative = FALSE;
				for (pTemp = pRB; pTemp->rbnext != NULL; pTemp = pTemp->rbnext)
				{
					if(pTemp->restype == AcDb::kDxfRegAppName)
					{
						bIsNative = (strcmp(pTemp->resval.rstring, DwgParams.szAppName) == 0);
						break;
					}
				}
				ads_relrb(pRB);
				if(bIsNative)
				{
					iCountPts++;
					ads_printf("%d no of native Point(s) found\n", iCountPts);
				}
			}
			if(pEnt->isA() == TINEntity::desc())
			{
				iCountTrg++;
				ads_printf("%d no of TINEntity(s) found\n", iCountTrg);
			}
			pEnt->close();
		}
		pBtblRcd->close();
		delete pBlkTblRcdItr;
	}
	return 0;
}
