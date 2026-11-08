// CRShip.cpp: implementation of the CCRShip class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "CR.h"
#include "CRRegion.h"
#include "CRContainer.h"
#include "CRShip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CHKSHIP if (!_strcmpi(itob(m_nID),"wugq"))
//#define CHKBUILDING if (0)
#define BKPTSHIP {int zzzzzz=1;}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//
//CCRShip::CCRShip(eID nShipID, CCRRegion *pRegionFrom/*, t_ship * pInfos*/, block * p_cr_ship, BOOL bNew):
//CCRContainer(nShipID, pRegionFrom, bNew),
//m_nDamagePrct(0),
//m_nTransportedWeight100(0),
//m_nCoast(COAST_UNKNOWN)
//{
//	CHKSHIP
//	BKPTSHIP
//
//	m_cr_container = p_cr_ship;
//
//	if (!bNew && pRegionFrom) 
//	{
//		if (!m_cr_container) m_cr_container = pRegionFrom->cr_ship(nShipID);
//		cr().geti(m_cr_container, CR_SIZE, m_nInitialSize);
//		cr().geti(m_cr_container, CR_DAMAGE, m_nDamagePrct);
//		cr().gets(m_cr_container, EN_NAME, m_sShortName);
//		cr().geti(m_cr_container, CR_CAPTAIN, m_nInitialOwnerID);
//		if (!cr().geti(m_cr_container, CR_COAST, (int&)m_nCoast))
//			m_nCoast = COAST_NONE;
//		
//	/*	if (!m_pInfos)
//		{
//			CString sType("");
//			cr().gets(m_cr_container, CR_TYPE, sType);
//			sType = CCR::German2English(sType);
//			int nUTID = findshiptype(sType);
//			if (nUTID!=IDUT_UNKNOWN) 
//				m_pInfos = shipFromUTID(nUTID);
//			else
//				errormsg("CCRShip::ship type is not recognized");
//		}
//*/
//		if (m_nDamagePrct > 0 /* && m_pInfos && m_nInitialSize==m_pInfos->size*/)
//			m_nInitialSize=(100-m_nDamagePrct)*m_nInitialSize/100;
//	}
//
//	
//	//if (bNew && m_pInfos) m_sShortName = m_pInfos->name;
//	m_sOutputName.Format("%s (%s)",(LPCSTR)m_sShortName,sid());
//	//m_sFullName.Format("%s '%s'",((bNew || !m_pInfos)?"new":m_pInfos->name),(LPCSTR)m_sOutputName);
//	m_sNewName = m_sShortName;
//}

CCRShip::CCRShip(CCR & ocr, block * bb, CCRRegion * pRegion):
CCRContainer(ocr, bb, pRegion),
m_nDamagePrct(0),
m_nTransportedWeight100(0),
m_nCoast(COAST_UNKNOWN)
{
	CHKSHIP
	BKPTSHIP

	cr().geti(cr_container(), CR_SIZE, m_nInitialSize);
	cr().geti(cr_container(), CR_DAMAGE, m_nDamagePrct);
	cr().gets(cr_container(), EN_NAME, m_sShortName);
	cr().geti(cr_container(), CR_CAPTAIN, m_nInitialOwnerID);
	if (!cr().geti(cr_container(), CR_COAST, (int&)m_nCoast))
		m_nCoast = COAST_NONE;
		
	if (m_nDamagePrct > 0)
		m_nInitialSize=(100-m_nDamagePrct)*m_nInitialSize/100;
	
	m_sOutputName.Format("%s (%s)",(LPCSTR)m_sShortName,sid());
	//m_sFullName.Format("%s '%s'",((bNew || !m_pInfos)?"new":m_pInfos->name),(LPCSTR)m_sOutputName);
	m_sNewName = m_sShortName;
}

CCRShip::~CCRShip(){}

int CCRShip::compareOnRegion(CCRShip* s1, CCRShip* s2) 
{ 
	if (!s1->m_pRegionFrom || !s2->m_pRegionFrom) return 0;
	return _strcmpi(s1->m_pRegionFrom->name(), s2->m_pRegionFrom->name());
}
/*
int CCRShip::compareOnType(CCRShip* s1, CCRShip* s2) 
{ 
	if (!s1->m_pInfos || !s2->m_pInfos) return 0;
	return strcmpi(s1->m_pInfos->name, s2->m_pInfos->name);
}*/

