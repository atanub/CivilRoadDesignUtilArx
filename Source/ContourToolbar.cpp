// File Name: ContourToolbar.cpp

#include "stdafx.h"
#include <rxmfcapi.h>
#include <adslib.h>
#include "ContourToolbar.h"
#include "ContourApp.h"
#include "Resource.h"
#include "ResourceHelper.h"
#include "AboutDlg.h"
#include "ResourceHelper.h"

extern CContourApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CContourToolbar

CContourToolbar::CContourToolbar()
{
}

CContourToolbar::~CContourToolbar()
{
}
BEGIN_MESSAGE_MAP(CContourToolbar, CToolBar)
	//{{AFX_MSG_MAP(CContourToolbar)
	ON_UPDATE_COMMAND_UI(ID_CONTOUR_PARAM,						OnUpdAlwaysEnabled)
	
	ON_UPDATE_COMMAND_UI(ID_BUTTON_GENTIN,						OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_DRAWCONTOUR,						OnUpdAlwaysEnabled)

	ON_UPDATE_COMMAND_UI(ID_BUTTON_IMPORTDATA,					OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ADD_POINTS,					OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_GEN_POINTS,					OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_HARD_BREAK,					OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_GEN_GRID_POINTS,				OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_GEN_POINTS_MULTI_SEG,		OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_CHK_CONT_POINTS,				OnUpdAlwaysEnabled)

	ON_UPDATE_COMMAND_UI(ID_BUTTON_AUTO_ANN,					OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_MANUAL_ANN,					OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_INSTANT_ANN,					OnUpdAlwaysEnabled)


	ON_UPDATE_COMMAND_UI(ID_BUTTON_PROFILE,						OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_PROFILE_EDIT,				OnUpdAlwaysEnabled)

	ON_UPDATE_COMMAND_UI(ID_BUTTON_HALIGNMENT,					OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_EDIT_HALIGNMENT,				OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ANN_HALIGNMENT,				OnUpdAlwaysEnabled)
	
	ON_UPDATE_COMMAND_UI(ID_ABOUT,								OnUpdAlwaysEnabled)

	ON_UPDATE_COMMAND_UI(ID_BUTTON_INPUT,							OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_CONTOUR,								OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ANNOTATION,						OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_PROFILE_MAIN,					OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_X_SECTION,								OnUpdAlwaysEnabled)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_UTIL,							OnUpdAlwaysEnabled)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CContourToolbar::Dump( CDumpContext &dc) const
{
}
void CContourToolbar::AssertValid() const
{
}


