// CRRegionsList.h: interface for the CCRRegionsList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRREGIONSLIST_H__150DCA78_03C4_48EF_9D32_B265B8BDF0E8__INCLUDED_)
#define AFX_CRREGIONSLIST_H__150DCA78_03C4_48EF_9D32_B265B8BDF0E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CRRegion.h"

//////////

class CCRRegions : public CCRRegionList
{
public:
	CCRRegions(BOOL bDestroyable=TRUE);
	virtual ~CCRRegions();

	CCRRegion * Add(CCR & cr, block *r, const char * name); 

	CCRRegion * FromName(const char * name);
	CCRRegion * FromCoords(int x, int y); 

/*	void CheckSilver(BOOL bReservedSilver) const;
	void ResetSilver() const; 
	void onCheckAll() const;*/

	BOOL FindUnit(eID nUnitID, CCRRegion *& pRegion) const;

//	void Stats(int nSortFlag);

//	void save();

private:

	CCRRegion * Add(int x, int y, int pers, const char * name); 

//	TItemInfo * m_madeItems;

};

#endif // !defined(AFX_CRREGIONSLIST_H__150DCA78_03C4_48EF_9D32_B265B8BDF0E8__INCLUDED_)
