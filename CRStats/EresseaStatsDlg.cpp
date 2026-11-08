// EresseaStatsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EresseaStats.h"
#include "geometry.h"
#include "STRegionsListCtrl.h"
#include "EresseaStatsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

USE_GEOMETRY_TEMPLATES;

TSettings CEresseaStatsDlg::m_settings;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEresseaStatsDlg dialog

CEresseaStatsDlg::CEresseaStatsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEresseaStatsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEresseaStatsDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_bSortNumericOnDiff = CEresseaStatsDlg::m_settings.m_bSortNumericOnDiff;
	m_bRegionsWithUnitsOnly = CEresseaStatsDlg::m_settings.m_bRegionsWithUnitsOnly;
	m_bRegionsSelectedOnly = CEresseaStatsDlg::m_settings.m_bRegionsSelectedOnly;
	m_bWithSeparatorLines = CEresseaStatsDlg::m_settings.m_bWithSeparatorLines;

	BOOL bWithDiffs = CEresseaStatsDlg::m_settings.m_bWithDiffs;

	CRegionItemInfo::m_bWithDiffs = bWithDiffs;
	if (!bWithDiffs) m_bSortNumericOnDiff = FALSE;
	CRegionItemInfo::m_bCompareIntByDiffs = m_bSortNumericOnDiff;
	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_sCRFilename = "";
	m_sHierarchyFilename = "";

}

void CEresseaStatsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEresseaStatsDlg)
	DDX_Control(pDX, IDC_LIST2, m_regions);
	DDX_Check(pDX, IDC_CHECKSORTNUM, m_bSortNumericOnDiff);
	DDX_Check(pDX, IDC_CHECKOCCUPYEDREGIONSONLY, m_bRegionsWithUnitsOnly);
	DDX_Check(pDX, IDC_CHECKSELECTEDREGIONSONLY, m_bRegionsSelectedOnly);
	DDX_Check(pDX, IDC_CHECKSEPARATORLINES, m_bWithSeparatorLines);
	DDX_Text(pDX,IDC_STATIC_REGIONSNUM, m_nNbRegions);
	DDX_Control(pDX,IDC_CHECKSORTNUM,m_ctlSortNum);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECKWITHDIFF, CRegionItemInfo::m_bWithDiffs);

}

BEGIN_MESSAGE_MAP(CEresseaStatsDlg, CDialog)
	//{{AFX_MSG_MAP(CEresseaStatsDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHECKSORTNUM, OnCheckSortnumByDiff)
	ON_BN_CLICKED(IDC_CHECKOCCUPYEDREGIONSONLY, OnCheckOccupyedRegionsOnly)
	ON_BN_CLICKED(IDC_CHECKSEPARATORLINES, OnCheckSeparatorLines)
	ON_BN_CLICKED(IDC_CHECKWITHDIFF, OnCheckWithDiff)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEresseaStatsDlg message handlers

BOOL CEresseaStatsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//AddConstraint(IDOK,CConstraint("X"));
	//AddConstraint(IDCANCEL,CConstraint("X"));

	//AddConstraint(IDC_LIST2,CConstraint("MCX+MCY"));
	AddConstraint(IDC_LIST2,CConstraint("CX+CY"));

	SetWindowText("Region statistics: "+m_sCRFilename);

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	int nNbColumns = m_regions.Build();
	
	RetrieveRegions();

	//done after RetrieveRegions to take into account scrollbars due to items insertion
	m_regions.SetColumnWidth(nNbColumns-1,LVSCW_AUTOSIZE_USEHEADER); //to fill last column until listctrl xmax
	m_regions.EnableSeparatorLines(m_bWithSeparatorLines);

	if (!CRegionItemInfo::m_bWithDiffs)
		m_ctlSortNum.EnableWindow(CRegionItemInfo::m_bWithDiffs);


	UpdateData(FALSE);

	//does not work due to CConstraint !!	
	//setInitialPosition();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEresseaStatsDlg::setInitialPosition()
{
	//CEresseaStatsDlg::m_settings.m_pl.length=0;
	if (CEresseaStatsDlg::m_settings.m_pl.length!=0)
		SetWindowPlacement(&CEresseaStatsDlg::m_settings.m_pl);
}

