// Buildings.cpp: implementation of the CBuildings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Building.h"
#include "Buildings.h"
#include "region.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBuildings sBuildingsList;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuildings::CBuildings()
{
}

CBuildings::~CBuildings()
{
	POSITION pos=GetHeadPosition();
	while  (pos) delete (GetNext(pos));
	RemoveAll();
}

BOOL CBuildings::find(CBuilding * & pBuilding, eID nID, CRegion * pRegion)
{
	BOOL bFound = FALSE;
	CBuilding * b = NULL;
	pBuilding = NULL;
	POSITION pos = GetHeadPosition();
	while (pos && !bFound)	{
		b = GetNext(pos);
		if (b->id()==nID && (!pRegion || (pRegion==b->regionFrom())))
			bFound = TRUE;
	}
	if (bFound) pBuilding = b;
	return (pBuilding != NULL);
}

BOOL CBuildings::getBuilding(CBuilding * & pBuilding, CRegion *pRegion, t_building * pInfos, eID nBuildingID)
{
	if (nBuildingID==808704)
	{
		int hhyyjj=1;
	}
	if (invalidID(nBuildingID)) 
		return Error(UNKNOWNBUILDING,"building",itob(nBuildingID));
	if (!pRegion) 
		return errormsg("getbuilding - internal error: unknown region");
	if (!find(pBuilding,nBuildingID,pRegion))
	{
		block * p_cr_building = pRegion->cr_building(nBuildingID);
		if (!p_cr_building) 
			return Error(UNKNOWNBUILDING,"building",itob(nBuildingID));
		pBuilding = new CBuilding(nBuildingID,pRegion,pInfos,p_cr_building);
		AddHead(pBuilding);
	}
	return (pBuilding!=NULL);
}


BOOL CBuildings::newBuilding(CBuilding * & pBuilding, CRegion *pRegion, t_building * pInfos)
{
	if (!pInfos) return errormsg("newbuilding - internal error: unknown building infos");
	if (!pRegion) return errormsg("newbuilding - internal error: unknown region");
	pBuilding = new CBuilding(newTempID(pRegion, pInfos->name, NULL),pRegion,pInfos,NULL,TRUE);
	AddHead(pBuilding);
	return (pBuilding!=NULL);
}

BOOL CBuildings::building(CBuilding * & pBuilding, CRegion *pRegion, t_building * pInfos, eID nBuildingID)
{
	if (nBuildingID==ID_UNKNOWN) return newBuilding(pBuilding, pRegion, pInfos);
	return getBuilding(pBuilding,pRegion,pInfos,nBuildingID);
}

//////////////////////

void CBuildings::Stats(int nSortFlag) 
{
	ERTRACE("buildings statistics...");

	if (nSortFlag == SORT_ON_TYPE) Sort(CBuilding::compareOnType); else
	if (nSortFlag == SORT_ON_REGION) Sort(CBuilding::compareOnRegion);

	output("\n================================\n");
	output("\nEXTENDED buildings after orders:\n");
	output("-----------------------------------------------------------------\n");
	output("     region              | type       | id    | made | final size\n");
	output("-----------------------------------------------------------------\n");

	POSITION pos = GetHeadPosition();
	while (pos)	GetNext(pos)->Stats(TRUE);

	output("\n\nALL buildings after orders:\n");
	output("----------------------------------------------------------------------\n");
	output("     region              | type       | id    | size | our men [other]\n");
	output("----------------------------------------------------------------------\n");

	pos = GetHeadPosition();
	while (pos)	GetNext(pos)->Stats(FALSE);

	ERTRACE("end buildings statistics");
}

void CBuildings::checkings()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->checkings();
}


void CBuildings::resolveOwners()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->resolveOwner();
}

void CBuildings::save()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->save();
}
