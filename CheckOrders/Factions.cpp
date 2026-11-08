// FactionsList.cpp: implementation of the CFactions class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CR.h"
#include "EcheckTypes.h"
#include "CamouflagedFaction.h"
#include "Faction.h"
#include "Factions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFactions::CFactions()
{

}

CFactions::~CFactions()
{
	POSITION pos=GetHeadPosition();
	while  (pos) delete (GetNext(pos));
	RemoveAll();
}

BOOL CFactions::find(CFaction * & pFaction, eID nID)
{
	BOOL bFound = FALSE;
	CFaction * f = NULL;
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

CFaction * CFactions::Add(eID nID, const char *pswd, const t_faction *f) 
{
	CFaction * pFaction = NULL;
	if (find(pFaction, nID)) return pFaction;
	AddHead (new CFaction(nID,pswd,f));
	return GetHead();
}

CFaction * CFactions::AddCamouflaged(const char *pRaceName, block *bUnit) 
{
	AddHead (new CCamouflagedFaction(pRaceName,bUnit));
	return GetHead();
}

CFaction * CFactions::Add(eID nID, const char *pRaceName) 
{
	CFaction * pFaction = NULL;
	if (find(pFaction, nID)) return pFaction;
	AddHead (new CFaction(nID,pRaceName));
	return GetHead();
}

CFaction * CFactions::Add(block * bfaction, const char * sPswd) 
{
	CFaction * pFaction = NULL;
	int nID = bfaction->ids[0];
	if (find(pFaction, nID)) return pFaction;
	AddHead (new CFaction(bfaction,sPswd));
	return GetHead();
}

int CFactions::nbWithOrders(int & nNbUnitsWithOrders) const 
{
	int n = 0;
	nNbUnitsWithOrders = 0;
	POSITION pos = GetHeadPosition();
	while (pos)	
	{
		CFaction * f = GetNext(pos);
		int nbUnits = f->nbUnitsWithConfirmedOrders();
		if (nbUnits > 0) 
	//	if (f->hasPassword()) && f->nbTotalMen() > 0) 
			n++;
		nNbUnitsWithOrders+=nbUnits;
	}
	return n;
}


void CFactions::Stats(int nSortFlag) 
{
	ERTRACE("factions statistics...");

	//if (nSortFlag == SORT_ON_POOLSILVER) Sort(CRegion::compareOnSilver); else
	//if (nSortFlag == SORT_ON_MEN) Sort(CRegion::compareOnMen);

	Sort(CFaction::compareOnKerubinstatus);

	output("\n================================\n");
	output("\nFactions:\n");
	output("-------------------------------------------------------------------\n");
	output("   name                    | race       | nbmen        |   nbheroes\n");
	output("-------------------------------------------------------------------\n");

	POSITION pos = GetHeadPosition();
	int nbTotalHeroes=0;
	int nbTotalMen=0;
	BOOL bSeparatorDone=FALSE;
	while (pos)	
	{
		CFaction *f = GetNext(pos);
		if (!bSeparatorDone && !f->isAllied())
		{
			bSeparatorDone = TRUE;
			output("===================================================================\n");
		}
		if (f->Stats())
		{
			nbTotalMen+=f->nbTotalMen();
			nbTotalHeroes+=f->nbTotalHeroes();
		}
	}
	CString sNbMen("");
	sNbMen.Format("%d",nbTotalMen);
	output("-------------------------------------------------------------------\n");
	output("Total:                                  | %-11s  | %4d\n",(LPCSTR)sNbMen,nbTotalHeroes);

	ERTRACE("end factions statistics");
}		