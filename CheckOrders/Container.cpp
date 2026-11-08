// Container.cpp: implementation of the CContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "crdata.h"
#include "kToTrace.h"
#include "Unit.h"
#include "Region.h"
#include "MyEresseaCR.h"
#include "EcheckTypes.h"
#include "Container.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CHKCONTAINER if (!_strcmpi(itob(m_nID),"ka1j"))
//#define CHKBUILDING if (0)
#define BKPTCONTAINER {int zzzzzz=1;}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContainer::CContainer(eID nID, CRegion *pRegion, BOOL bNew):
m_nInitialOwnerID(ID_UNKNOWN),
m_sShortName(""),
m_pRegionFrom(pRegion),
m_nExtendedSize(0),
m_nInitialSize(0),
m_bNew(bNew),
m_pOwner(NULL),
m_pInitialOwner(NULL),
m_units(NULL),
m_cr_container(NULL),
m_cr_effects(NULL),
m_nID(nID),
m_sNewName("")
{
}

CContainer::~CContainer()
{

}

BOOL CContainer::resolveOwner()
{
	CHKCONTAINER
	BKPTCONTAINER

	if (!m_pRegionFrom || m_pRegionFrom->nbPersons() == 0)
		return TRUE;

	CString sRegionName = m_pRegionFrom->outputName();
	outputSetRegionName(sRegionName);

	if (!m_pOwner) 
	{
		if (/*m_pInitialOwner &&*/ m_units.GetCount()==1)
			m_pOwner = m_units.GetHead();
	}

	return TRUE;
}

BOOL CContainer::updateNewName(CUnit * pOwner)
{
	if (pOwner && pOwner->updateContainerName(m_sNewName) && m_sNewName.CompareNoCase(m_sShortName))
	{
		m_sShortName = m_sNewName;
		return TRUE;
	}
	return FALSE;
}

BOOL CContainer::AddUnit(CUnit *pUnit, BOOL bAsOwner)
{
	CHKCONTAINER
	BKPTCONTAINER

	if (!m_units.Has(pUnit)) 
		m_units.AddHead(pUnit);

	if (bAsOwner) 
		SetOwner(pUnit); 
	return TRUE;
}

BOOL CContainer::RemoveUnit(CUnit *pUnit)
{
	CHKCONTAINER
	BKPTCONTAINER

	m_units.Remove(pUnit);
	if (pUnit==m_pOwner)
	{
		m_pOwner=NULL;
		if (pUnit!=m_pInitialOwner)
			pUnit->_error(UNITRECEIVEDCONTROLBUT,"",fullName());
	}
	return TRUE;
}

BOOL CContainer::hasOwner(const CUnit * pUnit) const
{
	CHKCONTAINER
	BKPTCONTAINER

	if (!m_pOwner) return FALSE;
	if (pUnit && m_pOwner!=pUnit) return FALSE;
	return TRUE;
}

BOOL CContainer::SetOwner(CUnit * pUnit)
{
	CHKCONTAINER
	BKPTCONTAINER

	BOOL bOK = TRUE;
	if (m_pOwner!=NULL && m_pOwner!=m_pInitialOwner && m_pOwner!=pUnit)
		bOK = Error(SHIPSEVERALCAPTAINS,fullName()); 
	m_pOwner = pUnit;

	AddUnit(pUnit);
	return bOK;
}

BOOL CContainer::SetInitialOwner(CUnit * pUnit)
{
	CHKCONTAINER
	BKPTCONTAINER

	if (!m_pInitialOwner)
	{
		m_pInitialOwner = pUnit;
	//	if (!m_bCastle) 
	//		m_pInitialOwner->setIsInBuilding(m_nID,m_pInfos->upkeep.silver);
		if (!m_pOwner) m_pOwner = pUnit;
	}
	return TRUE;
}


const char * CContainer::sid() const
{
	return itob(m_nID);
}

block * CContainer::cr_effects()
{
	if (m_cr_container) {
		if (m_cr_effects) return m_cr_effects;
		m_cr_effects = cr().effects(m_cr_container);
	}
	return m_cr_effects;
}