int CCRShip::finalDamagePrct() const
{
	return m_nDamagePrct;
	//return 100-((currentSize() * 100)/m_pInfos->size);
}
/*
void CCRShip::Trace()
{
	POSITION pos = m_units.GetHeadPosition();
	while (pos)
	{
		CCRUnit * u = m_units.GetNext(pos);
	//	STOutput("unit %s\n",u->outputName());
	}
}
*/
/*
int CCRShip::totalSailingSkill() const
{
	CHKSHIP
	BKPTSHIP

	int nb = 0;
	POSITION pos = m_units.GetHeadPosition();
	while (pos)
	{
		CCRUnit * u = m_units.GetNext(pos);
		if (u) 
			nb += u->finalSailingSkill();
	}
	return nb;
}
*/
/*
int CCRShip::nbMen() const
{
	int nb = 0;
	POSITION pos = m_units.GetHeadPosition();
	while (pos)
	{
		CCRUnit * u = m_units.GetNext(pos);
		if (u) 
			nb += u->people();
	}
	return nb;
}
*/
/*
void CCRShip::Stats(BOOL bExtendedShipsOnly)
{
	int nMaxRegionname=24;
	int nMaxShipname=24;
	int nMaxShipShortname=nMaxShipname-6;
	CString sType("| ");
	CString sRegion("");
	CString sShipname("");
	CString sShipFullname("");
	CString sSuffixID("");
	CString sAvailable_lbs("");

	CCRRegion * pFinalRegion = m_pRegionFrom;
//	if (m_pRegionFrom!=m_pRegionTo && m_pRegionTo)
//		pFinalRegion = m_pRegionTo;

	if (pFinalRegion->nbPersons() == 0)
		return ;

	
	if (bExtendedShipsOnly && m_nExtendedSize==0)
		return ;

	int nbMarins = nbMen();

	if (m_pRegionFrom) sRegion = m_pRegionFrom->name();
	CString sRegionName(sRegion.Left(nMaxRegionname));
	if (!sRegion.CompareNoCase(EN_OCEAN)) sRegionName.Format("%s (%+d,%+d)",(LPCSTR)sRegion,m_pRegionFrom->x(),m_pRegionFrom->y());

	if (sRegion.GetLength()>nMaxRegionname)sRegionName=sRegion.Left(nMaxRegionname-3)+"...";
	sRegion.Format("%-*s",nMaxRegionname,(LPCSTR)sRegionName);
	if (m_pInfos) sType.Format("| %-10s",m_pInfos->name);

	sShipname = m_sShortName.Left(nMaxShipShortname);
	if (m_sShortName.GetLength()>nMaxShipShortname) sShipname=sShipname.Left(nMaxShipShortname-3)+"...";
	sSuffixID.Format("(%s)",sid());
	sShipFullname.Format("%-*s %6s",nMaxShipShortname,(LPCSTR)sShipname,(LPCSTR)sSuffixID);
	
	if (bExtendedShipsOnly)
	{
		CString sExtent;
		sExtent.Format("| %+d",m_nExtendedSize);
		STOutput("%-25s%-10s | %-25s %-8s | %d",(LPCSTR)sRegion,(LPCSTR)sType,(LPCSTR)sShipFullname,(LPCSTR)sExtent,currentSize());
	}
	else
	{
		CString sNewRegion("");
		CString sPercentSize("");
		int nDamage = finalDamagePrct();
		if (nDamage > 0) sPercentSize.Format("%3d%%%%",nDamage);
		float flbs = availableCapacity();
		if (m_pRegionFrom!=pFinalRegion)
		{
			CString sTmpRegion = m_pRegionTo->name();
			sTmpRegion = sTmpRegion.Left(nMaxRegionname);
			CString sr(sTmpRegion);
			if (!sTmpRegion.CompareNoCase(EN_OCEAN)) sTmpRegion.Format("%s (%+d,%+d)",(LPCSTR)sr,m_pRegionTo->x(),m_pRegionTo->y());
			if (!sTmpRegion.CompareNoCase(REGION_VALUE_UNKNOWN)) sTmpRegion.Format("<unknown> (%+d,%+d)",m_pRegionTo->x(),m_pRegionTo->y());
			CString slbs("");
			slbs.Format(" - [free lbs:%8.2f]",flbs);
			sNewRegion.Format("%-15s %s",(LPCSTR)sTmpRegion,(LPCSTR)slbs);
		}
		else
		{
			if (flbs < 0)
			{
				CString slbs("");
				slbs.Format(" - [overloaded lbs:%8.2f]",-flbs);
				sNewRegion.Format("%-15s %s","",(LPCSTR)slbs);
			}
		}
		STOutput("%-25s%-10s | %-25s | %-4s   | %s",(LPCSTR)sRegion,(LPCSTR)sType,(LPCSTR)sShipFullname,(LPCSTR)sPercentSize,(LPCSTR)sNewRegion);
	}
}
*/

