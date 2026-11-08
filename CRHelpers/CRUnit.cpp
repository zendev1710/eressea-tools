// CRUnit.cpp: implementation of the CCRUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Eressea.h"
#include "CR.h"
#include "CRRegion.h"
#include "CRShip.h"
#include "CRBuilding.h"
#include "CRFaction.h"
#include "CRUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CHKUNIT if (!_strcmpi(itob(m_nID),"e8k9"))
#define CHKUNIT2 if (!_strcmpi(itob(m_nID),"zkxl"))


//#define CHKUNIT if (0)
#define BKPTUNIT {int zzzzzz=1;}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRUnit::CCRUnit(CCR & ocr, block * cr_unit, CCRRegion * pRegion):
	m_cr(ocr),
	m_pRegionFrom(NULL),
	m_cr_unit(cr_unit),
	m_nInitialPeople(0),
	m_pFaction(NULL),
	m_bUnknown(FALSE)
{
	CHKUNIT
	BKPTUNIT

	m_nID = cr_unit->ids[0];
	int nX=LOCATION_MAX_VALUE;
	int nY=LOCATION_MAX_VALUE;

	if (SetRegionFrom(pRegion))
	{
		nX=fromRegion()->x();
		nY=fromRegion()->y();
		fromRegion()->AddUnit(this);
	}
	SetXY(nX,nY);
	
	//cr_retrieveBasicInfos();

	init();

	CHKUNIT
	BKPTUNIT
}

CCRUnit::~CCRUnit()
{
}

CCR & CCRUnit::cr() { return m_cr;}

void CCRUnit::init() 
{
	m_nNbHorses = -1;
	m_nNbCarts = -1;
//	m_bGuarding = FALSE;
	m_bInAcademy = FALSE;
	m_bHeroes = FALSE;
	m_cr_talents = NULL;
//	m_items=NULL;
	m_nCombatStatus=UFT_UNKNOWN_VALUE;
	m_pInitialShip=NULL;
	m_pInitialBuilding=NULL;

	for (int i = 0;i<SK_MAX;i++)
		nSkills[i] = LEVEL_UNKNOWN;

	m_sOutputName.Format("%s (%s)",(LPCSTR)m_sShortName,sid());

	CHKUNIT
	BKPTUNIT
}


CCRRegion * CCRUnit::fromRegion() const 
{
	CHKUNIT
	BKPTUNIT

	return m_pRegionFrom;
}

CCRRegion * CCRUnit::SetRegionFrom(CCRRegion * pRegionFrom)
{
	CHKUNIT
	BKPTUNIT

	m_pRegionFrom = pRegionFrom;
	return fromRegion();
}

void CCRUnit::SetXY(int x, int y)
{
	m_nNewX=x;
	m_nNewY=y;
}

/*

BOOL CCRUnit::setFaction(CFaction *pFaction)
{
	if (!pFaction) return TRUE;
	if (m_pFaction==pFaction) return TRUE;
	if (m_pFaction && m_pFaction!=pFaction) 
		return errormsg("internal error - faction already set");
	m_pFaction = pFaction;
	return TRUE;
}

BOOL CCRUnit::fromOcean() const
{
	if (!fromRegion()) return FALSE;
	return fromRegion()->isOcean();
}

BOOL CCRUnit::isBuildingOwner() const
{
	CHKUNIT
	BKPTUNIT

	return IsInBuilding() && m_pBuilding->hasOwner(this);
}
*/
BOOL CCRUnit::setInitialMaintainCost(int nMaintainCost)
{
	CHKUNIT
	BKPTUNIT

	if (m_pInitialBuilding && (nMaintainCost>0))
	{	
		m_pInitialBuilding->SetInitialOwner(this);
		setMaintainCost(nMaintainCost);
		return TRUE;
	}
	return FALSE;
}

