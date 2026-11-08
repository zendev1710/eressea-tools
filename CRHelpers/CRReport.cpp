// CRReport.cpp: implementation of the CCRReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "CR.h"
#include "CRFactions.h"
#include "CRShips.h"
#include "CRBuildings.h"
#include "CRRegions.h"

#include "CRReport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRReport::CCRReport(CCR & cr):m_cr(cr)
{
 
}

CCRReport::~CCRReport()
{
}

void CCRReport::fill(int nOptions)
{
	if (nOptions & OPT_FACTIONS)	fillFactions(nOptions);
	if (nOptions & OPT_REGIONS)		fillRegions(nOptions);

/*
#define OPT_REGIONS			(1)
#define OPT_UNITS			(2)
#define OPT_SHIPS			(4)
#define OPT_BUILDINGS		(8)
#define OPT_MESSAGES		(16)
#define OPT_FACTIONS		(32)*/

}

void CCRReport::fillFactions(int nOptions)
{
	block * fs = m_cr.factions();
	block * f = fs;

	while (f)
	{
		if (!m_cr.b_isFaction(f))
			break;
		m_allFactions.Add(new CCRFaction(m_cr,f));
		f=f->next;
	}
}

void CCRReport::fillRegions(int nOptions)
{
	block * rs = m_cr.regions();
	block * r = rs;

	//browse regions
	while (r)
	{
		CString sTerrain("");
		CString sName("");
		CString sRegionName;

		// ignore oceans & firewalls
		m_cr.gets(r,CR_TERRAIN,sTerrain);
		if (sTerrain.IsEmpty() || !sTerrain.CompareNoCase(DE_FIREPROOF_WALL)) 
		{
			r=r->next;
			continue;
		}

		m_cr.gets(r,CR_NAME,sRegionName);
		CCRRegion * pr = m_allRegions.Add(m_cr,r,sRegionName);

		if (nOptions & OPT_UNITS) fillUnits(nOptions,pr);
		if (nOptions & OPT_SHIPS) fillShips(nOptions,pr);

		r=r->next;
	}

}

void CCRReport::fillUnits(int nOptions, CCRRegion * pr)
{
	block * r = pr->cr_region();
	block * us = m_cr.units(r);
	block * u = us;

	// browse units in current region
	while (u)
	{
		if (!m_cr.b_isUnit(u))
			break;
		m_allUnits.Add(new CCRUnit(m_cr,u,pr));
		//m_allFactions.Add(nPlayerID);
		u = u->next;
	}
}

/////////////////

void CCRReport::fillShips(int nOptions, CCRRegion * pr)
{
	block * r = pr->cr_region();
	block * ss = m_cr.ships(r);
	block * s = ss;

	// browse ships in current region
	while (s)
	{
		if (!m_cr.b_isShip(s))
			break;
		m_allShips.Add(new CCRShip(m_cr,s,pr));
		s = s->next;
	}
}

/////////////////



