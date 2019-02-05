// RoadDesignDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ContourApp.h"
#include "RoadDesignDataDlg.h"
#include "ResourceHelper.h"
#include "CrossSectionProfileDefinition.h"
#include "Util.h"
#include "DrawUtil.h"
#include "VAlignment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" HWND adsw_acadMainWnd();

/////////////////////////////////////////////////////////////////////////////
// CRoadDesignDataDlg dialog


CRoadDesignDataDlg::CRoadDesignDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRoadDesignDataDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRoadDesignDataDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pGridCtrlSectionDefinition = 0L;
	m_pGridCtrlSectionSetting = 0L;
	m_pGridCtrlVAlignmentData = 0L;

	m_pVAlignment = 0L;
}
CRoadDesignDataDlg::~CRoadDesignDataDlg()
{
	delete m_pGridCtrlSectionDefinition;
	delete m_pGridCtrlSectionSetting;
	delete m_pGridCtrlVAlignmentData;
	{//
		int i;
		
		for(i = 0; i < m_arrayOfSectionDefinition.GetSize(); i++)
		{
			delete m_arrayOfSectionDefinition[i];
		}
		m_arrayOfSectionDefinition.RemoveAll();
		
		for(i = 0; i < m_arrayOfSectionSettings.GetSize(); i++)
		{
			delete m_arrayOfSectionSettings[i];
		}
		m_arrayOfSectionSettings.RemoveAll();	
	}
}


void CRoadDesignDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRoadDesignDataDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRoadDesignDataDlg, CDialog)
	//{{AFX_MSG_MAP(CRoadDesignDataDlg)
	ON_BN_CLICKED(IDC_BUTTON_PICK_VALIGNMENT, OnButtonPickValignment)
	ON_BN_CLICKED(IDC_BUTTON_PICK_PLINE, OnButtonPickPline)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoadDesignDataDlg message handlers