BOOL CCRUnit::setIsInBuilding(eID nBuildingID, int nMaintainCost)
{
	CHKUNIT
	BKPTUNIT

	BOOL bAsOwner = (nMaintainCost>0);
	if (unknownID(nBuildingID) && m_cr_unit) 
		return FALSE;

	if (unknownID(nBuildingID) && !m_cr_unit)
	{
		BOOL bInBuilding = FALSE;
		if (cr_isInsideSomething(&bInBuilding) && bInBuilding)
			return setInitialMaintainCost(nMaintainCost); 
		return FALSE;
	}
	
	if (setInBuilding(nBuildingID,bAsOwner))
	{
		m_pInitialBuilding = m_pBuilding;
		setInitialMaintainCost(nMaintainCost); 
		return TRUE;
	}
	return FALSE;
}

/*
BOOL CCRUnit::IsInBuilding(eID nID) const 
{
	CHKUNIT
	BKPTUNIT

	if (!m_pBuilding) return FALSE;
	if (!unknownID(nID) && m_pBuilding->id()!=nID) return FALSE;
	return TRUE;
}
*/
BOOL CCRUnit::IsOnShip(eID nID) const 
{
	CHKUNIT
	BKPTUNIT

	if (!m_pShip) return FALSE;
	if (!unknownID(nID) && m_pShip->id()!=nID) return FALSE;
	return TRUE;
}

CCRRegion * CCRUnit::region() const
{
	return fromRegion();
}

const char * CCRUnit::regionName(/*BOOL bFrom*/) 
{ 
	CCRRegion *pRegion = region(/*bFrom*/);
	return (pRegion ? pRegion->name() : REGION_VALUE_UNKNOWN);
}

const char * CCRUnit::regionOutputName(/*BOOL bFrom*/) 
{ 
	CCRRegion *pRegion = region(/*bFrom*/);
	return (pRegion ? pRegion->outputName() : REGION_VALUE_UNKNOWN);
}


// is currently (at the tour beginning) on a ship
BOOL CCRUnit::setIsOnShip(eID nShipID, BOOL bAsCaptain)
{
	CHKUNIT
	BKPTUNIT

	if (setOnShip(nShipID,bAsCaptain))
	{
		m_pInitialShip = m_pShip;
		if (bAsCaptain)
			m_pInitialShip->SetInitialOwner(this);
		return TRUE;
	}
	return FALSE;
}

BOOL CCRUnit::setOnShip(eID nShipID, BOOL bAsCaptain)
{
	CHKUNIT
	BKPTUNIT

	if (invalidID(nShipID)) 
		return FALSE;
//		return _error(UNKNOWNSHIP,"<unknown>");
/*	if (!cr().allShips().ship(m_pShip,regionFrom(),nShipID)) 
		return FALSE;*/
		//return _error(UNKNOWNSHIP,itob(nShipID));
	m_pShip->AddUnit(this,bAsCaptain);
	return TRUE;
}

BOOL CCRUnit::setInBuilding(eID nBuildingID, BOOL bAsOwner)
{
	CHKUNIT
	BKPTUNIT

	if (invalidID(nBuildingID)) 
		return FALSE;
		//return _error(UNKNOWNBUILDING,"building","<unknown>");
/*	if (!cr().allBuildings().building(m_pBuilding,regionFrom(),nBuildingID)) 
		return FALSE;*/
	//	return _error(UNKNOWNBUILDING,"building",itob(nBuildingID));
	m_pBuilding->AddUnit(this,bAsOwner);
	return TRUE;
}

const char * CCRUnit::sid() const {
	CString sname("");
	sname.Format("%s%s", /*IsTemp() ? "TEMP " :*/ "", name());
	return _strdup((LPCSTR)sname);
}

/*
/////////////////

void CCRUnit::SetAliveStatus(int n)
{
	m_nLives=n;
}

BOOL CCRUnit::isUnknownUnit()
{
	cr_unit();
	return isUnknown();
}
*/
void  CCRUnit::setMaintainCost(int n) 
{
	CHKUNIT
	BKPTUNIT

	//m_money.m_nMaintainCost=n;
}

