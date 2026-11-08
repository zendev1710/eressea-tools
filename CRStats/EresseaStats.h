// EresseaStats.h : main header file for the ERESSEASTATS application
//

#if !defined(AFX_ERESSEASTATS_H__A51A579C_5A5C_41F6_BFE7_41CFCCCCDD04__INCLUDED_)
#define AFX_ERESSEASTATS_H__A51A579C_5A5C_41F6_BFE7_41CFCCCCDD04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CEresseaStatsApp:
// See EresseaStats.cpp for the implementation of this class
//

class CEresseaStatsApp : public CWinApp
{
public:
	CEresseaStatsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEresseaStatsApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CEresseaStatsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CString	m_sCRFilename;
	CString m_sHierarchyFilename;

	void loadSettings();
	void saveSettings();

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ERESSEASTATS_H__A51A579C_5A5C_41F6_BFE7_41CFCCCCDD04__INCLUDED_)
