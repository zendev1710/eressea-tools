// /------------------------------------------------------------
// | PapirusIncludeSource
// | 
// | File Name    : wSTCVItemInfo.h
// | Created By   : cguillon
// | Creation Date: Monday, September 04, 2000
// | 
// | Purpose      : 
// | 
//  ------------------------------------------------------------
#ifndef _WSTCVITEMINFO_H_
#define _WSTCVITEMINFO_H_

// ItemInfo.h: interface for the CItemInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMINFO_H__0104E656_C0A0_11D1_B974_425F59000000__INCLUDED_)
#define AFX_ITEMINFO_H__0104E656_C0A0_11D1_B974_425F59000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CItemInfo : public CObject 
{
public:
	virtual ~CItemInfo() {};
	CItemInfo (int iItem) : m_iItem(iItem), m_bEditable(FALSE) {};
	CItemInfo (int iItem, BOOL bEditable) : m_iItem(iItem), m_bEditable(bEditable) {};
	int GetItem() {return m_iItem;}
	BOOL IsEditable() {return m_bEditable;};
	void SetEditable(BOOL bEditable) {m_bEditable=bEditable;};

private:

	int m_iItem;
	BOOL m_bEditable;

};

#endif // !defined(AFX_ITEMINFO_H__0104E656_C0A0_11D1_B974_425F59000000__INCLUDED_)
#endif //_WSTCVITEMINFO_H_
//  ------------------------------------------------------------
