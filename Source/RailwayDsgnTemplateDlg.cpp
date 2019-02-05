// RailwayDsgnTemplateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ContourApp.h"
#include "RailwayDsgnTemplateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRailwayDsgnTemplateDlg dialog


CRailwayDsgnTemplateDlg::CRailwayDsgnTemplateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRailwayDsgnTemplateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRailwayDsgnTemplateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRailwayDsgnTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRailwayDsgnTemplateDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//DDX_Control(pDX, IDC_GRID, m_Grid);             // associate the grid window with a C++ object
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRailwayDsgnTemplateDlg, CDialog)
	//{{AFX_MSG_MAP(CRailwayDsgnTemplateDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRailwayDsgnTemplateDlg message handlers

BOOL CRailwayDsgnTemplateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_Grid.SubclassDlgItem(IDC_COMBOEDIT, this);

	RECT rect;

	rect.top	= 100;
	rect.bottom = 200;
	rect.left	= 50;
	rect.right	= 200;
	VERIFY(m_Grid.Create(rect, this, 100, WS_VISIBLE|WS_CHILD));

	//IDC_STATIC_DUMMY
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRailwayDsgnTemplateDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