void CEresseaStatsDlg::RetrieveRegions()
{
	CString terrain("");
	CString name("");

	long i = 0;
	BOOL bRegionHasUnits;
	BOOL bSelected;
	BOOL bAddItem;
	int nIndex;

	CRegionItemInfo::init();

	if (m_sCRFilename.IsEmpty()) return;
	if (m_sHierarchyFilename.IsEmpty()) return;

	if (!m_cr.Loaded())
	{
		m_cr.load(m_sCRFilename,m_sHierarchyFilename,&crdata_ireport_std);
		m_cr_regions = m_cr.regions();
	}

	block * r = m_cr_regions;

	while (r)
	{
		bSelected = FALSE;
		m_cr.gets(r,CR_TERRAIN,terrain);
		if (!m_cr.gets_ansi(r,CR_NAME, name)) 
			name = terrain;
		m_cr.geti(r,CR_REGIONSELECTED,bSelected);
		bRegionHasUnits = (m_cr.units(r)!=NULL);
		name.MakeLower();
		if (!name.IsEmpty() && name.CompareNoCase(CR_REGIONTYPE_OCEAN) && name.CompareNoCase(CR_REGIONTYPE_FIREPROOF_WALL) && name.CompareNoCase(CR_REGIONTYPE_EMPTY))
		{
			CString herb("");
			//CString herbAmount("");
			CString sTerrain(terrain);
			sTerrain.MakeLower();
			if (sTerrain.CompareNoCase(CR_REGIONTYPE_VOLCANO) && sTerrain.CompareNoCase(CR_REGIONTYPE_ACTIVE_VOLCANO)) {
				m_cr.gets(r,CR_HERB,herb);
				if (herb.IsEmpty()) 
				{
					CString sKeyName;
					CString sValue;
					sKeyName.Format("%s.herb",(LPCSTR)name);
					::GetPrivateProfileString("resources",(LPCSTR)sKeyName,"",sValue.GetBuffer(50),50,".\\regionstats.ini");
					sValue.ReleaseBuffer();
					if (!sValue.IsEmpty()) herb = CCR::English2German(sValue); else herb=HERB_UNKNOWN_VALUE;
				}
				//else 
				//	m_cr.gets(r,CR_HERBAMOUNT,herbAmount);
			}
			else
				herb=HERB_NONE_VALUE;

			//if (!name.CompareNoCase("goburmoutain"))
			//	int yuy=3;
			CRegionItemInfo * pItemInfo = new CRegionItemInfo(i,name,terrain,herb/*,herbAmount*/,bRegionHasUnits,bSelected);
			bAddItem = TRUE;
			if (!bRegionHasUnits)
			{ 
				m_regionsWithNoUnit.AddTail(pItemInfo);
				if (m_bRegionsWithUnitsOnly) {nIndex = -1; bAddItem = FALSE;}
			}

			if (bAddItem)
			{
				nIndex = m_regions.AddItem(name, i, 0, -1, -1,(long)pItemInfo);
				i++;
			}
			pItemInfo->set(r,nIndex,m_regions,m_cr);
		}
		r = r->next;
	}

	CRegionItemInfo::after();

	m_regions.SortColumn(CEresseaStatsDlg::m_settings.m_nSortCol,(ESortType)CEresseaStatsDlg::m_settings.m_nSortType);

	m_nNbRegions = i;
	m_nNbRegionsWithUnits = i;
	if (!m_bRegionsWithUnitsOnly) m_nNbRegions+=m_regionsWithNoUnit.GetCount();
}

