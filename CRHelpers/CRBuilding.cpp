// CRBuilding.cpp: implementation of the CCRBuilding class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "CR.h"
#include "CRRegion.h"
#include "CRContainer.h"
#include "CRBuilding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CHKBUILDING if (!_strcmpi(itob(m_nID),"vcoi"))
//#define CHKBUILDING if (0)
#define BKPTBUILDING {int zzzzzz=1;}

//static t_cost nullcost;
//static t_upkeep nullupkeep;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


int CCRBuilding::compareOnRegion(CCRBuilding* b1, CCRBuilding* b2) 
{ 
	if (!b1->m_pRegionFrom || !b2->m_pRegionFrom) return 0;
	return _strcmpi(b1->m_pRegionFrom->name(), b2->m_pRegionFrom->name());
}
/*
int CCRBuilding::compareOnType(CCRBuilding* b1, CCRBuilding* b2) 
{ 
	if (!b1->m_pInfos || !b2->m_pInfos) return 0;
	return strcmpi(b1->m_pInfos->name, b2->m_pInfos->name);
}
*/

//CCRBuilding::CCRBuilding(eID nBuildingID, CCRRegion *pRegion/*, t_building * pInfos*/, block * p_cr_building, BOOL bNew):
//CCRContainer(nBuildingID, pRegion, bNew),
////m_pInfos(pInfos),
//m_bCastle(FALSE),
//m_bAcademy(FALSE),
//m_nNbAliveMen(VALUE_UNKNOWN),
//m_nNbTotalMen(VALUE_UNKNOWN),
//m_nNbOtherMen(VALUE_UNKNOWN),
//m_nNbOtherAliveMen(VALUE_UNKNOWN),
//m_bUsed(FALSE)
//{
//	CHKBUILDING
//	BKPTBUILDING
//
//	m_cr_container = p_cr_building;
//
//	if (!bNew && pRegion) 
//	{
//		if (!m_cr_container) m_cr_container = pRegion->cr_building(nBuildingID);
//		cr().geti(m_cr_container, CR_SIZE, m_nInitialSize);
//		cr().geti(m_cr_container, CR_OWNER, m_nInitialOwnerID);
//		cr().gets(m_cr_container, EN_NAME, m_sShortName);
//
//		// MAKE CASTLE order - check it's really a castle
//	/*	if (!m_pInfos || (m_pInfos && !strcmpi(m_pInfos->name,EN_CASTLE)))
//		{
//			CString sType("");
//			cr().gets(m_cr_container, CR_TYPE, sType);
//			if (CCR::isCastleType(sType)) sType = EN_CASTLE; else sType = CCR::German2English(sType);
//			int nUTID = findbuildingtype(sType);
//			if (nUTID!=IDUT_UNKNOWN) 
//				m_pInfos = buildingFromUTID(nUTID);
//			else
//				errormsg("CCRBuilding::building type %s is not recognized on region %s",(LPCSTR)sType,pRegion->outputName());
//		}
//*/	}
//
///*	if (m_pInfos)
//	{
//		m_bCastle = !strcmpi(m_pInfos->name,EN_CASTLE);
//		m_bAcademy = !strcmpi(m_pInfos->name,EN_ACADEMY);
//		if (bNew) m_sShortName = m_pInfos->name;
//	}
//*/	m_sOutputName.Format("%s (%s)",(LPCSTR)m_sShortName,sid());
//	m_sFullName.Format("%s '%s'",""/*((bNew || !m_pInfos)?"new":m_pInfos->name)*/,(LPCSTR)m_sOutputName);
//	m_sNewName = m_sShortName;
//}


CCRBuilding::CCRBuilding(CCR & ocr, block * bb, CCRRegion * pRegion):
CCRContainer(ocr, bb, pRegion),
//m_pInfos(pInfos),
m_bCastle(FALSE),
m_bAcademy(FALSE),
m_nNbAliveMen(VALUE_UNKNOWN),
m_nNbTotalMen(VALUE_UNKNOWN),
m_nNbOtherMen(VALUE_UNKNOWN),
m_nNbOtherAliveMen(VALUE_UNKNOWN),
//m_units(FALSE),
m_bUsed(FALSE)
{
	CHKBUILDING
	BKPTBUILDING

//	int i;

	cr().geti(cr_container(), CR_SIZE, m_nInitialSize);
	cr().geti(cr_container(), CR_OWNER, m_nInitialOwnerID);
	cr().gets(cr_container(), EN_NAME, m_sShortName);

		// MAKE CASTLE order - check it's really a castle
	/*	if (!m_pInfos || (m_pInfos && !strcmpi(m_pInfos->name,EN_CASTLE)))
		{
			CString sType("");
			cr().gets(m_cr_container, CR_TYPE, sType);
			if (CCR::isCastleType(sType)) sType = EN_CASTLE; else sType = CCR::German2English(sType);
			int nUTID = findbuildingtype(sType);
			if (nUTID!=IDUT_UNKNOWN) 
				m_pInfos = buildingFromUTID(nUTID);
			else
				errormsg("CCRBuilding::building type %s is not recognized on region %s",(LPCSTR)sType,pRegion->outputName());
		}
*/	

/*	if (m_pInfos)
	{
		m_bCastle = !strcmpi(m_pInfos->name,EN_CASTLE);
		m_bAcademy = !strcmpi(m_pInfos->name,EN_ACADEMY);
		if (bNew) m_sShortName = m_pInfos->name;
	}
*/	m_sOutputName.Format("%s (%s)",(LPCSTR)m_sShortName,sid());
	m_sFullName.Format("%s '%s'",""/*((bNew || !m_pInfos)?"new":m_pInfos->name)*/,(LPCSTR)m_sOutputName);
	m_sNewName = m_sShortName;
}

