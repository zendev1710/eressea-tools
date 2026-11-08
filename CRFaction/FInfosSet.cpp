// FInfosSet.cpp: implementation of the CFInfosSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wLizard.h"
#include "FInfos.h"
#include "FInfosSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFInfosSet::CFInfosSet(BOOL bDestroyable):CFInfosList(bDestroyable)
{
}

CFInfosSet::~CFInfosSet()
{
//	POSITION pos=GetHeadPosition();
//	while  (pos) delete (GetNext(pos));
//	RemoveAll();
}

BOOL CFInfosSet::find(CFInfos * & pFaction, int nID)
{
	BOOL bFound = FALSE;
	CFInfos * f = NULL;
	pFaction = NULL;
	POSITION pos = GetHeadPosition();
	while (pos && !bFound)	{
		f = GetNext(pos);
		if (f->id()==nID)
			bFound = TRUE;
	}
	if (bFound) pFaction = f;
	return (pFaction != NULL);
}

CFInfos * CFInfosSet::Add(int nID) 
{
	CFInfos * pFaction = NULL;
	if (find(pFaction, nID)) 
		return pFaction;
	AddHead (new CFInfos(nID));
	return GetHead();
}

CFInfos * CFInfosSet::Add(block * bfaction) 
{
	CFInfos * pFaction = NULL;
	int nID = bfaction->ids[0];
	if (find(pFaction, nID)) return pFaction;
	AddHead (new CFInfos(bfaction));
	return GetHead();
}

