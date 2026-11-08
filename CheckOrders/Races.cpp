// Races.cpp: implementation of the CRaces class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CheckOrders.h"
#include "Race.h"
#include "Races.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaces::CRaces(BOOL bDestroyable):CRaceList(bDestroyable)
{
}

CRace * CRaces::getByName(const char *pName) const
{
	CRace * r=NULL;
	if (pName==NULL) return NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	
	{
		r = GetNext(pos);
		if (!_strcmpi(r->name(),pName)) return r;
	}
	return NULL;
}

CRaces::~CRaces()
{

}
