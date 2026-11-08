// Factions.h: interface for the CFactionsList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACTIONS_H__EAAF9517_115E_44FA_A57A_A6E2F7F55DBD__INCLUDED_)
#define AFX_FACTIONS_H__EAAF9517_115E_44FA_A57A_A6E2F7F55DBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Faction.h"

/////////

class CFactions : public CFactionList 
{
public:
	CFactions();
	virtual ~CFactions();

	CFaction * AddCamouflaged(const char *pRaceName, block *bUnit); 

	CFaction * Add(eID nID, const char *pswd, const t_faction *f);
	CFaction * Add(eID nID, const char *pRaceName);
	CFaction * Add(block * bfaction, const char * sPswd); 

	void Stats(int nSortFlag);

	int nbWithOrders(int & nNbUnitsWithOrders) const; 

private:

	BOOL find(CFaction * & pFaction, eID nID);

};

extern CFactions sFactionsList;

#endif // !defined(AFX_FACTIONS_H__EAAF9517_115E_44FA_A57A_A6E2F7F55DBD__INCLUDED_)
