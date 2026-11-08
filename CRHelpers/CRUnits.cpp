// CRUnits.cpp: implementation of the CCRUnits class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CR.h"
#include "CRRegions.h"
#include "CRUnits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRUnits::CCRUnits(BOOL bDestroyable):CCRUnitList(bDestroyable)
{
}

CCRUnits::~CCRUnits()
{

}
/*
void CCRUnits::RemoveTempStatus(CFaction *pFaction) {
	// Marks the temp flag of units of the last region.
	// wrong: temp numbers with GIVES or is noticeable in such a way
	//sUnitsList.RemoveTemp(NULL);
	POSITION pos = GetHeadPosition();
	while (pos) GetNext(pos)->RemoveTempStatus(pFaction);
}
*/

BOOL CCRUnits::Find(eID nID, CCRUnit *& pUnit) {
	CCRUnit *u = NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	{
		u = GetNext(pos);
		if (u->id()==nID)
		{
			pUnit = u;
			return TRUE;
		}
	}
	return FALSE;
}

/*
CCRUnit * CCRUnits::Find(int i, int t, int xRegion, int yRegion) {
	CCRUnit *u=NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	{
		u = GetNext(pos);
		if ((i < 0 && u->id() < 0) || (u->id()==i))
		{
			CCRRegion * r = u->regionFrom();
			if (!r || !r->PositionIs(xRegion,yRegion))
				continue;
			return u;
		}
	}
	return NULL;
}
*/

int CCRUnits::NumberFromRegion(CCRRegion *pRegion) const
{
	int n = 0;
	CCRUnit *t = NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	{
		t = GetNext(pos); 
		if (t->isOnRegion(pRegion))
			n++;
	}
	return n;
}

/*
CCRUnit * CCRUnits::Add(CFaction *pFaction, int n, int t, int xRegion, int yRegion, const char *sOrder)
{
	CCRUnit *u = Find(n,t,xRegion,yRegion);
	if (!u) {
		u=new CCRUnit(pFaction,n,t,xRegion,yRegion,sOrder);
		AddHead(u);
	}
	else
		if (pFaction) 
			u->setFaction(pFaction);
	
	return u;
}
*/

BOOL CCRUnits::Add(CCRUnit *u)
{
	if (Has(u)) 
		return FALSE;
	AddHead(u);
	return TRUE;
}

/*
void CCRUnits::checkMen()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->checkMen();
}

void CCRUnits::save()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->save();
	CCRUnit::onsaveCheckUnitsToRemove();
}
*/