/*
void  CCRUnit::setLearningCost(int n) 
{
	CHKUNIT
	BKPTUNIT

	m_money.m_nLearningCost=n;
}

void  CCRUnit::addMaintainCost(int n) 
{
	CHKUNIT
	BKPTUNIT

	m_money.m_nMaintainCost+=n;
}
*/

/////////////////////////////
// CR usage related functions

block * CCRUnit::cr_talents()
{
	if (m_cr_talents) return m_cr_talents;
	block *u = cr_unit();
	if (!u) return NULL;
	m_cr_talents = cr().talents(u);
	return m_cr_talents;
}

block * CCRUnit::cr_unit()
{
	CHKUNIT
	BKPTUNIT

	if (m_cr_unit) return m_cr_unit;
//	if (IsTemp()) 
//		return (block *)errormsg("internal error - cannot retrieve CR infos from TEMP unit:%s",outputName());
	block *r=cr_region();
	if(!r) return NULL;
	int nID=id();
	m_cr_unit = cr().units(r, &nID);
	if (!m_cr_unit)
	{ 
		if (!m_bUnknown) 
		{
			m_bUnknown = TRUE;
			block  * crunit = NULL;
			CCRRegion * pRegion = NULL;
			CCRUnit * u = this;
/*			if (s_bCheckings && somewhere(nID,pRegion))
				return (block *)_warning(6,UNITSOMEWHEREELSE,pRegion->outputName());
			return (block *)_warning(5,UNKNOWNUNIT); */
		}
		return NULL;
	}
	//cr_retrieveBasicInfos();
	return m_cr_unit;
}

// is he inside a building or ship
BOOL CCRUnit::cr_isInsideSomething(BOOL * pInBuilding, BOOL * pInShip)
{
/*	if (IsTemp())
	{
		CCRUnit * pMotherCRUnit = parent();
		if (!pMotherCRUnit) return FALSE;//errormsg("cr_isInsideSomething::no parent for TEMP unit: %s",outputName());
		pMotherCRUnit->cr_isInsideSomething();
		m_pInitialShip=pMotherCRUnit->m_pInitialShip;
		m_pInitialBuilding=pMotherCRUnit->m_pInitialBuilding;
		if (!m_pShip && !m_bLeaving && !m_bEntering) m_pShip = m_pInitialShip;
		if (!m_pInitialBuilding && !m_bLeaving && !m_bEntering) m_pBuilding = m_pInitialBuilding;
	}
	else*/
		cr_unit();
	if (pInBuilding) *pInBuilding=(m_pInitialBuilding!=NULL);
	if (pInShip) *pInShip=(m_pInitialShip!=NULL);
	return (m_pInitialBuilding!=NULL || m_pInitialShip!=NULL);
}

block * CCRUnit::cr_region()
{
	CCRRegion *r = regionFrom();
	if (!r) return NULL;
	return r->cr_region();
}

CCRUnit * CCRUnit::parent() const
{
/*	if (IsTemp())
	{
		if (IsRecruited()) return m_pRecruiter;
		return m_pMenGiver;
	}*/
	return NULL;
}

int CCRUnit::cr_combatStatus()
{
	CHKUNIT
	BKPTUNIT

	if (m_nCombatStatus!=UFT_UNKNOWN_VALUE) return m_nCombatStatus;
/*	if (IsTemp())
	{
		CCRUnit * motherCRUnit=parent();
		if (!motherCRUnit) m_nCombatStatus=COMBATSTATUS_UNKNOWN; else m_nCombatStatus = motherCRUnit->cr_combatStatus(); 
		return m_nCombatStatus;
	}*/
	if (!cr().geti(cr_unit(),CR_COMBATSTATUS,m_nCombatStatus))
		m_nCombatStatus = COMBATSTATUS_UNKNOWN;
	return m_nCombatStatus;
}

