// STImageList.h: interface for the CSTImageList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STIMAGELIST_H__C3778C5B_C1D6_40DD_9994_3A5871443774__INCLUDED_)
#define AFX_STIMAGELIST_H__C3778C5B_C1D6_40DD_9994_3A5871443774__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STCOLOR_GRAY	RGB(192,192,192)
#define STCOLOR_WHITE	RGB(255,255,255)
#define STCOLOR_MAGENTA	RGB(255, 0, 255)

#define STICON_WIDTH (16)
//#define STICON_WIDTH (21)

#define STICON_HEIGHT (16)

class CSTImageList : public CImageList  
{
public:
	CSTImageList();
	virtual ~CSTImageList();

	int AddBitmap(UINT nResourceID);
	void SetTransparentColor(COLORREF bkColor);

private:
	static HBITMAP LoadSysColorBitmap(int nResourceId);

	BOOL m_bBitmapBackgroundFlag;
	COLORREF m_bitmapBackground;
};

#endif // !defined(AFX_STIMAGELIST_H__C3778C5B_C1D6_40DD_9994_3A5871443774__INCLUDED_)
