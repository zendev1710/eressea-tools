#if !defined(AFX_STREGIONSLISTCTRL_H__0E84D351_2E4C_11D5_AE65_0050041B40F1__INCLUDED_)
#define AFX_STREGIONSLISTCTRL_H__0E84D351_2E4C_11D5_AE65_0050041B40F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// STRegionsListCtrl.h : header file
//

#include "crdata.h"
#include "wLizard.h"
#include "Eressea.h"
#include "CR.h"
#include "StatsCR.h"
#include "wSTCVItemInfo.h"
#include "wSTCVSortedListCtrl.h"

class CSTRegionsListCtrl;
class CRegionItemInfoList;

typedef enum EValueKind
{
	V_AMOUNT=0,
	V_DELTA=1,
} EValueKind;

#define V_NB	(V_DELTA+1)

////////

typedef enum EColumnsIndex {
	COL_NAME = 0,
	COL_TYPE,
	COL_HERB,
	COL_CASTLESIZE,
	COL_STONES,
	COL_IRONS,
	COL_LAEN,
	COL_TREES,
	COL_YOUNGTREES,
	COL_HORSES,
	COL_PEASANTS,
	COL_MAXTAX,
	COL_MAXENTERTAIN,
	COL_MAXTRADE,
	COL_SILVER
} EColumnsIndex;

typedef enum ESetType {
	SET_NONE = 0,
	SET_RESOURCE = 1,
	SET_RESOURCE_WITH_SKILL = 2,
	SET_SILVER = 3
} ESetType;

#define COL_NBMAX (COL_SILVER+1)
#define COL_FIRSTNUM (COL_HERB+1)

////////

typedef struct TColumnInfos
{
	CString		m_sColumnTitle;
	CString		m_sNumericCREntry; // empty only for column with not numeric values
	UINT		m_nColumnDefaultWidth;		
	UINT		m_nColumnCurrentWidth;
	ESetType	m_nSetType;

public:
	TColumnInfos::TColumnInfos(CString sColumnTitle, CString sNumericCREntry, UINT	nColumnDefaultWidth, UINT nColumnCurrentWidth, ESetType nSetType=SET_NONE):
	m_sColumnTitle(sColumnTitle),m_sNumericCREntry(sNumericCREntry),m_nColumnDefaultWidth(nColumnDefaultWidth),m_nColumnCurrentWidth(nColumnCurrentWidth),m_nSetType(nSetType){}
} TColumnInfos;


////////////

class CRegionItemInfo  : public CItemInfo
{
public:
	virtual ~CRegionItemInfo() {};
	CRegionItemInfo (int iItem, const CString & strName, const CString & strTerrain, const CString & strHerb/*, const CString & strHerbAmount*/, BOOL bHasUnits, BOOL bSelected);

	CString name() const {return m_strName;}
	CString type() const {return m_strType;}
	CString herb() const {return m_strHerb;}

	BOOL HasSpecial(int nCol) const;
	int resourceLevelFromValue(EColumnsIndex nCol, int nAmount) const;

	BOOL CRSelected() const { return m_bCRSelected;}
	int value(EColumnsIndex nIndex, BOOL bPrev=FALSE)const { return m_nValues[nIndex][bPrev?V_DELTA:V_AMOUNT];}

	void set(block *r, int nIndex, CSTRegionsListCtrl & regions, CStatsCR & cr);

public:

	void addToList(int nIndex, CSTRegionsListCtrl & regions) const;
	CString IntToStr(EColumnsIndex index) const;

	static BOOL m_bCompareIntByDiffs;
	static BOOL m_bWithDiffs;
	static BOOL m_bWithIndent;
	static UINT m_nMaxDigits[COL_NBMAX][V_NB];
	static UINT m_nMaxValues[COL_NBMAX][V_NB];

	static void init();
	static void after();

private:

	CString m_strName;
	CString m_strType;
	CString m_strHerb;
	BOOL m_bHasUnits;
	BOOL m_bMallorn;
	BOOL m_bCRSelected;

	int m_nValues[COL_NBMAX][V_NB];

	void checkMax(EColumnsIndex nCol) const;
	int taxRatioFromCastle() {	return e_taxationCoef(m_nValues[COL_CASTLESIZE][0]);}
};

DECLARE_LIST(CRegionItemInfo)

/////////////////////////////////////////////////////////////////////////////
// CSTRegionsListCtrl window

class CSTRegionsListCtrl : public CSTCVSortedListCtrl
{
// Construction
public:
	CSTRegionsListCtrl();

	virtual int CompareItems(CItemInfo *pItemInfo1, CItemInfo *pItemInfo2);

// Attributes
public:

	static TColumnInfos columnInfos[COL_NBMAX];

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSTRegionsListCtrl)
	//}}AFX_VIRTUAL

// Implementation
protected:

	virtual int onDrawText(CDC * pDC, LPCTSTR lpszString, int nCount, LPRECT lpRect, UINT nFormat, int nItem, int nColumn);
	virtual BOOL GetSortedColumnHasSpecial();
public:

	virtual ~CSTRegionsListCtrl();

	int Build();

	BOOL IsNumericColumn() const;
	void Rebuild(const CRegionItemInfoList & regionsWithNoUnit, int nNbRegionsWithUnits, BOOL bRegionsWithUnitsOnly, BOOL bRegionsSelectedOnly, BOOL bResort);
	void RebuildByDiffs();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSTRegionsListCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	static int compare_s(const CString & s1, const CString & s2, BOOL bAsc);
	static int compare_i(CRegionItemInfo *p1, CRegionItemInfo *p2, EColumnsIndex nIndex, ESortType nSortType);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STREGIONSLISTCTRL_H__0E84D351_2E4C_11D5_AE65_0050041B40F1__INCLUDED_)
