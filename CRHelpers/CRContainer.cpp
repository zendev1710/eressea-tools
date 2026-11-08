// Container.cpp: implementation of the CCRContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "CRContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
CCRContainer::CCRContainer(eID nID, CCRRegion *pRegion, BOOL bNew):
m_nInitialOwnerID(ID_UNKNOWN),
m_sShortName(""),
m_pRegionFrom(pRegion),
m_nExtendedSize(0),
m_nInitialSize(0),
m_bNew(bNew),
m_pOwner(NULL),
m_pInitialOwner(NULL),
m_cr_container(NULL),
m_nID(nID),
m_sNewName("")
{
}*/

CCRContainer::CCRContainer(CCR & cr, block *b, CCRRegion * pRegion):
m_cr(cr),
m_nInitialOwnerID(ID_UNKNOWN),
m_sShortName(""),
m_pRegionFrom(pRegion),
m_nExtendedSize(0),
m_nInitialSize(0),
m_pOwner(NULL),
m_pInitialOwner(NULL),
m_cr_container(b),
m_sNewName("")
{
	m_nID = b->ids[0];
}

CCRContainer::~CCRContainer()
{

}

CCR & CCRContainer::cr() { return m_cr;}
block * CCRContainer::cr_container() { return m_cr_container;}

int CCRContainer::nbUnits() const 
{ 
	/*if (!m_pUnits)*/ return 0;
//	return m_pUnits->GetCount();
}


BOOL CCRContainer::AddUnit(CCRUnit *pUnit, BOOL bAsOwner)
{
	if (!m_units.Has(pUnit)) 
		m_units.AddHead(pUnit);

	if (bAsOwner) 
		SetOwner(pUnit); 
	return TRUE;
}


BOOL CCRContainer::hasOwner(const CCRUnit * pUnit) const
{
	if (!m_pOwner) return FALSE;
	if (pUnit && m_pOwner!=pUnit) return FALSE;
	return TRUE;
}

BOOL CCRContainer::SetOwner(CCRUnit * pUnit)
{
	BOOL bOK = TRUE;
	if (m_pOwner!=NULL && m_pOwner!=m_pInitialOwner && m_pOwner!=pUnit)
		bOK = FALSE;
	//	bOK = Error(SHIPSEVERALCAPTAINS,fullName()); 
	m_pOwner = pUnit;

	AddUnit(pUnit);
	return bOK;
}

BOOL CCRContainer::SetInitialOwner(CCRUnit * pUnit)
{
	if (!m_pInitialOwner)
	{
		m_pInitialOwner = pUnit;
	//	if (!m_bCastle) 
	//		m_pInitialOwner->setIsInBuilding(m_nID,m_pInfos->upkeep.silver);
		if (!m_pOwner) m_pOwner = pUnit;
	}
	return TRUE;
}


const char * CCRContainer::sid() const
{
	return itob(m_nID);
}
