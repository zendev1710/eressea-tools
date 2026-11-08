// Races.cpp: implementation of the CCRRaces class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CRRace.h"
#include "CRRaces.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRRaces::CCRRaces(BOOL bDestroyable):CCRRaceList(bDestroyable)
{
}

CCRRace * CCRRaces::getByName(const char *pName) const
{
	CCRRace * r=NULL;
	if (pName==NULL) return NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	
	{
		r = GetNext(pos);
		if (!_strcmpi(r->name(),pName)) return r;
	}
	return NULL;
}

CCRRaces::~CCRRaces()
{

}