CCRBuilding::~CCRBuilding(){}

BOOL CCRBuilding::AddUnit(CCRUnit *pUnit, BOOL bAsOwner)
{
	if (!m_units.Has(pUnit)) 
		m_units.AddHead(pUnit);

	if (bAsOwner) 
		SetOwner(pUnit); 
	else
	{
		// check owner
		if (m_pInitialOwner==NULL)
			if (pUnit->id()==m_nInitialOwnerID)
				SetInitialOwner(pUnit);
	}
	return TRUE;
}

BOOL CCRBuilding::SetInitialOwner(CCRUnit * pUnit)
{
	CHKBUILDING
	BKPTBUILDING

	if (!m_pInitialOwner)
	{
		m_pInitialOwner = pUnit;
		if (!m_bCastle) 
		{
	//		m_pInitialOwner->setIsInBuilding(m_nID,(/*m_pInfos?m_pInfos->upkeep.silver:*/0));
		}
		if (!m_pOwner) m_pOwner = pUnit;
	}
	return TRUE;
}

void CCRBuilding::cr_setOtherMenInside()
{
	if (m_nNbOtherMen!=VALUE_UNKNOWN) return;
	m_nNbOtherMen=0;
	m_nNbOtherAliveMen=0;
}

/*
int CCRBuilding::nbMen(BOOL bAliveOnly)
{
	CHKBUILDING
	BKPTBUILDING

	if (bAliveOnly  && (m_nNbAliveMen != VALUE_UNKNOWN)) return m_nNbAliveMen;
	if (!bAliveOnly && (m_nNbTotalMen != VALUE_UNKNOWN)) return m_nNbTotalMen;

	cr_setOtherMenInside();

	int nb = 0;
	POSITION pos = m_units.GetHeadPosition();
	while (pos)
	{
		CCRUnit * u = m_units.GetNext(pos);
		if (u && (!bAliveOnly || u->isAlive())) nb += u->people();
	}

	if (bAliveOnly) m_nNbAliveMen = nb; else m_nNbTotalMen = nb;

	return nb;
}
*/

/*
void CCRBuilding::Stats(BOOL bExtendedBuildingsOnly)
{
	int nMax=24;
	CString sType("| ");
	CString sRegion("");
	CString sid;
	int x=LOCATION_UNKNOWN_VALUE;
	int y=LOCATION_UNKNOWN_VALUE;

	if (m_pRegionFrom->nbPersons() == 0)
		return ;

	if (bExtendedBuildingsOnly && m_nExtendedSize==0)
		return ;
	
	sid.Format("| %s",(m_bNew?"<new>":itob(m_nID)));

	if (m_pRegionFrom) 
	{
		sRegion = m_pRegionFrom->name();
		x = m_pRegionFrom->x();
		y = m_pRegionFrom->y();
	}
	
	CString sRegionName(sRegion.Left(nMax));
	if (sRegion.GetLength()>nMax)sRegionName=sRegion.Left(nMax-3)+"...";
	sRegion.Format("%-*s ",nMax,(LPCSTR)sRegionName,x,y);
	if (m_pInfos) sType.Format("| %-10s",m_pInfos->name);
	if (bExtendedBuildingsOnly)
	{
		CString sExtent;
		sExtent.Format("|  %+d",m_nExtendedSize);
		STOutput("%-25s%-10s %-7s %-6s  | %d",(LPCSTR)sRegion,(LPCSTR)sType,(LPCSTR)sid,(LPCSTR)sExtent,currentSize());
	}
	else
	{
		int nbTotalMen = nbMen();
		int nbOurMen = nbMen(TRUE);
		int nbOther = nbTotalMen-nbOurMen;
		CString sMen("");
		CString s2(" ");
		if (nbOther > 0) s2.Format(" [%d]",nbOther);
		sMen.Format("%4d %s",nbOurMen,(LPCSTR)s2);
		STOutput("%-25s%-10s %-7s | %4d | %s",(LPCSTR)sRegion,(LPCSTR)sType,(LPCSTR)sid,currentSize(),(LPCSTR)sMen);
	}
}
*/

