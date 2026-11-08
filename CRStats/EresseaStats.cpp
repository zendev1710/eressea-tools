// EresseaStats.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "wSTCmdLine.h"
#include "geometry.h"
#include "EresseaStats.h"
#include "EresseaStatsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GENERAL_SECTION		"columns"
#define MISC_SECTION		"misc"
#define PREV_SECTION		"prev"
#define SETTINGS_SECTION	"settings"


/////////////////////////////////////////////////////////////////////////////
// CEresseaStatsApp

BEGIN_MESSAGE_MAP(CEresseaStatsApp, CWinApp)
	//{{AFX_MSG_MAP(CEresseaStatsApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	// ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEresseaStatsApp construction

CEresseaStatsApp::CEresseaStatsApp():
	m_sCRFilename(""),
	m_sHierarchyFilename("")
{
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CEresseaStatsApp object

CEresseaStatsApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CEresseaStatsApp initialization

BOOL CEresseaStatsApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifndef _AFXDLL
	//Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CSTCmdLine cmdInfo;
	ParseCommandLine(cmdInfo);
	m_sCRFilename=cmdInfo.CRFilename();
	m_sHierarchyFilename = cmdInfo.HierarchyFilename();

	loadSettings();  // Load INI file options (including MRU)

	CGeometryWnd<CEresseaStatsDlg> dlg;
	m_pMainWnd = &dlg;

	dlg.m_sCRFilename = m_sCRFilename;
	dlg.m_sHierarchyFilename = m_sHierarchyFilename;

	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CEresseaStatsApp::loadSettings()
{
	CString sEntryW("");
	for (int i=0;i<COL_NBMAX;i++)
	{
		sEntryW.Format("column%d.width",i+1);
		CSTRegionsListCtrl::columnInfos[i].m_nColumnCurrentWidth=GetProfileInt(GENERAL_SECTION, sEntryW, CSTRegionsListCtrl::columnInfos[i].m_nColumnDefaultWidth);
	}
	if (m_sCRFilename.IsEmpty()) m_sCRFilename=GetProfileString(PREV_SECTION,"CR",m_sCRFilename);
	if (m_sHierarchyFilename.IsEmpty()) m_sHierarchyFilename=GetProfileString(PREV_SECTION,"Hierarchy",m_sHierarchyFilename);

	CEresseaStatsDlg::m_settings.m_bRegionsWithUnitsOnly = GetProfileInt(MISC_SECTION, "RegionsWithUnitsOnly", TRUE);
	CEresseaStatsDlg::m_settings.m_bRegionsSelectedOnly = GetProfileInt(MISC_SECTION, "RegionsSelectedOnly", TRUE);
	CEresseaStatsDlg::m_settings.m_bWithSeparatorLines = GetProfileInt(MISC_SECTION, "WithSeparatorLines", TRUE);
	CEresseaStatsDlg::m_settings.m_bWithDiffs = GetProfileInt(MISC_SECTION, "WithDiffs", FALSE);
	CEresseaStatsDlg::m_settings.m_bSortNumericOnDiff = GetProfileInt(MISC_SECTION, "SortOnNumericDiff", FALSE);
	CEresseaStatsDlg::m_settings.m_nSortType = GetProfileInt(MISC_SECTION, "SortType", SORT_ASC);
	CEresseaStatsDlg::m_settings.m_nSortCol = GetProfileInt(MISC_SECTION, "SortCol", 0);

	CEresseaStatsDlg::m_settings.m_pl.length=GetProfileInt(SETTINGS_SECTION, "pl.length", 0);
	if (CEresseaStatsDlg::m_settings.m_pl.length!=0)
	{
		CEresseaStatsDlg::m_settings.m_pl.flags=GetProfileInt(SETTINGS_SECTION, "pl.flags", 0);
		CEresseaStatsDlg::m_settings.m_pl.ptMaxPosition.x=GetProfileInt(SETTINGS_SECTION, "pl.ptMaxPosition.x", 0);
		CEresseaStatsDlg::m_settings.m_pl.ptMaxPosition.y=GetProfileInt(SETTINGS_SECTION, "pl.ptMaxPosition.y", 0);
		CEresseaStatsDlg::m_settings.m_pl.ptMinPosition.x=GetProfileInt(SETTINGS_SECTION, "pl.ptMinPosition.x", 0);
		CEresseaStatsDlg::m_settings.m_pl.ptMinPosition.y=GetProfileInt(SETTINGS_SECTION, "pl.ptMinPosition.y", 0);
		CEresseaStatsDlg::m_settings.m_pl.rcNormalPosition.bottom=GetProfileInt(SETTINGS_SECTION, "pl.rcNormalPosition.bottom", 0);
		CEresseaStatsDlg::m_settings.m_pl.rcNormalPosition.left=GetProfileInt(SETTINGS_SECTION, "pl.rcNormalPosition.left", 0);
		CEresseaStatsDlg::m_settings.m_pl.rcNormalPosition.top=GetProfileInt(SETTINGS_SECTION, "pl.rcNormalPosition.top", 0);
		CEresseaStatsDlg::m_settings.m_pl.rcNormalPosition.right=GetProfileInt(SETTINGS_SECTION, "pl.rcNormalPosition.right", 0);
		CEresseaStatsDlg::m_settings.m_pl.showCmd=GetProfileInt(SETTINGS_SECTION, "pl.showCmd", 0);
	}

}

void CEresseaStatsApp::saveSettings()
{
	CString sEntryW("");
	for (int i=0;i<COL_NBMAX;i++)
	{
		sEntryW.Format("column%d.width",i+1);
		WriteProfileInt(GENERAL_SECTION, sEntryW, CSTRegionsListCtrl::columnInfos[i].m_nColumnCurrentWidth);
	}
	WriteProfileString(PREV_SECTION,"CR",m_sCRFilename);
	WriteProfileString(PREV_SECTION,"Hierarchy",m_sHierarchyFilename);

	WriteProfileInt(MISC_SECTION, "RegionsWithUnitsOnly", CEresseaStatsDlg::m_settings.m_bRegionsWithUnitsOnly);
	WriteProfileInt(MISC_SECTION, "RegionsSelectedOnly", CEresseaStatsDlg::m_settings.m_bRegionsSelectedOnly);
	WriteProfileInt(MISC_SECTION, "WithSeparatorLines", CEresseaStatsDlg::m_settings.m_bWithSeparatorLines);
	WriteProfileInt(MISC_SECTION, "WithDiffs", CEresseaStatsDlg::m_settings.m_bWithDiffs);
	WriteProfileInt(MISC_SECTION, "SortOnNumericDiff", CEresseaStatsDlg::m_settings.m_bSortNumericOnDiff);
	WriteProfileInt(MISC_SECTION, "SortType", CEresseaStatsDlg::m_settings.m_nSortType);
	WriteProfileInt(MISC_SECTION, "SortCol", CEresseaStatsDlg::m_settings.m_nSortCol);

	if (CEresseaStatsDlg::m_settings.m_pl.length!=0)
	{
		WriteProfileInt(SETTINGS_SECTION, "pl.length", CEresseaStatsDlg::m_settings.m_pl.length);
		WriteProfileInt(SETTINGS_SECTION, "pl.flags", CEresseaStatsDlg::m_settings.m_pl.flags);
		WriteProfileInt(SETTINGS_SECTION, "pl.ptMaxPosition.x", CEresseaStatsDlg::m_settings.m_pl.ptMaxPosition.x);
		WriteProfileInt(SETTINGS_SECTION, "pl.ptMaxPosition.y", CEresseaStatsDlg::m_settings.m_pl.ptMaxPosition.y);
		WriteProfileInt(SETTINGS_SECTION, "pl.ptMinPosition.x", CEresseaStatsDlg::m_settings.m_pl.ptMinPosition.x);
		WriteProfileInt(SETTINGS_SECTION, "pl.ptMinPosition.y", CEresseaStatsDlg::m_settings.m_pl.ptMinPosition.y);
		WriteProfileInt(SETTINGS_SECTION, "pl.rcNormalPosition.bottom", CEresseaStatsDlg::m_settings.m_pl.rcNormalPosition.bottom);
		WriteProfileInt(SETTINGS_SECTION, "pl.rcNormalPosition.left", CEresseaStatsDlg::m_settings.m_pl.rcNormalPosition.left);
		WriteProfileInt(SETTINGS_SECTION, "pl.rcNormalPosition.top", CEresseaStatsDlg::m_settings.m_pl.rcNormalPosition.top);
		WriteProfileInt(SETTINGS_SECTION, "pl.rcNormalPosition.right", CEresseaStatsDlg::m_settings.m_pl.rcNormalPosition.right);
		WriteProfileInt(SETTINGS_SECTION, "pl.showCmd", CEresseaStatsDlg::m_settings.m_pl.showCmd);
	}
}

int CEresseaStatsApp::ExitInstance() 
{
	saveSettings();
	return CWinApp::ExitInstance();
}