/////////////////////////////////////////////////////////////////////////////
// CContourToolbar message handlers
void CContourToolbar::OnUpdAlwaysEnabled(CCmdUI*) 
{
}
// End of CContourToolbar class implementation
//////////////////////////////////////////////////////////////////
static int GetSubMenuID(WORD iIDButtonPressed)
{
	switch(iIDButtonPressed)
	{
	case ID_BUTTON_INPUT:
		return 0;
	case ID_CONTOUR:
		return 1;
	case ID_BUTTON_ANNOTATION:
		return 2;
	case ID_BUTTON_PROFILE_MAIN:
		return 3;
	case ID_X_SECTION:
		return 4;
	case ID_BUTTON_UTIL:
		return 5;
	default:
		return -1;//Invalid
	}
}
static BOOL PopSubCmdMenu(WORD iIDButtonPressed, CWnd* pWndParam)
{//Show Popup menu
	CMenu MenuParent;
	CMenu* pPopMenu;
	POINT Point;
	int iSubMenuID;
	
	iSubMenuID = GetSubMenuID(iIDButtonPressed);
	if(iSubMenuID < 0)
		return FALSE;

	VERIFY(MenuParent.LoadMenu(IDR_MENU_POPUP));
	
	pPopMenu = MenuParent.GetSubMenu( iSubMenuID );
	if(pPopMenu != NULL);
	{
		::GetCursorPos(&Point);
		pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, Point.x, Point.y, pWndParam);
	}
	return TRUE;
}
static BOOL ToolBarProc(WORD iIDButton, CWnd* pWndParam)
{
	CWinApp* pWinApp;
	CString cmdStr;

	pWinApp = acedGetAcadWinApp();
	
	if(PopSubCmdMenu(iIDButton, pWndParam))
	{
		return TRUE;//Handled
	}
	switch(iIDButton)
	{
		//Contour Parameters Related //////////////////////////
		case ID_CONTOUR_PARAM:
			cmdStr = "SetParam ";
			break;
		
		//Input Related //////////////////////////
		case ID_BUTTON_IMPORTDATA:
			cmdStr = "ImportData ";
			break;
		case ID_BUTTON_ADD_POINTS:
			cmdStr = "AddPoints ";
			break;
		case ID_BUTTON_GEN_POINTS:
			cmdStr = "GenPoints ";
			break;
		case ID_BUTTON_HARD_BREAK:
			cmdStr = "GenHardBreakLine ";
			break;
		case ID_BUTTON_GEN_POINTS_MULTI_SEG:
			cmdStr = "GenPtsFrMultiSeg ";
			break;
		case ID_BUTTON_GEN_GRID_POINTS:
			cmdStr = "GenGridPoints ";
			break;
		case ID_BUTTON_CHK_CONT_POINTS:
			cmdStr = "ChkContourPoints ";
			break;

		//TIN Generation ////////////////////////////////////////////////////////////////
		case ID_BUTTON_GENTIN:
			cmdStr = "GenTIN ";
			break;

		//Contour Generation ////////////////////////////////////////////////////////////////
		case ID_DRAWCONTOUR:
			cmdStr = "DrawContour ";
			break;
		case ID_DRAWCONTOUR_USER_SPEC:
			cmdStr = "DrawUserSpecContour ";
			break;
		case ID_DEL_CONTOUR:
			cmdStr = "DelAllContours ";
			break;
		case ID_COMPACT_CONTOUR_LINES:
			cmdStr = "CompactContourLines ";
			break;
		case ID_CALCULATE_VOLUME:
			cmdStr = "CalcVolume ";
			break;
		
		//Contour Annotation ////////////////////////////////////////////////////////////////
		case ID_BUTTON_AUTO_ANN:
			cmdStr = "AnnotateAllContours ";
			break;
		case ID_BUTTON_MANUAL_ANN:
			cmdStr = "AnnotateSelContours ";
			break;
		case ID_BUTTON_CONVERT_TOCONTOUR_ENT:
			cmdStr = "ConvertSelPLinesAsContour ";
			break;
		case ID_BUTTON_INSTANT_ANN:
			cmdStr = "InstantAnnotatation ";
			break;
		case ID_SET_CONT_ANN_PARAMS:
			cmdStr = "SetAnnotationParam ";
			break;

		//Profile ////////////////////////////////////////////////////////////////
		case ID_BUTTON_PROFILE:
			cmdStr = "DrawProfile ";
			break;
		case ID_BUTTON_PROFILE_EDIT:
			cmdStr = "EditProfile ";
			break;
		
		//Cross Section ////////////////////////////////////////////////////////////////
		case ID_DRAW_X_SECTION:
			cmdStr = "DrawXSection ";
			break;
		
		//Horizontal Alignment ////////////////////////////////////////////////////////////////
		case ID_BUTTON_HALIGNMENT:
			cmdStr = "HAlignment ";
			break;
		case ID_BUTTON_EDIT_HALIGNMENT:
			cmdStr = "EditHAlignment ";
			break;
		case ID_BUTTON_ANN_HALIGNMENT:
			cmdStr = "AnnotateHAlignmentVertex ";
			break;
		case ID_BUTTON_REVERT_HALIGNMENT:
			cmdStr = "RevertHAlignment ";
			break;

		//Vertical Alignment ////////////////////////////////////////////////////////////////
		case ID_BUTTON_VALIGNMENT:
			cmdStr = "VAlignment ";
			break;
		case ID_BUTTON_EDIT_VALIGNMENT:
			cmdStr = "EditVAlignment ";
			break;
		//NEAnnotation Entity//////////////////////////////////////////////
		case ID_DRAW_NEANN_ENTITY:
			cmdStr = "NEAnnotation ";
			break;
		case ID_EDIT_NEANN_ENTITY:
			cmdStr = "EditNEAnnotation ";
			break;

		//CoOrdinate Marker Entity//////////////////////////////////////////////
		case ID_DRAW_COORD_MARKER:
			cmdStr = "DrawCoOrdMarker ";
			break;
		case ID_EDIT_COORD_MARKER:
			cmdStr = "EditCoOrdMarker ";
			break;
		case ID_INS_COORD_MARKER:
			cmdStr = "InsCoOrdMarker ";
			break;

		//Draw Spot Levels On Selected Pts //////////////////////////////////////
		case ID_ANN_CONTOUR_PTS:
			cmdStr = "DrawSpotLevOnSelPts ";
			break;

		//Purge All ////////////////////////////////////////////////////////////
		case ID_PURGE_ALL:
			cmdStr = "PurgeAll ";
			break;

		//Utility: Divides polyline in equal intervals segmnets /////////////////
		case ID_DIVIDE_POLYLINE:
			cmdStr = "DividePLine ";
			break;
		case ID_REVERT_POLYLINE:
			cmdStr = "RevertPLine ";
			break;
		//About Dlg ////////////////////////////////////////////////////////////////
		case ID_ABOUT:
			cmdStr = "'AboutContour ";
			break;
		default:
			return FALSE;//NOT Handled
	}
	if(strlen(cmdStr) > 0)
		CContourApp::SendCmdToAutoCAD(cmdStr);
	
	
	// Force AutoCAD Toolbar to be updated.
	// It's updated only when AutoCAD is on idle.
	pWinApp->OnIdle(0);
	return TRUE;//Handled
}
/////////////////////////////////////////////////////////////////////////////
// CTBHandler

CTBHandler::CTBHandler()
{
}
CTBHandler::~CTBHandler()
{
}
BEGIN_MESSAGE_MAP(CTBHandler, CWnd)
	//{{AFX_MSG_MAP(CTBHandler)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTBHandler message handlers

BOOL CTBHandler::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	UINT iCtrlID, iMsgID;
	
	iCtrlID = LOWORD(wParam);
	iMsgID = HIWORD(wParam);

    if(ToolBarProc((unsigned short)iCtrlID, this))
    {
        return TRUE; // end of msg
    }
	return CWnd::OnCommand(wParam, lParam);
}

