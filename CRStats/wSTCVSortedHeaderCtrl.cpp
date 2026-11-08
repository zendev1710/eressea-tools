// /------------------------------------------------------------
// | PapirusCPPSource
// | 
// | File Name    : wSTCVSortedHeaderCtrl.cpp
// | Created By   : cguillon
// | Creation Date: Monday, September 04, 2000
// | 
// | Purpose      : 
// | 
//  ------------------------------------------------------------


#include "stdafx.h"

#include "resource.h"
#include "STImageList.h"
#include "wSTCVSortedHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*----------------------------------------------------------------------------*/


BEGIN_MESSAGE_MAP(CSTCVSortedHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CSTCVSortedHeaderCtrl)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*----------------------------------------------------------------------------*/

const int CSTCVSortedHeaderCtrl::NOT_SORTED = -1;

/*----------------------------------------------------------------------------*/

CSTCVSortedHeaderCtrl::CSTCVSortedHeaderCtrl():
	m_pbEditableColumns(NULL),
	m_peImagesType (NULL),
	m_pImages (NULL)
{
	m_nSortCol = NOT_SORTED;
	m_nSortType = SORT_ASC;
	m_bTextWithIcons = FALSE;
	m_IconsBkColor = STCOLOR_GRAY;
}

BOOL CSTCVSortedHeaderCtrl::SetEditableColumns()
{
	if (!m_pbEditableColumns) 
	{
		int nColumnCount = GetItemCount();
		m_pbEditableColumns = new BOOL [nColumnCount];
		for (int i=0;i<nColumnCount;i++) m_pbEditableColumns[i]=FALSE;
		return TRUE;
	}
	return FALSE;
}

void CSTCVSortedHeaderCtrl::SetIconsBkColor(COLORREF bkColor)
{
	m_IconsBkColor = bkColor;
	//if (m_pImagesList) m_pImagesList->SetTransparentColor(bkColor); 
}

BOOL CSTCVSortedHeaderCtrl::CreateImageList()
{
	BOOL bRes = FALSE;
	if (!m_pImages) 
	{
		int nColumnCount = GetItemCount();
		m_pImages = new CSTImageList [nColumnCount];
		m_peImagesType = new eImageType [nColumnCount];
		for (int i=0;i<nColumnCount;i++)
			 m_peImagesType[i]=IMGTYPE_NONE;
		bRes = TRUE;
	}
	return bRes;
}

BOOL CSTCVSortedHeaderCtrl::SetWithIcons()
{
	return CreateImageList();
}

BOOL CSTCVSortedHeaderCtrl::SetTextWithIcons(BOOL bTextWithIcons, BOOL bRefresh) 
{ 
	m_bTextWithIcons = bTextWithIcons;
	if (bRefresh) Invalidate();
	return TRUE;
}

ESortType CSTCVSortedHeaderCtrl::GetNextSortType(BOOL bHasSortSpecial) const 
{ 
	if (m_nSortType==SORT_SPECIAL) return SORT_ASC;
	if (!bHasSortSpecial && m_nSortType==SORT_DESC) return SORT_ASC;
	int  nSort = (int)m_nSortType;
	return (ESortType)(++nSort);
}

BOOL CSTCVSortedHeaderCtrl::SetEditable(int nCol, BOOL bEditable)
{
	int nColumnCount = GetItemCount();
	if (nCol >= nColumnCount) return FALSE;
	if (!m_pbEditableColumns) return FALSE;
	m_pbEditableColumns[nCol]=bEditable;
	return TRUE;
}

BOOL CSTCVSortedHeaderCtrl::IsEditable(int nCol)
{
	int nColumnCount = GetItemCount();
	if (nCol >= nColumnCount) return FALSE;
	if (!m_pbEditableColumns) return FALSE;
	return m_pbEditableColumns[nCol];
}

/*----------------------------------------------------------------------------*/

CSTCVSortedHeaderCtrl::~CSTCVSortedHeaderCtrl()
{
}

/*----------------------------------------------------------------------------*/

int CSTCVSortedHeaderCtrl::SetSortImage(int nCol, ESortType nSortType)
{
	int nPrevCol = m_nSortCol;

	m_nSortCol = nCol;
	m_nSortType = nSortType;

	SetOwnerDraw(nCol);

	// Invalidate header control so that it gets redrawn
	Invalidate();

	return nPrevCol;
}

/*----------------------------------------------------------------------------*/


void CSTCVSortedHeaderCtrl::SetOwnerDraw(int nCol)
{
	HD_ITEM hditem;
	
	hditem.mask = HDI_FORMAT;
	GetItem(nCol, &hditem);
	hditem.fmt |= HDF_OWNERDRAW;
	SetItem(nCol, &hditem);
}

/*----------------------------------------------------------------------------*/

void CSTCVSortedHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	// Get the column rect
	CRect rcLabel(lpDrawItemStruct->rcItem);
	
	// Draw the background
    pDC->FillRect(rcLabel, &CBrush(::GetSysColor(COLOR_3DFACE)));	

	// Labels are offset by a certain amount  
	// This offset is related to the width of a space character
	int offset = pDC->GetTextExtent(_T(" "), 1).cx*2;

	// Get the column text and format
	TCHAR buf[256];
	HD_ITEM hditem;	

	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = buf;
	hditem.cchTextMax = 255;
	
	GetItem(lpDrawItemStruct->itemID, &hditem);

	// Determine format for drawing column label
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS ;

	if (hditem.fmt & HDF_CENTER) 
		uFormat |= DT_CENTER;
	else if (hditem.fmt & HDF_RIGHT) 
		uFormat |= DT_RIGHT;
	else uFormat |= DT_LEFT;

	// Adjust the rect if the mouse button is pressed on it
	if (lpDrawItemStruct->itemState == ODS_SELECTED)
	{
		rcLabel.left++;
		rcLabel.top += 2;
		rcLabel.right++;
	}

	// Adjust the rect further if Sort arrow is to be displayed
	if (lpDrawItemStruct->itemID == static_cast<UINT>(m_nSortCol))
	{
		rcLabel.right -= 3 * offset;
	}
	
	rcLabel.left += offset;
	rcLabel.right -= offset;

	BOOL bDisplayText = TRUE;
	int nCol = lpDrawItemStruct->itemID;

	
	// draw an icon if needed
	if (m_peImagesType && m_pImages && m_peImagesType[nCol]!=IMGTYPE_NONE)
	{
		UINT fStyle = ILD_MASK;
		if (m_peImagesType[nCol]==IMGTYPE_BITMAP) fStyle=ILD_NORMAL/*|ILD_TRANSPARENT*/;
		CPoint ptImage(rcLabel.left,rcLabel.top);
		CPoint ptImage2(rcLabel.left+STICON_WIDTH,rcLabel.top+STICON_HEIGHT);
		CRect rectimg(ptImage,ptImage2);
		CBrush m_brBackground;
		m_brBackground.CreateSolidBrush(STCOLOR_GRAY/*m_IconsBkColor*/);
		pDC->FillRect (rectimg,&m_brBackground);
		m_pImages[nCol].Draw(pDC,0/*nCol*/,ptImage,/**/ILD_NORMAL/*ILD_TRANSPARENT ILD_MASK*/);

	//	if (ImageList_Draw(m_pImagesList->m_hImageList,nCol,pDC->m_hDC,rcLabel.left,rcLabel.top,fStyle))
			if (m_bTextWithIcons) rcLabel.left += STICON_WIDTH; else bDisplayText = FALSE;
	}

	// Draw column label
	if (bDisplayText)
		pDC->DrawText(buf, -1, rcLabel, uFormat);

	// Draw the Sort arrow
	if (lpDrawItemStruct->itemID == static_cast<UINT>(m_nSortCol))
	{
		CRect rcIcon(lpDrawItemStruct->rcItem);

		// Set up pens to use for drawing the triangle
		CPen penLight(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
		CPen penShadow(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		CPen *pOldPen = pDC->SelectObject(&penLight);

		if (m_nSortType==SORT_ASC)
		{
			// Draw triangle pointing upwards
			pDC->MoveTo(rcIcon.right - 2*offset, offset - 1);
			pDC->LineTo(rcIcon.right - 3*offset/2, rcIcon.bottom - offset);
			pDC->LineTo(rcIcon.right - 5*offset/2-2, rcIcon.bottom - offset);
			pDC->MoveTo(rcIcon.right - 5*offset/2-1, rcIcon.bottom - offset - 1);

			pDC->SelectObject(&penShadow);
			pDC->LineTo(rcIcon.right - 2*offset, offset-2);
		}
		else	
		if (m_nSortType==SORT_DESC)
		{
			// Draw triangle pointing downwards
			pDC->MoveTo(rcIcon.right - 3*offset/2, offset-1);
			pDC->LineTo(rcIcon.right - 2*offset-1, rcIcon.bottom - offset + 1);
			pDC->MoveTo(rcIcon.right - 2*offset-1, rcIcon.bottom - offset);

			pDC->SelectObject(&penShadow);
			pDC->LineTo(rcIcon.right - 5*offset/2-1, offset - 1);
			pDC->LineTo(rcIcon.right - 3*offset/2, offset - 1);
		}
		
		// Restore the pen
		pDC->SelectObject(pOldPen);
	}

}

BOOL CSTCVSortedHeaderCtrl::AddIcon(int nCol, UINT nID, BOOL bIsBitmap)
{
	int nRes = -1;
	if (!m_pImages || !m_peImagesType) return FALSE;
	if (m_pImages[nCol].Create(21,16,ILC_COLORDDB|ILC_MASK,1,1))
	{
		if (bIsBitmap) nRes = m_pImages[nCol].AddBitmap(nID); else nRes = m_pImages[nCol].Add(AfxGetApp()->LoadIcon(nID));
		if (nRes != -1) 
		{
			m_peImagesType[nCol] = (bIsBitmap?IMGTYPE_BITMAP:IMGTYPE_ICON);
			SetOwnerDraw(nCol);
			return TRUE;
		}
	}
	return FALSE;
}

/*----------------------------------------------------------------------------*/
