// Buildings.cpp: implementation of the CCRBuildings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "CRBuilding.h"
#include "CRRegion.h"
#include "CRBuildings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRBuildings::CCRBuildings()
{
}

CCRBuildings::~CCRBuildings()
{
	POSITION pos=GetHeadPosition();
	while  (pos) delete (GetNext(pos));
	RemoveAll();
}

BOOL CCRBuildings::find(CCRBuilding * & pBuilding, eID nID, CCRRegion * pRegion)
{
	BOOL bFound = FALSE;
	CCRBuilding * b = NULL;
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

BOOL CCRBuildings::getBuilding(CCRBuilding * & pBuilding, CCRRegion *pRegion/*, t_building * pInfos*/, eID nBuildingID)
{
	if (invalidID(nBuildingID)) return FALSE;//Error(UNKNOWNBUILDING,"building",itob(nBuildingID));
	if (!pRegion) return FALSE;//errormsg("getbuilding - internal error: unknown region");
	return find(pBuilding,nBuildingID,pRegion);
}

/*
BOOL CCRBuildings::newBuilding(CCRBuilding * & pBuilding, CCRRegion *pRegion, t_building * pInfos)
{
	if (!pInfos) return errormsg("newbuilding - internal error: unknown building infos");
	if (!pRegion) return errormsg("newbuilding - internal error: unknown region");
	pBuilding = new CCRBuilding(newTempID(pRegion, pInfos->name, NULL),pRegion,pInfos,NULL,TRUE);
	AddHead(pBuilding);
	return (pBuilding!=NULL);
}
*/

BOOL CCRBuildings::building(CCRBuilding * & pBuilding, CCRRegion *pRegion,/* t_building * pInfos,*/ eID nBuildingID)
{
//	if (nBuildingID==ID_UNKNOWN) return newBuilding(pBuilding, pRegion, pInfos);
	return getBuilding(pBuilding,pRegion/*,pInfos*/,nBuildingID);
}

//////////////////////
/*
void CCRBuildings::Stats(int nSortFlag) 
{
	if (nSortFlag == SORT_ON_TYPE) Sort(CCRBuilding::compareOnType); else
	if (nSortFlag == SORT_ON_REGION) Sort(CCRBuilding::compareOnRegion);

	output("\n================================\n");
	output("\nEXTENDED buildings after orders:\n");
	output("------------------------------------------------------------------\n");
	output("     region              | type       | id    | built | final size\n");
	output("------------------------------------------------------------------\n");

	POSITION pos = GetHeadPosition();
	while (pos)	GetNext(pos)->Stats(TRUE);

	output("\n\nALL buildings after orders:\n");
	output("----------------------------------------------------------------------\n");
	output("     region              | type       | id    | size | our men [other]\n");
	output("----------------------------------------------------------------------\n");

	pos = GetHeadPosition();
	while (pos)	GetNext(pos)->Stats(FALSE);

}

void CCRBuildings::checkings()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->checkings();
}
*/
/*
void CCRBuildings::resolveOwners()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->resolveOwners();
}*/
/*
void CCRBuildings::save()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->save();
}
*/