// Road.cpp: implementation of the CRoad class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "crdata.h"
#include "Eressea.h"
#include "kToTrace.h"
#include "Region.h"
#include "Road.h"
#include "MyEresseaCR.h"
#include "EcheckTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int CRoad::compareOnRegion(CRoad* b1, CRoad* b2) 
{ 
	if (!b1->m_pRegion || !b2->m_pRegion) return 0;
	return _strcmpi(b1->m_pRegion->name(), b2->m_pRegion->name());
}

CRoad::CRoad(CRegion *pRegion, eCoast nCoast, block * p_cr_road, BOOL bNew):
m_bNew(bNew),
m_nExtendedSize(0),
m_nInitialSize(0),
m_nCoast(nCoast),
m_pRegion(pRegion),
m_cr_road(NULL),
m_nMaxSize(0)
{
	m_cr_road = p_cr_road;

	if (pRegion->isTerrain(EN_PLAIN))		m_nMaxSize = 50; else
	if (pRegion->isTerrain(EN_FOREST))		m_nMaxSize = 50; else
	if (pRegion->isTerrain(EN_SWAMP))		m_nMaxSize = 75; else
	if (pRegion->isTerrain(EN_HIGHLAND))	m_nMaxSize = 100; else
	if (pRegion->isTerrain(EN_DESERT))		m_nMaxSize = 100; else
	if (pRegion->isTerrain(EN_MOUNTAIN))	m_nMaxSize = 250; else
	if (pRegion->isTerrain(EN_GLACIER))		m_nMaxSize = 250;

	if (!bNew && pRegion) 
	{
		int nPercent = 0;
		int nC = COAST_NONE;
		if (!m_cr_road) m_cr_road = pRegion->cr_road(nCoast);
		cr().geti(m_cr_road, CR_PERCENT, nPercent);
		cr().geti(m_cr_road, CR_DIRECTION, nC);
		ASSERT(nC==nCoast);
		m_nInitialSize = (nPercent*m_nMaxSize)/100;
	}
}

int CRoad::currentPercent() const
{
	if (maxSize()==0) return 0;
	return (currentSize()*100)/maxSize();
}
  
BOOL CRoad::checkings()
{
/*	CHKBUILDING
	BKPTBUILDING

	if (m_pRegionFrom->nbPersons() == 0)
		return TRUE;

	CString sRegionName = m_pRegionFrom->outputName();
	outputSetRegionName(sRegionName);

	if (!m_pOwner) 
	{
		if (m_pInitialOwner && m_units.GetCount()==1)
			m_pOwner = m_units.GetHead();

		CUnit * pOwner = m_pOwner;
		if (m_nInitialOwnerID!=ID_UNKNOWN && !pOwner)
		{
			pOwner = sUnitsList.Find(m_nInitialOwnerID);
			if (CUnit::existingUnit(pOwner, m_nInitialOwnerID, m_pRegionFrom))
			{
				if (pOwner->isAlive())
					errormsg("internal error: %s - %s is not marked as %s owner in .TXT orders file",(LPCSTR)sRegionName,pOwner->outputName(),fullName());
				// unit without orders - no pbm
			}
			m_pOwner = pOwner;
		}
	}

	// check if new owner is entering in the building
	// if yes then check all units in building give control to this unit
	if (m_pInitialOwner!=m_pOwner)
	{
		// owner changed
		if (!m_pOwner) 
			return Warning(BUILDINGNOOWNER,2,fullName(),m_pInitialOwner->outputName());

		BOOL bNewOwnerEntered=m_pOwner->Entering();
		BOOL bInitialOwnerLeaved=TRUE;

		if (!m_bNew && bNewOwnerEntered)
		{
			int nWarningLevel = 10;
			if (m_pRegionFrom->hasUnalliedUnits()) nWarningLevel = 8;
			if (!m_pRegionFrom->IsGuardedByUs())
				Warning(CONTROLMAYBELOST,nWarningLevel,fullName());
		}
	

		if (m_pInitialOwner)
		{
			m_pInitialOwner->checkMaintainCostGiven(m_nID,m_pOwner); // check cost is transferred
			if (m_pInitialOwner->IsInBuilding(m_nID))
				bInitialOwnerLeaved = FALSE;
		}

		// initial owner leave - check control given to new owner
		if (!m_bNew && (bInitialOwnerLeaved || bNewOwnerEntered))
		{
			POSITION pos = m_units.GetHeadPosition();
			while (pos)
			{
				CUnit * u = m_units.GetNext(pos);
				if (u!=m_pOwner) u->checkControlGiven(m_nID,m_pOwner);
			}
		}
	}
	
	if (!m_bNew)
	{ 
		//TODO: check maintain/upkeep
		//upkeep()....
		int nbTotalMen = nbMen();
		int nbAliveMen = nbMen(TRUE);
		int nb = nbTotalMen;
		if (m_pRegionFrom->war()) nb = nbAliveMen;

		int ns = currentSize();
		int nWarningLevel=4;
		if (m_bCastle) nWarningLevel=6;
		if (nb > ns && nbAliveMen > 0) 
			Warning(TOOMUCHMEN,nWarningLevel,fullName(),nb,ns);
		if (!m_bCastle && m_pOwner) m_pOwner->payMaintenance(this);
	}
*/
	return TRUE;
}

BOOL CRoad::save()
{
	if (currentSize()==0)
		return TRUE; //TODO:pbm

	//save in region if new
	if (m_bNew)
		m_cr_road = m_pRegion->cr_addNewRoad(this);

	if (!m_cr_road)
		return TRUE; //TODO:pbm

	if (m_bNew)
	{
		cr().sets(m_cr_road, CR_TYPE, CR_ROAD);
		cr().seti(m_cr_road, CR_DIRECTION, (int)m_nCoast);
	}

	if (m_nExtendedSize > 0)
		cr().seti(m_cr_road, CR_PERCENT, currentPercent());

/*
GRENZE 1
"Straﬂe";typ
2;richtung
20;prozent

GRENZE 1
"Straﬂe";typ
5;richtung
100;prozent
 *	
 */	
	return TRUE;
}

CRoad::~CRoad()
{

}

