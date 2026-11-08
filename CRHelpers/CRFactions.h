// CRFactions.h: interface for the CCRFactionsList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRFACTIONS_H__EAAF9517_115E_44FA_A57A_A6E2F7F55DBD__INCLUDED_)
#define AFX_CRFACTIONS_H__EAAF9517_115E_44FA_A57A_A6E2F7F55DBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CRTypes.h"
#include "CRFaction.h"

/////////

class CCRFactions : public CCRFactionList 
{
public:
	CCRFactions();
	virtual ~CCRFactions();

//	CCRFaction * Add(eID nID, const char *pswd, const t_faction *f);
	//CCRFaction * Add(eID nID, const char *pRaceName);
	//CCRFaction * Add(block * bfaction, const char * sPswd); 
	CCRFaction * Add(CCR & cr, block * bfaction); 
	CCRFaction * Add(CCRFaction * pf); 

	void Stats(int nSortFlag);

	int nbWithOrders(int & nNbUnitsWithOrders) const; 

private:


	BOOL find(CCRFaction * & pFaction, eID nID);

};

#endif // !defined(AFX_CRFACTIONS_H__EAAF9517_115E_44FA_A57A_A6E2F7F55DBD__INCLUDED_)
