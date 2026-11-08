// Units.cpp: implementation of the CUnits class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "eCheckTypes.h"
#include "Unit.h"
#include "Regions.h"
#include "Units.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnits::CUnits(BOOL bDestroyable):CUnitList(bDestroyable)
{
}

CUnits::~CUnits()
{

}

void CUnits::RemoveTempStatus(CFaction *pFaction) {
	// Marks the temp flag of units of the last region.
	// wrong: temp numbers with GIVES or is noticeable in such a way
	//sUnitsList.RemoveTemp(NULL/*sCurrentFaction*/);
	POSITION pos = GetHeadPosition();
	while (pos) GetNext(pos)->RemoveTempStatus(pFaction);
}

CUnit * CUnits::Find(int i, int t, CRegion * pRegion) {
	CUnit *u=NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	{
		u = GetNext(pos);
		if ((i < 0 && u->id() < 0) || (u->id()==i && val(u->TempStatus())==val(t)))
		{
			if (!pRegion || u->isFromRegion(pRegion))
				return u;
		}
	}
	return NULL;
}

CUnit * CUnits::Find(int i, int tempStatus, int xRegion, int yRegion) {
	CUnit *u=NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	{
		u = GetNext(pos);
		if ((i < 0 && u->id() < 0) || (u->id()==i && val(u->TempStatus())==val(tempStatus)))
		{
			CRegion * r = u->regionFrom();
			if (!r || !r->PositionIs(xRegion,yRegion))
				continue;
			return u;
		}
	}
	return NULL;
}

int CUnits::NumberFromRegion(CRegion *pRegion) const
{
	int n = 0;
	CUnit *t = NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	{
		t = GetNext(pos); 
		if (t->isOnRegion(pRegion))
			n++;
	}
	return n;
}


BOOL CUnits::InsertBySortedFaction(CUnit *u)
{
	//if (Has(u)) 
	//	return FALSE;

	CUnit *uc;
	CFaction * pfc;
	CFaction * pf = u->faction();
	CString sf("");
	POSITION posPrev = NULL;
	POSITION pos = NULL;
	// no faction (taught units, see getAUnit...) ==> end of the list
	if (pf) 
	{
		pos = GetHeadPosition();
		sf=pf->outputName();
	}
	while (pos)	{
		posPrev = pos;
		uc = GetNext(pos);
		if (uc==u)
			return TRUE;
		pfc = uc->faction();
		if (!pfc || sf.CompareNoCase(pfc->outputName())<0)
		{
			InsertBefore(posPrev,u);
			return TRUE;
		}
	}
	AddTail(u);
	return TRUE;
}

CUnit * CUnits::Add(CFaction *pFaction, int n, int t, int xRegion, int yRegion, const char *sOrder)
{
	CUnit *u = Find(n,t,xRegion,yRegion);
	if (!u) {
		u=new CUnit(pFaction,n,t,xRegion,yRegion,sOrder);
		AddTail(u); // at the end
	}
	else
		if (pFaction) 
			u->setFaction(pFaction);
	
	return u;
}

BOOL CUnits::Add(CUnit *u)
{
	if (Has(u)) 
		return FALSE;
	AddTail(u);
	return TRUE;
}

void CUnits::beforeCheck()
{
	POSITION pos=GetHeadPosition();
	while (pos) GetNext(pos)->beforeCheck();
}

void CUnits::save()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->save();
	CUnit::onsaveCheckUnitsToRemove();
}
