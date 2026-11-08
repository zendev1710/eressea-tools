// Roads.h: interface for the CRoad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROADS_H__7416E0FD_A3C5_4CC9_86E1_84665DA4694E__INCLUDED_)
#define AFX_ROADS_H__7416E0FD_A3C5_4CC9_86E1_84665DA4694E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Road.h"

class CRegion;

//////////

class CRoads : public CRoadList 
{
public:
	CRoads();
	virtual ~CRoads();

	BOOL newRoad(CRoad * & pRoad, CRegion *pRegion, eCoast nCoast);
	BOOL road(CRoad * & pRoad, CRegion *pRegion, eCoast nCoast);
	BOOL getRoad(CRoad * & pRoad, CRegion *pRegion, eCoast nCoast);

	// void Stats(int nSortFlag); 

	void checkings();
	void save();

private:

	BOOL find(CRoad * & pRoad, eCoast nCoast, CRegion * pRegion);

};

extern CRoads sRoadsList;

#endif // !defined(AFX_ROADS_H__7416E0FD_A3C5_4CC9_86E1_84665DA4694E__INCLUDED_)
