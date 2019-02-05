// DefineBoundaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ContourApp.h"
#include "DefineBoundaryDlg.h"
#include "ResourceHelper.h"
#include "DwgParamObj.h"

extern "C" HWND adsw_acadMainWnd();

/////////////////////////////////////////////////////////////////////////////
// CDefineBoundaryDlg dialog

CDefineBoundaryDlg::CDefineBoundaryDlg(BOUNDARYDEF* pBoundaryDef, CWnd* pParent /*=NULL*/)
	: CDialog(CDefineBoundaryDlg::IDD, pParent), m_pBoundaryDef(pBoundaryDef)
{
	//{{AFX_DATA_INIT(CDefineBoundaryDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDefineBoundaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefineBoundaryDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefineBoundaryDlg, CDialog)
	//{{AFX_MSG_MAP(CDefineBoundaryDlg)
	ON_BN_CLICKED(IDC_BUTTON_NEW_HOLE, OnButtonNewHole)
	ON_BN_CLICKED(IDC_BUTTON_DEL_HOLE, OnButtonDelHole)
	ON_BN_CLICKED(IDC_BUTTON_DEL_ALL_HOLES, OnButtonDelAllHoles)
	ON_BN_CLICKED(IDC_BUTTON_SEL_BOUNDARY, OnButtonSelBoundary)
	ON_BN_CLICKED(IDC_CHECK_DEFINE_BOUNDARY, OnCheckDefineBoundary)
	ON_BN_CLICKED(IDC_CHECK_CONSIDER_ALL_PTS, OnCheckConsiderAllPts)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_BOUNDARY, OnButtonViewBoundary)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_HOLE, OnButtonViewHole)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefineBoundaryDlg message handlers

BOOL CDefineBoundaryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDefineBoundaryDlg::OnButtonNewHole() 
{
	EndDialog(IDC_BUTTON_NEW_HOLE);
}

void CDefineBoundaryDlg::OnButtonDelHole() 
{
	// TODO: Add your control notification handler code here
	
}

void CDefineBoundaryDlg::OnButtonDelAllHoles() 
{
	// TODO: Add your control notification handler code here
	
}

void CDefineBoundaryDlg::OnButtonSelBoundary() 
{
	EndDialog(IDC_BUTTON_SEL_BOUNDARY);
}

void CDefineBoundaryDlg::OnCheckDefineBoundary() 
{
	// TODO: Add your control notification handler code here
	
}

void CDefineBoundaryDlg::OnCheckConsiderAllPts() 
{
	// TODO: Add your control notification handler code here
	
}

void CDefineBoundaryDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CDefineBoundaryDlg::OnButtonViewBoundary() 
{
	// TODO: Add your control notification handler code here
	
}

void CDefineBoundaryDlg::OnButtonViewHole() 
{
	// TODO: Add your control notification handler code here
	
}


///
void CDefineBoundaryDlg::DefineBoundary()
{
	CWnd* pAcadWnd;
	UINT iRetCode;
	BOOL bRet;
	BOUNDARYDEF BoundaryDef;
	
	pAcadWnd = CWnd::FromHandle(adsw_acadMainWnd());
	
	for(bRet = FALSE;;)
	{
	
		{//
			CTemporaryResourceOverride ResOverride;
		
			CDefineBoundaryDlg Dlg(&BoundaryDef, pAcadWnd);
			iRetCode = Dlg.DoModal();
		}

		switch(iRetCode)
		{
		case IDOK:
			{//Save in DwgParamObject......
				bRet = TRUE;
				goto CLEANUP;
			}
			break;
		case IDCANCEL:
			{//
				goto CLEANUP;
			}
			break;
		case IDC_BUTTON_NEW_HOLE:
			ads_printf("\n IDC_BUTTON_NEW_HOLE \n");
			break;
		case IDC_BUTTON_SEL_BOUNDARY:
			ads_printf("\n IDC_BUTTON_SEL_BOUNDARY \n");
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////
	CLEANUP :
	{//Deallocation
		//Deallocate....
		int i;
		for(i = 0; i < BoundaryDef.arrayHoleDef.GetSize(); i++)
		{
			HOLEDEF* pHole;
			
			pHole = BoundaryDef.arrayHoleDef[i];
			delete pHole;
		}
		BoundaryDef.arrayHoleDef.RemoveAll();
	}
}
