// Ships.cpp: implementation of the CShips class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Region.h"
#include "Ship.h"
#include "Ships.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CShips sShipsList;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShips::CShips()
{

}

CShips::~CShips()
{
	POSITION pos=GetHeadPosition();
	while  (pos) delete (GetNext(pos));
	RemoveAll();
}

void CShips::checkings()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->checkings();
}

void CShips::resolveOwners()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->resolveOwner();
}

// check if already in list
BOOL CShips::find(CShip * & pShip, eID nID, CRegion * pRegion)
{
	BOOL bFound = FALSE;
	CShip * s = NULL;
	pShip = NULL;
	POSITION pos = GetHeadPosition();
	while (pos && !bFound)	{
		s = GetNext(pos);
		if (((nID==ID_UNKNOWN && pRegion) || s->id()==nID) && (!pRegion || (pRegion==s->regionFrom())))
			bFound = TRUE;
	}
	if (bFound) 
		pShip = s;
	return (pShip != NULL);
}

BOOL CShips::getShip(CShip * & pShip, CRegion *pRegion, t_ship * pInfos, eID nShipID)
{
	if (invalidID(nShipID)) 
		return Error(UNKNOWNSHIP,itob(nShipID));
	if (!pRegion) 
		return errormsg("getship - internal error: unknown region");
	if (!find(pShip,nShipID,pRegion))
	{

		block * p_cr_ship = pRegion->cr_ship(nShipID);
		if (!p_cr_ship) 
			return Error(UNKNOWNSHIP,itob(nShipID));
		pShip = new CShip(nShipID,pRegion,pInfos,p_cr_ship);
		AddHead(pShip);
	}
	return (pShip!=NULL);
}


BOOL CShips::newShip(CShip * & pShip, CRegion *pRegion, t_ship * pInfos)
{
	if (!pInfos) 
		return errormsg("newship - internal error: unknown ship infos");
	if (!pRegion) return errormsg("newship - internal error: unknown region");
	pShip = new CShip(newTempID(pRegion, pInfos->name, NULL),pRegion,pInfos,NULL,TRUE);
	AddHead(pShip);
	return (pShip!=NULL);
}

BOOL CShips::ship(CShip * & pShip, CRegion *pRegion, eID nShipID, t_ship * pInfos)
{
	if (nShipID==ID_UNKNOWN) 
		return newShip(pShip, pRegion, pInfos);
	return getShip(pShip,pRegion,pInfos,nShipID);
}

void CShips::Stats(int nSortFlag) 
{
	ERTRACE("ships statistics...");

	if (nSortFlag == SORT_ON_TYPE) Sort(CShip::compareOnType); else
	if (nSortFlag == SORT_ON_REGION) Sort(CShip::compareOnRegion);

	output("\n================================\n");
	output("\nEXTENDED ships after orders:\n");
	output("---------------------------------------------------------------------------------\n");
	output("   SRC region            | type       | name                      | built  | size\n");
	output("---------------------------------------------------------------------------------\n");

	POSITION pos = GetHeadPosition();
	while (pos)	GetNext(pos)->Stats(TRUE);

	output("\nALL ships after orders:\n");
	output("-----------------------------------------------------------------------------------------\n");
	output(" SRC region              | type       | name                      | damage | TGT region  \n");
	output("-----------------------------------------------------------------------------------------\n");

	pos = GetHeadPosition();
	while (pos)	GetNext(pos)->Stats(FALSE);

	ERTRACE("end ship statistics");
}

void CShips::save()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->save();
}