void CEresseaStatsDlg::RefreshRegionsList(BOOL bResort)
{
	m_nNbRegions = m_nNbRegionsWithUnits;
	if (!m_bRegionsWithUnitsOnly) m_nNbRegions += m_regionsWithNoUnit.GetCount();
	m_regions.Rebuild(m_regionsWithNoUnit, m_nNbRegionsWithUnits, m_bRegionsWithUnitsOnly, m_bRegionsSelectedOnly, bResort);
}

void CEresseaStatsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CEresseaStatsDlg::SaveRegionsInfos()
{
	int nbItems = m_regions.GetItemCount();
	for (int i=0;i<nbItems;i++)
	{
		CRegionItemInfo *p=(CRegionItemInfo *)m_regions.GetItemData(i);
		if (p)
		{
		
			CString herb = p->herb();
			CString regionname = p->name();
			if (!herb.IsEmpty() && herb.CompareNoCase(HERB_UNKNOWN_VALUE)) 
			{
				CString sKeyName;
				sKeyName.Format("%s.herb",(LPCSTR)regionname);
				::WritePrivateProfileString("resources",(LPCSTR)sKeyName,herb,".\\regionstats.ini");
			}
		}
	}
}

void CEresseaStatsDlg::SaveSettings()
{
	// columns width
	for (int i=0;i<COL_NBMAX;i++)
		CSTRegionsListCtrl::columnInfos[i].m_nColumnCurrentWidth=m_regions.GetColumnWidth(i);

	CEresseaStatsDlg::m_settings.m_bRegionsSelectedOnly=m_bRegionsSelectedOnly;
	CEresseaStatsDlg::m_settings.m_bRegionsWithUnitsOnly=m_bRegionsWithUnitsOnly;
	CEresseaStatsDlg::m_settings.m_bSortNumericOnDiff = m_bSortNumericOnDiff;
	CEresseaStatsDlg::m_settings.m_bWithDiffs = CRegionItemInfo::m_bWithDiffs;
	CEresseaStatsDlg::m_settings.m_bWithSeparatorLines = m_bWithSeparatorLines;
	CEresseaStatsDlg::m_settings.m_nSortType = m_regions.GetSortType();
	CEresseaStatsDlg::m_settings.m_nSortCol = m_regions.GetSortedColumn();

	GetWindowPlacement(&CEresseaStatsDlg::m_settings.m_pl);
}

void CEresseaStatsDlg::OnDestroy()
{
	SaveRegionsInfos();
	SaveSettings();

	WinHelp(0L, HELP_QUIT);
	CDialog::OnDestroy();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEresseaStatsDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEresseaStatsDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CEresseaStatsDlg::OnCheckSortnumByDiff() 
{
	m_bSortNumericOnDiff=!m_bSortNumericOnDiff;
	CRegionItemInfo::m_bCompareIntByDiffs = m_bSortNumericOnDiff;
	m_regions.RebuildByDiffs();
	//if (m_regions.IsNumericColumn())
		m_regions.ReSort();
}

void CEresseaStatsDlg::OnCheckOccupyedRegionsOnly() 
{
	m_bRegionsWithUnitsOnly	= !m_bRegionsWithUnitsOnly;
	RefreshRegionsList(TRUE);
	UpdateData(FALSE);
}

void CEresseaStatsDlg::OnCheckSelectedRegionsOnly() 
{
	m_bRegionsSelectedOnly	= !m_bRegionsSelectedOnly;
	RefreshRegionsList(TRUE);
	UpdateData(FALSE);
}

void CEresseaStatsDlg::OnCheckSeparatorLines() 
{
	m_bWithSeparatorLines = !m_bWithSeparatorLines;
	m_regions.EnableSeparatorLines(m_bWithSeparatorLines);
	m_regions.Invalidate();
}

void CEresseaStatsDlg::OnCheckWithDiff() 
{
	CRegionItemInfo::m_bWithDiffs = !CRegionItemInfo::m_bWithDiffs;
	m_ctlSortNum.EnableWindow(CRegionItemInfo::m_bWithDiffs);
	m_regions.RebuildByDiffs();	
}
