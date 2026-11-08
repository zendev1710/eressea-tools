// Roads.cpp: implementation of the CRoads class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Region.h"
#include "Road.h"
#include "Roads.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRoads sRoadsList;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRoads::CRoads()
{
}

CRoads::~CRoads()
{
	POSITION pos=GetHeadPosition();
	while  (pos) delete (GetNext(pos));
	RemoveAll();
}

BOOL CRoads::find(CRoad * & pRoad, eCoast nCoast, CRegion * pRegion)
{
	BOOL bFound = FALSE;
	CRoad * b = NULL;
	pRoad = NULL;
	POSITION pos = GetHeadPosition();
	while (pos && !bFound)	{
		b = GetNext(pos);
		if (b->coast()==nCoast && (pRegion && (pRegion==b->region())))
			bFound = TRUE;
	}
	if (bFound) pRoad = b;
	return (pRoad != NULL);
}

BOOL CRoads::getRoad(CRoad * & pRoad, CRegion *pRegion, eCoast nCoast)
{
//	if (invalidID(nRoadID)) return Error(UNKNOWNBUILDING,"building",itob(nRoadID));
	if (!pRegion) return errormsg("getRoad - internal error: unknown region");
	if (!find(pRoad,nCoast,pRegion))
	{
		block * p_cr_road = pRegion->cr_road(nCoast);
		//if (!p_cr_road) return Error(UNKNOWNBUILDING,"building",itob(nRoadID));
		pRoad = new CRoad(pRegion,nCoast,p_cr_road,p_cr_road==NULL);
		AddHead(pRoad);
	}
	return (pRoad!=NULL);
}


BOOL CRoads::newRoad(CRoad * & pRoad, CRegion *pRegion, eCoast nCoast)
{
	if (!pRegion) return errormsg("newRoad - internal error: unknown region");
	pRoad = new CRoad(pRegion,nCoast,NULL,TRUE);
	AddHead(pRoad);
	return (pRoad!=NULL);
}

BOOL CRoads::road(CRoad * & pRoad, CRegion *pRegion, eCoast nCoast)
{
	//if (nRoadID==ID_UNKNOWN) return newRoad(pRoad, pRegion, pInfos);
	return getRoad(pRoad,pRegion,nCoast);
}

//////////////////////

/*
void CRoads::Stats(int nSortFlag) 
{
	if (nSortFlag == SORT_ON_TYPE) Sort(CRoad::compareOnType); else
	if (nSortFlag == SORT_ON_REGION) Sort(CRoad::compareOnRegion);

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
*/

void CRoads::checkings()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->checkings();
}

void CRoads::save()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->save();
}
