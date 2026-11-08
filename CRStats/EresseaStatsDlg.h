// EresseaStatsDlg.h : header file
//

#if !defined(AFX_ERESSEASTATSDLG_H__BE65441F_792D_47AF_B8F6_CC37847641EF__INCLUDED_)
#define AFX_ERESSEASTATSDLG_H__BE65441F_792D_47AF_B8F6_CC37847641EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StatsCR.h"
#include "wLizard.h"
#include "geometry.h"
#include "STRegionsListCtrl.h"

USE_GEOMETRY_TEMPLATES;

typedef struct TSettings
{
	BOOL	m_bSortNumericOnDiff;
	BOOL	m_bRegionsWithUnitsOnly;
	BOOL	m_bRegionsSelectedOnly;
	BOOL	m_bWithSeparatorLines;
	BOOL	m_bWithDiffs;
	UINT	m_nSortType;
	UINT	m_nSortCol;
	WINDOWPLACEMENT m_pl;
} TSettings;

#define HERB_UNKNOWN_VALUE "{?}"
#define HERB_NONE_VALUE "{none}"

/////////////////////////////////////////////////////////////////////////////
// CEresseaStatsDlg dialog

class CEresseaStatsDlg  : public CDialog
{
	DECLARE_GEOMETRY_RTSUPPORT()

// Construction
public:
	CEresseaStatsDlg(CWnd* pParent = NULL);	// standard constructor

	CString m_sCRFilename;
	CString m_sHierarchyFilename;

// Dialog Data
	//{{AFX_DATA(CEresseaStatsDlg)
	enum { IDD = IDD_ERESSEASTATS_DIALOG };
	CSTRegionsListCtrl m_regions;
	BOOL	m_bSortNumericOnDiff;
	BOOL	m_bRegionsWithUnitsOnly;
	BOOL	m_bRegionsSelectedOnly;
	BOOL	m_bWithSeparatorLines;
	CWnd	m_ctlSortNum;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEresseaStatsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:

	static TSettings m_settings;
	
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEresseaStatsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCheckSortnumByDiff();
	afx_msg void OnCheckOccupyedRegionsOnly();
	afx_msg void OnCheckSelectedRegionsOnly();
	afx_msg void OnCheckSeparatorLines();
	afx_msg void OnCheckWithDiff();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int m_nNbRegionsWithUnits;
	int m_nNbRegions;
	CStatsCR m_cr;
	CRegionItemInfoList m_regionsWithNoUnit;
	block * m_cr_regions;
	
	void SaveSettings();
	void SaveRegionsInfos();

	void RetrieveRegions();
	void RefreshRegionsList(BOOL bResort);

	void setInitialPosition();

public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ERESSEASTATSDLG_H__BE65441F_792D_47AF_B8F6_CC37847641EF__INCLUDED_)
