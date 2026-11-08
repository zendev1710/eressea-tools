// Ship.cpp: implementation of the CShip class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "crdata.h"
#include "kTotrace.h"
#include "Unit.h"
#include "Region.h"
#include "Regions.h"
#include "MyEresseaCR.h"
#include "echecktypes.h"
#include "Container.h"
#include "Ship.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CHKSHIP if (!_strcmpi(itob(m_nID),"mc6d"))
//#define CHKBUILDING if (0)
#define BKPTSHIP {int zzzzzz=1;}

BOOL CShip::s_bShow_lbs_details = FALSE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShip::CShip(eID nShipID, CRegion *pRegionFrom, t_ship * pInfos, block * p_cr_ship, BOOL bNew):
CContainer(nShipID, pRegionFrom, bNew),
m_pRegionTo(pRegionFrom),
m_pInfos(pInfos),
m_nDamagePrct(0),
m_nTransportedWeight(0),
m_nCoast(COAST_UNKNOWN),
m_nSpeedUpByMagicStatus(-1),
m_bSpeedUpByMagicSummonStormElemental(FALSE),
m_sResultCapacity(""),
m_bDamaged(FALSE)
{
	CHKSHIP
	BKPTSHIP

	m_moveInfos.m_nCount = 0;
	m_moveInfos.m_bRouting = FALSE;
	m_cr_container = p_cr_ship;

	if (!bNew && pRegionFrom) 
	{
		if (!m_cr_container) m_cr_container = pRegionFrom->cr_ship(nShipID);
		cr().geti(m_cr_container, CR_SIZE, m_nInitialSize);
		cr().geti(m_cr_container, CR_DAMAGE, m_nDamagePrct);
		cr().gets(m_cr_container, EN_NAME, m_sShortName);
		cr().geti(m_cr_container, CR_CAPTAIN, m_nInitialOwnerID);
		if (!cr().geti(m_cr_container, CR_COAST, (int&)m_nCoast))
			m_nCoast = COAST_NONE;
		if (!m_pInfos)
		{
			CString sType("");
			cr().gets(m_cr_container, CR_TYPE, sType);
			sType = CCR::German2English(sType);
			int nUTID = findshiptype(sType);
			if (nUTID!=IDUT_UNKNOWN) 
				m_pInfos = shipFromUTID(nUTID);
			else
				errormsg("CShip::ship type is not recognized");
		}

		if (m_nDamagePrct > 0 && m_pInfos && m_nInitialSize==m_pInfos->size)
		{
			m_nInitialSize=(100-m_nDamagePrct)*m_nInitialSize/100;
			m_bDamaged=TRUE;
		}
	}
	
	if (bNew && m_pInfos) m_sShortName = m_pInfos->name;
	m_sOutputName.Format("%s (%s)",(LPCSTR)m_sShortName,sid());
	m_sFullName.Format("%s '%s'",((bNew || !m_pInfos)?"new":m_pInfos->name),(LPCSTR)m_sOutputName);
	m_sNewName = m_sShortName;
}

int CShip::compareOnRegion(CShip* s1, CShip* s2) 
{ 
	if (!s1->m_pRegionFrom || !s2->m_pRegionFrom) 
		return 0;
	return _strcmpi(s1->m_pRegionFrom->name(), s2->m_pRegionFrom->name());
}

int CShip::compareOnType(CShip* s1, CShip* s2) 
{ 
	if (!s1->m_pInfos || !s2->m_pInfos) 
		return 0;
	return _strcmpi(s1->m_pInfos->name, s2->m_pInfos->name);
}

int CShip::finalDamagePrct() const
{
	if (m_nExtendedSize==0) 
		return m_nDamagePrct;
	return 100-((currentSize() * 100)/m_pInfos->size);
}

BOOL CShip::fullyBuilt(int & nFinalSize, int & nMaxSize) 
{
	nMaxSize = m_pInfos->size;
	nFinalSize = currentSize();
	if (!m_bDamaged && (m_nInitialSize < m_pInfos->size))
		return (currentSize() == m_pInfos->size);
	return TRUE;
}

void CShip::Trace()
{
	POSITION pos = m_units.GetHeadPosition();
	while (pos)
	{
		CUnit * u = m_units.GetNext(pos);
		STOutput("unit %s\n",u->outputName());
	}
}

