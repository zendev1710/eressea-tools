// Buildings.h: interface for the CBuildings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUILDINGS_H__7416E0FD_A3C5_4CC9_86E1_84665DA4694E__INCLUDED_)
#define AFX_BUILDINGS_H__7416E0FD_A3C5_4CC9_86E1_84665DA4694E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Building.h"

class CRegion;

//////////

class CBuildings : public CBuildingList 
{
public:
	CBuildings();
	virtual ~CBuildings();

	BOOL newBuilding(CBuilding * & pBuilding, CRegion *pRegion, t_building * pInfos);
	BOOL building(CBuilding * & pBuilding, CRegion *pRegion, t_building * pInfos = NULL, eID nBuildingID = ID_UNKNOWN);
	BOOL getBuilding(CBuilding * & pBuilding, CRegion *pRegion, t_building * pInfos, eID nBuildingID);

	void Stats(int nSortFlag); 

	void checkings();
	void resolveOwners();
	void save();

	BOOL find(CBuilding * & pBuilding, eID nID, CRegion * pRegion=NULL);

private:


};

extern CBuildings sBuildingsList;

#endif // !defined(AFX_BUILDINGS_H__7416E0FD_A3C5_4CC9_86E1_84665DA4694E__INCLUDED_)
