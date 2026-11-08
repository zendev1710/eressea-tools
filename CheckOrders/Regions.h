// RegionsList.h: interface for the CRegionsList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGIONSLIST_H__150DCA78_03C4_48EF_9D32_B265B8BDF0E8__INCLUDED_)
#define AFX_REGIONSLIST_H__150DCA78_03C4_48EF_9D32_B265B8BDF0E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Region.h"

//////////

class CRegions : public CRegionList
{
public:
	CRegions(BOOL bDestroyable=TRUE);
	virtual ~CRegions();

	CRegion * Add(int x, int y, int pers, const char * name = NULL); 

	CRegion * FromName(const char * name);
	CRegion * FromCoords(int x, int y); 

	void CheckSilver(BOOL bReservedSilver) const;
	void ResetSilver() const; 
	void onCheckAll() const;

	BOOL FindUnit(eID nUnitID, CRegion *& pRegion) const;

	void Stats(int nSortFlag);

	void save();

private:

	BOOL m_bDestroyable;	
	TItemInfo * m_madeItems;

};

extern CRegions sRegionsList;

#endif // !defined(AFX_REGIONSLIST_H__150DCA78_03C4_48EF_9D32_B265B8BDF0E8__INCLUDED_)
