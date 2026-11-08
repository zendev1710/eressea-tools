// /------------------------------------------------------------
// | PapirusCPPSource
// | 
// | File Name    : wSTCVSortedListCtrl.cpp
// | Created By   : cguillon
// | Creation Date: Monday, September 04, 2000
// | 
// | Purpose      : 
// | 
//  ------------------------------------------------------------


#include "stdafx.h"

#include "wSTCVInPlaceEdit.h"
#include "wSTCVSortedListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_IPEDIT 1100

/*----------------------------------------------------------------------------*/

BEGIN_MESSAGE_MAP(CSTCVSortedListCtrl, CSTCVListCtrlEx)

	//{{AFX_MSG_MAP(CSTCVSortedListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteItem)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT,OnEndLabelEdit)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT,OnBeginLabelEdit)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP

//		ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
//		ON_NOTIFY_REFLECT(LVN_ODFINDITEM, OnODFindItem)

	ON_MESSAGE(LVM_SETIMAGELIST, OnSetImageList)
	ON_MESSAGE(LVM_SETTEXTCOLOR, OnSetTextColor)
	ON_MESSAGE(LVM_SETTEXTBKCOLOR, OnSetTextBkColor)
	ON_MESSAGE(LVM_SETBKCOLOR, OnSetBkColor)

END_MESSAGE_MAP()

/*----------------------------------------------------------------------------*/

const int CSTCVSortedListCtrl::NOT_SORTED = CSTCVSortedHeaderCtrl::NOT_SORTED;

/*----------------------------------------------------------------------------*/

CSTCVSortedListCtrl::CSTCVSortedListCtrl(BOOL EnableSorting, BOOL DeleteItemData)
{
	m_bClientWidthSel=FALSE;

	m_cxClient=0;
	m_cxStateImageOffset=0;

	m_clrText=::GetSysColor(COLOR_WINDOWTEXT);
	m_clrTextBk=::GetSysColor(COLOR_WINDOW);
	m_clrBkgnd=::GetSysColor(COLOR_WINDOW);

	m_SeparatorLines = FALSE;
	m_Sorting = EnableSorting;
	m_bFullRowSel = FALSE;
	m_bDeleteItemData = DeleteItemData;
}

/*----------------------------------------------------------------------------*/

CSTCVSortedListCtrl::~CSTCVSortedListCtrl()
{
}

/*----------------------------------------------------------------------------*/

ESortType CSTCVSortedListCtrl::GetSortType () const
{
	// Delegate function to header control
	return m_HeaderCtrl.GetSortType();
}

/*----------------------------------------------------------------------------*/

ESortType CSTCVSortedListCtrl::GetNextSortType (BOOL bHasSortSpecial) const
{
	// Delegate function to header control
	return m_HeaderCtrl.GetNextSortType(bHasSortSpecial);
}

/*----------------------------------------------------------------------------*/

void CSTCVSortedListCtrl::EnableSeparatorLines (BOOL Enable)
{
	m_SeparatorLines = Enable;
}

/*----------------------------------------------------------------------------*/

int CSTCVSortedListCtrl::GetSortedColumn() const
{
	// Delegate function to header control
	return m_HeaderCtrl.GetSortedColumn();
}

/*----------------------------------------------------------------------------*/

void CSTCVSortedListCtrl::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();

	//if (m_Sorting) 
	{
		//Item 0 is the Header Control		
		m_HeaderCtrl.SubclassWindow(::GetDlgItem(m_hWnd,0));
	}
}

/*----------------------------------------------------------------------------*/

int CSTCVSortedListCtrl::InsertEditColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{
	int lRes=InsertColumn(nCol,lpszColumnHeading, nFormat, nWidth, nSubItem);
	m_HeaderCtrl.SetEditable(nCol);
	return lRes;
}

