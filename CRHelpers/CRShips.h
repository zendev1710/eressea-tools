// CRShips.h: interface for the CCRShips class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRSHIPS_H__82C25443_1D70_4300_88C4_3BCD19F9812F__INCLUDED_)
#define AFX_CRSHIPS_H__82C25443_1D70_4300_88C4_3BCD19F9812F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CRShip.h"

class CCRRegion;

//////////

class CCRShips : public CCRShipList
{
public:
	CCRShips();
	virtual ~CCRShips();

//	BOOL newShip(CCRShip * & pShip, CCRRegion *pRegion/*, t_ship * pInfos*/);
	BOOL ship(CCRShip * & pShip, CCRRegion *pRegion/*, t_ship * pInfos = NULL*/, eID nShipID = ID_UNKNOWN);
	BOOL getShip(CCRShip * & pShip, CCRRegion *pRegion/*, t_ship * pInfos*/, eID nShipID);

//	void checkings();
//	void save();

	void Stats(int nSortFlag); 

	BOOL find(CCRShip * & pShip, eID nID, CCRRegion * pRegion=NULL);

	BOOL Find(eID nID, CCRShip *& pShip);
	BOOL Add(CCRShip *s);

private:
	

};

#endif // !defined(AFX_CRSHIPS_H__82C25443_1D70_4300_88C4_3BCD19F9812F__INCLUDED_)
