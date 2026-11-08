// CRUnits.h: interface for the CCRUnitsList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRUS_H__648FF4F2_FE40_42C2_A15D_505B64F1785B__INCLUDED_)
#define AFX_CRUS_H__648FF4F2_FE40_42C2_A15D_505B64F1785B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////

#include "CRUnit.h"

//class CCRFaction;
//class CCRRegion;

////////

DECLARE_LIST(CCRUnit)

class CCRUnits: public CCRUnitList 
{
public:
	
	CCRUnits(BOOL bDestroyable=TRUE);
	virtual ~CCRUnits();

	BOOL Find(eID nID, CCRUnit *& pUnit);

	CCRUnit * Retrieve(int i, int t);
	//CUnit * Add(CCRFaction *pFaction, int n, int t, int xRegion, int yRegion, const char *sOrder);
	BOOL	Add(CCRUnit *u);

//	void RemoveTempStatus(CFaction *pFaction);

	int NumberFromRegion(CCRRegion *pRegion) const;

private:

};

/////////////

#endif // !defined(AFX_CRUS_H__648FF4F2_FE40_42C2_A15D_505B64F1785B__INCLUDED_)
