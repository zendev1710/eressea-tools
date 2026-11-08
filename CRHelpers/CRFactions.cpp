// CRFactionsList.cpp: implementation of the CCRFactions class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "CRFaction.h"
#include "CRFactions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRFactions::CCRFactions()
{

}

CCRFactions::~CCRFactions()
{
	POSITION pos=GetHeadPosition();
	while  (pos) delete (GetNext(pos));
	RemoveAll();
}

BOOL CCRFactions::find(CCRFaction * & pFaction, eID nID)
{
	BOOL bFound = FALSE;
	CCRFaction * f = NULL;
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
/*
CCRFaction * CCRFactions::Add(eID nID, const char *pswd, const t_faction *f) 
{
	CCRFaction * pFaction = NULL;
	if (find(pFaction, nID)) return pFaction;
	AddHead (new CCRFaction(nID,pswd,f));
	return GetHead();
}
*/
/*
CCRFaction * CCRFactions::Add(eID nID, const char *pRaceName) 
{
	CCRFaction * pFaction = NULL;
	if (find(pFaction, nID)) return pFaction;
	AddHead (new CCRFaction(nID,pRaceName));
	return GetHead();
}*/

/*
CCRFaction * CCRFactions::Add(block * bfaction, const char * sPswd) 
{
	CCRFaction * pFaction = NULL;
	int nID = bfaction->ids[0];
	if (find(pFaction, nID)) return pFaction;
	AddHead (new CCRFaction(bfaction,sPswd));
	return GetHead();
}
*/

CCRFaction * CCRFactions::Add(CCR & cr, block * bfaction) 
{
	return Add(new CCRFaction(cr,bfaction));
}

CCRFaction * CCRFactions::Add(CCRFaction * pf) 
{
	AddHead (pf);
	return GetHead();
}

int CCRFactions::nbWithOrders(int & nNbUnitsWithOrders) const 
{
	int n = 0;
	nNbUnitsWithOrders = 0;
	POSITION pos = GetHeadPosition();
	while (pos)	
	{
		CCRFaction * f = GetNext(pos);
		int nbUnits = f->nbUnitsWithConfirmedOrders();
		if (nbUnits > 0) 
	//	if (f->hasPassword()) && f->nbTotalMen() > 0) 
			n++;
		nNbUnitsWithOrders+=nbUnits;
	}
	return n;
}

/*
void CCRFactions::Stats(int nSortFlag) 
{

	//if (nSortFlag == SORT_ON_POOLSILVER) Sort(CRegion::compareOnSilver); else
	//if (nSortFlag == SORT_ON_MEN) Sort(CRegion::compareOnMen);

	Sort(CCRFaction::compareOnKerubinstatus);

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
		CCRFaction *f = GetNext(pos);
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
}*/		