// STImageList.cpp: implementation of the CSTImageList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EresseaStats.h"
#include "STImageList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSTImageList::CSTImageList()
{
	// set the color used for the transparent background in all bitmaps
	m_bitmapBackground = STCOLOR_GRAY;
	m_bBitmapBackgroundFlag = FALSE;
}

void CSTImageList::SetTransparentColor(COLORREF bkColor)
{
	m_bitmapBackground = bkColor;
	if (m_bitmapBackground != STCOLOR_GRAY)
		m_bBitmapBackgroundFlag = TRUE;
}

HBITMAP CSTImageList::LoadSysColorBitmap(int nResourceId)
{
	HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(nResourceId),RT_BITMAP);
	HRSRC hRsrc = ::FindResource(hInst,MAKEINTRESOURCE(nResourceId),RT_BITMAP);
	if (hRsrc == NULL)	return NULL;
	return AfxLoadSysColorBitmap(hInst, hRsrc, FALSE);
}

CSTImageList::~CSTImageList()
{

}

int CSTImageList::AddBitmap(UINT nResourceID)
{
	int bReturn;
	
	HBITMAP hbmp=LoadSysColorBitmap(nResourceID);
	if (hbmp)
	{
		CBitmap bmp;
		bmp.Attach(hbmp);
		if (m_bBitmapBackgroundFlag) bReturn=Add(&bmp,m_bitmapBackground);
		else bReturn=Add(&bmp,GetSysColor(COLOR_3DFACE));
		bmp.Detach();
		DeleteObject(hbmp);
	}
	else bReturn = -1;
	return(bReturn);
}

/*
// Create the full-color image list
// cx, cy = your icon width & height
// You could also use ILC_COLOR24 rather than ILC_COLOR32
CImageList imgl;
imgl.Create(cx, cy, ILC_MASK | ILC_COLOR32, 0, 0);

CBitmap bmp;
// Load your imagelist bitmap (bmp) here, however
//   you feel like (e.g. CBitmap::LoadBitmap)

COLORREF rgbTransparentColor;
// Set up your transparent color as appropriate

// Add the bitmap into the image list
imgl.Add(&bmp, rgbTransparentColor);
*/