int CShip::totalSailingSkill() const
{
	CHKSHIP
	BKPTSHIP

	int nb = 0;
	POSITION pos = m_units.GetHeadPosition();
	while (pos)
	{
		CUnit * u = m_units.GetNext(pos);
		if (u) 
			nb += u->finalSailingSkill();
	}
	return nb;
}

int CShip::nbMen() const
{
	CHKSHIP
	BKPTSHIP

	int nb = 0;
	POSITION pos = m_units.GetHeadPosition();
	while (pos)
	{
		CUnit * u = m_units.GetNext(pos);
		if (u) 
			nb += u->people();
	}
	return nb;
}

void CShip::Stats(BOOL bExtendedShipsOnly)
{
	CHKSHIP
	BKPTSHIP

	int nMaxRegionname=24;
	int nMaxShipname=24;
	int nMaxShipShortname=nMaxShipname-6;
	CString sType("| ");
	CString sRegion("");
	CString sShipname("");
	CString sShipFullname("");
	CString sSuffixID("");
	CString sAvailable_lbs("");

	if (m_pRegionFrom && !m_pRegionFrom->hasOrderedUnits())
		return;

	ERTRACE("%sship statistics on %s...",bExtendedShipsOnly?"extended only ":" ",outputName());

	CRegion * pFinalRegion = m_pRegionFrom;
	if (m_pRegionFrom!=m_pRegionTo && m_pRegionTo)
		pFinalRegion = m_pRegionTo;


	if (pFinalRegion->nbPersons() == 0)
		return ;

	
	if (bExtendedShipsOnly && m_nExtendedSize==0)
		return ;

	int nbMarins = nbMen();

	if (m_pRegionFrom) sRegion = m_pRegionFrom->outputName();
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
		CString sResult("");
		int nDamage = finalDamagePrct();
		if (nDamage > 0) sPercentSize.Format("%3d%%%%",nDamage);
		float flbs = availableCapacity(sResult,CShip::s_bShow_lbs_details);
		if (m_pRegionFrom!=pFinalRegion)
		{
			CString sTmpRegion = m_pRegionTo->outputName();
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
		if (CShip::s_bShow_lbs_details && flbs<0) {
			int nlen=sResult.GetLength();
			STOutput("%s",(LPCSTR)sResult);
		}
	}
}


BOOL CShip::canGo(const CPoint & firstMove) const
{
	if (!m_pRegionFrom) return FALSE;
	CPoint s(m_pRegionFrom->x(),m_pRegionFrom->y());
	if (m_nCoast == COAST_NONE) return TRUE; //jsut created, can start from everywhere
	return coastNear(m_nCoast,coastFrom(s, firstMove));
}

// all units on ship move like boat does
void CShip::UpdateVesselsPosition()
{
	CHKSHIP
	BKPTSHIP

	if (m_pRegionTo && m_pRegionTo!=m_pRegionFrom && m_units.GetCount()>0)
	{
		POSITION pos = m_units.GetHeadPosition();
		while (pos) m_units.GetNext(pos)->OnShipMoved(this,m_pRegionTo);
	}
}

int CShip::transported_weight(CString & sResult, BOOL bDetails)
{
	CHKSHIP
	BKPTSHIP
	CString str("");
	if (m_nTransportedWeight == 0 || (bDetails && m_sResultCapacity.IsEmpty()))
	{
		m_nTransportedWeight = 0;
		if (m_units.GetCount()>0)
		{
			POSITION pos = m_units.GetHeadPosition();
			while (pos) 
			{
				CString sru("");
				m_nTransportedWeight+=m_units.GetNext(pos)->weight(sru,bDetails);
				if (bDetails) str+=sru;
			}

		}
		m_sResultCapacity=str;
	}
	if (bDetails && !str.IsEmpty())
	{
		sResult=":\n            ************   details   ************\n";
		sResult+=m_sResultCapacity;
		sResult+="            *************************************";

	}

	return m_nTransportedWeight;
}


int CShip::capacity() const
{
	// TODO: if damaged, should be less
	if (m_pInfos) return m_pInfos->capacity*LBS_RATIO;
	return 0;
}