void CSTCVSortedListCtrl::SortColumn (int nCol, ESortType nSortType)
{
	if (m_Sorting) {
		// First set the sort order and column and draw the header
		// This must be called before SortItems to set sort order and column 
		if ((GetStyle() & LVS_TYPEMASK) == LVS_REPORT) {
			m_HeaderCtrl.SetSortImage(nCol, nSortType);
		}
		// Now sort all items in column using CListCtrl sorting mechanism
		SortItems(&CallbackCompareItems, reinterpret_cast<LPARAM>(this));
	}
}

/*----------------------------------------------------------------------------*/

void CSTCVSortedListCtrl::ReSort ()
{
    ASSERT(GetSortedColumn() != NOT_SORTED);
    SortColumn(GetSortedColumn(), GetSortType());
}

/*----------------------------------------------------------------------------*/

void CSTCVSortedListCtrl::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	ESortType nSort = SORT_ASC;

	// If clicked on already sorted column, reverse sort order
    if(pNMListView->iSubItem == GetSortedColumn())
		nSort = GetNextSortType(GetSortedColumnHasSpecial());
	SortColumn(pNMListView->iSubItem, nSort);

	*pResult = 0;
}

/*----------------------------------------------------------------------------*/

int CALLBACK CSTCVSortedListCtrl::CallbackCompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3)
{
	CSTCVSortedListCtrl* pSortedList = reinterpret_cast<CSTCVSortedListCtrl *>(lParam3);
	CItemInfo *pItemInfo1 = reinterpret_cast<CItemInfo*>(lParam1);
	CItemInfo *pItemInfo2 = reinterpret_cast<CItemInfo*>(lParam2);

	return pSortedList->CompareItems (pItemInfo1, pItemInfo2);
}

/*----------------------------------------------------------------------------*/

int CSTCVSortedListCtrl::CompareItems(CItemInfo *pItemInfo1, CItemInfo *pItemInfo2)
{
	return 0;
}

/*----------------------------------------------------------------------------*/

void CSTCVSortedListCtrl::DrawSeparatorLines() 
{
	if (m_SeparatorLines) {

		// Draw the lines only for LVS_REPORT mode
		if( (GetStyle() & LVS_TYPEMASK) == LVS_REPORT )	
		{
			// Get the number of columns
			CClientDC dc(this );
			CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
			int nColumnCount = pHeader->GetItemCount();

			// The bottom of the header corresponds to the top of the line
			RECT rect;
			pHeader->GetClientRect( &rect );
			int top = rect.bottom;

			// Now get the client rect so we know the line length and
			// when to stop
			GetClientRect( &rect );

			// The border of the column is offset by the horz scroll
			int borderx = 0 - GetScrollPos( SB_HORZ );
			for( int i = 0; i < nColumnCount; i++ )	
			{
				// Get the next border
				borderx += GetColumnWidth( i );

				// if next border is outside client area, break out
				if( borderx >= rect.right ) break;

				// Draw the line.
				dc.MoveTo( borderx-1, top);
				dc.LineTo( borderx-1, rect.bottom );
			}
		}	
	}
}

/*----------------------------------------------------------------------------*/