//////////
/*
void CCRUnit::setMother(CCRUnit *pMother)
{
	CHKUNIT
	BKPTUNIT

	m_pMother = pMother;
	if (m_pMother)
	{
		m_pInitialShip = m_pMother->m_pInitialShip;
		m_pInitialBuilding = m_pMother->m_pInitialBuilding;
		if (m_pInitialShip) setIsOnShip(m_pInitialShip->id());
		if (m_pInitialBuilding) setIsInBuilding(m_pInitialBuilding->id());
	}
}
*/
////////////////////

/*
int CCRUnit::nbWeapons(int nType, BOOL bWhenAttack)
{
	CHKUNIT
	BKPTUNIT

	int nbw = 0;
	int ifirst = 0;
	int ilast = WP_NBMAX-1;
	if (nType == WEAPONTYPE_MELEE) {ifirst=WP_FIRST_MELEE;ilast=WP_LAST_MELEE;}
	if (nType == WEAPONTYPE_POLEARM) {ifirst=WP_FIRST_POLEARM;ilast=WP_LAST_POLEARM;}
	if (nType == WEAPONTYPE_CROSSBOW) {ifirst=WP_FIRST_CROSSBOW;ilast=WP_LAST_CROSSBOW;}
	if (nType == WEAPONTYPE_BOW) {ifirst=WP_FIRST_BOW;ilast=WP_LAST_BOW;}
	if (nType == WEAPONTYPE_CATAPULT) {ifirst=WP_CATAPULT;ilast=WP_CATAPULT;}

	for (int i=ifirst;i<=ilast;i++)
	{
		int nID = s_weapons[i].m_nUTID;
		const char *pName=s_weapons[i].m_sName;
		int nWeight=(s_weapons[i].m_infos?s_weapons[i].m_infos->weight:0);
		if (nID!=-1)
		{
			TItemInfo * pItem = addItemInfo(nID,pName,UT_ITEM,nWeight);
			if (bWhenAttack) nbw += pItem->initialNumber();	else nbw += pItem->finalNumber();
		}
	}

	return nbw;
}
*/

/*
eSkill CCRUnit::skillnameToIndex(const char * pName)
{
	for (int i=0; i< SK_MAX; i++)
		if (!sSkillsEntries[i].CompareNoCase(pName)) return (eSkill)i;
	return SK_MAX;
}

int CCRUnit::defaultSkillLevel(eSkill nSkill) const
{
	ASSERT(m_pFaction);
	if (!m_pFaction) return 0;
	int nlvl= m_pFaction->raceModifier(nSkill);
	if (nlvl>0) nlvl++; //CG
	return nlvl;
}

int CCRUnit::skillLevelAfterRecruit(const char * pName) const
{
	return max(defaultSkillLevel(skillnameToIndex(pName))+1,0);
}

// german name is supposed to be given in param
int CCRUnit::level(const char * pName, int & nValue)
{
	// TODO: pbm - returns -1 sometimes
	if (nValue == LEVEL_UNKNOWN)
	{
		block * t = cr_talents();
		if (t) cr().skillLevel(m_cr_unit,pName,nValue,t);
	}
	return nValue; 
}

int CCRUnit::level(eSkill n, BOOL bAfterLearn)
{
//	if (n==m_eLearntSkill)
//		return learntSkillLevel(bAfterLearn);

	return level(sSkillsEntries[n],nSkills[n]);
}
*/

///////////////////////////////////////////

BOOL CCRUnit::IsCaptain() const 
{ 
	CHKUNIT
	BKPTUNIT

	return IsOnShip() && m_pShip->hasOwner(this);
}

BOOL CCRUnit::isAllied() const
{
	CHKUNIT
	BKPTUNIT

	if (!m_pFaction) return FALSE;//errormsg("isAllied::no faction for %s\n",outputName());
	return (m_pFaction->isAllied());
}


