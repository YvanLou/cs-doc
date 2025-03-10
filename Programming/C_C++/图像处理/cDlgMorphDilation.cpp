// cDlgMorphDilation.cpp : implementation file
//

#include "stdafx.h"
#include "ch1_1.h"
#include "cDlgMorphDilation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cDlgMorphDilation dialog


cDlgMorphDilation::cDlgMorphDilation(CWnd* pParent /*=NULL*/)
	: CDialog(cDlgMorphDilation::IDD, pParent)
{
	//{{AFX_DATA_INIT(cDlgMorphDilation)
	m_nMode = -1;
	m_nStructure1 = -1;
	m_nStructure2 = -1;
	m_nStructure3 = -1;
	m_nStructure4 = -1;
	m_nStructure5 = -1;
	m_nStructure6 = -1;
	m_nStructure7 = -1;
	m_nStructure8 = -1;
	m_nStructure9 = -1;
	//}}AFX_DATA_INIT
}


void cDlgMorphDilation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cDlgMorphDilation)
	DDX_Control(pDX, IDC_Structure9, m_Control9);
	DDX_Control(pDX, IDC_Structure8, m_Control8);
	DDX_Control(pDX, IDC_Structure7, m_Control7);
	DDX_Control(pDX, IDC_Structure6, m_Control6);
	DDX_Control(pDX, IDC_Structure5, m_Control5);
	DDX_Control(pDX, IDC_Structure4, m_Control4);
	DDX_Control(pDX, IDC_Structure3, m_Control3);
	DDX_Control(pDX, IDC_Structure2, m_Control2);
	DDX_Control(pDX, IDC_Structure1, m_Control1);
	DDX_Radio(pDX, IDC_Hori, m_nMode);
	DDX_Radio(pDX, IDC_Structure1, m_nStructure1);
	DDX_Radio(pDX, IDC_Structure2, m_nStructure2);
	DDX_Radio(pDX, IDC_Structure3, m_nStructure3);
	DDX_Radio(pDX, IDC_Structure4, m_nStructure4);
	DDX_Radio(pDX, IDC_Structure5, m_nStructure5);
	DDX_Radio(pDX, IDC_Structure6, m_nStructure6);
	DDX_Radio(pDX, IDC_Structure7, m_nStructure7);
	DDX_Radio(pDX, IDC_Structure8, m_nStructure8);
	DDX_Radio(pDX, IDC_Structure9, m_nStructure9);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cDlgMorphDilation, CDialog)
	//{{AFX_MSG_MAP(cDlgMorphDilation)
	ON_BN_CLICKED(IDC_Hori, OnHori)
	ON_BN_CLICKED(IDC_Vert, OnVert)
	ON_BN_CLICKED(IDC_custom, Oncustom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cDlgMorphDilation message handlers
void cDlgMorphDilation::Oncustom() 
{
	m_Control1.EnableWindow(TRUE);
	m_Control2.EnableWindow(TRUE);
	m_Control3.EnableWindow(TRUE);
	m_Control4.EnableWindow(TRUE);
	m_Control5.EnableWindow(TRUE);
	m_Control6.EnableWindow(TRUE);
	m_Control7.EnableWindow(TRUE);
	m_Control8.EnableWindow(TRUE);
	m_Control9.EnableWindow(TRUE);
	
}

void cDlgMorphDilation::OnVert() 
{
	m_Control1.EnableWindow(FALSE);
	m_Control2.EnableWindow(FALSE);
	m_Control3.EnableWindow(FALSE);
	m_Control4.EnableWindow(FALSE);
	m_Control5.EnableWindow(FALSE);
	m_Control6.EnableWindow(FALSE);
	m_Control7.EnableWindow(FALSE);
	m_Control8.EnableWindow(FALSE);
	m_Control9.EnableWindow(FALSE);
	
}

void cDlgMorphDilation::OnHori() 
{
	m_Control1.EnableWindow(FALSE);
	m_Control2.EnableWindow(FALSE);
	m_Control3.EnableWindow(FALSE);
	m_Control4.EnableWindow(FALSE);
	m_Control5.EnableWindow(FALSE);
	m_Control6.EnableWindow(FALSE);
	m_Control7.EnableWindow(FALSE);
	m_Control8.EnableWindow(FALSE);
	m_Control9.EnableWindow(FALSE);
	
}