BOOL CSTCVSortedListCtrl::SetFullRowSel(BOOL bFullRowSel)
{
	// no painting during change
	LockWindowUpdate();

	m_bFullRowSel=bFullRowSel;

	BOOL bRet;

	if(m_bFullRowSel)
		bRet=ModifyStyle(0L,LVS_OWNERDRAWFIXED);
	else
		bRet=ModifyStyle(LVS_OWNERDRAWFIXED,0L);

	// repaint window if we are not changing view type
	if(bRet && (GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
		Invalidate();

	// repaint changes
	UnlockWindowUpdate();

	return(bRet);
}

/*----------------------------------------------------------------------------*/

BOOL CSTCVSortedListCtrl::GetFullRowSel()
{
	return(m_bFullRowSel);
}

/*----------------------------------------------------------------------------*/

int CSTCVSortedListCtrl::onDrawText(CDC * pDC, LPCTSTR lpszString, int nCount, LPRECT lpRect, UINT nFormat, int nItem, int nColumn)
{
	if (pDC) return pDC->DrawText(lpszString,nCount,lpRect,nFormat);
	return 0;//TODO: or something else?
}

void CSTCVSortedListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	UINT uiFlags=ILD_TRANSPARENT;
	CImageList* pImageList;
	int nItem=lpDrawItemStruct->itemID;
	BOOL bFocus=(GetFocus()==this);
	COLORREF clrTextSave, clrBkSave;
	COLORREF clrImage=m_clrBkgnd;
	static _TCHAR szBuff[MAX_PATH];

	// Same as label offsets in CHeaderCtrl 
	int OFFSET_FIRST = pDC->GetTextExtent(_T(" "), 1 ).cx*2; // =2
	int OFFSET_OTHER = OFFSET_FIRST;						 // =6

	// get item data

	LV_ITEM lvi;
	lvi.mask=LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.iItem=nItem;
	lvi.iSubItem=0;
	lvi.pszText=szBuff;
	lvi.cchTextMax=sizeof(szBuff);
	lvi.stateMask=0xFFFF;		// get all state flags
	GetItem(&lvi);

	BOOL bSelected=(bFocus || (GetStyle() & LVS_SHOWSELALWAYS)) && lvi.state & LVIS_SELECTED;
	bSelected=bSelected || (lvi.state & LVIS_DROPHILITED);

	// set colors if item is selected

	CRect rcAllLabels;
	GetItemRect(nItem,rcAllLabels,LVIR_BOUNDS);
	CRect rcLabel;
	GetItemRect(nItem,rcLabel,LVIR_LABEL);
	rcAllLabels.left=rcLabel.left;
	if(m_bClientWidthSel && rcAllLabels.right<m_cxClient)
		rcAllLabels.right=m_cxClient;

//<SmC:09.01.2001 clear the whole line before drawing text>
////		if (m_bClientWidthSel &&
////			(GetStyle() & LVS_TYPEMASK) == LVS_REPORT &&
////			GetFullRowSel())

//		{
//			CRect rcClip;
//			pDC->GetClipBox(rcClip);
//	
//			CRect rcAllLabels2;
//			GetItemRect(0, rcAllLabels2, LVIR_BOUNDS);
//	
//			if(rcAllLabels2.right < m_cxClient)
//			{
//				rcClip.left = min(rcAllLabels2.right-1, rcClip.left);
//			}
//			rcClip.right = m_cxClient;
//			CRgn rgn;
//	 		BOOL bOk = rgn.CreateRectRgn(rcClip.left,rcClip.top,rcClip.right,rcClip.bottom);
//			ASSERT( bOk );
//			pDC->SelectClipRgn(&rgn,RGN_OR);
//			rgn.DeleteObject();
//			//keep this rectangle
//			if (bSelected)
//				m_LastSelectedRect = rcClip;
//		}
//</SmC>

	if(bSelected)
	{
		clrTextSave=pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave=pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));

		pDC->FillRect(rcAllLabels,&CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}
	else
		pDC->FillRect(rcAllLabels,&CBrush(m_clrTextBk));

	// set color and mask for the icon

	if(lvi.state & LVIS_CUT)
	{
		clrImage=m_clrBkgnd;
		uiFlags|=ILD_BLEND50;
	}
	else if(bSelected)
	{
		clrImage=::GetSysColor(COLOR_HIGHLIGHT);
		uiFlags|=ILD_BLEND50;
	}

	// draw state icon

	UINT nStateImageMask=lvi.state & LVIS_STATEIMAGEMASK;
	if(nStateImageMask)
	{
		int nImage=(nStateImageMask>>12)-1;
		pImageList=GetImageList(LVSIL_STATE);
		if(pImageList)
			pImageList->Draw(pDC,nImage,CPoint(rcItem.left,rcItem.top),ILD_TRANSPARENT);
	}

	// draw normal and overlay icon

	CRect rcIcon;
	GetItemRect(nItem,rcIcon,LVIR_ICON);

	pImageList=GetImageList(LVSIL_SMALL);
	if(pImageList)
	{
		UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;
		if(rcItem.left<rcItem.right-1)
			ImageList_DrawEx(pImageList->m_hImageList,lvi.iImage,pDC->m_hDC,rcIcon.left,rcIcon.top,16,16,m_clrBkgnd,clrImage,uiFlags | nOvlImageMask);
	}

	// draw item label

	GetItemRect(nItem,rcItem,LVIR_LABEL);
	rcItem.right-=m_cxStateImageOffset;

	rcLabel=rcItem;
	rcLabel.left+=OFFSET_FIRST;
	rcLabel.right-=OFFSET_FIRST;

	onDrawText(pDC,szBuff,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS,nItem,0);

	// draw labels for extra columns

	LV_COLUMN lvc;
	lvc.mask=LVCF_FMT | LVCF_WIDTH;

	for(int nColumn=1; GetColumn(nColumn,&lvc); nColumn++)
	{
		rcItem.left=rcItem.right;
		rcItem.right+=lvc.cx;

		int nRetLen=GetItemText(nItem,nColumn,szBuff,sizeof(szBuff));
		if(nRetLen==0) continue;

		UINT nJustify=DT_LEFT;

		switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
		{
		case LVCFMT_RIGHT:
			nJustify=DT_RIGHT;
			break;
		case LVCFMT_CENTER:
			nJustify=DT_CENTER;
			break;
		default:
			break;
		}

		rcLabel=rcItem;
		rcLabel.left+=OFFSET_OTHER;
		rcLabel.right-=OFFSET_OTHER;

		onDrawText(pDC,szBuff,-1,rcLabel,nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS,nItem,nColumn);
	}

	// draw focus rectangle if item has focus

	//<SmC:09.01.2001 clear the last selected rect>
	//	rcAllLabels.right = m_cxClient;
	//</SmC>

	if(lvi.state & LVIS_FOCUSED && bFocus)
		pDC->DrawFocusRect(rcAllLabels);

	// set original colors if item was selected

	if(bSelected)
	{
	    pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

/*----------------------------------------------------------------------------*/

LRESULT CSTCVSortedListCtrl::OnSetImageList(WPARAM wParam, LPARAM lParam)
{
	if((int)wParam==LVSIL_STATE)
	{
		int cx, cy;

		if(::ImageList_GetIconSize((HIMAGELIST)lParam,&cx,&cy))
			m_cxStateImageOffset=cx;
		else
			m_cxStateImageOffset=0;
	}

	return(Default());
}

/*----------------------------------------------------------------------------*/

LRESULT CSTCVSortedListCtrl::OnSetTextColor(WPARAM wParam, LPARAM lParam)
{
	m_clrText=(COLORREF)lParam;
	return(Default());
}

/*----------------------------------------------------------------------------*/

LRESULT CSTCVSortedListCtrl::OnSetTextBkColor(WPARAM wParam, LPARAM lParam)
{
	m_clrTextBk=(COLORREF)lParam;
	return(Default());
}

/*----------------------------------------------------------------------------*/

LRESULT CSTCVSortedListCtrl::OnSetBkColor(WPARAM wParam, LPARAM lParam)
{
	m_clrBkgnd=(COLORREF)lParam;
	return(Default());
}

/*----------------------------------------------------------------------------*/

void CSTCVSortedListCtrl::OnSize(UINT nType, int cx, int cy) 
{
	m_cxClient=cx;
	CListCtrl::OnSize(nType, cx, cy);
}

/*----------------------------------------------------------------------------*/

void CSTCVSortedListCtrl::OnPaint() 
{
	// in full row select mode, we need to extend the clipping region
	// so we can paint a selection all the way to the right
	if(m_bClientWidthSel && (GetStyle() & LVS_TYPEMASK)==LVS_REPORT && GetFullRowSel())
	{
		CRect rcAllLabels;
		GetItemRect(0,rcAllLabels,LVIR_BOUNDS);

		if(rcAllLabels.right<m_cxClient)
		{
			// need to call BeginPaint (in CPaintDC c-tor)
			// to get correct clipping rect
			CPaintDC dc(this);

			CRect rcClip;
			dc.GetClipBox(rcClip);

			if (rcClip.left >= rcAllLabels.right)
				rcClip.left = rcAllLabels.right-1;
			rcClip.right=m_cxClient;

			InvalidateRect(rcClip,FALSE);
			// EndPaint will be called in CPaintDC d-tor
		}
	}

	//<SmC:09.01.2001 clear the last selected rect>
//		if (m_LastSelectedRect.left>=0)
//			InvalidateRect(m_LastSelectedRect, FALSE);
	//</SmC>

	CListCtrl::OnPaint();
	DrawSeparatorLines();

}

/*----------------------------------------------------------------------------*/

void CSTCVSortedListCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CListCtrl::OnSetFocus(pOldWnd);

	// check if we are getting focus from label edit box
	//correction assert
	if(pOldWnd!=NULL && ::IsWindow(pOldWnd->m_hWnd) && pOldWnd->GetParent()==this)
		return;

	// repaint items that should change appearance
	if(m_bFullRowSel && (GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
		RepaintSelectedItems();
}

/*----------------------------------------------------------------------------*/

void CSTCVSortedListCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CListCtrl::OnKillFocus(pNewWnd);

	// check if we are losing focus to label edit box
	if(pNewWnd!=NULL && pNewWnd->GetParent()==this)
		return;

	// repaint items that should change appearance
	if(m_bFullRowSel && (GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
		RepaintSelectedItems();
}

/*----------------------------------------------------------------------------*/

void CSTCVSortedListCtrl::RepaintSelectedItems()
{
	CRect rcItem, rcLabel;

// invalidate focused item so it can repaint properly

	int nItem=GetNextItem(-1,LVNI_FOCUSED);

	if(nItem!=-1)
	{
		GetItemRect(nItem,rcItem,LVIR_BOUNDS);
		GetItemRect(nItem,rcLabel,LVIR_LABEL);
		rcItem.left=rcLabel.left;

		InvalidateRect(rcItem,FALSE);
	}

// if selected items should not be preserved, invalidate them

	if(!(GetStyle() & LVS_SHOWSELALWAYS))
	{
		for(nItem=GetNextItem(-1,LVNI_SELECTED);
			nItem!=-1; nItem=GetNextItem(nItem,LVNI_SELECTED))
		{
			GetItemRect(nItem,rcItem,LVIR_BOUNDS);
			GetItemRect(nItem,rcLabel,LVIR_LABEL);
			rcItem.left=rcLabel.left;

			InvalidateRect(rcItem,FALSE);
		}
	}

// update changes 

	UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////////////////////

//gestion de l'edition des subitems

int CSTCVSortedListCtrl::HitTestEx(CPoint &point, int *col) const
{
	int colnum = 0;
	int row = HitTest( point, NULL );
	
	if( col ) *col = 0;

	// Make sure that the ListView is in LVS_REPORT
	if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
		return row;

	// Get the top and bottom row visible
	row = GetTopIndex();
	int bottom = row + GetCountPerPage();
	if( bottom > GetItemCount() )
		bottom = GetItemCount();
	
	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows
	for( ;row <=bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetItemRect( row, &rect, LVIR_BOUNDS );
		if( rect.PtInRect(point) )
		{
			// Now find the column
			for( colnum = 0; colnum < nColumnCount; colnum++ )
			{
				int colwidth = GetColumnWidth(colnum);
				if( point.x >= rect.left 
					&& point.x <= (rect.left + colwidth ) )
				{
					if( col ) *col = colnum;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}

CEdit* CSTCVSortedListCtrl::EditSubLabel( int nItem, int nCol )
{
	// The returned pointer should not be saved

	if (!m_HeaderCtrl.IsEditable(nCol)) return NULL;

	// Make sure that the item is visible
	if( !EnsureVisible( nItem, TRUE ) ) return NULL;

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( nCol >= nColumnCount || GetColumnWidth(nCol) < 5 )
		return NULL;

	// Get the column offset
	int offset = 0;
	for( int i = 0; i < nCol; i++ )
		offset += GetColumnWidth( i );

	CRect rect; 
	CRect rcClient; 
	GetClientRect(&rcClient); 

#define correction_scroll_type2

#ifdef correction_scroll_type1
	
	while (true) 
	{ 
		GetSubItemRect(nItem, nCol, LVIR_BOUNDS, rect); 
		
		// Now scroll if we need to expose the column 
		if(rect.left >= 0 && (rect.left + GetColumnWidth(nCol) - 3) <= rcClient.right) 
		{ 
			break; // cell is in view 
		} 
		
		CSize size; 
		size.cx = rect.left; 
		size.cy = 0; 
		
		if (!Scroll(size)) break; // scroll failed 
	} 
	//+++
#endif


#ifdef correction_scroll_type2
	//+++
	GetItemRect( nItem, &rect, LVIR_BOUNDS );
	
	// Now scroll if we need to expose the column
	if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
	{
		CSize size;
		if(offset + rect.left < 0)
			size.cx = -(offset - rect.left);
		else
			size.cx = offset - rect.left;
		
		/*
		size.cx = offset + rect.left;
		size.cy = 0;
		*/

		Scroll(size);
		//rect.left -= size.cx;
		GetItemRect( nItem, &rect, LVIR_BOUNDS );
	}
	  
#endif
	  

	// Get Column alignment
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn( nCol, &lvcol );
	DWORD dwStyle ;
	if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else dwStyle = ES_CENTER;

	//+++
#ifdef correction_scroll_type1
	rect.left += 4; 
#else
	rect.left += offset+4;
	rect.right = rect.left + GetColumnWidth( nCol ) - 3 ;
#endif
	//fin correction liée à la correction faite plus haut

	if( rect.right > rcClient.right) rect.right = rcClient.right;

	dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
	CEdit *pEdit = new CSTCVInPlaceEdit(nItem, nCol, GetItemText( nItem, nCol ));
	if (pEdit)
		pEdit->Create( dwStyle, rect, this, IDC_IPEDIT );


	return pEdit;
}

void CSTCVSortedListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSTCVSortedListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CSTCVSortedListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO *plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM	*plvItem = &plvDispInfo->item;

	if (plvItem->pszText != NULL)
	{
		SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
	}
	*pResult = FALSE;
}

void CSTCVSortedListCtrl::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO *plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM	*plvItem = &plvDispInfo->item;
	int nCol=plvItem->iSubItem;

	//NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	//int nCol=pNMListView->iSubItem;

	if (m_HeaderCtrl.IsEditable(nCol)) *pResult = 0;
	else *pResult = TRUE; //pas d'édition possible
}

void CSTCVSortedListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int index;
	CListCtrl::OnLButtonDown(nFlags, point);

	int colnum;
	if( ( index = HitTestEx( point, &colnum )) != -1 )
	{
		UINT flag = LVIS_FOCUSED;
		if( (GetItemState( index, flag ) & flag) == flag && colnum > 0)
		{
			// Add check for LVS_EDITLABELS
			if( GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS )
				EditSubLabel( index, colnum );
		}
		else
			SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED); 
	}
}

/////////////////////////////////////////////

void CSTCVSortedListCtrl::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    if (m_bDeleteItemData) {
		delete (CItemInfo*)pNMListView->lParam;
    }

	*pResult = 0;
}

//	void CSTCVSortedListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
//	{
//		LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
//		LV_ITEM* pItem= &(pDispInfo)->item;
//		int iItemIndex= pItem->iItem;
//	
//	//		DATA *ptr = array[iItemIndex];
//	//	
//	//		if(pItem->mask & LVIF_TEXT)
//	//		{
//	//			if(pItem->iSubItem == 0)	//field 1
//	//				lstrcpyn(pItem->pszText, ptr->field1, pItem->cchTextMax);
//	//			else
//	//			if(pItem->iSubItem == 1)	//field 2
//	//				lstrcpyn(pItem->pszText, ptr->field2, pItem->cchTextMax);
//	//			else
//	//			if(pItem->iSubItem == 2)	//field 3
//	//				lstrcpyn(pItem->pszText, ptr->field3, pItem->cchTextMax);
//	//		}
//	//	
//	//		*pResult = 0;
//	
//	/////////////////////////
//	
//	//		if (pItem->mask & LVIF_TEXT) //valid text buffer?
//	//		{
//	//			// then display the appropriate column
//	//			switch(pItem->iSubItem)
//	//			{
//	//			case 0:
//	//				lstrcpy(pItem->pszText, rLabel.m_strText);
//	//				break;
//	//			case 1:
//	//				sprintf(pItem->pszText, "0x%08LX", rLabel.m_Addr);
//	//				break;
//	//			default:
//	//				ASSERT(0);
//	//				break;
//	//			}
//	//		}
//	//	
//	//		if (pItem->mask & LVIF_IMAGE)
//	//		{
//	//			// then display the appropriate column
//	//			switch(pItem->iSubItem)
//	//			{
//	//			case 0:
//	//				{
//	//	/*			long index;
//	//				BOOL iBPState
//	//				BPMgr.Find(rLabel.m_Addr, &index, &iBPState);
//	//				*/
//	//				switch(rLabel.m_Addr)
//	//				{
//	//				case 0x00000004:
//	//					pItem->iImage = 0;
//	//					break;
//	//				case 0x00000006:
//	//					pItem->iImage = 1;
//	//					break;
//	//				default:
//	//					pItem->iImage = 2;
//	//					break;
//	//				}
//	//				}
//	//				break;
//	//			default:
//	//				ASSERT(0);
//	//				break;
//	//			}
//	//		}
//	//		
//	//		*pResult = 0;
//	
//		*pResult = -1;
//	}
//	
//	// Handle the quick key event.
//	//  Move to the next item starting with the pressed key.
//	//  Searches from the current item + 1 to the end, then starts from 0 to the current item.
//	//
//	void CSTCVSortedListCtrl::OnODFindItem(NMHDR* pNMHDR, LRESULT* pResult) 
//	{
//		NMLVFINDITEM* pFindInfo = (NMLVFINDITEM*)pNMHDR;
//		LVFINDINFO FindItem = pFindInfo->lvfi;
//	
//	//		int i;
//	//		if(FindItem.flags & LVFI_STRING)
//	//		{
//	//			TCHAR chKeyL = FindItem.psz[0];
//	//			TCHAR chKeyH = _toupper(FindItem.psz[0]);;
//	//	
//	//			// Search to end.
//	//			for( i = pFindInfo->iStart; i < m_LabelCount; i++ )
//	//			{
//	//				if(		( chKeyL == m_arLabels[i].m_strText[0] )
//	//					||	( chKeyH == m_arLabels[i].m_strText[0] )
//	//					)
//	//				{
//	//					*pResult = i;
//	//					return;
//	//				}
//	//			}
//	//	
//	//			// Search from 0 to start.
//	//			for( i = 0; i < pFindInfo->iStart; i++ )
//	//			{
//	//				if(		( chKeyL == m_arLabels[i].m_strText[0] )
//	//					||	( chKeyH == m_arLabels[i].m_strText[0] )
//	//					)
//	//				{
//	//					*pResult = i;
//	//					return;
//	//				}
//	//			}
//	//		}
//	//	
//		*pResult = -1;	// Default action.
//	}

void CSTCVSortedListCtrl::OnDestroy() 
{
	CSTCVListCtrlEx::OnDestroy();
}


/*----------------------------------------------------------------------------*/