BOOL CCRShip::canGo(const CPoint & firstMove) const
{
	if (!m_pRegionFrom) return FALSE;
	CPoint s(m_pRegionFrom->x(),m_pRegionFrom->y());
	if (m_nCoast == COAST_NONE) return TRUE; //jsut created, can start from everywhere
	return coastNear(m_nCoast,coastFrom(s, firstMove));
}

// X100
/*
int CCRShip::transportedWeight100()
{
	if (m_nTransportedWeight100 == 0)
	{
		if (m_units.GetCount()>0)
		{
			POSITION pos = m_units.GetHeadPosition();
			while (pos) m_nTransportedWeight100+=m_units.GetNext(pos)->totalWeight100();
		}
	}
	return m_nTransportedWeight100;
}
*/

// X100
/*
int CCRShip::capacity100() const
{
	// TODO: if damaged, should be less
	if (m_pInfos) return m_pInfos->capacity*100;
	return 0;
}


float CCRShip::availableCapacity()
{
	return (float)(capacity100()-transportedWeight100())/100.0;
}
*/

/*
BOOL CCRShip::SetRegionTo(CCRRegion * pRegion, const TMoveInfo & moveInfos)
{
	CHKSHIP
	BKPTSHIP

	m_pRegionTo=pRegion;
	m_moveInfos.copy(moveInfos);
	UpdateSailorsPosition();
	return TRUE;
}
*/
/*
int CCRShip::moneyOn() const
{
	int nMoney = 0;
	POSITION pos = m_units.GetHeadPosition();
	while (pos) 
	{
		CCRUnit * pUnit = m_units.GetNext(pos);
		nMoney+= pUnit->money();
	}
	return nMoney;
}
*/
/*
BOOL CCRShip::checkings()
{
	CHKSHIP
	BKPTSHIP

	CCRRegion * pRegion = m_pRegionFrom;
	if (m_pRegionFrom!=m_pRegionTo && m_pRegionTo)
		pRegion = m_pRegionTo;

	if (pRegion->nbPersons() == 0)
		return TRUE;


	CString sRegionName = m_pRegionFrom->outputName();
	outputSetRegionName(sRegionName);

	if (!m_pOwner) 
	{
		if (m_pInitialOwner)
		{
			if (m_units.GetCount()==1)
			{
				m_pOwner = m_units.GetHead();
				if (m_pInitialOwner!=m_pOwner)
					m_pOwner->onReceiveShipControl(m_nID,m_pInitialOwner->ChangedOfContainer());
			}
		}

		CCRUnit * pOwner = m_pOwner;
		if (m_nInitialOwnerID!=ID_UNKNOWN && !pOwner)
		{
			pOwner = sUnitsList.Find(m_nInitialOwnerID);
			if (CCRUnit::existingUnit(pOwner, m_nInitialOwnerID, m_pRegionFrom))
			{
				//if (pOwner->isAlive())
				//	errormsg("internal error: %s - %s is not marked as %s captain in .TXT orders file",(LPCSTR)sRegionName,pOwner->outputName(),fullName());
				// unit without orders - no pbm
				// Warning(SHIPENEMYCAPTAIN,2,fullName(),pUnit->outputName());
			}
			m_pOwner = pOwner;
		}
	}

	// check if new owner is entering in the ship
	// if yes then check all units in ship give control to this unit
	if (m_pInitialOwner!=m_pOwner)
	{
		// owner changed
		if (!m_pOwner) 
			return Error(SHIPNOCAPTAIN,fullName(),m_pInitialOwner?m_pInitialOwner->outputName():"");

		BOOL bInitialOwnerLeaved=TRUE;
		BOOL bNewOwnerEntered=m_pOwner->Entering();

		if (!m_bNew && bNewOwnerEntered)
		{
			int nWarningLevel = 10;
			if (m_pRegionFrom->hasUnalliedUnits()) nWarningLevel = 8;
			if (!m_pRegionFrom->IsGuardedByUs())
				Warning(CONTROLMAYBELOST,nWarningLevel,fullName());
		}
	

		if (m_pInitialOwner)
		{
			if (m_pInitialOwner->IsOnShip(m_nID))
				bInitialOwnerLeaved = FALSE;
		}

		if (!m_bNew && bInitialOwnerLeaved)
		{
			POSITION pos = m_units.GetHeadPosition();
			while (pos)
			{
				CCRUnit * u = m_units.GetNext(pos);
				if (u!=m_pOwner) u->checkControlGiven(m_nID,m_pOwner);
			}
		}
	}

	return TRUE;
}

BOOL CCRShip::markCrossedRegions(eCoast & nArrivingCoast)
{
	CString s(outputName());
	nArrivingCoast = COAST_NONE;
	int nNbMoves = m_moveInfos.m_nCount;
	if (m_moveInfos.m_bRouting) nNbMoves = m_moveInfos.m_nRoutingIndex;

	CCRRegion *pRegionFrom = regionFrom();
	CCRRegion *pRegion = NULL;
	CPoint pPrev;
	CPoint p;
	pRegionFrom->cr_addThroughShip(s);
	pPrev=CPoint(pRegionFrom->x(),pRegionFrom->y());
	for (int j=1;j<=nNbMoves;j++)
	{
		p=m_moveInfos.moves[j];
		pRegion = sRegionsList.Add(p.x, p.y, 0);
		if (pRegion) 
		{
			pRegion->cr_addThroughShip(s);
			if (j==nNbMoves && !pRegion->isOcean())
				nArrivingCoast = coastTo(pPrev,p);
		}
		pPrev=p;
	}

	return TRUE;
}

BOOL CCRShip::save()
{
	CHKSHIP
	BKPTSHIP


	CCRRegion * pRegion = m_pRegionFrom;
	if (m_pRegionFrom!=m_pRegionTo && m_pRegionTo)
		pRegion = m_pRegionTo;

	if (pRegion->nbPersons() == 0)
		return TRUE;

	// save in region if new
	if (m_bNew)
		m_cr_container = pRegion->cr_addNewShip(this);

	if (!m_cr_container)
		return TRUE; //TODO: errormsg

	if (m_bNew)
	{
		cr().sets(m_cr_container, CR_TYPE, CCR::English2German(m_pInfos->name));
		cr().sets(m_cr_container, CR_NAME, m_pInfos->name);
		if (m_pOwner)
		{
			CFaction *pFaction = m_pOwner->faction();
			if (pFaction)
				cr().seti(m_cr_container, CR_PLAYER, pFaction->id());
		}
	}

	//save in to-region
	//TODO: pbm when move goes on same region 
	int nNbMoves = m_moveInfos.m_nCount;	
	if (m_moveInfos.m_bRouting) nNbMoves = m_moveInfos.m_nRoutingIndex;
	if (nNbMoves > 0)
	{
		eCoast nArrivingCoast = COAST_NONE;
		markCrossedRegions(nArrivingCoast);
		// update coast value
		if (!pRegion->isOcean())
			cr().seti(m_cr_container, CR_COAST, nArrivingCoast);
		else
			cr().onSave_removeEntry(m_cr_container,CR_COAST);

		if (m_pRegionFrom!=pRegion)
			cr().onSave_move(m_cr_container,m_pRegionFrom->cr_region(),m_pRegionTo->cr_region());
	}

	if (m_nExtendedSize > 0)
	{
		cr().seti(m_cr_container, CR_SIZE, currentSize());
		if (currentSize()==m_pInfos->size) 
		{
			cr().onSave_removeEntry(m_cr_container,CR_DAMAGE); 
			cr().seti(m_cr_container, CR_CAPACITY, m_pInfos->capacity);
		}
		else cr().seti(m_cr_container, CR_DAMAGE, finalDamagePrct());
	}

	// set new captain if changed
	if (m_pInitialOwner!=m_pOwner)
	{
		if (m_pOwner)
			cr().seti(m_cr_container, CR_CAPTAIN, m_pOwner->id());
		else
			cr().onSave_removeEntry(m_cr_container,CR_CAPTAIN);
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