// X100
/*
int CCRUnit::capacity100(int nNbMoves, BOOL bOnRoad)
{
	if (!m_pFaction) return FALSE;//errormsg("maxWeight::no faction for %s",outputName());
	const CCRRace * pRace = m_pFaction->race();
	if (!pRace) return FALSE;//errormsg("maxWeight::no faction for %s",outputName());

	if (bOnRoad && nNbMoves>1) nNbMoves--;
	int ncc = nbCarts()*140;
	int nch = (nbHorses()-2*nbCarts())*20;
	return ((nch+ncc)*100+(nNbMoves==2?-pRace->manWeight()*people()*100:pRace->carryCapacity100()*people()));
}
*/

// X100
/*
int CCRUnit::transportedWeight100()
{
	if (people()==0) return 0;

	int lbsitems1 = 0;
	int lbsitems2 = 0;
	int lbsitems3 = 0;

	// items for those an exchange has been done (give or receive)
	TItemInfo * pItem = m_items;
	while (pItem) 
	{
		if (pItem->m_sName.CompareNoCase(EN_HORSE) && pItem->m_sName.CompareNoCase(EN_CART))
			lbsitems1 += pItem->finalWeight()*100;
		pItem = pItem->m_pNext;
	}

	// others items unit already had
	if (!IsTemp()) 
	{
		block * bu = cr_unit();
		if (bu)
		{
			block * bi = cr().items(bu);
			entry * e = NULL;
			if (bi) e = bi->entries;
			CString sItemName("");
			while (e)
			{
				int nType=-1;
				int i=-1;
				sItemName = CCR::German2English(e->tag->name);
				if (sItemName.CompareNoCase(EN_HORSE) && sItemName.CompareNoCase(EN_CART))
				{
					//'pbm silver'
					if (itemKind(sItemName, i, nType) || !sItemName.CompareNoCase(EN_SILVER))
					{
						if (nType!=UT_HERB && nType!=UT_POTION && !itemInfo(i, nType))
						{
							int n=0;
							if (cr().geti(bi, e->tag->name, n))
							{
								int lbs100 = ItemWeight(i)*n*100;
								lbsitems2 += lbs100;
							}
						}
					}
					else
					{
						int jj=1;
					}
				}
				e=e->next;
			}
		}
	}

//	CHKUNIT
//	output("lbs for %s:%f\n",outputName(),(float)(lbsitems1+lbsitems2)/100.0);

	// add silvers weight 
	lbsitems2+=money();

	// add carried units weight 
	if (m_carried)
	{
		POSITION pos = m_carried->GetHeadPosition();
		while (pos)	lbsitems3+=m_carried->GetNext(pos)->totalWeight100();
	}

	return lbsitems1+lbsitems2+lbsitems3;
}

// X100: transported weight & men, horse & cart weights
int CCRUnit::totalWeight100()
{
	if (people()==0) return 0;

	int nw = transportedWeight100();
	int nwh = nbHorses()*50*100;
	int nwc = nbCarts()*40*100;
	int nwm = 10;
	int nwmm = 0;
	if (m_pFaction) 
	{
		const CCRRace * pRace = m_pFaction->race();
		if (pRace) nwmm = pRace->manWeight();
	}
	if (nwmm!=0) nwm = nwmm; else
	{
		int rr=1;
	} 
	nwm *= people()*100;
	return nw+nwh+nwc+nwm;
}
*/

/*
BOOL CCRUnit::somewhere(eID nCRUnitID, CCRRegion *& pRegion)
{
	pRegion = NULL;
	if (!cr().Loaded()) return FALSE;
	return FALSE;//sRegionsList.FindUnit(nCRUnitID, pRegion);
}
*/

/*
BOOL CCRUnit::existingUnit(CCRUnit *& pCRUnit, eID nCRUnitID, CCRRegion * pRegion)
{
	if (pCRUnit) return TRUE;
	pCRUnit = NULL;
	if (!cr().Loaded()) return FALSE;
	block * r = NULL;
	if (pRegion) r = pRegion->cr_region();
	block * crunit = cr().units(r,&nCRUnitID);
	if (crunit)	
	{
		pCRUnit = new CCRUnit(crunit,nCRUnitID,pRegion);
//		sUnitsList.Add(pCRUnit);
		return TRUE;
	}
	return FALSE;
}
*/

