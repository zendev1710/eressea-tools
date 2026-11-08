// Units.h: interface for the CUnitsList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNITS_H__648FF4F2_FE40_42C2_A15D_505B64F1785B__INCLUDED_)
#define AFX_UNITS_H__648FF4F2_FE40_42C2_A15D_505B64F1785B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFaction;

////////

#include "wLizard.h"
#include "Unit.h"

class CUnits: public CUnitList 
{
public:
	
	CUnits(BOOL bDestroyable=TRUE);
	virtual ~CUnits();

	CUnit * Find(int i, int tempStatus=0, CRegion *pRegion=NULL);
	CUnit * Find(int i, int tempStatus, int xRegion, int yRegion);

	CUnit * Retrieve(int i, int tempStatus);
	CUnit * Add(CFaction *pFaction, int n, int tempStatus, int xRegion, int yRegion, const char *sOrder);
	BOOL	Add(CUnit *u);

	BOOL InsertBySortedFaction(CUnit *u);

	void RemoveTempStatus(CFaction *pFaction);

	int NumberFromRegion(CRegion *pRegion) const;

	void beforeCheck();

	void save();

private:

};

/////////////

extern CUnits sUnitsList;

/////////////

#endif // !defined(AFX_UNITS_H__648FF4F2_FE40_42C2_A15D_505B64F1785B__INCLUDED_)
