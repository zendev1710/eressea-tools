// CRBuilding.h: interface for the CCRBuilding class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRB_H__39D6F085_5DA5_42F2_A728_B90798DA874A__INCLUDED_)
#define AFX_CRB_H__39D6F085_5DA5_42F2_A728_B90798DA874A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CRContainer.h"

///////////

class CCRBuilding : public CCRContainer
{
public:

	CCRBuilding(CCR & cr, block * bb, CCRRegion *pRegion);

	virtual ~CCRBuilding();
//	const t_cost & cost() const;
//	const t_upkeep & upkeep() const;
//	const t_building * infos() const { return m_pInfos;}

	BOOL isCastle() const { return m_bCastle;}
	BOOL isAcademy() const { return m_bAcademy;}

    static int compareOnRegion(CCRBuilding* b1, CCRBuilding* b2);
    static int compareOnType(CCRBuilding* b1, CCRBuilding* b2);

	void Stats(BOOL bExtendedOnly);

	BOOL isWorking();

	int nbMen(BOOL bAliveOnly=FALSE);

	void Trace();

	BOOL checkings();
//	BOOL resolveOwners();
	BOOL save();

	// overriden

	BOOL AddUnit(CCRUnit *pUnit, BOOL bAsOwner = FALSE);
	BOOL SetInitialOwner(CCRUnit * pUnit);

	void setUsed() { m_bUsed = TRUE;}

protected:

	void cr_setOtherMenInside();

private:

	BOOL m_bUsed;
	BOOL m_bCastle;
	BOOL m_bAcademy;
	int m_nNbAliveMen;
	int m_nNbTotalMen;
	int m_nNbOtherMen;
	int m_nNbOtherAliveMen;
//	t_building * m_pInfos;
};

DECLARE_SORTABLE_LIST(CCRBuilding)

#endif // !defined(AFX_CRB_H__39D6F085_5DA5_42F2_A728_B90798DA874A__INCLUDED_)
