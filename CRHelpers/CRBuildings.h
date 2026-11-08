// CRBuildings.h: interface for the CCRBuildings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRBS_H__7416E0FD_A3C5_4CC9_86E1_84665DA4694E__INCLUDED_)
#define AFX_CRBS_H__7416E0FD_A3C5_4CC9_86E1_84665DA4694E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CRBuilding.h"

class CCRRegion;
//class CCRReport;

//////////

class CCRBuildings : public CCRBuildingList 
{
public:
	CCRBuildings();
	virtual ~CCRBuildings();

//	BOOL newBuilding(CCRBuilding * & pBuilding, CCRRegion *pRegion/*, t_building * pInfos*/);
	BOOL building(CCRBuilding * & pBuilding, CCRRegion *pRegion/*, t_building * pInfos = NULL*/, eID nBuildingID = ID_UNKNOWN);
	BOOL getBuilding(CCRBuilding * & pBuilding, CCRRegion *pRegion/*, t_building * pInfos*/, eID nBuildingID);

	void Stats(int nSortFlag); 

	void checkings();
	void resolveOwners();
	void save();

	BOOL find(CCRBuilding * & pBuilding, eID nID, CCRRegion * pRegion=NULL);

private:

};

#endif // !defined(AFX_CRBS_H__7416E0FD_A3C5_4CC9_86E1_84665DA4694E__INCLUDED_)
