// /------------------------------------------------------------
// | PapirusIncludeSource
// | 
// | File Name    : wSTCVSortedHeaderCtrl.h
// | Created By   : cguillon
// | Creation Date: Monday, September 04, 2000
// | 
// | Purpose      : 
// | 
//  ------------------------------------------------------------
#ifndef _WSTCVSORTEDHEADERCTRL_H_
#define _WSTCVSORTEDHEADERCTRL_H_


#if !defined(AFX_SORTEDHEADERCTRL_H__3D138C13_BE82_11D1_B971_ACBC3F000000__INCLUDED_)
#define AFX_SORTEDHEADERCTRL_H__3D138C13_BE82_11D1_B971_ACBC3F000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// wSTCVSortedHeaderCtrl.h : header file
//

#include "STImageList.h"

/////////////////////////////////////////////////////////////////////////////
// CSTCVSortedHeaderCtrl window

typedef enum eImageType
{
	IMGTYPE_NONE = 0,
	IMGTYPE_ICON,
	IMGTYPE_BITMAP
} eImageType;

/////////

typedef enum ESortType {
//	SORT_NONE = 0,	
	SORT_ASC = 0,
	SORT_DESC,
	SORT_SPECIAL,
} ESortType;

class CSTCVSortedHeaderCtrl : public CHeaderCtrl
{
public:
	CSTCVSortedHeaderCtrl();

// Implementation
public:
	virtual ~CSTCVSortedHeaderCtrl();
	int SetSortImage(int nCol, ESortType nSortType);
	BOOL SetEditable(int nCol, BOOL bEditable=TRUE);
	BOOL IsEditable(int nCol);

	//BOOL IsAscSorted() const { return m_bSortAsc;}
	int GetSortedColumn() const {return m_nSortCol;}
	ESortType GetSortType() const { return m_nSortType;}
	ESortType GetNextSortType(BOOL bHasSortSpecial) const;

	BOOL AddIcon(int nCol, UINT nID, BOOL bIsBitmap=FALSE);

	BOOL SetEditableColumns();
	BOOL SetWithIcons();
	BOOL SetTextWithIcons(BOOL bTextWithIcons, BOOL bRefresh=TRUE);
	void SetIconsBkColor(COLORREF bkColor);

	static const int NOT_SORTED;

protected:

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSTCVSortedHeaderCtrl)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSTCVSortedHeaderCtrl)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void SetOwnerDraw(int nCol);

	BOOL CreateImageList();

	int m_nSortCol;
	ESortType m_nSortType;

	BOOL m_bTextWithIcons;

	BOOL  * m_pbEditableColumns;
	eImageType * m_peImagesType;
	CSTImageList * m_pImages;

	COLORREF m_IconsBkColor;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SORTEDHEADERCTRL_H__3D138C13_BE82_11D1_B971_ACBC3F000000__INCLUDED_)
#endif //_WSTCVSORTEDHEADERCTRL_H_
//  ------------------------------------------------------------