float CShip::availableCapacity(CString & sResult, BOOL bDetails)
{
	CHKSHIP
	BKPTSHIP

	return (float)(capacity()-transported_weight(sResult,bDetails))/LBS_RATIOF;
}

BOOL CShip::SetRegionTo(CRegion * pRegion, const TMoveInfo & moveInfos)
{
	CHKSHIP
	BKPTSHIP

	m_pRegionTo=pRegion;
	m_moveInfos.copy(moveInfos);
	UpdateVesselsPosition();
	return TRUE;
}

BOOL CShip::insectInside()
{
	CHKSHIP
	BKPTSHIP

	POSITION pos = m_units.GetHeadPosition();
	while (pos) 
		if (m_units.GetNext(pos)->isInsect())
			return TRUE;
	return FALSE;
}

int CShip::moneyOn() const
{
	int nMoney = 0;
	POSITION pos = m_units.GetHeadPosition();
	while (pos) 
	{
		CUnit * pUnit = m_units.GetNext(pos);
		nMoney+= pUnit->money();
	}
	return nMoney;
}

BOOL CShip::checkings()
{
	CHKSHIP
	BKPTSHIP

	CRegion * pRegion = m_pRegionFrom;
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

		CUnit * pOwner = m_pOwner;
		if (m_nInitialOwnerID!=ID_UNKNOWN && !pOwner)
		{
			pOwner = sUnitsList.Find(m_nInitialOwnerID);
			if (CUnit::existingUnit(pOwner, m_nInitialOwnerID, m_pRegionFrom))
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
				CUnit * u = m_units.GetNext(pos);
				if (u!=m_pOwner) u->checkControlGiven(m_nID,m_pOwner);
			}
		}
	}

	return TRUE;
}

BOOL CShip::markCrossedRegions(eCoast & nArrivingCoast)
{
	CString s(outputName());
	nArrivingCoast = COAST_NONE;
	int nNbMoves = m_moveInfos.m_nCount;
	if (m_moveInfos.m_bRouting) nNbMoves = m_moveInfos.m_nRoutingIndex;

	CRegion *pRegionFrom = regionFrom();
	CRegion *pRegion = NULL;
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

BOOL CShip::overloaded(float & fAvailable_lbs, CString & sResult)
{
	CHKSHIP
	BKPTSHIP

	fAvailable_lbs = availableCapacity(sResult,CShip::s_bShow_lbs_details);
	return (fAvailable_lbs < 0);
}

BOOL CShip::save()
{
	CHKSHIP
	BKPTSHIP

	CRegion * pRegion = m_pRegionFrom;
	if (m_pRegionFrom!=m_pRegionTo && m_pRegionTo)
		pRegion = m_pRegionTo;

	if (pRegion->nbPersons() == 0)
		return TRUE; //TODO error

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
	else 
	{
		cr().onSave_removeEntry(m_cr_container,CR_CURRENT_CAPACITY); // or set current capacity
		cr().onSave_removeEntry(m_cr_container,CR_CURRENT_CARGO); // or set current cargaison
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
			//differences between maxladung & capacity? unknown
			cr().seti(m_cr_container, CR_MAX_CAPACITY1, m_pInfos->capacity);
			cr().seti(m_cr_container, CR_MAX_CAPACITY2, m_pInfos->capacity*100);
		}
		else 
		{
			//TODO: change capacity (?)
			//TODO: a boat being built cannot move if not 100% built
			//a boat being repared can move 
			cr().seti(m_cr_container, CR_DAMAGE, finalDamagePrct());
		}
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

BOOL CShip::SpeedUpPermanentlyByMagic() 
{
	if (m_nSpeedUpByMagicStatus==-1) {
		m_nSpeedUpByMagicStatus=0;
		block *beffects = cr_effects();
		CString se("the winds seem to favor this ship.");
		if (beffects)
		{
			entry * e = beffects->entries;
			while (e) {
				CString s = e->data.cp;
				s.MakeLower();
				if (s.Find(se)==0) {
					m_nSpeedUpByMagicStatus=1;
					break;
				}	
				e = e->next;
			}
		}
	}
	return (m_nSpeedUpByMagicStatus==1);
}

CShip::~CShip()
{

}
