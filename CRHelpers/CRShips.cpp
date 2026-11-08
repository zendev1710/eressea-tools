// CRShips.cpp: implementation of the CCRShips class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CRRegion.h"
#include "CRShip.h"
#include "CRShips.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRShips::CCRShips()
{

}

CCRShips::~CCRShips()
{
	POSITION pos=GetHeadPosition();
	while  (pos) delete (GetNext(pos));
	RemoveAll();
}
/*
void CCRShips::checkings()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->checkings();
}*/

BOOL CCRShips::Find(eID nID, CCRShip *& pShip) 
{
	CCRShip *s = NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	{
		s = GetNext(pos);
		if (s->id()==nID)
		{
			pShip = s;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCRShips::Add(CCRShip *s)
{
	//if (Has(s)) return FALSE;
	AddHead(s);
	return TRUE;
}

// check if already in list
BOOL CCRShips::find(CCRShip * & pShip, eID nID, CCRRegion * pRegion)
{
	BOOL bFound = FALSE;
	CCRShip * s = NULL;
	pShip = NULL;
	POSITION pos = GetHeadPosition();
	while (pos && !bFound)	{
		s = GetNext(pos);
		if (((unknownID(nID) && pRegion) || s->id()==nID) && (!pRegion || (pRegion==s->regionFrom())))
			bFound = TRUE;
	}
	if (bFound) pShip = s;
	return (pShip != NULL);
}

//BOOL CCRShips::getShip(CCRShip * & pShip, CCRRegion *pRegion/*, t_ship * pInfos*/, eID nShipID)
//{
//	if (invalidID(nShipID)) return FALSE;//Error(UNKNOWNSHIP,itob(nShipID));
//	if (!pRegion) return FALSE;//errormsg("getship - internal error: unknown region");
//	if (!find(pShip,nShipID,pRegion))
//	{
//
//		block * p_cr_ship = pRegion->cr_ship(nShipID);
//		if (!p_cr_ship) return FALSE;//Error(UNKNOWNSHIP,itob(nShipID));
//		pShip = new CCRShip(nShipID,pRegion,/*pInfos,*/p_cr_ship);
//		AddHead(pShip);
//	}
//	return (pShip!=NULL);
//}


/*
BOOL CCRShips::newShip(CCRShip * & pShip, CCRRegion *pRegion, t_ship * pInfos)
{
	if (!pInfos) return errormsg("newship - internal error: unknown ship infos");
	if (!pRegion) return errormsg("newship - internal error: unknown region");
	pShip = new CCRShip(newTempID(pRegion, pInfos->name, NULL),pRegion,pInfos,NULL,TRUE);
	AddHead(pShip);
	return (pShip!=NULL);
}
*/
//BOOL CCRShips::ship(CCRShip * & pShip, CCRRegion *pRegion, /*t_ship * pInfos,*/ eID nShipID)
//{
////	if (nShipID==ID_UNKNOWN) return newShip(pShip, pRegion, pInfos);
//	return getShip(pShip,pRegion/*,pInfos*/,nShipID);
//}
/*
void CCRShips::Stats(int nSortFlag) 
{
	if (nSortFlag == SORT_ON_TYPE) Sort(CCRShip::compareOnType); else
	if (nSortFlag == SORT_ON_REGION) Sort(CCRShip::compareOnRegion);

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

}

void CCRShips::save()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->save();
}*/
