// Building.h: interface for the CBuilding class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUILDING_H__39D6F085_5DA5_42F2_A728_B90798DA874A__INCLUDED_)
#define AFX_BUILDING_H__39D6F085_5DA5_42F2_A728_B90798DA874A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wLizard.h"
#include "crdata.h"
#include "SortableObList.h"
#include "EcheckTypes.h"
#include "Container.h"
#include "Units.h"

class CCRRegion;

///////////

class CBuilding : public CContainer
{
public:

	CBuilding(eID nBuildingID, CRegion *pRegion, t_building * pInfos = NULL, block * p_cr_building = NULL, BOOL bNew = FALSE);

	virtual ~CBuilding();
	const t_cost & cost() const;
	const t_upkeep & upkeep() const;
	const t_building * infos() const { return m_pInfos;}

	BOOL isCastle() const { return m_bCastle;}
	BOOL isAcademy() const { return m_bAcademy;}

	BOOL upkeepPaidByMagic(); 

    static int compareOnRegion(CBuilding* b1, CBuilding* b2);
    static int compareOnType(CBuilding* b1, CBuilding* b2);

	void Stats(BOOL bExtendedOnly);

	BOOL isWorking();

	int nbMen(BOOL bAliveOnly=FALSE);

	void Trace();

	BOOL checkings();
	BOOL save();

	// overriden

	BOOL AddUnit(CUnit *pUnit, BOOL bAsOwner = FALSE);
	BOOL SetInitialOwner(CUnit * pUnit);

	void setUsed() { m_bUsed = TRUE;}
	BOOL used() const { return m_bUsed;}
	BOOL costPaid() const { return m_bCostPaid;}
	BOOL setCostPaid() { m_bCostPaid=TRUE;}

protected:

	void cr_setOtherMenInside();

private:
	int m_nUpkeepPaidByMagicStatus;
	BOOL m_bUsed;
	BOOL m_bCastle;
	BOOL m_bAcademy;
	BOOL m_bCostPaid;
	int m_nNbAliveMen;
	int m_nNbTotalMen;
	int m_nNbOtherMen;
	int m_nNbOtherAliveMen;
	t_building * m_pInfos;
};

DECLARE_SORTABLE_LIST(CBuilding);

#endif // !defined(AFX_BUILDING_H__39D6F085_5DA5_42F2_A728_B90798DA874A__INCLUDED_)
