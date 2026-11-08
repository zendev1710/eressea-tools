// CRShip.h: interface for the CCRShip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRSHIP_H__27822B33_C16C_4054_867F_94003DF5F871__INCLUDED_)
#define AFX_CRSHIP_H__27822B33_C16C_4054_867F_94003DF5F871__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CRContainer.h"

class CCRRegion;
class CCR;

class CCRShip: public CCRContainer  
{
public:
	CCRShip(CCR & cr, block * bb, CCRRegion * pRegion);
	virtual ~CCRShip();

	//BOOL SetRegionTo(CCRRegion * pRegion, const TMoveInfo & moveInfos);
	
//	const t_ship * infos() const { return m_pInfos;}
	CCRRegion * regionFrom() const { return m_pRegionFrom;}

    static int compareOnRegion(CCRShip* b1, CCRShip* b2);
    static int compareOnType(CCRShip* b1, CCRShip* b2);

	BOOL checkings();

//	void UpdateSailorsPosition();
//	void Stats(BOOL bExtendedShipsOnly);

	int finalDamagePrct() const;

	BOOL canGo(const CPoint & firstMove) const;

	int totalSailingSkill() const;

	int damagePrct() const { return m_nDamagePrct;}
	
	void Trace();

	void UpdateSailorsPosition(CCRRegion * pRegion);

	int nbMen() const;
	int moneyOn() const;

	BOOL save();
	//BOOL markCrossedRegions(eCoast & nArrivingCoast);

	// overriden
	BOOL isBuilding() const { return FALSE;}

	float availableCapacity();

private:

	int capacity100() const;
	int transportedWeight100();

	int			m_nTransportedWeight100;
	int			m_nDamagePrct;
	eCoast		m_nCoast;
	//CCRRegion	* m_pRegionTo;
	//t_ship		* m_pInfos;
//	TMoveInfo	m_moveInfos;

};

DECLARE_SORTABLE_LIST(CCRShip)

#endif // !defined(AFX_CRSHIP_H__27822B33_C16C_4054_867F_94003DF5F871__INCLUDED_)