BOOL CRoadDesignDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	
	// TODO: Add extra initialization here
	CRect rect;
	CString strTmp;
	int i;
	
	{//
		GetDlgItem(IDC_STATIC_DEFINE_DSGN_SECTION)->GetWindowRect(rect);
		GetDlgItem(IDC_STATIC_DEFINE_DSGN_SECTION)->ShowWindow(SW_HIDE);
		ScreenToClient(rect);

		m_pGridCtrlSectionDefinition = new CGridCtrl(1, 5, 1, 0);
		m_pGridCtrlSectionDefinition->Create(rect, this, 100);
		m_pGridCtrlSectionDefinition->ShowWindow(SW_SHOW);
    
		m_pGridCtrlSectionDefinition->SetItemText(0, 0, "Section Name");
		m_pGridCtrlSectionDefinition->SetItemText(0, 1, "Type");
		m_pGridCtrlSectionDefinition->SetItemText(0, 2, "Width");
		m_pGridCtrlSectionDefinition->SetItemText(0, 3, "Length");
		m_pGridCtrlSectionDefinition->SetItemText(0, 4, "# of Vertices");

		for(i = 0; i < m_arrayOfSectionDefinition.GetSize(); i++)
		{//Code for New Row
			const CCrossSectionProfileDefinition* pRec = m_arrayOfSectionDefinition[i];

			
			m_pGridCtrlSectionDefinition->InsertRow(pRec->getName());
			
			m_pGridCtrlSectionDefinition->SetItemText(i+1, 2, RTOS(pRec->getXExtent()));
			m_pGridCtrlSectionDefinition->SetItemText(i+1, 3, RTOS(pRec->getLength()));
			strTmp.Format("%d", pRec->getVertices().length());
			m_pGridCtrlSectionDefinition->SetItemText(i+1, 4, strTmp);
			
			m_pGridCtrlSectionDefinition->SetCellType(i+1, 1, RUNTIME_CLASS(CGridCellCombo));
				
			CStringArray options;
			options.Add(_T("Cut"));
			options.Add(_T("Fill"));

			CGridCellCombo *pCell = (CGridCellCombo*) m_pGridCtrlSectionDefinition->GetCell(i+1, 1);
			pCell->SetOptions(options);
			pCell->SetStyle(CBS_DROPDOWNLIST); //CBS_DROPDOWN, CBS_DROPDOWNLIST, CBS_SIMPLE

			m_pGridCtrlSectionDefinition->SetItemState(i+1, 2, m_pGridCtrlSectionDefinition->GetItemState(i+1, 2) | GVIS_READONLY);
			m_pGridCtrlSectionDefinition->SetItemState(i+1, 3, m_pGridCtrlSectionDefinition->GetItemState(i+1, 3) | GVIS_READONLY);
			m_pGridCtrlSectionDefinition->SetItemState(i+1, 4, m_pGridCtrlSectionDefinition->GetItemState(i+1, 4) | GVIS_READONLY);
		}
	}
	{//
		GetDlgItem(IDC_STATIC_SET_DSGN_SECTION)->GetWindowRect(rect);
		GetDlgItem(IDC_STATIC_SET_DSGN_SECTION)->ShowWindow(SW_HIDE);
		ScreenToClient(rect);

		m_pGridCtrlSectionSetting = new CGridCtrl(1, 3, 1, 1);
		m_pGridCtrlSectionSetting->Create(rect, this, 100);
		m_pGridCtrlSectionSetting->ShowWindow(SW_SHOW);
    
		m_pGridCtrlSectionSetting->SetItemText(0, 0, "Start Chainage");
		m_pGridCtrlSectionSetting->SetItemText(0, 1, "End Chainage");
		m_pGridCtrlSectionSetting->SetItemText(0, 2, "Section Name");
	}
	{//
		GetDlgItem(IDC_STATIC_VALIGNMENT_DATA)->GetWindowRect(rect);
		GetDlgItem(IDC_STATIC_VALIGNMENT_DATA)->ShowWindow(SW_HIDE);
		ScreenToClient(rect);

		m_pGridCtrlVAlignmentData = new CGridCtrl(1, 3, 1, 1);
		m_pGridCtrlVAlignmentData->Create(rect, this, 100);
		m_pGridCtrlVAlignmentData->ShowWindow(SW_SHOW);
    
		m_pGridCtrlVAlignmentData->SetItemText(0, 0, "X");
		m_pGridCtrlVAlignmentData->SetItemText(0, 1, "Z");
		m_pGridCtrlVAlignmentData->SetItemText(0, 2, "Raddi");

		if(m_pVAlignment != 0L)
		{
			const AcGePoint3dArray& arrayVertices = m_pVAlignment->GetPointData();
			const AcGeDoubleArray&	arrayRadius = m_pVAlignment->GetRadius();
			for(i = 0; i < arrayVertices.length(); i++)
			{//Code for New Row
				const AcGePoint3d& Rec = arrayVertices[i];
				
				//m_pGridCtrlVAlignmentData->SetFixedColumnCount(3);


				m_pGridCtrlVAlignmentData->InsertRow(RTOS(Rec.x));				
				m_pGridCtrlVAlignmentData->SetItemText(i+1, 1, RTOS(Rec.y));				
				m_pGridCtrlVAlignmentData->SetItemText(i+1, 2, RTOS(arrayRadius[i]));

				m_pGridCtrlVAlignmentData->SetItemState(i+1, 0, m_pGridCtrlVAlignmentData->GetItemState(i+1, 0) | GVIS_READONLY);
				m_pGridCtrlVAlignmentData->SetItemState(i+1, 1, m_pGridCtrlVAlignmentData->GetItemState(i+1, 1) | GVIS_READONLY);
				m_pGridCtrlVAlignmentData->SetItemState(i+1, 2, m_pGridCtrlVAlignmentData->GetItemState(i+1, 2) | GVIS_READONLY);
			}
		}
	}
	return TRUE;
}

void CRoadDesignDataDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CRoadDesignDataDlg::OnButtonPickValignment() 
{
	EndDialog(IDC_BUTTON_PICK_VALIGNMENT);
}

void CRoadDesignDataDlg::OnButtonPickPline() 
{
	EndDialog(IDC_BUTTON_PICK_PLINE);
}

///////////////////////////////////////////////////////////////////
void CRoadDesignDataDlg::GetData()
{
	CTemporaryResourceOverride ResOverride;
	CRoadDesignDataDlg Dlg(CWnd::FromHandle(adsw_acadMainWnd()));
	UINT iResult;
	
	do
	{
		iResult = Dlg.DoModal();
		if((iResult == IDOK) || (iResult == IDCANCEL))
		{
			break;
		}

		switch(iResult)
		{
		case IDC_BUTTON_PICK_PLINE:
			{
				CCrossSectionProfileDefinition* pRec;
				
				pRec = CCrossSectionProfileDefinition::Define();
				if(0L != pRec)
				{
					Dlg.m_arrayOfSectionDefinition.Add(pRec);
				}
			}
			break;
		case IDC_BUTTON_PICK_VALIGNMENT:
			{
				CVAlignment* pRec;
				
				pRec = CVAlignmentJig::GetVAlignmentEntity("\nSelect Vertical Alignment Entity:");
				Dlg.m_pVAlignment = pRec;
			}
			break;
		case IDOK:
			break;
		case IDCANCEL:
			break;
		default:
			break;
		}
	}
	while(true);
}
