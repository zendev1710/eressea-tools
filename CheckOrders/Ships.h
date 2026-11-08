// Ships.h: interface for the CShips class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHIPS_H__82C25443_1D70_4300_88C4_3BCD19F9812F__INCLUDED_)
#define AFX_SHIPS_H__82C25443_1D70_4300_88C4_3BCD19F9812F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Ship.h"

class CRegion;

//////////

class CShips : public CShipList
{
public:
	CShips();
	virtual ~CShips();

	BOOL newShip(CShip * & pShip, CRegion *pRegion, t_ship * pInfos);
	BOOL ship(CShip * & pShip, CRegion *pRegion, eID nShipID = ID_UNKNOWN, t_ship * pInfos = NULL);
	BOOL getShip(CShip * & pShip, CRegion *pRegion, t_ship * pInfos, eID nShipID);

	void resolveOwners();
	void checkings();
	void save();

	void Stats(int nSortFlag); 

	BOOL find(CShip * & pShip, eID nID, CRegion * pRegion=NULL);

private:
	

};

extern CShips sShipsList;

#endif // !defined(AFX_SHIPS_H__82C25443_1D70_4300_88C4_3BCD19F9812F__INCLUDED_)