void CCRUnit::checkInsideWhat()
{
	if (!m_cr_unit)	cr_isInsideSomething();
}

BOOL CCRUnit::isInBuilding(const char * sBuildingType, BOOL * pbWorking) const
{
	if (!m_pBuilding) return FALSE;
/*	const t_building * tinf = m_pBuilding->infos();
	if (!tinf) return FALSE;
	if (strcmpi(tinf->name,sBuildingType)) return FALSE;
	if (pbWorking) *pbWorking = m_pBuilding->isWorking();*/
	return TRUE;
}

////////////////////
// static methods
/*
int CCRUnit::paramIndexToCombatStatus(int i)
{
	switch (i)
	{
		case P_AGGRESSIVE	: return COMBATSTATUS_AGGRESSIVE;
		case P_FRONT		: return COMBATSTATUS_AGGRESSIVE; //???
		case P_BEHIND		: return COMBATSTATUS_REAR;
		case P_DEFENSIVE	: return COMBATSTATUS_DEFENSIVE;
		case P_NOT			: return COMBATSTATUS_NOT;
		case P_FLEE			: return COMBATSTATUS_FLEE;
		default				: break;
	}
	return COMBATSTATUS_UNKNOWN;
}
*/
/*
void CCRUnit::loadIDsFrom(TUTItemInfo tabitems[], int nMax, int nFirst)
{
	for (int i=nFirst;i<nMax;i++)
	{
		tabitems[i].m_nUTID=finditem(tabitems[i].m_sName);
		if (tabitems[i].m_nUTID==-1)
		{
			Error(UNKNOWNITEM,(LPCSTR)tabitems[i].m_sName);
			continue;
		}
		tabitems[i].m_infos=itemFromUTID(tabitems[i].m_nUTID);
	}
}

void CCRUnit::loadUTIDs()
{
	//loadIDsFrom(s_potions,PO_NBMAX);
	loadIDsFrom(s_weapons,WP_NBMAX);
	loadIDsFrom(s_armourItems,AR_NBMAX);
	loadIDsFrom(s_items,IT_NBMAX,IT_SILVER+1); //ignore IT_SILVER
}

*/
/*
BOOL CCRUnit::_warning(int nErrorNum, int nLevel, ...)
{
	outputSetRegionName(regionOutputName());
	va_list argList;
	va_start(argList, nLevel);
	errvalist(nLevel, nErrorNum, outputPrefix(), argList);
	va_end(argList);
	return FALSE;
}
*/
const char * CCRUnit::outputName() const
{
	return (LPCSTR)m_sOutputName;
}


CString CCRUnit::outputPrefix() const
{
	CString sResult;
	sResult.Format("[%s] %s: ",m_pFaction?m_pFaction->name():"unknown",(LPCSTR)outputName());
	return sResult;
}
/*
BOOL CCRUnit::_error(int nErrorNum,...)
{
	CHKUNIT
	BKPTUNIT

	ERTRACE("CRUnit::_error %d",nErrorNum);
	outputSetRegionName(regionOutputName());
	va_list argList;
	va_start(argList, nErrorNum);
	errvalist(0, nErrorNum, outputPrefix(), argList);
	va_end(argList);
	return FALSE;
}
*/