/*
BOOL CCRBuilding::isWorking()
{
	if (!m_bCastle)
	{
		// check maintain cost/owner, 
		if (!m_pOwner) return FALSE;
	}
	// check numbers of men in it (compared to size)
	return (nbMen()<=currentSize());
}
*/
/*
BOOL CCRBuilding::resolveOwners()
{
	CHKBUILDING
	BKPTBUILDING

	if (m_pRegionFrom->nbPersons() == 0)
		return TRUE;

	CString sRegionName = m_pRegionFrom->outputName();
//	outputSetRegionName(sRegionName);

	if (!m_pOwner) 
	{
		if (m_pInitialOwner && m_units.GetCount()==1)
			m_pOwner = m_units.GetHead();
	}

	return TRUE;
}
*/

/*
BOOL CCRBuilding::checkings()
{
	CHKBUILDING
	BKPTBUILDING

	if (m_pRegionFrom->nbPersons() == 0)
		return TRUE;

	outputSetRegionName(m_pRegionFrom->outputName());

	//resolveOwners();

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
				CCRUnit * u = m_units.GetNext(pos);
				if (u!=m_pOwner) u->checkControlGiven(m_nID,m_pOwner);
			}
		}
	}
	
	if (!m_bNew)
	{ 
		int nbTotalMen = nbMen();
		int nbAliveMen = nbMen(TRUE);
		int nb = nbTotalMen;
		if (m_pRegionFrom->war()) nb = nbAliveMen;

		int ns = currentSize();
		int nWarningLevel=4;
		if (m_bCastle) nWarningLevel=6;
		if (nb > ns && nbAliveMen > 0 && (m_bUsed || m_bCastle)) 
			Warning(TOOMUCHMEN,nWarningLevel,fullName(),nb,ns);
		if (!m_bCastle && m_pOwner) m_pOwner->payMaintenance(this);
	}

	return TRUE;
}
*/

/*
const t_cost & CCRBuilding::cost() const
{
	return (m_pInfos?m_pInfos->cost:nullcost);
}

const t_upkeep & CCRBuilding::upkeep() const
{
	return (m_pInfos?m_pInfos->upkeep:nullupkeep);
}


BOOL CCRBuilding::save()
{
	CHKBUILDING
	BKPTBUILDING


	if (m_pRegionFrom->nbPersons() == 0)
		return TRUE;

	int nCoef2 = 0;
	CString sType("");

	//save in region if new
	if (m_bNew)
		m_cr_container = m_pRegionFrom->cr_addNewBuilding(this);

	if (!m_cr_container)
		return TRUE; //TODO:pbm

	if (m_bNew)
	{
		if (m_bCastle) 
			sType = e_castleNameFromCoef(e_castleCoef(currentSize()));
		else
		if (m_pInfos) sType=m_pInfos->name;
		cr().sets(m_cr_container, CR_TYPE, CCR::English2German(sType));
		cr().sets(m_cr_container, CR_NAME, m_pInfos->name);
		if (m_pOwner)
		{
			CFaction *pFaction = m_pOwner->faction();
			if (pFaction)
				cr().seti(m_cr_container, CR_PLAYER, pFaction->id());
		}
	}

	if (m_nExtendedSize > 0)
	{
		cr().seti(m_cr_container, CR_SIZE, currentSize());

		// change castle type if new size step reacht
		if (m_bCastle && m_pInfos)
		{
			int nCoef1 = e_castleCoef(initialSize());
			int nCoef2 = e_castleCoef(currentSize());
			if (nCoef2 > nCoef1)
				cr().sets(m_cr_container, CR_TYPE, CCR::English2German(e_castleNameFromCoef(nCoef2)));
		}
	}
	
	// set new owner if changed
	if (m_pInitialOwner!=m_pOwner)
	{
		if (m_pOwner)
			cr().seti(m_cr_container, CR_OWNER, m_pOwner->id());
		else
			cr().onSave_removeEntry(m_cr_container,CR_OWNER); // no more owner
	}

	if (!m_bNew && updateNewName(m_pOwner))
	{
		m_sOutputName.Format("%s (%s)",(LPCSTR)m_sShortName,sid());
		//m_sFullName.Format("%s '%s'",m_pInfos->name,(LPCSTR)m_sOutputName);
		cr().sets(m_cr_container,CR_NAME,m_sNewName);
	}
	
	return TRUE;
}
*/

