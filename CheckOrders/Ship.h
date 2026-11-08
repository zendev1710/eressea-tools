// Ship.h: interface for the CShip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHIP_H__27822B33_C16C_4054_867F_94003DF5F871__INCLUDED_)
#define AFX_SHIP_H__27822B33_C16C_4054_867F_94003DF5F871__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wLizard.h"
#include "crdata.h"
#include "SortableObList.h"
#include "EcheckTypes.h"
#include "Container.h"
#include "Units.h"

class CRegion;

class CShip: public CContainer  
{
public:
	CShip(eID nShipID, CRegion *pRegion, t_ship * pInfos = NULL, block * p_cr_ship = NULL, BOOL bNew = FALSE);
	virtual ~CShip();

	BOOL SetRegionTo(CRegion * pRegion, const TMoveInfo & moveInfos);
	BOOL insectInside();

	const t_ship * infos() const { return m_pInfos;}
	CRegion * regionFrom() const { return m_pRegionFrom;}

    static int compareOnRegion(CShip* b1, CShip* b2);
    static int compareOnType(CShip* b1, CShip* b2);

	static BOOL s_bShow_lbs_details;

	BOOL checkings();
	BOOL overloaded(float & fAvailable_lbs, CString & sResult);

	void UpdateVesselsPosition();
	void Stats(BOOL bExtendedShipsOnly);

	int finalDamagePrct() const;

	BOOL canGo(const CPoint & firstMove) const;

	int totalSailingSkill() const;

	int damagePrct() const { return m_nDamagePrct;}
	
	void Trace();

	void UpdateSailorsPosition(CRegion * pRegion);

	int nbMen() const;
	int moneyOn() const;

	BOOL save();
	BOOL markCrossedRegions(eCoast & nArrivingCoast);

	// overriden
	BOOL isBuilding() const { return FALSE;}

	float availableCapacity(CString & sResult, BOOL bDetails);

	BOOL SpeedUpPermanentlyByMagic(); //MOVE: +1
	BOOL SpeedUpByMagicSummonStormElemental() const { return m_bSpeedUpByMagicSummonStormElemental;}
	BOOL fullyBuilt(int & nFinalSize, int & nMaxSize); 

	void SetSpeedUpByMagicSummonStormElemental() { m_bSpeedUpByMagicSummonStormElemental=TRUE;}
private:

	int capacity() const;
	int transported_weight(CString & sResult, BOOL bDetails);

	BOOL		m_bSpeedUpByMagicSummonStormElemental;
	int			m_nSpeedUpByMagicStatus;
	int			m_nTransportedWeight;
	int			m_nDamagePrct;
	eCoast		m_nCoast;
	CRegion		* m_pRegionTo;
	t_ship		* m_pInfos;
	TMoveInfo	m_moveInfos;
	CString		m_sResultCapacity;
	BOOL		m_bDamaged;
};

DECLARE_SORTABLE_LIST(CShip)

#endif // !defined(AFX_SHIP_H__27822B33_C16C_4054_867F_94003DF5F871__INCLUDED_)