BOOL CCRUnit::cr_retrieveBasicInfos()
{
	CHKUNIT
	BKPTUNIT

	if (!m_cr_unit) return FALSE;

	if (!m_pFaction)
	{
		// retrieve faction
		int nPlayerID;
		CString sRaceName("");
		cr().gets(m_cr_unit,CR_TYPE,sRaceName);
		if (cr().gets(m_cr_unit,EN_NAME,m_sShortName))
		{
			m_sOutputName.Format("%s (%s)",(LPCSTR)m_sShortName,sid());
//			m_sNewName = m_sShortName;
		}
		if (cr().geti(m_cr_unit,CR_PLAYER,nPlayerID))
		{	; } //			m_pFaction = sFactionsList.Add(nPlayerID,CCR::German2English(sRaceName));

		// retrieve nb men
		//ASSERT(m_nPeople==0);
		int nb = 0;
		if (cr().geti(m_cr_unit,CR_NBMEN,nb))
			setInitialNbPeople(nb);
	}

	int nbID;
	int nHeroStatus = 0;
	int nGuardingStatus = 0;
	if (cr().geti(m_cr_unit,CR_CASTLE,nbID)) setIsInBuilding(nbID);
	if (cr().geti(m_cr_unit,CR_HERO,nHeroStatus) && nHeroStatus==1) m_bHeroes=TRUE;
	if (!m_bInitialGuard && cr().geti(m_cr_unit,CR_GUARDED,nGuardingStatus) && nGuardingStatus==1) setInitialGuarding();

	return (m_pFaction!=NULL);
}

BOOL CCRUnit::retrieveFaction()
{
	if (m_pFaction==NULL && cr().Loaded())
		cr_unit();
	
	return (m_cr_unit!=NULL);
}

BOOL CCRUnit::setInitialNbPeople(int n) 
{ 
	CHKUNIT
	BKPTUNIT

/*	if (m_nPeople!=0 && !m_pMenGiver)
	{
		if (isAlive()) return FALSE;//errormsg("setInitialNbPeople::internal error - inital people already set for unit %s",outputName());
		if (m_nPeople!=n) return FALSE;//errormsg("setInitialNbPeople::internal error - nbpeople different for unit %s",outputName());
		m_nPeople = 0;
	}*/
	m_nInitialPeople = n;
	//return addPeople(n);
	return TRUE;
}

////////////////////

BOOL CCRUnit::setInitialGuarding()
{
	m_bInitialGuard = TRUE;
//	return onRegionChangeGuardsNumber(1);
	return TRUE;
}

////////////////////

/*
int CCRUnit::studycost(const t_skills *talent) {
	if (!talent) return FALSE;//errormsg("studycost:: talent is NULL");
	if (talent->kosten<0) {
		int i;
		// Learn magic [ area ] 2000 - Lernkosten=2000 silver
		const char *s=getstr();
		i=findMagicSchool(s);
		if (i>=0) {
			//output(SCHOOLCHOSEN,magiegebiet[i]);
			i=atoi(igetstr(NULL));
		} else
			i=atoi(s);
		if (i<100) {
			i=200;
			_warning(ASSUMING200STUDYCOSTS,8);
		}
		return i;
	}
	return talent->kosten;
}
*/

BOOL CCRUnit::isInsect()		const { return (m_pFaction?m_pFaction->isRace(EN_INSECTS):FALSE);}
BOOL CCRUnit::isOrc()			const { return (m_pFaction?m_pFaction->isRace(EN_ORCS):FALSE);}
BOOL CCRUnit::isAquarian()	const { return (m_pFaction?m_pFaction->isRace(EN_AQUARIANS):FALSE);}
BOOL CCRUnit::isCat()			const { return (m_pFaction?m_pFaction->isRace(EN_CATS):FALSE);}
BOOL CCRUnit::isHuman()		const { return (m_pFaction?m_pFaction->isRace(EN_HUMANS):FALSE);}
BOOL CCRUnit::isElf()			const { return (m_pFaction?m_pFaction->isRace(EN_ELVES):FALSE);}
BOOL CCRUnit::isDwarf()		const { return (m_pFaction?m_pFaction->isRace(EN_DWARVES):FALSE);}
BOOL CCRUnit::isTroll()		const { return (m_pFaction?m_pFaction->isRace(EN_TROLLS):FALSE);}
BOOL CCRUnit::isRace(const char *pRaceName)	const { return (m_pFaction?m_pFaction->isRace(pRaceName):FALSE);}

const char * CCRUnit::name() const 
{ 
	return itob(m_nID);
}


