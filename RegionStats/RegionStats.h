// RegionStats.h : main header file for the REGIONSTATS application
//

#if !defined(AFX_REGIONSTATS_H__2F35D70B_A286_49A9_B1E1_C2D2EB020C45__INCLUDED_)
#define AFX_REGIONSTATS_H__2F35D70B_A286_49A9_B1E1_C2D2EB020C45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CRegionStatsApp:
// See RegionStats.cpp for the implementation of this class
//

class CRegionStatsApp : public CWinApp
{
public:
	CRegionStatsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegionStatsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CRegionStatsApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIONSTATS_H__2F35D70B_A286_49A9_B1E1_C2D2EB020C45__INCLUDED_)
