// CRRegion.h: interface for the CCRRegion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRREGION_H__4BA0A37E_87B0_43D7_BB41_2694C8A8077D__INCLUDED_)
#define AFX_CRREGION_H__4BA0A37E_87B0_43D7_BB41_2694C8A8077D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CRTypes.h"
#include "CRUnits.h"

class CCR;
struct block;

#define REGION_VALUE_UNKNOWN			S_VALUE_UNKNOWN

/////////

class CCRRegion : public CObject 
{

public:

	/////////

	BOOL hasKnownMen();

	void AddNbPersons(int nNbPersons);

	CCRRegion(CCR & cr, block *r, const char *pName);
	virtual ~CCRRegion();

	block * throughJourneyUnits();
	
	BOOL isOcean() const;
	BOOL isNeighbour(int nx, int ny) const;
	BOOL isNeighbour(CCRRegion * pRegion) const;

	BOOL PositionIs(int x, int y) const { return m_nX==x && m_nY==y;}

	BOOL isTerrain(const CString & sName) {return (!terrain().CompareNoCase(sName));}

	BOOL hasUnalliedUnits();

	// accessors

	int x() const { return m_nX;}
	int y() const { return m_nY;}

	const char *name() const { return (LPCSTR)m_sName;}
	const char *outputName(BOOL bWithCoords=FALSE) const;

	int cr_wageForRace(BOOL bIsOrc);

	int cr_wage();
	int cr_castleSize();
	int cr_buildingSize(int nID, BOOL bIsCastle, block * & p_cr_building);
	int cr_shipSize(int nID, block * & p_cr_ship);

	BOOL cr_hasAcademy();
	BOOL cr_resolveName();
	BOOL cr_hasBuilding(const char *pNameType);
	BOOL cr_hasShip(eID nID);
	BOOL cr_hasShip(const char *pNameType);
	BOOL cr_hasUnit(eID nUnitID);

	block * cr_units();

	block * cr_region();
	block * cr_castle();
	block * cr_building(const char *pType);
	block * cr_building(eID nID);
	block * cr_ship(eID nID);
	block * cr_buildings();
	block * cr_academy();

	block * cr_road(int nDirection);
	block * cr_roads();

	block * cr_luxuries();

	static BOOL s_bNameWithCoords;

//	BOOL AddUnit(CCRUnit *pUnit/*, BOOL bUnitHasOrders = FALSE*/);

	CString terrain();

	CString herbName();

	BOOL cr_hasAnHarbour();

	int nbPersons() const { return m_nNbPersons;}

	BOOL AddUnit(CCRUnit *pUnit);

	BOOL isLand();

private:

	block * cr_resource(const CString & sWantedType);

	int m_nNbPersons;
	int m_nX;
	int m_nY;

	CCRUnits m_units;
//	CCRUnits * m_pUnits;

	CString m_sName;
	CString m_sType;
	CString m_sHerbName;

	block *m_cr_region;
	block *m_cr_castle;
	block *m_cr_luxuries;
	block *m_cr_roads;
	block *m_cr_buildings;
	block *m_cr_academy;
	block *m_cr_units;
	block *m_cr_resources;

	CCR & m_cr;

	CCR & cr();

	//////////
	
};

DECLARE_SORTABLE_LIST(CCRRegion)

#endif // !defined(AFX_CRREGION_H__4BA0A37E_87B0_43D7_BB41_2694C8A8077D__INCLUDED_)
