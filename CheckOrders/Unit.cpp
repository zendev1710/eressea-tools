// Unit.cpp: implementation of the CUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Unit.h"
#include "Eressea.h"
#include "crnames.h"
#include "CR.h"
#include "MyEresseaCR.h"
#include "EcheckTypes.h"
#include "Faction.h"
#include "Region.h"
#include "Ship.h"
#include "Building.h"
#include "Road.h"
#include "Units.h"
#include "Regions.h"
#include "Factions.h"
#include "Roads.h"
#include "Ships.h"
#include "Buildings.h"
#include "Message.h"
#include "Messages.h"
#include "Spells.h"
#include "Order.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WARNLEVEL_UNALLIED_MAYNOT_WAR (10)
#define WARNLEVEL_UNALLIED_MAYNOT (8)

#define CHKUNIT if (!_strcmpi(m_sShortName,"TEMP 7go2"))

//#define CHKUNIT if (!_strcmpi(itob(m_nID),"nego"))
#define CHKUNIT2 if (!_strcmpi(itob(m_nID),"3yex"))

#define igetkeyword(s)	findtoken(igetstr(s), UT_KEYWORD)

//#define CHKUNIT if (0)
#define BKPTUNIT {int zzzzzz=1;}

BOOL CUnit::s_bShow_lbs_details = FALSE;

static CUnitList s_soonHeroes(FALSE);
static CUnitList s_attackers(FALSE);
static CUnitList s_taxers(FALSE);
static CUnitList s_entertainers(FALSE);
static CUnitList s_traders(FALSE);
static CUnitList s_workers(FALSE);
static CUnitList s_recruited(FALSE);
static CUnitList s_teachers(FALSE);
static CUnitList s_makers(FALSE);
static CUnitList s_guards(FALSE);
static CUnitList s_movers(FALSE);
static CUnitList s_moversNeedRoads(FALSE);
static CUnitList s_transportedByHorse(FALSE);
static CUnitList s_carriers(FALSE);

static CUnitList s_potentialReceivers(FALSE);

static CUnits s_learners(FALSE);
static CUnits s_controlGivers(FALSE);

static CUnits s_unitsToRemove(FALSE);

/////////////

int magicCost(int nCurrentLevel, int nModifier)
{
	int nNewLevel = nCurrentLevel-nModifier+1;
	int sum=50;
	int add=50;
	for (int i=1;i<=nNewLevel;i++)
	{
		sum+=add;
		add+=50;
	}
	return sum;
}

/////////////

CString CUnit::sSkillsEntries[SK_MAX] = 
{
	CR_ALCHEMY,
	CR_ARMOURSMITHING,
	CR_BOW_SKILL,
	CR_CARTMAKING,
	CR_CATAPULT_SKILL,
	CR_CROSSBOW_SKILL,
	CR_ENDURANCE,
	CR_ENTERTAINMENT,
	CR_ESPIONAGE,
	CR_FORESTRY,
	CR_HERBALISM,
	CR_MAGIC,
	CR_MASONRY,
	CR_MELEE,
	CR_MINING,
	CR_PERCEPTION,
	CR_POLEARM,
	CR_QUARRYING,
	CR_RIDING,
	CR_ROADWORK,
	CR_SAILING,
	CR_SHIPCRAFT,
	CR_STEALTH,
	CR_TACTICS,
	CR_TAMING,
	CR_TAX,
	CR_TRADE,
	CR_WEAPONSMITHING,
	CR_UNARMED_COMBAT
};

CString CUnit::sEffectNames[EFT_MAX] = 
{
	EN_POTION_BUSYBEER,
	EN_POTION_BRAIN_WAX,
	EN_POTION_PEASANT_BLOOD
};

TUTItemInfo CUnit::s_armourItems[AR_NBMAX] = 
{
	TUTItemInfo(EN_CHAINMAIL,SK_ARMOURSMITHING),
	TUTItemInfo(EN_PLATEMAIL,SK_ARMOURSMITHING),
	TUTItemInfo(EN_SHIELD,SK_ARMOURSMITHING),
	TUTItemInfo(EN_LAEN_MAIL,SK_ARMOURSMITHING),
	TUTItemInfo(EN_LAEN_SHIELD,SK_ARMOURSMITHING)
};

TUTItemInfo CUnit::s_weapons[WP_NBMAX] = 
{
	TUTItemInfo(EN_SWORD,SK_WEAPONSMITHING),
	TUTItemInfo(EN_AXE,SK_WEAPONSMITHING),
	TUTItemInfo(EN_ADAMANTIUM_AXE,SK_WEAPONSMITHING),
	TUTItemInfo(EN_CLAYMORE,SK_WEAPONSMITHING),
	TUTItemInfo(EN_RUSTY_SWORD,SK_WEAPONSMITHING),
	TUTItemInfo(EN_LAEN_SWORD,SK_WEAPONSMITHING),
	TUTItemInfo(EN_FLAMING_SWORD,SK_NONE),
	TUTItemInfo(EN_SPEAR,SK_WEAPONSMITHING),
	TUTItemInfo(EN_HALBERD,SK_WEAPONSMITHING),
	TUTItemInfo(EN_LANCE,SK_WEAPONSMITHING),
	TUTItemInfo(EN_MALLORNSPEAR,SK_WEAPONSMITHING),
	TUTItemInfo(EN_CROSSBOW,SK_WEAPONSMITHING),
	TUTItemInfo(EN_MALLORNCROSSBOW,SK_WEAPONSMITHING),
	TUTItemInfo(EN_BOW,SK_WEAPONSMITHING),
	TUTItemInfo(EN_MALLORNBOW,SK_WEAPONSMITHING),
	TUTItemInfo(EN_ELVENBOW,SK_WEAPONSMITHING),
	TUTItemInfo(EN_CATAPULT,SK_CATAPULT)
};

TUTItemInfo CUnit::s_items[IT_NBMAX] = 
{
	TUTItemInfo(EN_SILVER),
	TUTItemInfo(EN_WOOD,SK_FORESTRY),
	TUTItemInfo(EN_STONE,SK_QUARRYING),
	TUTItemInfo(EN_IRON,SK_MINING),
	TUTItemInfo(EN_HORSE,SK_TAMING),
	TUTItemInfo(EN_MALLORN,SK_FORESTRY),
	TUTItemInfo(EN_LAEN,SK_MINING),
	TUTItemInfo(EN_CART,SK_CARTMAKING),
	TUTItemInfo(EN_CATAPULT,SK_CARTMAKING),
	TUTItemInfo(EN_CATAPULT_AMMUNITION,SK_QUARRYING),
	TUTItemInfo(EN_SEED,SK_HERBALISM),
	TUTItemInfo(EN_ADAMANTIUM)
};

BOOL CUnit::s_bCheckings = FALSE;

/*
TUTItemInfo CUnit::s_potions[PO_NBMAX] = 
{
	TUTItemInfo(EN_POTION_GOLIATH_WATER,SK_ALCHEMY),		
	TUTItemInfo(EN_POTION_SEVEN_MILE_TEA,SK_ALCHEMY),
	TUTItemInfo(EN_POTION_OF_TRUTH,SK_ALCHEMY),
	TUTItemInfo(EN_POTION_WATER_OF_LIFE,SK_ALCHEMY),
	TUTItemInfo(EN_POTION_PEASANT_BLOOD,SK_ALCHEMY),		
	TUTItemInfo(EN_POTION_BUSYBEER,SK_ALCHEMY),
	TUTItemInfo(EN_POTION_OINTMENT,SK_ALCHEMY),
	TUTItemInfo(EN_POTION_BERSERKERS_BLOOD,SK_ALCHEMY),
	TUTItemInfo(EN_POTION_DUNCEBUN,SK_ALCHEMY),
	TUTItemInfo(EN_POTION_BRAIN_WAX,SK_ALCHEMY),
	TUTItemInfo(EN_POTION_OF_NEST_WARMTH,SK_ALCHEMY),
	TUTItemInfo(EN_POTION_HORSEPOWER,SK_ALCHEMY)
};
*/

CItemReceivedInfo::CItemReceivedInfo(int nID, const char * pName, int nType, int nNb, CUnit * pGiver): 
m_nNb(nNb), m_pGiver(pGiver)
{
	m_nID = nID; 
	m_nType = nType;
	m_sName = pName;
} 

TItemInfo::TItemInfo(int id, const char *pName, int nNbHaving, int nWeight, int nType)
{
	m_nID = id; 
	m_nType = nType;
	m_nNbHaving = 0; 
	m_nNbReceived = 0;
	m_nNbGiven = 0;
	m_nNbReserved = 0;
	m_nNbMade = 0;
	m_nNbWon = 0;
	m_nNbSpent = 0;
	m_nWeight = nWeight;
	m_nNbHaving = nNbHaving;
	m_sName = pName;
	m_pAllReceiver = NULL;
}

void TItemInfo::addMade(int n, CRegion *pRegion) 
{
	if (n > 0)
	{
		m_nNbMade+=n; 
		if (pRegion) pRegion->onMadeItem(this,n);
	}
}

int divr(int n, int divider)
{
	if (divider==0) return 0;
	if (divider==1) return n;
	return (n+1)/divider;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnit::CUnit(CFaction *pFaction, int nID, int nTempStatus, int xRegion, int yRegion, const char *sOrder, int nbPeople):
	m_nID(nID),
	m_nPeople(nbPeople),
	m_nInitialPeople(0),
	m_nTemp(nTempStatus),
	m_nLives(ALIVE_UNKNOWN),
	m_pFaction(pFaction),
	m_cr_unit(NULL),
	m_bUnknown(FALSE),
	m_bCRConfirmedOrders(FALSE),
	m_bReceivedSomething(FALSE)
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	if (m_pFaction && !m_pFaction->hasPassword())
		SetAliveStatus(ALIVE_ILLUSION);
	m_sOrder=sOrder;

	BOOL bHasOrders = !m_sOrder.IsEmpty();

	if (SetRegionFrom(sRegionsList.FromCoords(xRegion,yRegion)))
		fromRegion()->AddUnit(this,bHasOrders);
	SetXY(xRegion,yRegion);

	init();

	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT
}

CUnit::CUnit(block * cr_unit, eID nID, CRegion * pRegion):
	m_nID(nID),
	m_pRegionFrom(NULL),
	m_cr_unit(cr_unit),
	m_sOrder(""),
	m_nTemp(0),
	m_nPeople(0),
	m_nInitialPeople(0),
	m_pFaction(NULL),
	m_bUnknown(FALSE),
	m_bCRConfirmedOrders(FALSE),
	m_bReceivedSomething(FALSE)
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	int nX=LOCATION_MAX_VALUE;
	int nY=LOCATION_MAX_VALUE;

	if (SetRegionFrom(pRegion))
	{
		nX=fromRegion()->x();
		nY=fromRegion()->y();
	}
	SetXY(nX,nY);
	
	cr_retrieveBasicInfos();

	init();

	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT
}

void CUnit::init() 
{
	// m_bFromOcean = FALSE;
	m_nPotentialAddedWeight=0;
	m_sCurrentOrder = "";
	m_nCurrentCmd = K_NO_CMD;
	m_sRegionNewName = "";
	m_sContainerNewName = "";
	m_sNewGroup = "";
	m_bWarningSilver = FALSE;
	m_bReceivedUnit = FALSE;
	m_nDisguisedFactionID=ID_UNKNOWN;
	m_bUnknownReceiver = FALSE;
	m_nNbMagicBags = -1;
	m_nNbTrollBelts = -1;
	m_nNbHorses = -1;
	m_nNbElvenHorses = -1;
	m_nNbCarts = -1;
	m_nNbRingsQuickFingers = -1;
	m_bTaught = FALSE;
	m_bGuarding = FALSE;
	m_bCombatStatusChanged = FALSE;
	m_bInAcademy = FALSE;
	m_bHeroes = FALSE;
	m_bPromotion = FALSE;
	m_nMoveStatus=MOVESTATUS_NOMOVE;
	m_carried = NULL;
	m_pCarrier=NULL;
	m_pRider=NULL;
	m_pRegionTo=NULL;
	m_pMadeRoad=NULL;
	m_pMadeBuilding=NULL;
	m_pMadeShip=NULL;
	m_pShip=NULL;
	m_pBuilding=NULL;
	m_sStartOfOrders="";
	m_sLongOrder="";
	spell=0;
	m_nNbBoughtLuxuries=0;
	m_nNbMaxTradeItems=0;
	m_nNbSoldItems=0;
	m_nNbBoughtItems=0;
	m_pMenGiver=NULL;
	m_pRecruiter=NULL;
	m_pMother=NULL;
	m_pNewController=NULL;
	m_cr_talents=NULL;
	m_cr_effects=NULL;
	m_buys=NULL;
	m_sells=NULL;
	m_items=NULL;
	m_teachs=NULL;
	m_pLearntSkill=NULL;
	m_bAttacking=FALSE;
	m_bLearning=FALSE;
	m_bWarningTeachTooMuch=FALSE;
	m_nStarvingStatus=UFT_UNKNOWN_VALUE;
	m_nCombatStatus=UFT_UNKNOWN_VALUE;
	m_nAura=UFT_UNKNOWN_VALUE;
	m_nFamiliarMageID=UFT_UNKNOWN_VALUE;
	m_eLearntSkill = SK_MAX;
	m_nLearntSkillLevels[ILRN_SKILL_BEFORE]=LEVEL_UNKNOWN;
	m_nLearntSkillLevels[ILRN_SKILL_AFTER]=LEVEL_UNKNOWN;
	m_nNbMenToTeach=0;
	m_nNbMenToBeTaught=0;
	m_nNbTeachers=0;
	m_nSpecifiedLearntCost=0;
	m_nEntertainmentAmount=-1;
	m_nTaxAmount=-1;
	m_bEntering=FALSE;
	m_bLeaving=FALSE;
	m_bImplicitLeaving=FALSE;
	m_pInitialShip=NULL;
	m_pInitialBuilding=NULL;
	m_pLastTeacher=NULL;

	for (int i = 0;i<SK_MAX;i++)
		m_nSkills[i] = LEVEL_UNKNOWN;

	for (int k = 0;k<EFT_MAX;k++) {
		m_nEffects[k] = EFFECT_UNKNOWN;
		m_bSpendEffects[k]=FALSE;
	}

	m_money.reset();

	m_moveInfos.m_bUseRoads = FALSE;
	m_moveInfos.m_bRouting = FALSE;
	m_moveInfos.m_nRoutingIndex = 0;
	m_moveInfos.m_nCount = 0;
	m_moveInfos.moves[0] = CPoint(m_nNewX,m_nNewY);

	if (IsTemp()) 
	{	
		m_sShortName = sid();
		m_sNewName = m_sShortName;
	}
	m_sOutputName.Format("%s (%s)",(LPCSTR)UTF8_to_ansi(m_sShortName),sid());

	CHKUNIT
	BKPTUNIT
}

CRegion * CUnit::regionTo() const 
{ 
	CHKUNIT
	BKPTUNIT

	return region(FALSE);
}

CRegion * CUnit::toRegion() const 
{
	CHKUNIT
	BKPTUNIT

	return m_pRegionTo;
}

CRegion * CUnit::fromRegion() const 
{
	CHKUNIT
	BKPTUNIT

	return m_pRegionFrom;
}

CRegion * CUnit::SetRegionTo(CRegion * pRegionTo)
{
	CHKUNIT
	BKPTUNIT

	m_pRegionTo = pRegionTo;
	return toRegion();
}

CRegion * CUnit::SetRegionFrom(CRegion * pRegionFrom)
{
	CHKUNIT
	BKPTUNIT

	m_pRegionFrom = pRegionFrom;
	//if (m_pRegionFrom && m_pRegionFrom->isOcean())
	//	m_bFromOcean=TRUE;
	return fromRegion();
}

BOOL CUnit::OnRename(int nType, const char *pname)
{
	CHKUNIT
	BKPTUNIT

	CString sName(pname);
	switch (nType)
	{
		case P_UNIT: m_sNewName=extractName(sName); break;
		case P_BUILDING:
		case P_CASTLE:
		case P_SHIP:
		m_sContainerNewName = extractName(sName); break;
		case P_REGION: m_sRegionNewName = extractName(sName); break;

		// TODO: P_FACTION
		default: break;
	}
	return TRUE;
}

BOOL CUnit::setRegionFrom(int Rx, int Ry)
{
	CHKUNIT
	BKPTUNIT

	CRegion * pRegion = sRegionsList.Add(Rx, Ry, people());

	ASSERT(pRegion);
	ASSERT(pRegion==fromRegion());
	ASSERT(m_nNewX==Rx);
	ASSERT(m_nNewY==Ry);
	
	if (pRegion!=fromRegion()) 
		return errormsg("setRegionFrom::bad region for unit %s",outputName());
	if (m_nNewX!=Rx) 
		return errormsg("setRegionFrom::bad region x coord for unit %s",outputName());
	if (m_nNewY!=Ry) 
		return errormsg("setRegionFrom::bad region y coord for unit %s",outputName());
	return TRUE;
}

BOOL CUnit::setFaction(CFaction *pFaction)
{
	if (!pFaction) 
		return TRUE;
	if (m_pFaction==pFaction) 
		return TRUE;
	if (m_pFaction && m_pFaction!=pFaction) 
		return errormsg("internal error - faction already set");
	m_pFaction = pFaction;
	return TRUE;
}

/*
BOOL CUnit::addOrder(const char *pOrder, int ikeyword)
{
	m_orders.AddTail(pOrder);
	return TRUE;
}
*/

// returns TRUE if order must be taken into account
BOOL CUnit::checkOrder(const char *sOrder, int this_command) {
	if (HasLongOrder()) {
		CString s(longOrder());
		int nInd = s.Find(' ');
		if (nInd!=-1) s = s.Left(nInd);
		int i=findkeyword(s);
		switch (i) {
			case K_CAST: if (this_command==i) return TRUE;
			case K_SELL:
			case K_BUY:
				if (this_command==K_SELL || this_command==K_BUY) return TRUE;
				// There is possible several SELL per unit
		}
		if ((i==K_FOLLOW && this_command!=K_FOLLOW) || (i!=K_FOLLOW && this_command==K_FOLLOW))
			return TRUE;		//CONSEQUENCE is only trigger

		return _warning(UNITALREADYHASLONGORDERS, 1, 0, longOrder());
	}
	return setLongOrder(sOrder);
}


CUnit::~CUnit()
{
	TTeachInfo *pt=m_teachs;
	TItemInfo *pi=m_items;
	TTradeItem *ps=m_sells;
	TTradeItem *pb=m_buys;
	TItemInfo *pinext=NULL;
	TTeachInfo *ptnext=NULL;
	TTradeItem *psnext=NULL;
	TTradeItem *pbnext=NULL;

	while (pt) {
		ptnext = pt->m_pNext;
		delete (pt);
		pt = ptnext;
	}
	while (pi) {
		pinext = pi->m_pNext;
		delete (pi);
		pi = pinext;
	}

	while (ps) {
		psnext = ps->m_pNext;
		delete (ps);
		ps = psnext;
	}
	while (pb) {
		pbnext = pb->m_pNext;
		delete (pb);
		pb = pbnext;
	}

	if (m_carried) 
		delete m_carried;

	m_carried=NULL;
	m_teachs=NULL;
	m_items=NULL;
	m_sells=NULL;
	m_buys=NULL;
}

BOOL CUnit::isWeapon(int ni, int *p) {for (int i=0;i<WP_NBMAX;i++) if (s_weapons[i].m_nUTID==ni) {if (p) *p=i; return TRUE;} return FALSE;}
BOOL CUnit::isArmour(int ni, int *p) {for (int i=0;i<AR_NBMAX;i++) if (s_armourItems[i].m_nUTID==ni) {if (p) *p=i; return TRUE;} return FALSE;}
BOOL CUnit::isItem(int ni, int *p) {for (int i=IT_SILVER+1;i<IT_NBMAX;i++) if (s_items[i].m_nUTID==ni) {if (p) *p=i; return TRUE;} return FALSE;}

// before any exchange
int CUnit::cr_nbItems(const char *pName)
{
	int nb = 0;
	if (!IsTemp()) 
		nb = cr().nbItems(this,pName);
	return nb;
}

int CUnit::nbMaxHorses(int nNbMoves, BOOL bMovingOnRoad)
{
	// ROAD management
	if (bMovingOnRoad && nNbMoves>1) nNbMoves--;
	int nRidinglevel = max(level(SK_RIDING),0);
	if (nNbMoves == 1) return (nRidinglevel*4+1)*people();
	if (nNbMoves == 2) return (nRidinglevel*2)*people();
	return 0;
}

int CUnit::nbMaxCarts(int nNbMoves, BOOL bMovingOnRoad)
{
	return nbMaxHorses(nNbMoves,bMovingOnRoad)/2;
}

int CUnit::nbRingsQuickFingers()
{
	CHKUNIT
	BKPTUNIT

	if (m_nNbRingsQuickFingers==-1) m_nNbRingsQuickFingers = nbItems(ID_UNKNOWN,EN_RING_OF_QUICK_FINGERS);
	return m_nNbRingsQuickFingers;
}

int CUnit::nbMagicBags()
{
	CHKUNIT
	BKPTUNIT

	if (m_nNbMagicBags==-1) 
		m_nNbMagicBags = nbItems(ID_UNKNOWN,EN_MAGIC_BAG);
	return m_nNbMagicBags;
}


int CUnit::nbHorses()
{
	CHKUNIT
	BKPTUNIT

	if (m_nNbHorses==-1) 
		m_nNbHorses = nbItems(ID_UNKNOWN,EN_HORSE);
	return m_nNbHorses;
}

int CUnit::nbTrollBelts()
{
	CHKUNIT
	BKPTUNIT

	if (m_nNbTrollBelts==-1) 
		m_nNbTrollBelts = nbItems(ID_UNKNOWN,EN_TROLLBELT);
	return m_nNbTrollBelts;
}


int CUnit::nbElvenHorses()
{
	CHKUNIT
	BKPTUNIT

	if (m_nNbElvenHorses==-1) m_nNbElvenHorses = nbItems(ID_UNKNOWN,EN_ELVEN_HORSE);
	return m_nNbElvenHorses;
}

int CUnit::nbCarts()
{
	CHKUNIT
	BKPTUNIT

	if (m_nNbCarts==-1) m_nNbCarts = nbItems(ID_UNKNOWN,EN_CART);
	return m_nNbCarts;
}

// after all exchanges & making
// just UT_ITEM type items - does not work for potions, herbs...
int CUnit::nbItems(int nID, const char *pName)
{
	if (K_NOTHING==nID)
	{
		if (pName==NULL || !_strcmpi(pName,"")) 
			return errormsg("CUnit::nbItems - internal error: unknown item");
		nID = finditem(pName);
	}
	TItemInfo * pItem = itemInfo(nID,UT_ITEM);
	if (pItem) return pItem->number();
	if (pName==NULL || !_strcmpi(pName,"")) pName = ItemName(nID);
	return cr_nbItems(pName);
}

int CUnit::nbHerbs(int nID, const char *pName)
{
	if (K_NOTHING==nID)
	{
		if (pName==NULL || !_strcmpi(pName,"")) 
			return errormsg("CUnit::nbHerbs - internal error: unknown item");
		nID = findherb(pName);
	}
	TItemInfo * pItem = itemInfo(nID,UT_HERB);
	if (pItem) return pItem->number();
	if (pName==NULL || !_strcmpi(pName,"")) 
		pName = herbname(nID);
	return cr_nbItems(pName);
}

int CUnit::nbPotions(int nID, const char *pName)
{
	if (K_NOTHING==nID)
	{
		if (pName==NULL || !_strcmpi(pName,"")) 
			return errormsg("CUnit::nbPotions - internal error: unknown item");
		nID = findpotion(pName);
	}
	TItemInfo * pItem = itemInfo(nID,UT_POTION);
	if (pItem) return pItem->number();
	if (pName==NULL || !_strcmpi(pName,"")) 
		pName = potionname(nID);
	return cr_nbItems(pName);
}

int CUnit::nbElements(int nID, const char *pName, int nType)
{
	if (nType==UT_ITEM) return nbItems(nID,pName);
	if (nType==UT_HERB) return nbHerbs(nID,pName);
	if (nType==UT_POTION) return nbPotions(nID,pName);
	return 0;
}

int CUnit::nbUTItems(eItem nUTID)
{
	if (nUTID >= IT_NBMAX) return errormsg("CUnit::nbUTItems - internal error: invalid id");
	if (nUTID == IT_SILVER) return money();
	return nbItems(s_items[nUTID].m_nUTID, s_items[nUTID].m_sName);
}

void CUnit::SetXY(int x, int y)
{
	m_nNewX=x;
	m_nNewY=y;
}

BOOL CUnit::fromOcean() const
{
	if (!fromRegion()) return FALSE;
	return fromRegion()->isOcean();
}

BOOL CUnit::isBuildingOwner() const
{
	CHKUNIT
	BKPTUNIT

	return IsInBuilding() && m_pBuilding->hasOwner(this);
}

BOOL CUnit::setInitialMaintainCost(int nMaintainCost)
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

BOOL CUnit::setIsInBuilding(eID nBuildingID, int nMaintainCost)
{
	CHKUNIT
	BKPTUNIT

	BOOL bAsOwner = (nMaintainCost>0);
	if (nBuildingID == ID_UNKNOWN && m_cr_unit) 
		return errormsg("CUnit::setIsInBuilding - internal error: unknown ID");

	if (nBuildingID == ID_UNKNOWN && !m_cr_unit)
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

BOOL CUnit::onLeaveShip(BOOL bOrderSpecified)
{
	CHKUNIT
	BKPTUNIT

	if (!m_pInitialShip) 
		return _warning(UNITNOTIN,4);

	m_pInitialShip->RemoveUnit(this);
	m_pShip = NULL;
	return TRUE;
}

BOOL CUnit::onLeaveBuilding(BOOL bOrderSpecified)
{
	CHKUNIT
	BKPTUNIT

	if (!m_pInitialBuilding) 
	{
		if (bOrderSpecified) 
			return _warning(UNITNOTIN,4);
		return TRUE;
	}

	m_pInitialBuilding->RemoveUnit(this);
	m_pBuilding=NULL;
	return TRUE;
}

BOOL CUnit::OnEnter(int nType, int nID, BOOL bAsOwner)
{
	CHKUNIT
	BKPTUNIT

	OnLeave();
	if (m_bEntering) 
		_warning(UNITALREADYENTERED,4);
	// if (m_bLeaving) return _warning(UNITLEAVEENTER,4); // ENTER fails

	m_bEntering = TRUE;
	if (nType == P_SHIP) 
		return OnEnterShip(nID,bAsOwner);
	return OnEnterBuilding(nID,bAsOwner);
}

BOOL CUnit::OnLeave(BOOL bOrderSpecified)
{
	CHKUNIT
	BKPTUNIT

	if (bOrderSpecified) 
	{
		m_bLeaving = TRUE;
		if (m_bEntering) 
			_warning(UNITLEAVEENTER,4);
	}
	
	m_bImplicitLeaving = TRUE;

	checkInsideWhat();

	if (m_pInitialShip) 
		return onLeaveShip(bOrderSpecified);
	return onLeaveBuilding(bOrderSpecified);
}

BOOL CUnit::onReceiveBuildingControl(eID nBuildingID, BOOL bGiverChangedOfContainer, int nCost)
{
	CHKUNIT
	BKPTUNIT

	//CHKUNIT2
	//BKPTUNIT

	setReceivedSomething();
	checkInsideWhat();
	CString sbid = itob(nBuildingID);
	if (invalidID(nBuildingID)) 
		return errormsg("onReceiveBuildingControl::unit %s - invalid id",outputName());
	if (!m_pBuilding)
	{

		// ignore in case the control giver changed from one container to another one:
		// GIVE CONTROL can be for the initial or new container
		if (m_pShip && bGiverChangedOfContainer) 
			return FALSE; 
		return _error(UNITRECEIVEDCONTROLBUT,"building",(LPCSTR)sbid);
	}
	if (m_pBuilding && m_pBuilding->id()!=nBuildingID) 
	{
		// ignore in case the control giver changed from one container to another one:
		// GIVE CONTROL can be for the initial or new container
		if (bGiverChangedOfContainer) 
			return FALSE;
		if (!m_bEntering) 
			return _error(UNITRECEIVEDCONTROLBUT,"building",(LPCSTR)sbid);
		return _error(UNITRECEIVEDCONTROLBUT,"building",(LPCSTR)sbid);
	}

	if (setInBuilding(nBuildingID,TRUE))
	{
		if (nCost > 0)
			setMaintainCost(nCost);
		return TRUE;
	}

	return FALSE;
}

BOOL CUnit::onReceiveShipControl(eID nShipID, BOOL bGiverChangedOfContainer)
{
	CHKUNIT
	BKPTUNIT

//	CHKUNIT2
//	BKPTUNIT

	setReceivedSomething();
	checkInsideWhat();

	CString sbid = itob(nShipID);

	if (invalidID(nShipID)) 
		return errormsg("onReceiveShipControl::unit %s - invalid id",outputName());
	if (!m_pShip) 
		return _error(UNITRECEIVEDCONTROLBUT,"ship",(LPCSTR)sbid);
	if (m_pShip && m_pShip->id()!=nShipID) 
	{
		// ignore in case the control giver changed from one container to another one:
		// GIVE CONTROL can be for the initial or new container
		if (bGiverChangedOfContainer)
		{
		 
			return FALSE; 
		}
		if (!m_bEntering) 
			return _error(UNITRECEIVEDCONTROLBUT,"ship",(LPCSTR)sbid);
		return _error(UNITRECEIVEDCONTROLBUT,"ship",(LPCSTR)sbid);
	}
	return setOnShip(nShipID,TRUE);
}

BOOL CUnit::IsInBuilding(eID nID) const 
{
	CHKUNIT
	BKPTUNIT

	if (!m_pBuilding) return FALSE;
	if (nID!=ID_UNKNOWN && m_pBuilding->id()!=nID) return FALSE;
	return TRUE;
}

BOOL CUnit::IsOnShip(eID nID) const 
{
	CHKUNIT
	BKPTUNIT

	if (!m_pShip) 
		return FALSE;
	if (nID!=ID_UNKNOWN && m_pShip->id()!=nID) 
		return FALSE;
	return TRUE;
}

void CUnit::onCheckGiveShipControl(const CShip *pShip, BOOL bChangedOfContainer)
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	if (m_pNewController)
	{
		int nShipID = ID_UNKNOWN;
		if (pShip) nShipID = pShip->id();
		if (invalidID(nShipID)) 
			_error(CANNOTGIVECONTROL,m_pNewController->outputName()); 
		else
			m_pNewController->onReceiveShipControl(nShipID,bChangedOfContainer);
		return;
	}
	errormsg("onCheckGiveShipControl::unit %s - no control receiver",outputName());
}

void CUnit::onCheckGiveBuildingControl(const CBuilding * pBuilding, BOOL bChangedOfContainer)
{
	CHKUNIT2
	BKPTUNIT

	// TODO= respect sequence order: ENTER, GIVE CONTROL, MAKE SHIP
	
	if (m_pNewController)
	{
		int nBuildingID=ID_UNKNOWN;
		if (pBuilding) nBuildingID = pBuilding->id();
		if (nBuildingID==ID_UNKNOWN) 
		{		
			CBuilding *pBuilding = m_pInitialBuilding;
			if (m_bEntering && m_pBuilding!=m_pMadeBuilding) pBuilding=m_pBuilding;
			if (pBuilding) nBuildingID=pBuilding->id();
		}
		if (invalidID(nBuildingID)) 
		{
			_error(CANNOTGIVECONTROL,m_pNewController->outputName());
			return ;
		}
		int nCost = m_money.m_nMaintainCost;
		m_money.m_nMaintainCost = 0;
		m_pNewController->onReceiveBuildingControl(nBuildingID,bChangedOfContainer,nCost);
		return;
	}
	errormsg("onCheckGiveBuildingControl::unit %s - no control receiver",outputName());
}

BOOL CUnit::onGiveUnit(CUnit * toUnit)
{
	CHKUNIT
	BKPTUNIT

	ASSERT(toUnit);

	if (!m_pFaction) 
		return errormsg("onGiveUnit::unit %s - no faction",outputName());
	if (m_pFaction->nbMenGivenUnits()+people()>5) 
		return _error(UNITGIVEUNITTOOMUCH);
	m_pFaction->AddNbMenGivenUnits(people());
	return onGiveMen(people(),toUnit,FALSE,TRUE); 
}



BOOL CUnit::onGiveControl(CUnit * toUnit)
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	ASSERT(toUnit);
	m_pNewController = toUnit;
	s_controlGivers.AddTail(this);
	return TRUE;
}

BOOL CUnit::onGiveHerbs(CUnit * toUnit, BOOL bAutomatic)
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	int nWarningLevel=(bAutomatic?17:6);

	ASSERT(toUnit);
	
	// give to himself - TODO error
	if (toUnit==this)
		return FALSE;// _warning(UNITNOHERBTOGIVE,4);

	BOOL bGiven = FALSE;
	int nb=0;
	CString sItemName("");
	entry * e = NULL;
	block * u = NULL;
	if (!IsTemp())
		u = cr_unit();
	if (u)
	{
		block * bi = cr().items(u);
		if (bi) e = bi->entries;
		while (e)
		{
			sItemName = CCR::German2English(e->tag->name);
			int nHerbID = K_NOTHING;
			if (itemIsHerb(sItemName, nHerbID)) {
				int nbGiven=e->data.i;
				if (onGive(nHerbID,sItemName,UT_HERB,nbGiven,toUnit,bAutomatic,TRUE,FALSE))
					bGiven = TRUE;
			}
			e=e->next;
		}
	}
	if (!bGiven && !m_bAttacking && !bAutomatic)
		return _warning(UNITNOHERBTOGIVE,nWarningLevel);

	return TRUE;
}

BOOL CUnit::onGiveAll(CUnit * toUnit, BOOL bAutomatic)
{
	CHKUNIT
	BKPTUNIT

//	CHKUNIT2
//	BKPTUNIT

	//ASSERT(toUnit);

	int nWarninLevel=(bAutomatic?15:6);

	BOOL bGiven=FALSE;
	int nb=0;
	int i=-1;
	int nbSilver = 0;
	CString sItemName("");
	block * u = cr_unit();
	block * bi = cr().items(u);

	entry * e = NULL;
	if (bi) e = bi->entries;
	while (e)
	{
		int nType=-1;
		sItemName = CCR::German2English(e->tag->name);
		if (!sItemName.CompareNoCase(EN_SILVER) || itemKind(sItemName, i, nType))
		{
			int nb = e->data.i;
			if (onGive(i,sItemName,nType,nb,toUnit,bAutomatic,TRUE,FALSE))
				bGiven = TRUE;
		}
		else
			errormsg("onGiveAll::internal error: unrecognized %s item in CR for %s\n",(LPCSTR)sItemName,outputName());
		e=e->next;
	}


	if (!bGiven) return _warning(UNITNOTHINGTOGIVE,nWarninLevel);

	return TRUE;
}

BOOL CUnit::onAttackedBy(CUnit * toUnit)
{
	CHKUNIT
	BKPTUNIT

	ASSERT(toUnit);

	BOOL bHalfOnly = TRUE; // TODO - customize this option

	int nWarninLevel=10;

	BOOL bGiven=FALSE;
	int nb=0;
	int i=-1;
	int nbSilver = 0;
	CString sItemName("");
	block * u = cr_unit();
	if (u)
	{
		block * bi = cr().items(u);

		entry * e = NULL;
		if (bi) e = bi->entries;
		while (e)
		{
			int nType=-1;
			sItemName = CCR::German2English(e->tag->name);
			if (itemKind(sItemName, i, nType) || !sItemName.CompareNoCase(EN_SILVER))
			{
				int nb = e->data.i;
				if (bHalfOnly) nb/=2;
				onLoose(i,sItemName,nType,nb,toUnit);
			}
			//else
			//	errormsg("onAttackedBy::internal error: unrecognized %s item in CR for %s\n",(LPCSTR)sItemName,outputName());
			e=e->next;
		}
	}
	return TRUE;
}

BOOL CUnit::onMakeBuildingOrder(t_building * pInfos, int nBuildingID, int nAmount)
{
	CHKUNIT
	BKPTUNIT

	CString sBuildingType(pInfos?pInfos->name:"building");

	//ERTRACE("%s - onmakebuilding %s (%d)",outputName(), (LPCSTR)sBuildingType, nAmount);

	if (nAmount<VALUE_NOTSPECIFIED) nAmount=VALUE_NOTSPECIFIED;
	if (invalidID(nBuildingID)) nBuildingID = ID_UNKNOWN;

	CRegion *pRegion=regionFrom();
	if (!sBuildingsList.building(m_pMadeBuilding,pRegion,pInfos,nBuildingID))
		return _error(UNKNOWNBUILDING,(LPCSTR)sBuildingType,itob(nBuildingID));

	if (m_pMadeBuilding->isNew())
	{
		nBuildingID = m_pMadeBuilding->id();

		// the maker systematically enters into new building, as owner
		OnEnter(P_BUILDING,nBuildingID,TRUE);

		// check if same kind of building existing on the region
		if (pInfos && pRegion->cr_hasBuilding((LPCSTR)sBuildingType))
			_warning(BUILDINGEXIST,2,(LPCSTR)sBuildingType);
	}

	return AddMakeOrder(nAmount,UT_BUILDING,nBuildingID);
}

int CUnit::nbEffects(const char *pEffectName) 
{
	ASSERT_VALID(this);
	CHKUNIT
	BKPTUNIT

	int nNb=0;
	block *beffects = cr_effects();
	CString se(CCR::English2German(pEffectName));
	se.MakeLower();
	
	if (beffects)
	{

		entry * e = beffects->entries;
		while (e) {
			CString s = e->data.cp;
			s.MakeLower();
			if (s.Find(se)!=-1)
			{
				// bug here
				char sc[255];
				strcpy_s(sc,255,(LPCSTR)s);
				if (sscanf_s(sc,"%d",&nNb,sizeof(int))>0)
					return nNb;
			}
			e = e->next;
		}
	}
	return nNb;
}

BOOL CUnit::modifyEffect(const char *pEffectName, int nNb) 
{
	block *beffects = cr_effects();
	CString se(CCR::English2German(pEffectName));
	CString sel(se);
	sel.MakeLower();
	char sss[50];
	if (!beffects)
	{
		if (nNb > 0) {
			block * bs = cr().cmds(m_cr_unit);
			if (!bs) bs = m_cr_unit->children; // unuseful ?
			m_cr_effects = cr().onSave_addBlock(m_cr_unit, bs, CR_EFFECTS, 0);
			sprintf_s(sss,50,"%d %s",nNb,(LPCSTR)se);
			cr().sets(m_cr_effects,NULL,sss);
		}
		return TRUE;
	}

	entry * eprev=NULL;
	entry * e = beffects->entries;
	while (e) {
		CString s = e->data.cp;
		s.MakeLower();
		if (s.Find(sel)!=-1)
		{
			if (nNb==0)
			{
				//remove entry
				entry * enext = e->next;
				if (eprev) eprev->next=enext; else beffects->entries=enext;
				free(e->data.cp);
				free(e);
				e = enext;
				return TRUE;
			}
			//modify entry
			sprintf_s(sss,50,"%d %s",nNb,(LPCSTR)se);
			free(e->data.cp);
			e->data.cp = (char*)malloc(1+strlen(sss));
			strcpy_s(e->data.cp, 1+strlen(sss), sss);
			return TRUE;
		}
		eprev=e;
		e = e->next;
	}

	if (nNb>0)
	{
		//TODO:addentry - bug if some other EFFECTS exist
		//DOESNOT WORK
		sprintf_s(sss,50,"%d %s",nNb,(LPCSTR)se);
		cr().sets(beffects,NULL,sss);
		return TRUE;
	}

	return FALSE;
}

BOOL CUnit::AddMakeOrder(int nAmount, int nUTType, eID nID, int nUTID)
{
	CHKUNIT
	BKPTUNIT

	m_makeOrder.m_nSize = nAmount;
	m_makeOrder.m_nUTType = nUTType;
	m_makeOrder.m_nUTID = nUTID;
	m_makeOrder.m_nID = nID;
	s_makers.AddTail(this);
	return TRUE;

}

BOOL CUnit::onMakeShipOrder(t_ship * pInfos, int nShipID, int nAmount)
{
	if (nAmount<VALUE_NOTSPECIFIED) nAmount=VALUE_NOTSPECIFIED;
	if (invalidID(nShipID)) nShipID = ID_UNKNOWN;
	CRegion *pRegion=regionFrom();
	if (!sShipsList.ship(m_pMadeShip,pRegion,nShipID,pInfos)) 
		return _error(UNKNOWNBUILDING,(pInfos?pInfos->name:"ship"),itob(nShipID));

	// maker becomes captain
	if (m_pMadeShip->isNew())
		setOnShip(m_pMadeShip->id(),TRUE);
	
	return AddMakeOrder(nAmount,UT_SHIP,nShipID);
}

BOOL CUnit::onMakeItemOrder(int nItemID, int nAmount)
{
	CHKUNIT
	BKPTUNIT
	return AddMakeOrder(nAmount,UT_ITEM,VALUE_UNKNOWN,nItemID);
}

BOOL CUnit::onMakePotionOrder(int nPotionID, int nAmount)
{
	CHKUNIT
	BKPTUNIT
	return AddMakeOrder(nAmount,UT_POTION,VALUE_UNKNOWN,nPotionID);
}

BOOL CUnit::onUsePotion(int nPotionID, int nAmount, BOOL bAutomatic)
{
	CHKUNIT
	BKPTUNIT

	// TODO: some potions are not used by USE order:
	// healing potion
	// TODO: check USE duncebun "xxid"
	// TODO: use 1 potion+use 1 another potion==>error (can use only 1 kind of item)
	// sample: Merlin (vzv2): '@USE 1 "peasant love potion"' - The unit already uses potions of truth.
	//TODO: 1 goliath water=+20lbs*people()
	//TODO: 1 seven mile tea=10 men like a horse
	//TODO:horsepower potion/T3:  horses goes twice as fast (2x plus loin)
	//TODO:potion of nest warmth/T3: the winter an insect can recruit outside from deserts
	CString s(potionname(nPotionID));
	eEffect nEff=effectnameToIndex(s);
	BOOL bWOL = FALSE;
	BOOL bEff = FALSE;

	if (nEff==EFT_MAX) {
		if (!s.CompareNoCase(EN_POTION_WATER_OF_LIFE))
			bWOL = TRUE;
	}
	else
		bEff=TRUE;

	if (!onGive(nPotionID,s,UT_POTION,nAmount,NULL,bAutomatic,FALSE,TRUE))
		return FALSE;

	BOOL bUsed = TRUE;

	if (bWOL)
	{ 
		int nbUsedWood=0;
		if (nAmount>0)
			nbUsedWood = nAmount*10;
		int nbWoodsHaving = 0;
		TItemInfo * pItem = addItemInfo(s_items[IT_WOOD].m_nUTID,s_items[IT_WOOD].m_sName,UT_ITEM);
		nbWoodsHaving = pItem->initialNumber();
		if (nbWoodsHaving<nbUsedWood) {
			//TODO: error/warning
			nbUsedWood=nbWoodsHaving;
		}
		if (nbUsedWood>0)
			pItem->spend(nbUsedWood);
	}
	else 
	if (bEff) {
		switch(nEff) {
			case EFT_BUSYBEER:
				addEffect(EFT_BUSYBEER,10*nAmount,TRUE);
				break;
			case EFT_BRAINWAX:
				addEffect(EFT_BRAINWAX,10*nAmount,TRUE);
				break;
			case EFT_PEASANT_BLOOD:
				//only demon can use peasant blood
				if (!isDemon())
					return _error(PEASANTBLOODFORDEMON);
				addEffect(EFT_PEASANT_BLOOD,100*nAmount,FALSE);
				break;
			default: 
				//pbm
				break;
		}
	}

	if (bUsed && cr().toSave())
		sMessagesList.Add(new CMessage(ICRMSG_USES, this, NULL, NULL, NULL, NULL, NULL, s));
	return bUsed;
}

BOOL CUnit::onUseSpecialItem(int nItemID, int nAmount, BOOL bAutomatic)
{
	CHKUNIT
	BKPTUNIT

	onGive(nItemID,ItemName(nItemID),UT_ITEM,nAmount,NULL,bAutomatic,FALSE,TRUE);
	return TRUE;
}

BOOL CUnit::onMakeHerbOrder(int nAmount)
{
	CHKUNIT
	BKPTUNIT

	if (level(SK_HERBALISM)<1) return _error(CANNOTMAKEHERB);
	return AddMakeOrder(nAmount,UT_HERB);
}

BOOL CUnit::onResearchHerbOrder()
{
	CHKUNIT
	BKPTUNIT

	if (level(SK_HERBALISM)<7) return _error(CANNOTRESEARCHHERB);
	CString sHerbName=fromRegion()->herbName();
	if (cr().toSave())
		sMessagesList.Add(new CMessage(ICRMSG_RESEARCH_HERB, this, fromRegion(), NULL, NULL, NULL, NULL, CCR::German2English((LPCSTR)sHerbName)));

	return TRUE;
}

BOOL CUnit::onMakeRoadOrder(eCoast nCoast, int nAmount)
{
	CHKUNIT
	BKPTUNIT

	if (nAmount<VALUE_NOTSPECIFIED) nAmount=VALUE_NOTSPECIFIED;
	if (level(SK_ROADWORK)<1) return _error(CANNOTMAKEROAD);

	CRegion *pRegion = regionFrom();
	if (!pRegion) return errormsg("onMakeRoadOrder::internal error - no from-region");

	if (pRegion->isGlacier() && !pRegion->cr_hasBuilding(EN_TUNNEL)) 
		return _error(MAKEROADNEED,EN_TUNNEL);
	if (pRegion->isTerrain(EN_DESERT) && !pRegion->cr_hasBuilding(EN_CARAVANSERAI)) 
		return _error(MAKEROADNEED,EN_CARAVANSERAI);
	if (pRegion->isTerrain(EN_SWAMP) && !pRegion->cr_hasBuilding(EN_DAM)) 
		return _error(MAKEROADNEED,EN_DAM);

	if (!sRoadsList.road(m_pMadeRoad,pRegion,nCoast))
		return _error(UNKNOWNBUILDING,"road",""/*nCoast*/);

	m_makeOrder.m_nParam = P_ROAD;
	return AddMakeOrder(nAmount,UT_NONE);
}

// nShipNumber should be a positive value
BOOL CUnit::OnEnterShip(eID nShipID, BOOL bAsOwner)
{
	CHKUNIT
	BKPTUNIT

	return setOnShip(nShipID,bAsOwner);
}

BOOL CUnit::OnEnterBuilding(int nBuildingID, BOOL bAsOwner)
{
	CHKUNIT
	BKPTUNIT

	if (setInBuilding(nBuildingID,bAsOwner))
	{
		if (level(SK_STEALTH)>0) 
		{
			int nLevel = 9;
			if (m_pBuilding && m_pBuilding->isAcademy())
				nLevel = 10;
			_warning(UNITENTERSTEALTH,nLevel,m_pBuilding?m_pBuilding->outputName():"building");
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CUnit::hasTeacher() const
{
	return m_nNbTeachers != 0;
}

int CUnit::addTeacher(CUnit * pTeacher, int & nbTaught, int & nStudentSkillLevel, int & nTeacherSkillLevel)
{
	CHKUNIT
	BKPTUNIT

	nbTaught = 0;
	if (m_nLives<ALIVE_USER_CRUNIT)
	{
		if (IsTemp())
			return _warning(RECRUITMUSTLEARN,6,pTeacher->outputName());
		retrieveFaction();
	}

	int nbPersons = people();
	if (nbPersons==0) 
		return _warning(NULLPERSONSLEARN,4,pTeacher->outputName());
	
	if (!hasTeacher()) 
		m_nNbMenToBeTaught=nbPersons;
	m_nNbTeachers++;
	
	if (m_pFaction && isAlive())
	{
		if (!m_bLearning || !m_pLearntSkill) 
			return _warning(UNITISTEACHED,2,pTeacher->outputName(),_msg(BUTDOESNTLEARN));
	
		nStudentSkillLevel = learntSkillLevel(FALSE);

		// a T2-or-more teacher can always teach a recruited unit, even if positive modifier for this one
		if (IsTemp() && IsRecruited()) 
			nStudentSkillLevel = 0;

		nTeacherSkillLevel = max(pTeacher->level(CCR::English2German(m_pLearntSkill->name,TRUE), nTeacherSkillLevel),0);
		if (nTeacherSkillLevel-nStudentSkillLevel<2) 
			return _warning(UNITBADTEACH,2,pTeacher->outputName(),m_pLearntSkill->name);
	}

	m_pLastTeacher = pTeacher;
	nbTaught = min(pTeacher->nbMenToTeach(),m_nNbMenToBeTaught);
	m_nNbMenToBeTaught -= nbTaught;
	if (isAlive() && cr().toSave())
	{
		m_bTaught = TRUE;
		sMessagesList.Add(new CMessage(ICRMSG_TEACH, pTeacher, NULL, this, NULL, NULL, NULL, CCR::German2English(m_pLearntSkill->name),nStudentSkillLevel+1));
		sMessagesList.Add(new CMessage(ICRMSG_TAUGHT, this, NULL, pTeacher, NULL, NULL, NULL, CCR::German2English(m_pLearntSkill->name)));
	}
	return TRUE;
}

BOOL CUnit::OnMove()
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	CRegion *pRegionFrom = regionFrom();
	if (!pRegionFrom) 
		return errormsg("CUnit::OnMove error - no region for %s",outputName());
	if (toRegion()) 
		return errormsg("OnMove::%s has already moved",outputName());
	CRegion * pRegionTo = sRegionsList.Add(m_nNewX, m_nNewY, 0);
	if (pRegionTo)
	{
		// Insects cannot enter glaciers.
		// TODO: and if they are on a ship?
		if (isInsect() && pRegionTo->isGlacier())
			return _error(INSECTCANNOTENTERGLACIER);
		pRegionFrom->RemoveUnit(this);
		pRegionTo->AddNbPersons(m_nPeople);
		SetRegionTo(pRegionTo);
		pRegionTo->AddUnit(this);
	}

	return (toRegion()!=NULL);
}

BOOL CUnit::OnShipMoved(const CShip * pShip, CRegion *pRegionTo)
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	if (!pShip) 
		return errormsg("OnShipMoved::null ship",outputName());
	if (!IsOnShip(pShip->id())) 
		return errormsg("OnShipMoved::%s is not on %s",outputName(),pShip->fullName());
	if (!pRegionTo) 
		return errormsg("OnShipMoved::null region",outputName());
	if (!IsCaptain())
	{
		SetMoveStatus(MOVESTATUS_SOMEWHERE);
		SetXY(pRegionTo->x(),pRegionTo->y());
		OnMove();
	}
	return (toRegion()!=NULL);
}

CRegion * CUnit::region(BOOL bFrom) const
{
	if (!bFrom && toRegion()) return toRegion();
	return fromRegion();
}

const char * CUnit::regionName(BOOL bFrom) 
{ 
	CRegion *pRegion = region(bFrom);
	return (pRegion ? pRegion->name() : REGION_VALUE_UNKNOWN);
}

const char * CUnit::regionOutputName(BOOL bFrom) 
{ 
	CRegion *pRegion = region(bFrom);
	return (pRegion ? pRegion->outputName() : REGION_VALUE_UNKNOWN);
}

BOOL CUnit::AddCarried(CUnit * pCarried)
{
	if (!m_carried)
	{
		m_carried = new CUnits(FALSE);
		s_carriers.AddTail(this);
	}
	m_carried->AddTail(pCarried);
	return TRUE;
}

BOOL CUnit::RemovedCarried(CUnit * pCarried)
{
	if (!m_carried) return FALSE;
	m_carried->Remove(pCarried);
	return TRUE;
}

BOOL CUnit::OnCarriedBy(CUnit *pCarrier)
{

	CHKUNIT
	BKPTUNIT

	if (!pCarrier)
		return FALSE; //TODO: error
	
	if (m_pCarrier)
	{
	/*	CUnit * pBadCarrier = m_pCarrier;
		if (m_pRider && (pCarrier==m_pRider)) 
			m_pCarrier = m_pRider;*/
		return FALSE; //TODO: error
	}
	m_pCarrier = pCarrier;
	SetMoveStatus(MOVESTATUS_CARRIED);
	pCarrier->AddCarried(this);
	return TRUE;
}

BOOL CUnit::OnRideOrder(CUnit *pRider)
{
	CHKUNIT
	BKPTUNIT

	if (m_pRider)
	{
	/*	CUnit * pBadCarrier = m_pCarrier;
		if (m_pRider && (pCarrier==m_pRider)) 
			m_pCarrier = m_pRider;*/
		return FALSE; //TODO: error
	}
	m_pRider = pRider; 
	s_transportedByHorse.AddTail(this);
	return TRUE;
}

// is currently (at the tour beginning) on a ship
BOOL CUnit::setIsOnShip(eID nShipID, BOOL bAsCaptain)
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

BOOL CUnit::setOnShip(eID nShipID, BOOL bAsCaptain)
{
	CHKUNIT
	BKPTUNIT

	if (invalidID(nShipID)) 
		return _error(UNKNOWNSHIP,"<unknown>");
	if (!sShipsList.ship(m_pShip,regionFrom(),nShipID)) 
		return _error(UNKNOWNSHIP,itob(nShipID));
	m_pShip->AddUnit(this,bAsCaptain);
	return TRUE;
}

BOOL CUnit::setInBuilding(eID nBuildingID, BOOL bAsOwner)
{
	CHKUNIT
	BKPTUNIT

	if (invalidID(nBuildingID)) 
		return _error(UNKNOWNBUILDING,"building","<unknown>");
	if (!sBuildingsList.building(m_pBuilding,regionFrom(),NULL,nBuildingID)) 
		return _error(UNKNOWNBUILDING,"building",itob(nBuildingID));
	m_pBuilding->AddUnit(this,bAsOwner);
	return TRUE;
}

const char * CUnit::sid() const {
	static CString sname("");
	sname.Format("%s%s", IsTemp() ? "TEMP " : "", name());
	return (LPCSTR)sname;
	//return _strdup((LPCSTR)sname);
}


void CUnit::SetMoveStatus(int nStatus) 
{ 
	CHKUNIT
	BKPTUNIT
	m_nMoveStatus = nStatus;
}

BOOL CUnit::OnMoveOrder(const TMoveInfo & moveinfos) 
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	if (HasMoved())
		return _error(UNITALREADYHASMOVED);

	m_moveInfos.m_bUseRoads = moveinfos.m_bUseRoads;
	m_moveInfos.m_bRouting = moveinfos.m_bRouting;
	m_moveInfos.m_bPauseInRoute = moveinfos.m_bPauseInRoute;
	m_moveInfos.m_nRoutingIndex = moveinfos.m_nRoutingIndex;
	m_moveInfos.m_nCount = moveinfos.m_nCount;
	for (int i = 0; i < NBMAXMOVE; i++) 
		m_moveInfos.moves[i] = moveinfos.moves[i];
	int nIndex = moveinfos.m_nCount;
	if (moveinfos.m_bRouting) nIndex = moveinfos.m_nRoutingIndex;
	SetMoveStatus(MOVESTATUS_BYHIMSELF);	// déplace lui-même
	SetXY(moveinfos.moves[nIndex].x,moveinfos.moves[nIndex].y);
	s_movers.AddTail(this);
	return TRUE;
}

/////////////////

void CUnit::empty() 
{
	CHKUNIT
	BKPTUNIT
	m_money.reset();
	m_nPeople=0; 
	m_nLives=ALIVE_ILLUSION;
}


void CUnit::CopyTo(CUnit *to) const {
	CHKUNIT
	BKPTUNIT

	//TODO LATER (only used by the DEFAULT order)
	to->m_nID=m_nID;
	to->m_nPeople=m_nPeople;
	to->m_money.m_nMoney=m_money.m_nMoney;
	to->SetTempStatus(m_nTemp);
	to->m_nLives=m_nLives;
	to->m_sStartOfOrders=m_sStartOfOrders;
	to->m_sLongOrder=m_sLongOrder;
	to->m_sOrder=m_sOrder;
	//to->lernt=lernt;
}

void CUnit::SetAliveStatus(int n)
{
	CHKUNIT
	BKPTUNIT

	m_nLives=n;
}

BOOL CUnit::setStartOfOrder(const char *pOrder)
{
	CHKUNIT
	BKPTUNIT

	m_sStartOfOrders = pOrder;
	// extract name from UNIT xxx; name [
	CString sName(m_sStartOfOrders);
	if (!IsTemp())
	{
		int nInd1 = sName.Find(";");
		int nInd2 = sName.Find("[");
		if (nInd1!=-1 && nInd2!=-1) sName=sName.Mid(nInd1+1,nInd2-nInd1-1); else sName="";
		sName.TrimLeft();
		sName.TrimRight();
	}
	else
	{
		int nInd = sName.Find("TEMP");
		if (nInd>0) sName=sName.Mid(nInd); else sName="";
		sName.TrimLeft();
		sName.TrimRight();
	}
	m_sShortName = sName;
	m_sNewName = m_sShortName;
	m_sOutputName.Format("%s (%s)",(LPCSTR)UTF8_to_ansi(m_sShortName),sid());
	
	int nAliveStatus = ALIVE_USER_UNIT;

	if (m_pFaction && !m_pFaction->hasPassword())
		nAliveStatus = ALIVE_ILLUSION;

	SetAliveStatus(nAliveStatus);

	return TRUE;
}

BOOL CUnit::setLongOrder(const char *sLongOrder)
{
	m_sLongOrder = sLongOrder;
	if (!fromRegion()) 
		return errormsg("CUnit::setLongOrder: internal error - null fromregion");
	if (!fromRegion()->isOcean() || isAquarian()) 
		return TRUE;
	// long order is ignored on ocean for a not aquarian
	return _warning(NOLONGORDER,12);
}

//////////////////
// money management

int  CUnit::money() const {
	CHKUNIT
	BKPTUNIT
	return m_money.m_nMoney;
}

int  CUnit::reservedSilver() const {
	CHKUNIT
	BKPTUNIT
	return m_money.m_nReserved;
}

void CUnit::emptyMoney() 
{
	CHKUNIT
	BKPTUNIT
	m_money.reset();
	//m_money.m_nReserved=m_money.m_nMoney=0;
} 

void CUnit::setMoney(int n) 
{ 
	CHKUNIT
	BKPTUNIT
	m_money.m_nMoney=n;
}

BOOL CUnit::isCostingBuildingOwner() const
{
	BOOL bIs = m_pBuilding && m_money.m_nMaintainCost > 0;
	//TO DO: see why assert makes error
	//ASSERT(!bIs || isBuildingOwner());
	return bIs;
}

BOOL CUnit::SpendMaintainCost()
{
	CHKUNIT2
	BKPTUNIT

	if (isBuildingOwner())
	{
		if (!m_pBuilding->upkeepPaidByMagic()) {
			m_money.m_nMoney-=m_money.m_nMaintainCost;
			m_money.m_nReserved-=m_money.m_nMaintainCost;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CUnit::inUsedBuilding() const
{
	return m_pBuilding && m_pBuilding->used();
}

BOOL CUnit::SpendLearningCost()
{
	CHKUNIT
	BKPTUNIT

	if (learningCost())
	{
		m_money.m_nMoney-=m_money.m_nLearningCost;
		m_money.m_nReserved-=m_money.m_nLearningCost;
		return TRUE;
	}
	return FALSE;
}

void CUnit::addMoney(int nVariation, BOOL bFromReservedAlso) 
{ 
	CHKUNIT
	BKPTUNIT
	m_money.m_nMoney+=nVariation;
	if (bFromReservedAlso) m_money.m_nReserved+=nVariation;
}

void CUnit::setInitialMoney(int nVariation) 
{ 
	CHKUNIT
	BKPTUNIT
	m_money.m_nInitialMoney = nVariation;
	addMoney(nVariation);
}
	
BOOL CUnit::spendMoney(int nVariation, BOOL bFromReservedAlso) 
{ 
	CHKUNIT
	BKPTUNIT

	//m_nReserved should be used there !
	int nRes=m_money.m_nMoney-nVariation;
	m_money.m_nMoney=nRes;
//	m_money.m_nMoney=bFromReservedAlso?nRes:max(nRes,0);
	if (bFromReservedAlso) 
		m_money.m_nReserved-=nVariation;
	return (nRes >= 0);
}

BOOL CUnit::moneyToRegion(BOOL bConsumeUpkeep) const
{
	CRegion * r=regionTo();
	if (!r)	
		return errormsg("moneyToRegion::no region for %s",outputName());

	CHKUNIT
	BKPTUNIT

	r->AddSilver(m_money,r!=regionFrom());
	if (bConsumeUpkeep) 
		r->SpendSilver(upkeepCost(),TRUE);
	return TRUE;
}

BOOL CUnit::SpendUpkeepCost(int & nNeededMoney)
{
	CHKUNIT
	BKPTUNIT

	if (!isAlive()) 
		return TRUE;

	BOOL bRes = TRUE;
	nNeededMoney = upkeepCost();
	m_money.m_nMoney-= nNeededMoney;
	if (m_money.m_nMoney < 0)
	{
		nNeededMoney = min(nNeededMoney,-m_money.m_nMoney);
		return FALSE;
	}
	nNeededMoney = 0;
	return TRUE;
}

// RESERVE ORDER
BOOL CUnit::onReserveSilver(int n, BOOL bEach)
{ 
	CHKUNIT
	BKPTUNIT

	if (bEach) 
		n*=people();
	int nPossible = min(n,m_money.m_nMoney-m_money.m_nReserved);
	if (nPossible < n) {
		n = nPossible;
	}
	if (n>0){
		m_money.m_nReserved+=n; 
		if (regionTo()) 
			regionTo()->AddReservedSilver(n);
	}
	return TRUE;
}

// RESERVE ORDER
BOOL CUnit::onReserve(int nThing, int n, int nUTType, BOOL bEach)
{ 
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	const char *pName = NULL;
	switch(nUTType) {
	case UT_ITEM: pName = ItemName(nThing);	break;
	case UT_POTION: pName = potionname(nThing);	break;
	case UT_HERB: pName = herbname(nThing);	break;
	default: break;
	}
	TItemInfo * pItem = addItemInfo(nThing,pName,nUTType);
	//ASSERT(pItem->nbGiven()==0);
	ASSERT(pName!=NULL);
	if (n==VALUE_ALL) 
		n=pItem->initialNumber();
	if (bEach) 
		n*=people();
	int nmin=min(pItem->initialNumber()-pItem->nbReserved(),n);
	nmin=max(0,nmin);
	if (nmin < n)
	{
		int nWarningLevel=8;
		_warning(RESERVEDTOOMUCH,nWarningLevel,pItem->m_sName,n,nmin);
	}
	n=nmin;
	pItem->reserve(n);
	return (n>0);
}

BOOL CUnit::onTeachOrder(int nUnitID, int temp)
{
	CHKUNIT
	BKPTUNIT

	BOOL bMarkAsTeacher = !hasStudent();
	if (!addStudent(nUnitID,temp)) return FALSE;
	if (bMarkAsTeacher)
		s_teachers.AddTail(this);
	return TRUE;
}

BOOL CUnit::onLearnOrder(t_skills * sk)
{
	CHKUNIT
	BKPTUNIT

	int nCost = studycost(sk);
	BOOL bMarkAsLearner = !m_bLearning;
	m_pLearntSkill = sk;
	m_nSpecifiedLearntCost = nCost;
	m_bLearning = TRUE;
	m_eLearntSkill = skillnameToIndex(CCR::English2German(m_pLearntSkill->name,TRUE));

	if (bMarkAsLearner) s_learners.AddTail(this);
	return TRUE;
}

BOOL CUnit::onReceiveMoney(int n, BOOL bFromReservedAlso, BOOL bExplicitGive)
{
	CHKUNIT
	BKPTUNIT

	addMoney(n,bFromReservedAlso);
	if (bExplicitGive)
		setReceivedSomething();
	return TRUE;
}

BOOL CUnit::onTransferMoney(int n, CUnit *pReceiver, BOOL bFromReservedAlso, BOOL bExplicitGive, BOOL bAutomatic)
{
	CHKUNIT
	BKPTUNIT
	CHKUNIT2
	BKPTUNIT


	int nPossible=max(0,money()-reservedSilver());
	if (n == VALUE_ALL) n = nPossible;
	int nGiven=min(n,nPossible);
	//	if (n == VALUE_ALL) // means ALL money given
//		n = max(0,money()-reservedSilver());

	if (nGiven > 0)
	{
		if (!spendMoney(nGiven) || (nGiven < n))
		{ 
			if (bExplicitGive && isAlive() && !m_bWarningSilver)
			{
				m_bWarningSilver = TRUE;
				_warning(UNITMISSSILVER,bAutomatic?8:4);
			}
			//n+=money();
		}
		n=nGiven;
		if (pReceiver && n>0)
		{
			pReceiver->onReceiveMoney(n,bFromReservedAlso,bExplicitGive);
			if (cr().toSave())
			{
				sMessagesList.Add(new CMessage(ICRMSG_GIVE, this, NULL, pReceiver, NULL, NULL, NULL, EN_SILVER, n));
				sMessagesList.Add(new CMessage(ICRMSG_RECEIVE, pReceiver, NULL, this, NULL, NULL, NULL, EN_SILVER, n));
			}
		}
	}
	return (n>0);
}

BOOL CUnit::isUnknownUnit()
{
	if (IsTemp())
	{
		CUnit * motherUnit=parent();
		if (!motherUnit) 
			return TRUE;
		//31/03/07
		if (!IsMadeTemp() && !IsRecruited()) 
			return TRUE; 
		return FALSE;
	}

	cr_unit();
	return isUnknown();
}

// GIVE order
BOOL CUnit::onGiveSilverOrder(int n, CUnit *pReceiver, BOOL bAutomatic)
{
	CHKUNIT
	BKPTUNIT
	
	return onGive(IT_SILVER,EN_SILVER,UT_ITEM,n,pReceiver,bAutomatic,FALSE,FALSE);
}

BOOL CUnit::onGiveMaxMoney(int & nSilver, CUnit *pReceiver, BOOL bFromSameFaction)
{
	CHKUNIT
	BKPTUNIT

	if (!pReceiver) return FALSE;
	if (bFromSameFaction && pReceiver->faction()!=faction()) return FALSE;
	int nGivenSilver = max(0,money()-reservedSilver());
	nGivenSilver = min(nGivenSilver,nSilver);
	if (nGivenSilver > 0)
	{
		m_money.m_nMoney-=nGivenSilver;
		nSilver -= nGivenSilver;
		pReceiver->addMoney(nGivenSilver);
		if (!bFromSameFaction)
		{
			//TODO: stock donated silver from one faction to another in same region
		}
//		if (cr().toSave())
//			sMessagesList.Add(new CMessage(ICRMSG_GIVE, this, NULL, pReceiver, NULL, NULL, NULL,EN_SILVER,n));
		return TRUE;
	}
	return FALSE;
}

BOOL CUnit::notEnoughMoney() const 
{ 
	CHKUNIT
	BKPTUNIT

	return m_money.m_nMoney<0;
}

void  CUnit::setMaintainCost(int n) 
{
	CHKUNIT
	BKPTUNIT

	m_money.m_nMaintainCost=n;
}

void  CUnit::setLearningCost(int n) 
{
	CHKUNIT
	BKPTUNIT

	m_money.m_nLearningCost=n;
}

//////////////////

void CUnit::SetTempStatus(int nTemp)
{
	m_nTemp = nTemp;
}

void CUnit::RemoveTempStatus(CFaction *pFaction)
{
	m_nTemp = -m_nTemp;
}

void CUnit::reset() {
	m_nID=-1;
	SetTempStatus(UNIT_STATUS_NOTTEMP);
}

/////////////////////////////
// CR usage related functions

block * CUnit::cr_talents()
{
	if (m_cr_talents) return m_cr_talents;
	block *u = cr_unit();
	if (!u) return NULL;
	m_cr_talents = cr().talents(u);
	return m_cr_talents;
}

block * CUnit::cr_effects()
{
	if (!IsTemp()) {
		if (m_cr_effects) return m_cr_effects;
		block *u = cr_unit();
		if (!u) return NULL;
		m_cr_effects = cr().effects(u);
	}
	return m_cr_effects;
}

block * CUnit::cr_unit()
{
	CHKUNIT
	BKPTUNIT

	if (m_cr_unit) 
		return m_cr_unit;
	if (IsTemp()) 
		return (block *)errormsg("internal error - cannot retrieve CR infos from TEMP unit:%s",outputName());
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
			CRegion * pRegion = NULL;
			CUnit * u = this;
			if (s_bCheckings && somewhere(nID,pRegion))
				return (block *)_warning(6,UNITSOMEWHEREELSE,pRegion->outputName());
			return (block *)_warning(5,UNKNOWNUNIT); 
		}
		return NULL;
	}
	cr_retrieveBasicInfos();
	return m_cr_unit;
}

// is he inside a building or ship
BOOL CUnit::cr_isInsideSomething(BOOL * pInBuilding, BOOL * pInShip)
{
	CHKUNIT
	BKPTUNIT

	if (IsTemp())
	{
		// could check m_pMother before
		CUnit * pMotherUnit = parent();
		if (!pMotherUnit || pMotherUnit==this) 
			return FALSE;
			//return errormsg("cr_isInsideSomething::no parent for TEMP unit: %s",outputName());
		pMotherUnit->cr_isInsideSomething();
		// CG 01/11/2008: initialship can be the one of the m_pMother
		if (!m_pInitialShip)
			m_pInitialShip=pMotherUnit->m_pInitialShip;
		// CG 01/11/2008: initialbuilding can be the one of the m_pMother
		if (!m_pInitialBuilding)
			m_pInitialBuilding=pMotherUnit->m_pInitialBuilding;
		if (!m_pShip && !m_bLeaving && !m_bEntering) 
			m_pShip = m_pInitialShip;
		if (!m_pInitialBuilding && !m_bLeaving && !m_bEntering) 
			m_pBuilding = m_pInitialBuilding;
	}
	else
		cr_unit();
	if (pInBuilding) *pInBuilding=(m_pInitialBuilding!=NULL);
	if (pInShip) *pInShip=(m_pInitialShip!=NULL);
	return (m_pInitialBuilding!=NULL || m_pInitialShip!=NULL);
}

block * CUnit::cr_region()
{
	CRegion *r = regionFrom();
	if (!r) return NULL;
	return r->cr_region();
}

CUnit * CUnit::parent() const
{
	if (IsTemp())
	{
		if (IsRecruited()) 
			return m_pRecruiter;
		return m_pMenGiver;
	}
	return NULL;
}

int CUnit::cr_aura()
{
	CHKUNIT
	BKPTUNIT

	if (m_nAura!=UFT_UNKNOWN_VALUE) 
		return m_nAura;
	if (IsTemp())
	{
		m_nAura = 0;
		return m_nAura;
	}
	cr().geti(cr_unit(),CR_AURA,m_nAura);
	return m_nAura;
}

int CUnit::cr_combatStatus()
{
	CHKUNIT
	BKPTUNIT

	if (m_nCombatStatus!=UFT_UNKNOWN_VALUE) 
		return m_nCombatStatus;
	if (IsTemp())
	{
		CUnit * motherUnit=parent();
		if (!motherUnit) m_nCombatStatus=COMBATSTATUS_UNKNOWN; else m_nCombatStatus = motherUnit->cr_combatStatus(); 
		return m_nCombatStatus;
	}
	if (!cr().geti(cr_unit(),CR_COMBATSTATUS,m_nCombatStatus))
		m_nCombatStatus = COMBATSTATUS_UNKNOWN;
	return m_nCombatStatus;
}

BOOL CUnit::isStarving()
{
	CHKUNIT
	BKPTUNIT

	return cr_starvingStatus()==1;
}

BOOL CUnit::cr_starvingStatus()
{
	CHKUNIT
	BKPTUNIT

	if (m_nStarvingStatus!=UFT_UNKNOWN_VALUE) return m_nStarvingStatus;
	if (IsTemp())
	{
		CUnit * motherUnit=parent();
		if (!motherUnit) m_nStarvingStatus=0; else m_nStarvingStatus = motherUnit->cr_starvingStatus(); 
		return m_nStarvingStatus;
	}
	if (!cr().geti(cr_unit(),CR_STARVINGSTATUS,m_nStarvingStatus))
		m_nStarvingStatus = 0;
	return m_nStarvingStatus;
}

//////////

TItemInfo * CUnit::itemInfo(int iItem, int nType) const
{
	TItemInfo * pItem = m_items;
	if (iItem==K_NOTHING) return NULL;
	while (pItem) if (pItem->m_nID==iItem && pItem->m_nType==nType) return pItem; else pItem = pItem->m_pNext;
	return NULL;
}

TItemInfo * CUnit::addItemInfo(int iItem, const char *pName, int nType, int nWeight)
{
	TItemInfo * pItem = itemInfo(iItem,nType);
	if (!pItem) 
	{
		if (nType==UT_ITEM && nWeight==0) nWeight=ItemWeight(iItem);
		pItem = new TItemInfo(iItem,pName,cr_nbItems(pName),nWeight,nType);
		addlist(&m_items,pItem);
	}
	return pItem;
}

BOOL CUnit::onReceive(int iItem, const char * pName, int nType, int n, BOOL bByAttacking)
{
	CHKUNIT
	BKPTUNIT

	// cats cannot use platemails
	if (!bByAttacking && isCat() && !_strcmpi(pName,EN_PLATEMAIL))
		_warning(CATNOPLATEMAIL,2);

	TItemInfo * pItem = addItemInfo(iItem,pName,nType);
	pItem->addReceived(n,bByAttacking);
	if (!bByAttacking) setReceivedSomething();
	return TRUE;
}

BOOL CUnit::onPotentialReceive(int iItem, const char * pName, int nType, int n, CUnit *pGiver)
{
	CHKUNIT
	BKPTUNIT

	BOOL bToAdd=m_potentialReceives.IsEmpty();
	m_potentialReceives.AddTail( new CItemReceivedInfo(iItem,pName,nType,n,pGiver));
	if (bToAdd)
		s_potentialReceivers.AddTail(this);
	return TRUE;
}

BOOL CUnit::onGive(int iItem, const char *pName, int nType, int & n, CUnit * pReceiver, BOOL bAutomatic, BOOL bFromGiveAll, BOOL bFromUse)
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	int nWarninLevel=(bAutomatic?15:6);
	CRegion * pReceiverRegion = NULL;
	if (pReceiver && !pReceiver->IsTemp())
	{
		if (!m_pRegionFrom->cr_hasUnit(pReceiver->id()))
		{
			//m_bUnknownReceiver = TRUE;
			return bAutomatic;
		}
	}

	if (!_strcmpi(pName,EN_SILVER))
		return onTransferMoney(bFromGiveAll?VALUE_ALL:n,pReceiver,TRUE,TRUE,bAutomatic);

	TItemInfo * pItem = addItemInfo(iItem,pName,nType);
	int nbh = pItem->nbLeft();
	int nbw = m_bAttacking?pItem->nbWon():0;
	int nw = 0;

	if (n < 0) 
	{
		if (pItem->nbGiven() > 0)
		{
			if (pItem->m_pAllReceiver)  
				return (bAutomatic?FALSE:_warning(UNITALREADYGAVE,nWarninLevel,pName)); //'GIVE ALL' after a previous GIVE ALL
			pItem->m_pAllReceiver = pReceiver;
		}
		n = nbh; // ALL
		if (m_bAttacking) nw = pItem->nbWon();
	}
	nbh -= n;
	if (nbh < 0)
	{
		if (!m_bAttacking && !bFromGiveAll)
		{		
			// TODO:
			// if target unit will move and item has weight, change warn level or check whether give is possible by pool item
			// if not GIVE ALL xxx
			_warning(bFromUse?UNITUSETOOMUCH:UNITGIVETOOMUCH,nWarninLevel,n,pName);
		}
		n -= -nbh;
		// at this step we do not know if unit will move or not
		if (pReceiver && pItem->basicWeight()>0)
			pReceiver->onPotentialReceive(iItem,pName,nType,-nbh,this);

		if (nbw > 0) nw = min(-nbh,nbw);
	}

	
	if (n > 0) pItem->give(n);
	if (nw > 0) pItem->addReceived(-nw,TRUE);

	if (n+nw>0)
	{
		if (pReceiver && pReceiver->onReceive(iItem,pName,nType,n+nw) && cr().toSave())
		{
			sMessagesList.Add(new CMessage(ICRMSG_GIVE, this, NULL, pReceiver, NULL, NULL, NULL,CCR::German2English(pName),n+nw));
			sMessagesList.Add(new CMessage(ICRMSG_RECEIVE, pReceiver, NULL, this, NULL, NULL, NULL,CCR::German2English(pName),n+nw));
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CUnit::onLoose(int iItem, const char *pName, int nType, int n, CUnit * pReceiver)
{
	CHKUNIT
	BKPTUNIT

//	CHKUNIT2
//	BKPTUNIT


	int nWarninLevel=10;
	if (!_strcmpi(pName,EN_SILVER)) 
		return TRUE; //what to do there ?

	TItemInfo * pItem = addItemInfo(iItem,pName,nType);
	if (pItem->m_pAllReceiver) return TRUE; //already loose items received by another attacker

	int nbh = pItem->nbLeft();
	pItem->m_pAllReceiver = pReceiver;
	if (n<0) n = nbh; // ALL
	nbh -= n;
	if (nbh < 0) n -= -nbh;
	if (n > 0)
	{
		pItem->spend(n);
		if (pReceiver && pReceiver->onReceive(iItem,pName,nType,n,TRUE) && cr().toSave())
		{
			//sMessagesList.Add(new CMessage(ICRMSG_GIVE, this, NULL, pReceiver, NULL, NULL, NULL,CCR::German2English(pName),n));
		}
		return TRUE;
	}

	return FALSE;
}

void CUnit::setMother(CUnit *pMother)
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

BOOL CUnit::onGiveMen(int n, CUnit * pReceiver, BOOL bAutomatic, BOOL bGiveUnit)
{
	CHKUNIT2
	BKPTUNIT

	//TODO: GIVE UNIT ==> nbtotalmen of faction to change
	int nbPersons = people();
	if (n<0) 
		n = nbPersons; 
	else 
	if (n > nbPersons)
	{
		_warning(UNITMISSPERSON, 4);
		n = nbPersons;
	}
	if (pReceiver && !pReceiver->addPeople(n,this,bGiveUnit)) 
		return FALSE;

	return removePeople(n);
}

////////////////////

int CUnit::nbWeapons(int nType, BOOL bWhenAttack)
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
		if (nID!=-1)
		{
			TItemInfo * pItem = addItemInfo(nID,pName,UT_ITEM,s_weapons[i].m_infos?s_weapons[i].m_infos->weight():0);
			if (bWhenAttack) 
				nbw += pItem->initialNumber();	
			else 
				nbw += pItem->number();
		}
	}

	return nbw;
}

eSkill CUnit::skillnameToIndex(const char * pName)
{
	for (int i=0; i< SK_MAX; i++)
		if (!sSkillsEntries[i].CompareNoCase(pName)) return (eSkill)i;
	return SK_MAX;
}

eEffect CUnit::effectnameToIndex(const char * pName)
{
	for (int i=0; i< EFT_MAX; i++)
		if (!sEffectNames[i].CompareNoCase(pName)) return (eEffect)i;
	return EFT_MAX;
}

int CUnit::defaultSkillLevel(eSkill nSkill) const
{
	CHKUNIT
	BKPTUNIT

	ASSERT(m_pFaction);
	if (!m_pFaction) return 0;
	int nlvl= m_pFaction->raceModifier(nSkill);
	//if (nlvl>0) nlvl++; //CG
	return nlvl;
}

int CUnit::skillLevelAfterRecruit(const char * pName) const
{
	return max(defaultSkillLevel(skillnameToIndex(pName)),0);
}

// german name is supposed to be given in param
int CUnit::level(const char * pName, int & nValue)
{
	// TODO: pbm - returns -1 sometimes
	if (nValue == LEVEL_UNKNOWN)
	{
		if (IsTemp())
		{
			BOOL bModifierLevel = FALSE;
			if (IsRecruited())
			{	
				nValue = skillLevelAfterRecruit(pName); 
				return nValue;
			}
			if (!m_pMenGiver) 
				return _warning(UNITUNKNOWNSKILLLEVEL,4,(LPCSTR)CCR::German2English(pName));
			int nlvl = m_pMenGiver->level(pName,nValue);
			return nlvl;
		}

		block * t = cr_talents();
		if (t) cr().skillLevel(m_cr_unit,pName,nValue,t);
	}
	return nValue; 
}

int CUnit::level(eSkill n, BOOL bAfterLearn)
{
	CHKUNIT
	BKPTUNIT

	if (m_pMenGiver==this) 
		return 0;

	if (n==m_eLearntSkill)
		return learntSkillLevel(bAfterLearn);

	if (IsTemp() && !IsRecruited() && m_pMenGiver)
	{
		// MADE temp (not recruited)
		if (m_nSkills[n]!=LEVEL_UNKNOWN) return m_nSkills[n];
		int nlvl = m_pMenGiver->level(n);
		m_nSkills[n] = nlvl;

		return m_nSkills[n];
	}

	return level(sSkillsEntries[n],m_nSkills[n]);
}

int CUnit::addEffect(eEffect nEff, int n, BOOL bMinZero) 
{
	CHKUNIT
	BKPTUNIT
	
	ASSERT_VALID(this);

	if (nEff>=EFT_MAX)
		return 0; //pbm

	int nb=effect(nEff);
	if (nb<0) nb=0;
	nb+=n;
	if (bMinZero && nb<0) nb=0;
	if (m_nEffects[nEff]!=nb)
		m_bSpendEffects[nEff]=TRUE; //TODO - optimize=store initial & finally compare
	m_nEffects[nEff]=nb;
	return nb;
}

int CUnit::effect(eEffect nEff)
{
	CHKUNIT
	BKPTUNIT

	ASSERT_VALID(this);

	if (nEff>=EFT_MAX)
		return 0; //pbm

	if (m_nEffects[nEff]!=EFFECT_UNKNOWN) return m_nEffects[nEff];
	m_nEffects[nEff]=0;
	//TODO - TEMP UNIT
	if (!IsTemp() && !IsRecruited())
		m_nEffects[nEff]=nbEffects(sEffectNames[nEff]);
	return m_nEffects[nEff];
}

///////////////////////////////////////////
// orders concerning money

BOOL CUnit::onEarnMoney(eSkill nSkill)
{
	CHKUNIT
	BKPTUNIT
	int nLevel = level(nSkill);
	if (nLevel < 1) 
		return _warning(UNITNOSKILLLEVEL,2,(LPCSTR)CCR::German2English(sSkillsEntries[nSkill]));
	return TRUE;
}

///////////////////
// trade management

BOOL CUnit::addTradeItems(int nItemID, int & nb, BOOL bBuy)
{
	CHKUNIT
	BKPTUNIT

	// remember how many & what is bought or sold
	int * pNb = (bBuy?&m_nNbBoughtItems:&m_nNbSoldItems);
	TTradeItem **pTrades = (bBuy?&m_buys:&m_sells);
	TTradeItem *pTrade = *pTrades;
	TTradeItem *pItem = *pTrades;
	if (nb==0) 
		return FALSE; //sell/buy 0 means nothing
	if 	(hasTradeItems(nItemID,&pItem))	{
		if (nb==VALUE_ALL) 
			pItem->m_nNb=VALUE_ALL; 
		else 
			pItem->m_nNb += nb;
	}
	else 
	{
		CString sName("");
		CRegion *r = regionFrom();
		pItem = NULL;
		int nPrice = 0;
		if (!r) return  errormsg("CUnit::addTradeItems - unknown region");
		if (!r->luxuryInfos(nItemID,sName,nPrice)) 
			return errormsg("CUnit::addTradeItems - impossible to retrieve infos for itemID %d",nItemID);
		pItem = new TTradeItem(nItemID,nb,sName,nPrice,pTrades);
	}
	if (nb!=VALUE_ALL) *pNb += nb; else {}//TODO: 'SELL ALL' management
	return (pItem!=NULL);
}

BOOL CUnit::hasTradeItems(int nItemID, TTradeItem ** pItem) const
{
	CHKUNIT
	BKPTUNIT

	if (nItemID == ITEMID_NONE)	return (m_sells!=NULL || m_buys!=NULL);
	if (pItem==NULL) return FALSE;
	TTradeItem *pCurr = *pItem;
	while(pCurr) 
	{
		if (pCurr->m_nItemID == nItemID)
		{
			*pItem = pCurr;
			return TRUE;
		}
		pCurr = pCurr->m_pNext;
	}
	return FALSE;
}

////////////

BOOL CUnit::addStudent(int nUnitID, int bIsTemp)
{
	CHKUNIT
	BKPTUNIT

	// remember how many & what is bought or sold
	TTeachInfo **pTeachs = &m_teachs;
	TTeachInfo *pTeach = m_teachs;
	if 	(hasStudent(nUnitID,bIsTemp,&pTeach))
		return _warning(UNITALREADYTEACH,4,itob(nUnitID));
	pTeach = new TTeachInfo(nUnitID,bIsTemp,pTeachs);
	return (pTeach!=NULL);
}

BOOL CUnit::hasStudent(int nUnitID, int bIsTemp, TTeachInfo ** pTeach) const
{
	CHKUNIT
	BKPTUNIT

	if (nUnitID == -1)	return (m_teachs!=NULL);
	if (pTeach==NULL) return FALSE;
	TTeachInfo *pCurr = *pTeach;
	while (pCurr) 
	{
		if (pCurr->m_nUnitID == nUnitID && bIsTemp==pCurr->m_bIsTemp)
		{
			*pTeach = pCurr;
			return TRUE;
		}
		pCurr = pCurr->m_pNext;
	}
	return FALSE;
}

BOOL CUnit::buy(TTradeItem * pBuy, CRegion *r)
{
	CHKUNIT
	BKPTUNIT

	if (!pBuy) return FALSE;
	if (!r) r = regionFrom();
	if (!r) return FALSE;
	int ns=0;
	if (r->canBuy(pBuy,ns))
	{
		int prInitial=pBuy->cost(ns);
		int pr=prInitial;
		// TODO: if not enough money change pBuy->m_nNb;
		if (!spendMoney(pr,TRUE))
		{
			pr+=m_money.m_nMoney;
			// update pBuy->m_nNb according to pr silver available
			pBuy->setCost(ns,pr);
		}

		if (pr > 0)
		{
			if (pr<prInitial)
				_warning(UNITBUYNOTENOUGHMONEY,4,pBuy->m_sName,prInitial-pr);

			m_money.m_nByBuy += pr;
			addItemInfo(pBuy->m_nItemID, pBuy->m_sName, UT_ITEM)->addMade(pBuy->m_nNb,regionFrom());
			//TODO: specify correct wanted value
			//TODO: 1 or several messages for several buy ?
			if (cr().toSave())
			{
				sMessagesList.Add(new CMessage(ICRMSG_BUY, this, NULL, NULL, NULL, NULL, NULL, pBuy->m_sName,pBuy->m_nNb,pBuy->m_nNb));
				sMessagesList.Add(new CMessage(ICRMSG_PAYBUY, this, NULL, NULL, NULL, NULL, NULL,"",abs(m_money.m_nByBuy),abs(m_money.m_nByBuy)));
			}
	
			return r->onBuy(pBuy);
		}
		//
		return _warning(UNITBUYNOTENOUGHMONEY,4,pBuy->m_sName,prInitial);
	}
	return FALSE;
}

BOOL CUnit::sell(TTradeItem * pSell, CRegion *r, int & nbAvailableTrades)
{
	CHKUNIT
	BKPTUNIT
	BOOL bAll = FALSE;

	if (!pSell) return FALSE;
	if (!r) r = regionFrom();
	if (!r) return FALSE;
	if (!r->canSellLuxury(pSell->m_nItemID)) 
		return _error(UNITITEMNOTSELLABLE,(LPCSTR)pSell->m_sName);
	if (r->canSell(pSell))
	{
		if (pSell->m_nNb == VALUE_ALL)
		{
			pSell->m_nNb = r->nbMaxLuxuriesToSell();
			bAll = TRUE;
		}

		int nb = nbItems(pSell->m_nItemID,pSell->m_sName);
		if (pSell->m_nNb > nbAvailableTrades)
		{
			int nWarnLevel=4;
			if (bAll==TRUE) nWarnLevel=5;
			if (nb > 0)
				_warning(UNITSELLNOTENOUGHTRADE,nWarnLevel,pSell->m_nNb,(LPCSTR)pSell->m_sName,nbAvailableTrades);
			pSell->m_nNb = nbAvailableTrades;
		}
	
		if (nb < pSell->m_nNb) 
		{
			CString s1;
			CString s2("none");
			int nwlvl = 17;
			s1.Format("%d %s",pSell->m_nNb,(LPCSTR)pSell->m_sName);
			if (nb != 0)
			{
				s2.Format("only %d",nb);
				nwlvl=8;
			}
			_warning(UNITCANNOTSELLSOMUCH,nwlvl,s1,s2);
			pSell->m_nNb = nb;
		}
		if (pSell->m_nNb > 0)
		{
			if (r->onSell(pSell))
			{
				int pr = pSell->m_nPrice*pSell->m_nNb;
				m_money.m_nBySell+= pr;
				addItemInfo(pSell->m_nItemID, pSell->m_sName, UT_ITEM)->spend(pSell->m_nNb);
				//TODO: specify correct wanted value
				//TODO: 1 or several messages for several sell of the same luxury ? several for the moment
				if (cr().toSave())
					sMessagesList.Add(new CMessage(ICRMSG_SELL, this, NULL, NULL, NULL, NULL, NULL, pSell->m_sName,pSell->m_nNb,pSell->m_nNb));
				nbAvailableTrades-=pSell->m_nNb;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CUnit::onRecruit(int nbPersons, CUnit *pRecruiter)
{
	CRegion * r = regionFrom();
	if (isInsect() && !r->isTerrain(EN_DESERT))
	{
		//TODO: During the months of Herdfeuer, Eiswind and Schneebann, insects can only recruit in deserts.
		// return _error();
	}

	if (!m_sShortName.CompareNoCase("TEMP thjz"))
		int hhyyuu=1;

	m_pRecruiter = pRecruiter;
	spendMoney(nbPersons*m_pFaction->RecruitCost(),TRUE);
	if (!r) 
		return errormsg("CUnit::onRecruit: region not found for %s",outputName());
	addPeople(nbPersons);
	if (isOrc()) 
		nbPersons=(nbPersons+1)/2;
	r->AddNbPersons(nbPersons,TRUE);
	s_recruited.AddTail(this); // mark unit as recruited - checks will be done later
	return TRUE;
}

BOOL CUnit::onAttackOrder(CUnit * attacked_unit)
{
	//TODO: when done before GIVE
//	if (attacked_unit) 
//		attacked_unit->onAttackedBy(this);
	
	if (m_bAttacking) 
		return TRUE;
	m_bAttacking = TRUE;
	s_attackers.AddTail(this); // mark unit as attacker - checks will be done later
	regionFrom()->setWar();
	return TRUE;
}

BOOL CUnit::onCombatStatus(int i)
{
	CHKUNIT
	BKPTUNIT

	m_nCombatStatus = paramIndexToCombatStatus(i);
	m_bCombatStatusChanged=TRUE;
	return TRUE;
}

////////////////////////
// actions earning money

BOOL CUnit::onTaxOrder(int nAmount)
{
	if (nAmount<=0) nAmount=-1;
	m_nTaxAmount = nAmount;

	// check tax level using loaded CR
	if (!onEarnMoney(SK_TAX)) 
		return FALSE;
	s_taxers.AddTail(this); // mark unit as taxer - other checks will be done later
	return TRUE;
}

BOOL CUnit::onEntertainOrder(int nAmount)
{
	if (nAmount<=0) nAmount=-1;
	m_nEntertainmentAmount = nAmount;

	// check entertainment level using loaded CR
	if (!onEarnMoney(SK_ENTERTAIN)) return FALSE;
	s_entertainers.AddTail(this); // mark unit as entertainer - other checks will be done later
	return TRUE;
}

BOOL CUnit::onTradeOrder(int nItemID, int nb, BOOL bBuy)
{
	CHKUNIT
	BKPTUNIT

	// check trade level using loaded CR
	if (!onEarnMoney(SK_TRADE)) return FALSE;
	BOOL bMarkAsTrader = !hasTradeItems();
	if (!addTradeItems(nItemID,nb,bBuy)) return FALSE;

	if (bMarkAsTrader) 
		s_traders.AddTail(this); // mark unit as trader - other checks will be done later
	return TRUE;
}

//////////////////////////////

BOOL CUnit::IsCaptain() const 
{ 
	CHKUNIT
	BKPTUNIT

	return IsOnShip() && m_pShip->hasOwner(this);
}

int CUnit::finalSailingSkill()
{
	CHKUNIT
	BKPTUNIT

	return people()*max(0,level(SK_SAILING,TRUE));
}

BOOL CUnit::onCheckMove(BOOL bBefore)
{
	CHKUNIT
	BKPTUNIT

	if (bBefore) 
		return onCheckBeforeMove();
	return onCheckAfterMove();
}

BOOL CUnit::onCheckTransported()
{
	CHKUNIT
	BKPTUNIT

	if (!m_pRider) 
		return errormsg("onCheckTransported::internal error");

	if (m_pCarrier && (m_pRider != m_pCarrier))
		return _warning(RIDESWRONGUNIT,1,m_pCarrier->outputName(),m_pRider->outputName());

	if (HasMovedSomewhere())
	{
		int nErrNum = UNITALREADYHASMOVED;
		if (IsOnShip()) 
			nErrNum = UNITMUSTLEAVE;
		return _error(nErrNum);
	}

	if (!m_pCarrier)
	{
		if (m_pRider->isAlive())
			return _error(DOESNTCARRY,m_pRider->outputName());
	}
	
	OnLeave();

	return TRUE;
}

BOOL CUnit::onCheckCarrier()
{
	CHKUNIT
	BKPTUNIT

	if (!m_carried)	return errormsg("onCheckCarrier::internal error");
	if (!HasMovedByHimself())
		return	_error(CARRIERDOESNTMOVE);
	return TRUE;
}

BOOL CUnit::onPotentialAddedWeights()
{
	CHKUNIT
	BKPTUNIT
	// for the moment only movers are checked
	POSITION pos = m_potentialReceives.GetHeadPosition();
	while (pos) {
		CItemReceivedInfo * pi=m_potentialReceives.GetNext(pos);
		m_nPotentialAddedWeight+=ItemWeight(pi->itemID()*pi->nb());
	}
	
	return TRUE;
}

BOOL CUnit::onCheckPotentialReceiver()
{
	CHKUNIT
	BKPTUNIT
/*
	onPotentialAddedWeights();
	// for the moment only movers are checked
	if (changedRegion() || nbMoves()>0)
	{
			POSITION pos = m_potentialReceives.GetHeadPosition();
			while (pos) {
				CItemReceivedInfo * pi=m_potentialReceives.GetNext(pos);
				_warning(CANOVERLOAD,4,pi->nb(),pi->name(),pi->u()->outputName());
			}
	}
	*/
	return TRUE;
}

BOOL CUnit::isMovingToOcean(int & nNbMoves) const
{
	//check ocean cross by foot or by horse
	for (int j=1;j<=nNbMoves;j++)
	{
		CPoint p(m_moveInfos.moves[j]);
		CRegion *pRegion = sRegionsList.Add(p.x, p.y, 0);
		if (pRegion && pRegion->isOcean())
		{
			nNbMoves = j-1;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CUnit::moveUsingRoads(int nNbMoves)
{
	CHKUNIT
	BKPTUNIT

	CString s(outputName());
	eCoast nCoastS = COAST_NONE;
	eCoast nCoastT = COAST_NONE;

	CRegion *pRegionFrom = regionFrom();
	CRegion *pRegionS = pRegionFrom;
	CRegion *pRegionT = NULL;
	CPoint pPrev = CPoint(pRegionFrom->x(),pRegionFrom->y());
	CPoint p;
	for (int j=1;j<=nNbMoves;j++)
	{
		p=m_moveInfos.moves[j];
		pRegionT = sRegionsList.Add(p.x, p.y, 0);
		nCoastS = coastFrom(pPrev,p);
		nCoastT = coastNeighbour(nCoastS);
		if (!pRegionS) return FALSE;
		if (!pRegionT) return FALSE;
		// just built roads must be taken into account
		if (!pRegionS->cr_road(nCoastS)) return FALSE;
		if (!pRegionT->cr_road(nCoastT)) return FALSE;
		pPrev=p;
		pRegionS = pRegionT;
	}

	return TRUE;
}

BOOL CUnit::onCheckMoveOnLand()
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	//int j;
	int nNbMoves = m_moveInfos.m_nCount;
	if (m_moveInfos.m_bRouting) nNbMoves = m_moveInfos.m_nRoutingIndex;
	m_moveInfos.m_nCount = 0;
	m_moveInfos.m_nRoutingIndex = 0;


	if (!IsOnShip() && isAlive())
	{
/*		// TODO: nbmaxmove elven horse ?
		int nbeh = nbElvenHorses();
		int nbh = nbHorses();
		int nbc = nbCarts();
		//int nbUsableHorses = nbh;
		m_moveInfos.m_bUseRoads = moveUsingRoads(nNbMoves);
		int nmaxHorses = 0;
		//TODO: max elven horses according nbmoves & riding level
		int nmaxElvenHorses = nbeh;
			BOOL bErrorHorses = FALSE;

			for (int jj=1;jj<3 && nNbMoves>0;jj++)
			{
				nmaxHorses = nbMaxHorses(nNbMoves,m_moveInfos.m_bUseRoads);
				if (nbh > nmaxHorses)
				{
					//nbUsableHorses = nmaxHorses;
					m_nNbHorses = nmaxHorses;
					if (!bErrorHorses)
						_error(UNITTOOMANYHORSES);
					bErrorHorses = TRUE;
					nNbMoves--;
					continue;
				}
				// supposed maxcart includes elven horse
				int nbmaxCarts = (nmaxHorses+nmaxElvenHorses)/2;
				if (nbc > nbmaxCarts)
				{
					m_nNbCarts = nbmaxCarts;
					if (!bErrorHorses)
						_error(UNITTOOMANYCARTS);
					bErrorHorses = TRUE;
					nNbMoves--;
					continue;
				}
				break;
			}

			int nNbM = nNbMoves;
			if (m_moveInfos.m_bUseRoads) nNbM--;
			//if (nNbM >= 2 && nbUsableHorses==0)
			if (nNbM >= 2 && m_nNbHorses==0)
			{
				nNbMoves = 1;
				if (m_moveInfos.m_bUseRoads) nNbMoves++;
				_error(UNITMOVESTOOFAR," (no horse)");
			}
*/	}

	// with road:by horse max=3, by foot max=2
	if (nNbMoves>=4  && (!IsOnShip() || !IsCaptain()))
	{
		_error(UNITMOVESTOOFAR,"");
		nNbMoves = 3;
	}
	else
	if (nNbMoves>=2 && IsOnShip() && !IsCaptain())
	{
		_error(UNITMOVESTOOFAR,"");
		nNbMoves = 1;
	}

	if (m_moveInfos.m_bRouting) 
		m_moveInfos.m_nRoutingIndex = nNbMoves; 
	else 
		m_moveInfos.m_nCount = nNbMoves;
	return TRUE;
}

BOOL CUnit::onCheckBeforeMove()
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	int j;
	int nNbMoves = m_moveInfos.m_nCount;
	if (m_moveInfos.m_bRouting) nNbMoves = m_moveInfos.m_nRoutingIndex;
	m_moveInfos.m_nCount = 0;
	m_moveInfos.m_nRoutingIndex = 0;

	if (IsOnShip())
	{
		const t_ship * infos = m_pShip->infos();
		CString sShipType(infos?infos->name:"ship");
		if (IsCaptain())
		{
			if (isAlive() && infos)
			{
				if (people()==0)
					return _warning(NULLPERSONSSAIL,4);

				// check captain skill
				int nCaptainSailing = finalSailingSkill();
				if (nCaptainSailing < infos->captainSkill)
					return _error(UNITCAPTAINSAILING,(LPCSTR)sShipType,m_pShip->fullName(),nCaptainSailing,infos->captainSkill);

				// check sailor skill
				int n = m_pShip->totalSailingSkill();
				if (n < infos->totalSkill)
					return _error(SHIPSAILSKILL,(LPCSTR)sShipType, m_pShip->fullName(),n,infos->totalSkill);
			}

			// check coast from where going
			if (!m_pShip->canGo(m_moveInfos.moves[1]))
			{
				if (!regionFrom()->cr_hasAnHarbour())
					return _error(SHIPCANNOTGO,(LPCSTR)sShipType, m_pShip->fullName()); 
			}

			if (infos)
			{
				// check max moves
				int nbMaxMoves = infos->moverange;

				if (m_pShip->SpeedUpByMagicSummonStormElemental()) 
					nbMaxMoves*=2;

				if (isAquarian()) 
					nbMaxMoves++;

				if (m_pShip->SpeedUpPermanentlyByMagic()) 
					nbMaxMoves++;

				if (nNbMoves > nbMaxMoves)
				{
					nNbMoves = nbMaxMoves;

					// CG 21/02/08: ROUTE without PAUSE: means MOVE max, no error message
					if (m_moveInfos.m_bRouting && !m_moveInfos.m_bPauseInRoute) {
						m_moveInfos.m_nCount=nbMaxMoves;
						m_moveInfos.m_nRoutingIndex=nbMaxMoves;	
						SetXY(m_moveInfos.moves[nbMaxMoves].x,m_moveInfos.moves[nbMaxMoves].y);
					}
					else
						_error(SHIPMOVESTOOFAR,(LPCSTR)sShipType,m_pShip->fullName());
				}
			}

			//check entire direction (no land cross...)
			for (j=1;j<nNbMoves-1;j++)
			{
				CPoint p(m_moveInfos.moves[j]);
				CRegion *pRegion = sRegionsList.Add(p.x, p.y, 0);
				if (pRegion && !pRegion->isOcean())
				{
					_error(SHIPMEETLAND,(LPCSTR)sShipType,m_pShip->fullName());
					nNbMoves = j;
					break;
				}
			}

		}
		else
		{
			if (HasMovedByHimself())
			{
				if (regionFrom()->isOcean())
				{
					if (!isAquarian()) 
						return _error(UNITCANNOTSWIM);
				}
				
				if (isMovingToOcean(nNbMoves)) 
					return 	_error(UNITMOVESOCEAN);

				if (isStarving())
					return _error(UNITMOVESTARVING);

				OnLeave();
			}
			else
			// check if no captain on ship
			if (!m_pShip->hasOwner())
			{
				if (m_pShip->nbUnits() > 1)
					_warning(SHIPNOTCAPTAIN,2,m_pShip->fullName());
				m_pShip->SetOwner(this);
			}
		}
	} // IsOnShip


	if (!IsOnShip())
	{
		if (isAlive())
		{
			// TODO: nbmaxmove elven horse ?
			int nbeh = nbElvenHorses();
			int nbh = nbHorses();
			int nbc = nbCarts();
			//int nbUsableHorses = nbh;
			m_moveInfos.m_bUseRoads = moveUsingRoads(nNbMoves);
			int nmaxHorses = 0;
			//TODO: max elven horses according nbmoves & riding level
			int nmaxElvenHorses = nbeh;
			BOOL bErrorHorses = FALSE;

			for (int jj=1;jj<3 && nNbMoves>0;jj++)
			{
				nmaxHorses = nbMaxHorses(nNbMoves,m_moveInfos.m_bUseRoads);
				if (nbh > nmaxHorses)
				{
					//nbUsableHorses = nmaxHorses;
					if (nmaxHorses > 0)
						m_nNbHorses = nmaxHorses;
					if (!bErrorHorses)
					if (nmaxHorses==0) 
						_error(UNITMOVESTOOFAR,""); 
					else 
						_error(UNITTOOMANYHORSES); //MOVES TOO FAR if nbmoves too mush
					bErrorHorses = TRUE;
					nNbMoves--;
					continue;
				}
				// supposed maxcart includes elven horse
				int nbmaxCarts = (nmaxHorses+nmaxElvenHorses)/2;
				if (isTroll() && nNbMoves==1) nbmaxCarts+=people()/4; //Four trolls can pull a cart, but move 1 hex

				if (nbc > nbmaxCarts)
				{
					m_nNbCarts = nbmaxCarts;
					if (!bErrorHorses)
						_error(UNITTOOMANYCARTS);
					bErrorHorses = TRUE;
					nNbMoves--;
					continue;
				}
				break;
			}

			int nNbM = nNbMoves;
			if (m_moveInfos.m_bUseRoads) 
				nNbM--;
			if (nNbM >= 2 && m_nNbHorses==0)
			{
				nNbMoves = 2;
				m_moveInfos.m_bUseRoads = moveUsingRoads(nNbMoves); // update
				if (!m_moveInfos.m_bUseRoads) 
					nNbMoves = 1;
				if (!m_moveInfos.m_bRouting || m_moveInfos.m_bPauseInRoute)
					_error(UNITMOVESTOOFAR," (no horse)");
			}
		}

		if (isMovingToOcean(nNbMoves)) 
			_error(UNITMOVESOCEAN);
	}

	// with road:by horse max=3, by foot max=2
	if (nNbMoves>=4  && (!IsOnShip() || !IsCaptain()))
	{
		_error(UNITMOVESTOOFAR,"");
		nNbMoves = 3;
	}
	else
	if (nNbMoves>=2 && IsOnShip() && !IsCaptain())
	{
		_error(UNITMOVESTOOFAR,"");
		nNbMoves = 1;
	}

	if (m_moveInfos.m_bRouting) 
		m_moveInfos.m_nRoutingIndex = nNbMoves; 
	else 
		m_moveInfos.m_nCount = nNbMoves;
	return TRUE;
}

int CUnit::nbMoves() const
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	int nNbMoves = m_moveInfos.m_nCount;
	if (m_moveInfos.m_bRouting) nNbMoves = m_moveInfos.m_nRoutingIndex;
	return nNbMoves;
}

int CUnit::decreaseNbMoves()
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	int nNbMoves = 0;
	if (m_moveInfos.m_nCount > 0) nNbMoves = (--m_moveInfos.m_nCount);
	if (m_moveInfos.m_bRouting) nNbMoves = (--m_moveInfos.m_nRoutingIndex);
	return nNbMoves;
}

BOOL CUnit::onRegionChangeGuardsNumber(int nb)
{
	if (!m_pFaction && !IsTemp()) cr_unit();
	if (!m_pFaction) 
		return errormsg("onRegionChangeGuardsNumber::no faction for %s\n",outputName());
	if (!m_pRegionFrom) 
		return errormsg("onRegionChangeGuardsNumber::no regionfrom for %s\n",outputName());
	m_pRegionFrom->addGuards(m_pFaction->isAllied(),nb);
	return TRUE;
}

BOOL CUnit::isAllied() const
{
	CHKUNIT
	BKPTUNIT

	if (!m_pFaction) 
		return errormsg("isAllied::no faction for %s\n",outputName());
	return (m_pFaction->isAllied());
}

BOOL CUnit::onCheckAfterMove()
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	int nNbMoves = nbMoves();
	if (nNbMoves == 0) 
		return FALSE;

	if (IsOnShip())
	{
		// if not kaptain, OnMove called inside CShip::UpdateVesselsPosition
		if (IsCaptain())
		{
			const t_ship * infos = m_pShip->infos();
			CString sShipType(infos?infos->name:"ship");

			int nFinalSize;
			int nMaxSize;
			if (!m_pShip->fullyBuilt(nFinalSize,nMaxSize))
				return _error(SHIPNOTBUILT,(LPCSTR)sShipType, m_pShip->fullName(),nFinalSize,nMaxSize); 

			// check if damaged
			int nLevel = 12;
			int ndamage = m_pShip->finalDamagePrct();
			if (ndamage>0)
			{
				if (ndamage >= 10) nLevel -= 2;
				if (ndamage >= 25) nLevel -= 2;
				if (ndamage >= 50) nLevel -= 2;
				_warning(SHIPDAMAGED,nLevel,(LPCSTR)sShipType,m_pShip->fullName(),ndamage);
			}

			// check ship capacity
			float fAvailable_lbs=0;
			CString sResult("");
			if (m_pShip->overloaded(fAvailable_lbs,sResult))
			{
				//ERTRACE("sRes length=%d\n",sResult.GetLength());
				//ERTRACE("sRes =%s\n",sResult);
				_error(SHIPTOOHEAVILYLOADED,m_pShip->fullName(),-fAvailable_lbs,sResult);
				while (nNbMoves > 0)
					nNbMoves = decreaseNbMoves();
			}

			// check if glacier & insect
			if (m_pShip->insectInside())
			{
				CRegion * pRegionTo = sRegionsList.Add(m_nNewX, m_nNewY, 0);
				while (nNbMoves > 0 && pRegionTo->isGlacier())
				{
					_error(SHIPCANNOTENTERGLACIER,m_pShip->fullName());
					nNbMoves = decreaseNbMoves();
					SetXY(m_moveInfos.moves[nNbMoves].x,m_moveInfos.moves[nNbMoves].y);
					pRegionTo = sRegionsList.Add(m_moveInfos.moves[nNbMoves].x, m_moveInfos.moves[nNbMoves].y, 0);
				}
			}

			// set new boat position
			if (nNbMoves > 0 && OnMove())
			{
				m_pShip->SetRegionTo(toRegion(),m_moveInfos);
				if (cr().toSave())
					sMessagesList.Add(new CMessage(ICRMSG_SAIL, this, fromRegion(), NULL, toRegion(), NULL,m_pShip));
			}
		}
	}
	else
	{
		// compare weight to max weight
		CHKUNIT
		BKPTUNIT

		CString sResults("");
		int nMaxWeight = capacity(nNbMoves,m_moveInfos.m_bUseRoads);
		int nWeight = transported_weight(sResults,FALSE);

		if (nWeight > nMaxWeight)
		{
			int nErr = TOOHEAVILYLOADED;
			nNbMoves = decreaseNbMoves();
			if (nNbMoves > 0 && (nWeight > capacity(nNbMoves-1,m_moveInfos.m_bUseRoads)))
				nErr = MOVETOOHEAVILYLOADED;
			if (CUnit::s_bShow_lbs_details) {
				sResults="\n            ============   details   ============\n";
				weight(sResults,CUnit::s_bShow_lbs_details); //details on weight
				sResults+="            =====================================";
			}
			_error(nErr,displayedWeight(nWeight - nMaxWeight),sResults);
		}

		if (nNbMoves > 0 && OnMove())
		{
			int nMsgIndex = ICRMSG_WALK;
			CRegion * pCrossedRegion = NULL;
			if (nNbMoves > 1)
			{
				pCrossedRegion = sRegionsList.Add(m_moveInfos.moves[1].x, m_moveInfos.moves[1].y, 0);
				nMsgIndex = ICRMSG_RIDE;
			}
			if (cr().toSave())
				sMessagesList.Add(new CMessage(nMsgIndex, this, fromRegion(), NULL, toRegion(), pCrossedRegion));
			if (m_carried)
			{
				POSITION pos = m_carried->GetHeadPosition();
				while (pos)	m_carried->GetNext(pos)->onCarriedByHorse(this,m_moveInfos);
			}
		}
	}

	if (nNbMoves > 0) {
	// guard status changed only if really moved
		m_bGuarding = FALSE;
		if (m_bInitialGuard) 
			onRegionChangeGuardsNumber(-1);
		return TRUE;
	}

	return FALSE;
}

int CUnit::capacity(int nNbMoves, BOOL bOnRoad)
{
	if (!m_pFaction) 
		return errormsg("maxWeight::no faction for %s",outputName());
	const CRace * pRace = m_pFaction->race();
	if (!pRace) 
		return errormsg("maxWeight::no faction for %s",outputName());

	if (bOnRoad && nNbMoves>1) 
		nNbMoves--;
	int nbcarts=nbCarts();
	int nbhorses=nbHorses();
	// TO CHECK: can one man carry more than 1 trollbelt? supposed he cannot
	int nbtrollbelts=min(people(),nbTrollBelts());
	int ncc = nbcarts*CART_CAPACITY;
	int nch = (nbhorses-2*nbcarts)*HORSE_CAPACITY;
	int nr = ((nch+ncc)+(nNbMoves==2?-pRace->manWeight()*people():pRace->carryCapacity()*people()));
	// for trollbelt: 50 * race capacity
	if (nNbMoves==1)
		nr+=nbtrollbelts*49*pRace->carryCapacity();
	return nr;
}

int CUnit::transported_weight(CString & sDetails, BOOL bDetails)
{
	CHKUNIT
	BKPTUNIT

/*	if (m_nTransportedWeight!=-1)
		return m_nTransportedWeight;
	m_nTransportedWeight = 0;
*/
	if (people()==0) 
		return 0;
	
	CString sItemsDetails("");

	int lbsitems_received = 0;
	int lbsitems_having = 0;
	int lbsitems_money = 0;
	int lbs_carried_units = 0;
	int nb_magic_bags = nbMagicBags();

	// each magic bag can contain 200 lbs
	int lbsitems_in_magic_bags_max=nb_magic_bags*MAGIC_BAG_CAPACITY;
	if (nb_magic_bags>0 && bDetails){
	 //TODO
	} 
	// items for those an exchange has been done (give or receive)
	TItemInfo * pItem = m_items;
	while (pItem) 
	{
		if (pItem->m_sName.CompareNoCase(EN_HORSE) && pItem->m_sName.CompareNoCase(EN_CART)) {
			int lbsi = pItem->totalWeight();
			int lbsioutmb = lbsi;
			if (lbsi > 0) {
				lbsioutmb=itemWeightOutsideMagicBag(lbsi,pItem->m_sName,lbsitems_in_magic_bags_max);
				if (bDetails)  {
					CString sTmp("");
					if (lbsioutmb!=lbsi)
						sTmp.Format("%40s%d %s(s): %.2f lbs (%.2f lbs inside magic bag)\n"," ",pItem->number(),pItem->m_sName,displayedWeight(lbsioutmb),displayedWeight(lbsi-lbsioutmb));
					else
						sTmp.Format("%40s%d %s(s): %.2f lbs\n"," ",pItem->number(),pItem->m_sName,displayedWeight(lbsi));
					sItemsDetails+=sTmp;
				}
			}
			lbsitems_received+=lbsioutmb;
		}
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
				BOOL bSilver = !sItemName.CompareNoCase(EN_SILVER);
				if (sItemName.CompareNoCase(EN_HORSE) && sItemName.CompareNoCase(EN_CART) &&!bSilver)
				{
					if (itemKind(sItemName, i, nType))
					{
						if (nType!=UT_HERB && nType!=UT_POTION && !itemInfo(i, nType))
						{
							int n=0;
							if (cr().geti(bi, e->tag->name, n)) {
								int lbsi=ItemWeight(i)*n;
								int lbsioutmb=lbsi;
								if (lbsi>0) {
									lbsioutmb=itemWeightOutsideMagicBag(lbsi,sItemName,lbsitems_in_magic_bags_max);
									if (bDetails)  {
										CString sTmp("");
										if (lbsioutmb!=lbsi)
											sTmp.Format("%40s%d %s(s): %.2f lbs (%.2f lbs inside magic bag)\n"," ",n,sItemName,displayedWeight(lbsioutmb),displayedWeight(lbsi-lbsioutmb));
										else
											sTmp.Format("%40s%d %s(s): %.2f lbs\n"," ",n,sItemName,displayedWeight(lbsi));
										sItemsDetails+=sTmp;
									}

								}
								lbsitems_having+=lbsioutmb;

							}
						}
					}
				}
				e=e->next;
			}
		}
	}

//	CHKUNIT
//	output("lbs for %s:%f\n",outputName(),(float)(lbsitems1+lbsitems2)/100.0);

	// add silvers weight 
	lbsitems_money = money();
	int lbsitems_money_outmb=lbsitems_money;
	if (lbsitems_money>0) {
		lbsitems_money_outmb=itemWeightOutsideMagicBag(lbsitems_money, EN_SILVER, lbsitems_in_magic_bags_max);
		if (bDetails)  {
			CString sTmp("");
			if (lbsitems_money_outmb!=lbsitems_money)
				sTmp.Format("%40s%d %s(s): %.2f lbs (%.2f lbs inside magic bag)\n"," ",lbsitems_money_outmb,EN_SILVER,displayedWeight(lbsitems_money_outmb),displayedWeight(lbsitems_money-lbsitems_money_outmb));
			else
				sTmp.Format("%40s%d %s(s): %.2f lbs\n"," ",lbsitems_money,EN_SILVER,displayedWeight(lbsitems_money));
			sItemsDetails+=sTmp;
		}
	}
	sDetails+=sItemsDetails;

	// add carried units weight 
	if (m_carried)
	{
		if (bDetails){
			CString stmp("");	
			stmp.Format("%32scarried units:\n"," ");
			sDetails+=stmp;
		}
		POSITION pos = m_carried->GetHeadPosition();
		while (pos)	{
			CString sResults("");
			lbs_carried_units+=m_carried->GetNext(pos)->weight(sResults,bDetails);
			sDetails+=sResults;
		}
	}
	return (lbsitems_received+lbsitems_having)+lbsitems_money_outmb+lbs_carried_units;
}

// X100: transported weight & men, horse & cart weights
int CUnit::weight(CString & sResults, BOOL bDetails)
{
	CHKUNIT
	BKPTUNIT

	if (people()==0) return 0;

	CString sDetailsTransported("");
	CString sDetails("");

	int nweight_items = transported_weight(sDetailsTransported,bDetails);

	// weight of elven horse supposed to be same as horse
	int nwh = nbHorses()*HORSE_WEIGHT;
	int nweh = nbElvenHorses()*HORSE_WEIGHT;
	int nwk = nbCarts()*CART_WEIGHT;

	int nwm = MAN_WEIGHT;
	int nwmm = 0;
	CString sRace("men");
	if (m_pFaction) 
	{
		const CRace * pRace = m_pFaction->race();
		if (pRace) {
			nwmm = pRace->manWeight();
			sRace = pRace->name();
		}
	}
	if (nwmm!=0) 
		nwm = nwmm; 

	nwm *= people();
	int nwmunit=nwh+nwk+nwm;
	if (bDetails) {
		CString sTmp("");
		sTmp.Format("%30s: %d %s (%.2f lbs) - total weight (%.2f lbs)\n",outputName(),people(),(LPCSTR)sRace,displayedWeight(nwm),displayedWeight(nwmunit+nweight_items));
		sDetails+=sTmp;
		if (nwh>0) { sTmp.Format("%35s%d horse(s): %.2f lbs\n"," ",nbHorses(),displayedWeight(nwh));sDetails+=sTmp;}
		if (nweh>0) { sTmp.Format("%35s%d elven horse(s): %.2f lbs\n"," ",nbElvenHorses(),displayedWeight(nweh));sDetails+=sTmp;}
		if (nwk>0) { sTmp.Format("%35s%d cart(s): %.2f lbs\n"," ",nbCarts(),displayedWeight(nwk));sDetails+=sTmp;}
		sResults+=sDetails;
		sResults+=sDetailsTransported;
	}
	return nwmunit+nweight_items;
}

BOOL CUnit::onCheckGuard()
{
	// if (m_bInitialGuard && !moving) ; // TODO: warning
	// if (moving && attacking) ; // TODO: warning
	// if (moving && !attacking) ; // TODO: error

	CHKUNIT
	BKPTUNIT

	if (HasMoved())
	//if (nbMoves() > 0)
		return FALSE; // no warning

	CRegion *r = regionFrom();

	if (r->isOcean())
		return FALSE;

	if (level(SK_STEALTH)>0) 
		_warning(UNITGUARDSTEALTH,(r->war()?8:7));

	// check weapons & weaponskill level
	BOOL bGuarding = FALSE;
	int nbw=0;
	int nbw2=0;
	if (cr().Loaded())
	{
		if (!bGuarding && (level(SK_MELEE,TRUE)>0)) { 
			nbw2=nbWeapons(WEAPONTYPE_MELEE); nbw+=nbw2;
			if (nbw2 > 0) bGuarding = TRUE;
		}
		if (!bGuarding && (level(SK_POLEARM,TRUE)>0)) { 
			nbw2=nbWeapons(WEAPONTYPE_POLEARM); nbw+=nbw2;
			if (nbw2 > 0) bGuarding = TRUE;
		}
		if (!bGuarding && (level(SK_CROSSBOW,TRUE)>0)) { 
			nbw2=nbWeapons(WEAPONTYPE_CROSSBOW); nbw+=nbw2;
			if (nbw2 > 0) bGuarding = TRUE;
		}
		if (!bGuarding && (level(SK_BOW,TRUE)>0)) { 
			nbw2=nbWeapons(WEAPONTYPE_BOW); nbw+=nbw2;
			if (nbw2 > 0) bGuarding = TRUE;
		}
		if (!bGuarding)
			return _warning(nbw>0?UNITCANNOTGUARDNOWEAPON:UNITCANNOTGUARDNOWEAPONEDSKILL,2);

		if (cr_combatStatus()==COMBATSTATUS_FLEE)
			return _warning(UNITCANNOTGUARDFLEE,2);

		m_bGuarding = TRUE;
		onRegionChangeGuardsNumber(1);
	}

	return TRUE;
}

BOOL CUnit::checkPeacefulMen(const CRegion *pRegion)
{
	// peaceful men must not be in aggressive mode
	if (isAlive() && !m_bAttacking && nbWeapons(0,TRUE)==0){
		int bb= cr_combatStatus();
		if ((bb==COMBATSTATUS_FRONT || bb==COMBATSTATUS_AGGRESSIVE) && m_pRegionFrom==pRegion)
			return _warning(UNITBADCOMBATSTATUS,-6); // "Ents";Typ todo	
	}
	return TRUE;
}

BOOL CUnit::feedDemon()
{
	CHKUNIT
	BKPTUNIT

	if (!isDemon())
		return FALSE;
	 
	int nb = addEffect(EFT_PEASANT_BLOOD,-people(),FALSE);
	if (nb!=0)
		regionTo()->AddNbDemonFeed(nb,m_pFaction);
	
	return TRUE;
}

BOOL CUnit::onCheckTax()
{
	CHKUNIT
	BKPTUNIT

	// if no CR, TAX supposed to work with min taxation level & all men supposed to be adequatly weaponed
	int nTaxLevel = 1;
	int nbPeople = people();

	if (cr().Loaded())
	{
		nTaxLevel = level(SK_TAX);
		if (nTaxLevel == LEVEL_UNKNOWN) return _warning(UNITUNKNOWNTAXSKILL,4);
		if (nTaxLevel == 0)	return _error(UNITNOTAXSKILL);

		// cannot tax if region guarded by an unallied
		CRegion * r = regionFrom();
		if (!r) return errormsg("onCheckTax::region not found for %s",outputName());
		BOOL bWar = FALSE;
		if (r->IsGuardedByOther(&bWar))
			return _warning(UNITNOTAXGUARDED,(bWar?WARNLEVEL_UNALLIED_MAYNOT_WAR:WARNLEVEL_UNALLIED_MAYNOT));

		// check weapons & weaponskill level
		int n = 0;
		int nMeleeLevel = max(0,level(SK_MELEE));
		int nPolearmLevel = max(0,level(SK_POLEARM));
		int nCrossbowLevel = max(0,level(SK_CROSSBOW));
		int nBowLevel = max(0,level(SK_BOW));
		if (nMeleeLevel+nPolearmLevel+nCrossbowLevel+nBowLevel == 0) 
			return _error(UNITNOWEAPONEDTAXSKILL);
		if (nMeleeLevel>0) n+=nbWeapons(WEAPONTYPE_MELEE);
		if (n < nbPeople && nPolearmLevel>0) n+=nbWeapons(WEAPONTYPE_POLEARM); 
		if (n < nbPeople && nCrossbowLevel>0) n+=nbWeapons(WEAPONTYPE_CROSSBOW); 
		if (n < nbPeople && nBowLevel>0) n+=nbWeapons(WEAPONTYPE_BOW); 
		if (n==0) 
			return _error(UNITNOTAXNOWEAPON);
		if (n < people()) 
			_warning(UNITNOTAXNOTENOUGHWEAPON,4);
		nbPeople = min(n,nbPeople);
	}

	m_money.m_nByTax=positiveMin(nTaxLevel*EARNMONEY_AMOUNT_TAX*nbPeople,m_nTaxAmount);
	addMoney(m_money.m_nByTax,TRUE);

	//TODO: specify correct wanted value
	if (cr().toSave())
		sMessagesList.Add(new CMessage(ICRMSG_EARN_TAX, this, fromRegion(), NULL, NULL, NULL, NULL,"",m_money.m_nByTax,m_money.m_nByTax));

	return TRUE;
}

BOOL CUnit::onCheckEntertain()
{
	CHKUNIT
	BKPTUNIT
	
	// if no CR, ENTERTAINMENT supposed to work with min entertainment level
	int nEntertainmentLevel = level(SK_ENTERTAIN);
	if (nEntertainmentLevel == LEVEL_UNKNOWN) return _warning(UNITUNKNOWNENTERTAINSKILL,4);
	if (nEntertainmentLevel == 0) return _error(UNITNOENTERTAINSKILL);
	CRegion * r = regionFrom();
	if (!r) return errormsg("onCheckEntertain::region not found for %s",outputName());

	// may not entertain if region guarded by an unallied & on ship
	BOOL bWar = FALSE;
	if (IsOnShip() && r->IsGuardedByOther(&bWar))
		return _warning(UNITNOENTERTAINONSHIP,(bWar?WARNLEVEL_UNALLIED_MAYNOT_WAR:WARNLEVEL_UNALLIED_MAYNOT));

	m_money.m_nByEntertain=positiveMin(nEntertainmentLevel*EARNMONEY_AMOUNT_ENTERTAINMENT*people(),m_nEntertainmentAmount);
	addMoney(m_money.m_nByEntertain,TRUE);

	//TODO: specify correct wanted value
	if (cr().toSave())
		sMessagesList.Add(new CMessage(ICRMSG_EARN_ENTERTAIN, this, fromRegion(), NULL, NULL, NULL, NULL,"",m_money.m_nByEntertain,m_money.m_nByEntertain));
	return TRUE;
}

BOOL CUnit::onCheckBuys(CRegion * r)
{
	CHKUNIT
	BKPTUNIT

	if (!r) return FALSE;
	BOOL bOK = TRUE;
	TTradeItem * pBuy = m_buys;
	int nbAvailableTrades = m_nNbMaxTradeItems;
	// TODO: check the following scenario:
	// BUY 10 gem
	// BUY 20 gem
	while (pBuy)
	{
		if (!r->canBuyLuxury(pBuy->m_nItemID)) 
			bOK = _error(UNITITEMNOTBUYABLE,(LPCSTR)ItemName(pBuy->m_nItemID));
		else
		{	
			if (pBuy->m_nNb > nbAvailableTrades)
			{
				_error(UNITBUYNOTENOUGHTRADE,m_nNbMaxTradeItems,level(SK_TRADE));
				pBuy->m_nNb = nbAvailableTrades;
				bOK = FALSE;
			}
			if (buy(pBuy,r)) nbAvailableTrades-=pBuy->m_nNb; else bOK = FALSE;
		}
		pBuy = pBuy->m_pNext;
	}
	m_nNbBoughtLuxuries = m_nNbMaxTradeItems-nbAvailableTrades;
	// TODO: if not enough money (or no money) to buy, error
	if (m_money.m_nByBuy > 0)
	{
	}
	return bOK;
}

BOOL CUnit::onCheckSells(CRegion * r)
{
	CHKUNIT
	BKPTUNIT

	if (!r) return FALSE;
	int nbTotal = 0;
	BOOL bOK = TRUE;
	TTradeItem * pSell = m_sells;
	// TODO: check the following scenario:
	// SELL 10 gem
	// SELL 20 gem
	int nbAvailableTrades = m_nNbMaxTradeItems - m_nNbBoughtLuxuries;
	// message wanted for each SELL order pbm, so do not stop after the first warning
	while (pSell)
	{
		if (!sell(pSell,r,nbAvailableTrades)) 
			bOK = FALSE;
		pSell = pSell->m_pNext;
	}

	if (m_money.m_nBySell > 0)
	{
		// TODO: transfer money to region owner:
		// Foundation 1 - 0% 
		// Tradepost 2 - 6% 
		// Fortification 10 - 12% 
		// Tower 50 - 18% 
		// Stronghold 250 - 24% 
		// Fortress 1250 - 30% 
		// Citadel 6250 - 36% 
		// if (!oldowner()) { nx = x % de m_money.m_nBySell, m_money.m_nBySell-= nx; owner money +=m_money.m_nBySell) owner earns nx}   
		int pr=m_money.m_nBySell;
		addMoney(pr,TRUE);
		if (cr().toSave())
			sMessagesList.Add(new CMessage(ICRMSG_EARN_SELL, this, fromRegion(), NULL, NULL, NULL, NULL,"",m_money.m_nBySell,m_money.m_nBySell));
	}

	if (nbAvailableTrades>0)
	{
		CString sBuyableItem=r->buyableLuxuryName();
		int nWarningLevelBis=(nbAvailableTrades>50?10:15);
		_warning(UNITCANBUYMORE,nWarningLevelBis,nbAvailableTrades,sBuyableItem);
	}
	return bOK;
}

BOOL CUnit::onCheckTrade()
{
	CHKUNIT
	BKPTUNIT

	if (!hasTradeItems()) 
		return TRUE;

	int nTradeLevel = level(SK_TRADE);
	if (nTradeLevel == LEVEL_UNKNOWN) return _error(UNITMAYNOTTRADE);
	if (nTradeLevel == 0) return _error(UNITWILLNOTTRADE);
	m_nNbMaxTradeItems = TRADE_NBITEMS_BY_LEVEL*nTradeLevel*people();

	CRegion * r = regionFrom();
	if (!r) return	errormsg("onCheckTrade::region not found for %s",outputName());

	// insects can trade in deserts and swamps without needing any form of building.
	if (!(isInsect() && (r->isTerrain(EN_SWAMP) || r->isTerrain(EN_DESERT))))
	{
		// trade not possible if castlesize<2
		if (r->cr_castleSize() < CASTLESIZE_TRADEPOST) 
			return _error(NOTRADENOCASTLE);
	}

	// may not trade if region guarded by an unallied & on ship
	BOOL bWar = FALSE;
	if (IsOnShip() && r->IsGuardedByOther(&bWar))
		return _warning(UNITNOTRADEONSHIP,(bWar?WARNLEVEL_UNALLIED_MAYNOT_WAR:WARNLEVEL_UNALLIED_MAYNOT));

	onCheckBuys(r);
	onCheckSells(r);

	return TRUE;
}

BOOL CUnit::onCheckWork()
{
	// if no CR, WORK supposed to work with min wage value
	int w = WAGE_MIN_VALUE;

	if (cr().Loaded())
	{
		CRegion * r = regionFrom();
		if (!r) 
			return errormsg("onCheckWork::region not found for %s",outputName());
		BOOL bWar = FALSE;
		if (r->isOcean()) {
			if (!isAquarian())
				return FALSE;
		}
		else
		{
			if (IsOnShip())
			{
				if (r->IsGuardedByOther(&bWar))
					return _warning(UNITNOWORKONSHIP,(bWar?WARNLEVEL_UNALLIED_MAYNOT_WAR:WARNLEVEL_UNALLIED_MAYNOT));
			}
			w = r->cr_wageForRace(isOrc());
			// TODO: Up to 100 aquarians per region can issue earn 10 silver a turn with WORK.
		}
	}
	
	if (w<=0)
		return FALSE;

	m_money.m_nByWork = w*people();
	addMoney(m_money.m_nByWork,TRUE);
	if (cr().toSave())
		sMessagesList.Add(new CMessage(ICRMSG_EARN_WORK, this, fromRegion(), NULL, NULL, NULL, NULL,"",m_money.m_nByWork,m_money.m_nByWork));

	return TRUE;
}

BOOL CUnit::onCheckAttack()
{
	CHKUNIT
	BKPTUNIT

	int nCombatStatus = cr_combatStatus();
	if (nCombatStatus > COMBATSTATUS_DEFENSIVE) 
		return _warning(UNITATTACKBADSTATUS,1);
	if (isStarving())
		return _error(UNITATTACKSTARVING);
	int nbw = nbWeapons(0,TRUE);
	if (nbw < people()) 
		return _warning(UNITATTACKNOTENOUGHWEAPON,4,people()-nbw);
	return TRUE;
}

BOOL CUnit::ChangedOfContainer() const
{
	CHKUNIT
	BKPTUNIT

	BOOL bWasInContainer = (m_pInitialShip!=NULL || m_pInitialBuilding!=NULL);
	BOOL bIsInNewContainer = ((m_pShip!=NULL && m_pShip!=m_pInitialShip) || (m_pBuilding!=NULL && m_pBuilding!=m_pInitialBuilding));
	return (bWasInContainer && bIsInNewContainer);
}

void CUnit::onCheckGiveControl()
{
	CHKUNIT
	BKPTUNIT
	
	CHKUNIT2
	BKPTUNIT

	BOOL bWasInContainer = (m_pInitialShip!=NULL || m_pInitialBuilding!=NULL);
	BOOL bIsInNewContainer = ((m_pShip!=NULL && m_pShip!=m_pInitialShip) || (m_pBuilding!=NULL && m_pBuilding!=m_pInitialBuilding));
	BOOL bChangedOfContainer = (bWasInContainer && bIsInNewContainer);

	// sequence order: ENTER, GIVE, MAKE
	if (m_pInitialShip) onCheckGiveShipControl(m_pInitialShip,bChangedOfContainer);
	if (m_pShip) onCheckGiveShipControl(m_pShip,bChangedOfContainer);
	if (m_pInitialBuilding) onCheckGiveBuildingControl(m_pInitialBuilding,bChangedOfContainer); 
	if (m_pBuilding) onCheckGiveBuildingControl(m_pBuilding,bChangedOfContainer);
}

BOOL CUnit::onCheckRecruited()
{
	// cannot recruit if region guarded by an unallied, except if stealth skill
	CRegion * r = regionFrom();
	if (!r) return errormsg("onCheckRecruited::region not found for %s",outputName());
	BOOL bWar = FALSE;
	if (r->IsGuardedByOther(&bWar))
	{

		int nStealthLevel = 0;
		CUnit * pParent = parent();
		if (pParent) nStealthLevel = pParent->level(SK_STEALTH);
		return _warning(UNITMAYNOTRECRUIT,(bWar?WARNLEVEL_UNALLIED_MAYNOT_WAR:(nStealthLevel>0?WARNLEVEL_UNALLIED_MAYNOT+2:WARNLEVEL_UNALLIED_MAYNOT)));
	}
	return TRUE;
}

BOOL CUnit::somewhere(eID nUnitID, CRegion *& pRegion)
{
	pRegion = NULL;
	if (!cr().Loaded()) return FALSE;
	return sRegionsList.FindUnit(nUnitID, pRegion);
}

BOOL CUnit::existingUnit(CUnit *& pUnit, eID nUnitID, CRegion * pRegion)
{
	if (pUnit) return TRUE;
	pUnit = NULL;
	if (!cr().Loaded()) return FALSE;
	block * r = NULL;
	if (pRegion) r = pRegion->cr_region();
	block * crunit = cr().units(r,&nUnitID);
	if (crunit)	
	{
		pUnit = new CUnit(crunit,nUnitID,pRegion);
		sUnitsList.Add(pUnit);
		return TRUE;
	}
	return FALSE;
}

BOOL CUnit::onCheckTeachers()
{
	CHKUNIT
	BKPTUNIT

	BOOL bOK = FALSE;
	TTeachInfo * pTeach = m_teachs;
	CUnit * student = NULL;
	CRegion * r = regionFrom();
	if (!r) return errormsg("CUnit::onCheckTeachers error - no region for %s",outputName());
	if (people()==0) return _warning(NULLPERSONSTEACH,4);
	m_nNbMenToTeach = people()*10;

	while (pTeach)
	{
		int nStudentSkillLevel = LEVEL_UNKNOWN;
		int nTeacherSkillLevel = LEVEL_UNKNOWN;
		int nLearnerID =  pTeach->m_nUnitID;
		CString sLearnerID(itob(nLearnerID));
		student = s_learners.Find(nLearnerID, pTeach->m_bIsTemp,r);
		if (!student) student = sUnitsList.Find(nLearnerID, pTeach->m_bIsTemp); //so it's not a learner
		if (!existingUnit(student,nLearnerID,r)) 
			_warning(5,UNITLEARNERNOTFOUND,(LPCSTR)sLearnerID);
		else
		{
			if (!student->isFromRegion(r))
				_warning(5,UNITLEARNERNOTONREGION,student->outputName());
			else
			{
				int nbTaughtMen = 0;
				if (student->addTeacher(this,nbTaughtMen,nStudentSkillLevel,nTeacherSkillLevel))
					m_nNbMenToTeach -= nbTaughtMen;
			}
		}
		pTeach = pTeach->m_pNext;
	}

	if (m_nNbMenToTeach > 0)
	{
		int nWarnLvl = 9;
		if (m_nNbMenToTeach>=100) nWarnLvl = 7;
		if (m_nNbMenToTeach>=10) nWarnLvl = 8;
		_warning(UNITCANSTILLTEACH,nWarnLvl,m_nNbMenToTeach);
	}

	return TRUE;
}

int CUnit::learntSkillLevel(BOOL bAfterLearn)
{
	CHKUNIT
	BKPTUNIT

	int ni=ILRN_SKILL_BEFORE;
	if (bAfterLearn) ni=ILRN_SKILL_AFTER;

	if (m_nLearntSkillLevels[ni]!=VALUE_UNKNOWN) return m_nLearntSkillLevels[ni];
	int nlvl=VALUE_UNKNOWN;
	if (m_pLearntSkill) 
	{
		nlvl = max(level(CCR::English2German(m_pLearntSkill->name,TRUE), nlvl),0);
		if (ni==ILRN_SKILL_AFTER && nlvl<=0)
			nlvl=defaultSkillLevel(m_eLearntSkill);
	}
	m_nLearntSkillLevels[ni]=nlvl;
	return m_nLearntSkillLevels[ni];
}

BOOL CUnit::onCheckLearners()
{
	CHKUNIT
	BKPTUNIT
	
	if (!m_pLearntSkill) 
		return FALSE; //TODO error

	if (m_nNbMenToBeTaught>0 && m_nNbTeachers>0 && m_pLastTeacher)
		m_pLastTeacher->_warning(UNITTEACHTOOMUCH,4);

	int nCost = m_pLearntSkill->kosten;

	if (nCost < 0)
	{
		// variable cost - only MAGIC for now
		// eSkill ns = skillnameToIndex(CCR::English2German(m_pLearntSkill->name),TRUE);
		//if (ns==SK_MAGIC)
		// correct 'learn magic' cost according to current skill level & race modifiers
		nCost = magicCost(learntSkillLevel(FALSE),defaultSkillLevel(SK_MAGIC));
	}

	if (fromRegion()->cr_hasAcademy())
	{
		BOOL bWorkingAcademy = FALSE;
		checkInsideWhat();
		//TODO: correct bug: bWorkingAcademy can be FALSE if owner leaves
		if (isInBuildingType(EN_ACADEMY,&bWorkingAcademy))
		{ 
			m_bInAcademy = TRUE;
			if (bWorkingAcademy)
				if (nCost==0) 
					nCost=50; 
				else 
					nCost*=2;
		}
	}

	if (m_nSpecifiedLearntCost!=0 && nCost!=m_nSpecifiedLearntCost) 
	{
		int rrr=1;
	} //warning

	int nNbPersons = people();
	int x = nCost * nNbPersons;
	if (x > 0) 
	{
		setLearningCost(x);
		if (cr().toSave())
			sMessagesList.Add(new CMessage(ICRMSG_STUDY, this, fromRegion(), NULL, NULL, NULL, NULL, CCR::German2English(m_pLearntSkill->name),x));
	}

	int nWarningLevel = 8;
	if (m_nNbTeachers == 0 && nNbPersons >= 10)
	{
		nWarningLevel = 9;
		m_nNbMenToBeTaught=nNbPersons;
	}
	if (m_nNbMenToBeTaught > 0)
		_warning(UNITNEEDSTEACHERS,nWarningLevel,(m_nNbMenToBeTaught+9)/10,m_nNbMenToBeTaught);

	int nbEffetsBrainwax=min(nNbPersons,effect(EFT_BRAINWAX));
	if (nbEffetsBrainwax > 0)
		addEffect(EFT_BRAINWAX,-nbEffetsBrainwax,TRUE);

	return TRUE;
}

BOOL CUnit::onCheckHeroes()
{
	if (!m_pFaction) 
		return errormsg("onCheckHeroes:internal error - no faction");
	//TODO:check nbmaxheroes according to the nbmen number
	int nCost = m_pFaction->nbTotalMen()*people();
	m_pFaction->AddNewHeroes(people());
	if (!spendMoney(nCost,TRUE))
	{
		_error(MONEYPROMOTION,nCost);
		m_bPromotion = FALSE;
		return FALSE;
	}

	if (cr().toSave())
		sMessagesList.Add(new CMessage(ICRMSG_PROMOTE, this, NULL, NULL, NULL, NULL, NULL, NULL, nCost));

	return TRUE;
}

BOOL CUnit::onCheckMakers()
{
	CHKUNIT
	BKPTUNIT

	if (m_makeOrder.m_nUTType==UT_BUILDING) 
		return onCheckMakeBuilding();
	if (m_makeOrder.m_nUTType==UT_SHIP) 
		return onCheckMakeShip();
	if (m_makeOrder.m_nUTType==UT_HERB) 
		return onCheckMakeHerb();
	if (m_makeOrder.m_nUTType==UT_POTION) 
		return onCheckMakePotion();
	if (m_makeOrder.m_nUTType==UT_ITEM) 
		return onCheckMakeItem();
	if (m_makeOrder.m_nUTType==UT_NONE && m_makeOrder.m_nParam==P_ROAD) 
		return onCheckMakeRoad();
	return errormsg("CUnit::onCheckMakers - unknown made item type %d for %s",m_makeOrder.m_nUTType,outputName());
}

BOOL CUnit::onRemindMakers()
{
	CHKUNIT
	BKPTUNIT

	CString sBuildingType("");
	if (m_makeOrder.m_nUTType==UT_ITEM) {
		int nIndex=ID_UNKNOWN;
		if (isWeapon(m_makeOrder.m_nUTID,&nIndex) || isArmour(m_makeOrder.m_nUTID,&nIndex)) 
			sBuildingType=EN_SMITHY; 
		else
		if (isItem(m_makeOrder.m_nUTID,&nIndex)) {
			if (nIndex==IT_WOOD) sBuildingType=EN_SAWMILL; else
			if (nIndex==IT_MALLORN) sBuildingType=EN_SAWMILL; else
			if (nIndex==IT_STONE) sBuildingType=EN_QUARRY; else
			if (nIndex==IT_IRON) sBuildingType=EN_MINE; else
			if (nIndex==IT_LAEN) sBuildingType=EN_MINE; else
			if (nIndex == IT_CATAPULT_AMMUNITION) sBuildingType=EN_QUARRY;
		}
		if (!sBuildingType.IsEmpty() && isInBuildingType(sBuildingType))
			m_pBuilding->setUsed();

		return TRUE;
	}
	return FALSE;
}

BOOL CUnit::onCheckMakeBuilding()
{
	CHKUNIT
	BKPTUNIT

	int nExtendedSize = 0;
	int nbPersons = people();
	
	CBuilding * pBuilding = m_pMadeBuilding;
	if (!pBuilding) 
		return _error(UNKNOWNBUILDING,"building",itob(m_makeOrder.m_nID));
	const t_building * b = pBuilding->infos();
	CString bname(b?b->name:"building");
	t_cost usedResources;
	t_cost bcost;
	bcost.set(0);
	if (b) b->cost.copyTo(bcost);
	CString sLackItems("");
	BOOL bIsCastle = pBuilding->isCastle();
	if (!canBuild(pBuilding->currentSize(), bcost, usedResources, nExtendedSize, sLackItems, bIsCastle))
	{
		if (!sLackItems.IsEmpty())
		{
			sLackItems.MakeLower();
			_error(BUILDLACK,pBuilding->fullName(),(LPCSTR)sLackItems);
		}
		return FALSE;
	}
	if (b && b->masonryMinSkill > 0) // not for castle
	{
		int nNeededSkill = b->masonryMinSkill * nExtendedSize;
		int nSkillLevel = level(SK_MASONRY);
		int nTotalSkill = nSkillLevel * nbPersons;
		if (nTotalSkill < nNeededSkill) {
			int nRestrictedExtendedSize=nTotalSkill/b->masonryMinSkill;;
			_warning(BUILDSKILL,6,nNeededSkill-nTotalSkill,(LPCSTR)CCR::German2English(CR_MASONRY),b->name,nExtendedSize-nRestrictedExtendedSize);
			nExtendedSize = nRestrictedExtendedSize;
		}
		else
		{
			int nbUnusedMen = (nTotalSkill-nNeededSkill)/nSkillLevel;
			if (nbUnusedMen > 0) 
				_warning(CANUSEBUILDERFOROTHER,6,nbUnusedMen,pBuilding->infos()->name);
		}
	}
	if (m_makeOrder.m_nSize > 0)
	{
		if (nExtendedSize > m_makeOrder.m_nSize) 
			_warning(CANBUILDMORE,4,(LPCSTR)bname,nExtendedSize, m_makeOrder.m_nSize,"resources"); else
		if (nExtendedSize < m_makeOrder.m_nSize) 
			_warning(CANNOTBUILDSOMUCH,4,(LPCSTR)bname, nExtendedSize, m_makeOrder.m_nSize);
		nExtendedSize = min(nExtendedSize,m_makeOrder.m_nSize);
	}	
	build(bcost,nExtendedSize,bIsCastle);
	if (m_pMadeBuilding)
	{
		m_pMadeBuilding->addExtendedSize(nExtendedSize);
		if (cr().toSave())
			sMessagesList.Add(new CMessage(ICRMSG_BUILD_BUILDING, this, NULL, NULL, NULL, NULL, m_pMadeBuilding, "", nExtendedSize, nExtendedSize));
	}
	return TRUE;
}

BOOL CUnit::onCheckMakeRoad()
{
	CHKUNIT
	BKPTUNIT

	int nExtendedSize = 0;
	int nbPersons = people();
	
	// Swamps require a dam, 

// Swamp* 75
// Highland 100
// Desert+ 100
// Mountain 250
// Glacier# 250
// Plain 50
// Forest 50
/*
GRENZE 1
"Straße";typ
2;richtung
20;prozent

GRENZE 1
"Straße";typ
5;richtung
100;prozent
 *	
 */

	CRoad * pRoad = m_pMadeRoad;
	if (!pRoad) return _error(UNKNOWNBUILDING,"road",0);
	CString bname("road");
	t_cost usedResources;
	t_cost bcost;
	bcost.set(0);
	CString sLackItems("");

	int nMaxExtendedSize = pRoad->maxSize()-pRoad->currentSize();
	if (nMaxExtendedSize == 0) return _warning(2,ROADCOMPLETED);

	if (!canBuild(pRoad->currentSize(), bcost, usedResources, nExtendedSize, sLackItems, FALSE, TRUE))
	{
		if (!sLackItems.IsEmpty())
		{
			sLackItems.MakeLower();
			_error(BUILDLACK,"road",(LPCSTR)sLackItems);
		}
		return FALSE;
	}

	nExtendedSize = min(nExtendedSize,nMaxExtendedSize);

	if (m_makeOrder.m_nSize > 0)
	{
		if (nExtendedSize > m_makeOrder.m_nSize) 
			_warning(CANBUILDMORE,4,EN_ROAD,nExtendedSize, m_makeOrder.m_nSize,EN_STONE); else
		if (nExtendedSize < m_makeOrder.m_nSize) 
			_warning(CANNOTBUILDSOMUCH,4,EN_ROAD, nExtendedSize, m_makeOrder.m_nSize);
		nExtendedSize = min(nExtendedSize,m_makeOrder.m_nSize);
	}	

	build(bcost,nExtendedSize,FALSE,FALSE,TRUE);

	if (m_pMadeRoad)
	{
		m_pMadeRoad->addExtendedSize(nExtendedSize);
		//if (cr().toSave())
		//	sMessagesList.Add(new CMessage(ICRMSG_BUILD, this, NULL, NULL, NULL, NULL, m_pMadeRoad, "", nExtendedSize, nExtendedSize));
	}

	return TRUE;
}

BOOL CUnit::onCheckMakeShip()
{
	CHKUNIT
	BKPTUNIT

	CShip * pShip = m_pMadeShip;
	if (!pShip) return _error(UNKNOWNBUILDING,"ship",itob(m_makeOrder.m_nID));
	const t_ship * tinf = pShip->infos();
	int nExtendedSize = 0;
	CString sLackItems("");

	if (!canBuildShip(pShip->currentSize(), tinf, nExtendedSize, sLackItems))
	{
		if (!sLackItems.IsEmpty()) 
			_error(BUILDLACK,pShip->fullName(),sLackItems);
		return FALSE;
	}

	if (m_makeOrder.m_nSize > 0)
	{
		if (nExtendedSize > m_makeOrder.m_nSize) 
			_warning(CANBUILDMORE,4,tinf->name,nExtendedSize, m_makeOrder.m_nSize,EN_WOOD); else
		if (nExtendedSize < m_makeOrder.m_nSize) 
			_warning(CANNOTBUILDSOMUCH,4,tinf->name, nExtendedSize, m_makeOrder.m_nSize);
		nExtendedSize = min(nExtendedSize,m_makeOrder.m_nSize);
	}

	if (nExtendedSize > tinf->size - pShip->currentSize())
	{
		int nAddedSize = nExtendedSize;
		nExtendedSize = tinf->size - pShip->currentSize();
		_warning(SHIPFULLYBUILT,nAddedSize-nExtendedSize);
	}
		
	spendItem(IT_WOOD, nExtendedSize);
	m_pMadeShip->addExtendedSize(nExtendedSize);
	if (cr().toSave())
		sMessagesList.Add(new CMessage(ICRMSG_BUILD_SHIP, this, NULL, NULL, NULL, NULL, m_pMadeShip, "", nExtendedSize, nExtendedSize));
	return TRUE;
}

BOOL CUnit::onCheckMakeHerb()
{
	int nUTID=ID_UNKNOWN;
	int nsk=level(SK_HERBALISM);

	ASSERT_VALID(this);

	int nbEffetsBusybeer=min(people(),effect(EFT_BUSYBEER));
	int nNb=(nbEffetsBusybeer*2+(people()-nbEffetsBusybeer))*nsk;
	if (nbEffetsBusybeer>0)
		addEffect(EFT_BUSYBEER,-nbEffetsBusybeer,TRUE);
	//int nNb = nsk*people();
	//if (m_bMakeDoubly) nNb*=2;

	CString sHerbName=fromRegion()->herbName();
	nUTID=findherb(sHerbName);
	addItemInfo(nUTID, sHerbName, UT_HERB)->addMade(nNb,regionFrom());
	if (cr().toSave())
		sMessagesList.Add(new CMessage(ICRMSG_MAKE_HERB, this, fromRegion(), NULL, NULL, NULL, NULL, CCR::German2English((LPCSTR)sHerbName),nNb,nNb));

	return TRUE;
}

BOOL CUnit::onCheckMakePotion()
{
	CHKUNIT
	BKPTUNIT

	ASSERT_VALID(this);

	CString sHerbname("");
	CString sHerbnames("");
	CString sHerbnameTmp("");

	CString sTooManyHerbnames("");

	int j;
	int nUTID=m_makeOrder.m_nUTID;
	t_potion * inf = potionFromUTID(nUTID);
	if (!inf) return errormsg("CUnit::onCheckMakePotion: internal error - bad UTID %d for %s",nUTID,outputName());
	int nminskill = inf->nskill;
	int nsk = level(SK_ALCHEMY);

	int nbEffetsBusybeer=min(people(),effect(EFT_BUSYBEER));
	int nbmax=(((nbEffetsBusybeer*2+(people()-nbEffetsBusybeer))*nsk)/2)/nminskill;

//	int nKoef=1;
//	if (m_bMakeDoubly) nKoef=2;
//	int nbmax = ((nsk*people()*nKoef)/2)/nminskill;

	int nb = nbmax;
	if (nbmax == 0) 
		return _error(POTIONSKILL,inf->name,nminskill);

	for (j=0;j<inf->nbherbs;j++)
	{
		int nind = inf->herb_utids[j];
		if (nind > 0)
		{ 
			sHerbname=herbname(nind);
			sHerbnames+="'";
			sHerbnames += sHerbname;
			sHerbnames += "' ";
			int nbh=nbHerbs(nind);
			if (nbh > nb) {
				sHerbnameTmp.Format("' %d %s' ",nbh-nb,(LPCSTR)sHerbname);
				sTooManyHerbnames += sHerbnameTmp;
			}
			nb=min(nb,nbh);	
		}
		else if (nind < 0)
		{
			//item - sample=dragonblood
			nind=-nind;
			sHerbnames+="'";
			CString sit(ItemName(nind));
			sHerbnames += sit;
			sHerbnames += "' ";
			nb=min(nb,nbItems(nind,sit));	
		}
	}
	if (nb==0) 
		return _error(POTIONHERBS,nbmax,inf->name,(LPCSTR)sHerbnames); 
	if (m_makeOrder.m_nSize > 0) 
		nb=min(nb,m_makeOrder.m_nSize);
	if (nb<nbmax && m_makeOrder.m_nSize <= 0)
		_warning(UNITMAKEITEM,1,nb,(LPCSTR)inf->name,nbmax,(LPCSTR)sHerbnames);
	if (!sTooManyHerbnames.IsEmpty())
		_warning(UNITUNUSEDHERB,4,(LPCSTR)sTooManyHerbnames);
		
	addItemInfo(nUTID, inf->name, UT_POTION)->addMade(nb,regionFrom());

	//TODO: compute real nbEffetsBusybeer used according to real made potions
	if (nbEffetsBusybeer>0)
		addEffect(EFT_BUSYBEER,-nbEffetsBusybeer,TRUE);

	for (j=0;j<inf->nbherbs;j++)
	{
		int nind = inf->herb_utids[j];
		if (nind > 0)
			addItemInfo(nind,herbname(nind),UT_HERB)->spend(nb);
		else
			addItemInfo(-nind,ItemName(-nind),UT_ITEM)->spend(nb);
	}

	if (cr().toSave())
		sMessagesList.Add(new CMessage(ICRMSG_PRODUCE_ITEM, this, fromRegion(), NULL, NULL, NULL, NULL, CCR::German2English((LPCSTR)inf->name),nb,nb));

	return TRUE;
}

BOOL CUnit::onCheckProduceItem(eItem nIndex)
{
	CHKUNIT
	BKPTUNIT

	t_cost d;
	d.set(1);

	CString sBuildingType("");
	int nMinSkill = (s_items[nIndex].m_infos?s_items[nIndex].m_infos->minskill:0);
	eSkill nsk = s_items[nIndex].m_nSkillID;
	if (nMinSkill==0) 
		return _error(CANNOTMAKEITEM,(LPCSTR)s_items[nIndex].m_sName);
	if ((int)nsk==-1) 
		return _error(CANNOTMAKEITEM,(LPCSTR)s_items[nIndex].m_sName);
	//int nWSLevel = level(nsk);
	BOOL bResource = TRUE;
	if (nIndex==IT_WOOD) {d.woods = 2; sBuildingType=EN_SAWMILL;} else
	if (nIndex==IT_MALLORN) {d.mallorn = 2; sBuildingType=EN_SAWMILL;} else
	if (nIndex==IT_STONE) {d.stones = 2; sBuildingType=EN_QUARRY;} else
	if (nIndex==IT_IRON) {d.iron = 2; sBuildingType=EN_MINE;} else
	if (nIndex==IT_LAEN) { d.laen = 2; sBuildingType=EN_MINE;} else
	if (nIndex!=IT_HORSE)
	{
		bResource = FALSE;
		if (nIndex == IT_CATAPULT_AMMUNITION) 
			sBuildingType=EN_QUARRY;
	}
	return onCheckMake(nIndex, s_items, nsk, sBuildingType, d, bResource);
}

void CUnit::checkInsideWhat()
{
	if (!m_cr_unit)	
		cr_isInsideSomething();
}

BOOL CUnit::inBuildingType(CString &sType) const
{
	sType="";
	if (!m_pBuilding) return FALSE;
	const t_building * tinf = m_pBuilding->infos();
	if (!tinf) return FALSE;
	sType=tinf->name;
	return TRUE;
}

BOOL CUnit::isInBuildingType(const char * sBuildingType, BOOL * pbWorking) const
{
	CString	sType("");
	if (inBuildingType(sType) && !_strcmpi((LPCSTR)sType,sBuildingType)) {
		if (pbWorking) *pbWorking = m_pBuilding->isWorking();
		return TRUE;
	}
	return FALSE;
}

BOOL CUnit::onCheckMake(int nIndex, const TUTItemInfo * tabItems, eSkill nsk, const char * sBuildingName, t_cost & d, BOOL bResource)
{
	CHKUNIT
	BKPTUNIT

	ASSERT_VALID(this);

	int nSize=0;
	t_cost cres;
	cres.set(0);

	if (SK_NONE==nsk)
		return FALSE; //TODO: error: this kind of item cannot be made
	BOOL bInBuilding = FALSE;
	BOOL bBuildingWorking = FALSE;
	int nMinResourceLevel = 1;
	int nMinSkill = (tabItems[nIndex].m_infos?tabItems[nIndex].m_infos->minskill:0);
	if (nMinSkill == 0) 
		return _error(CANNOTMAKEITEM,(LPCSTR)tabItems[nIndex].m_sName);

	if (!isAlive())
	{
		nSize = m_makeOrder.m_nSize;
		if (nSize==-1) {
			errormsg("onCheckMake::-1 %s done for %s",(LPCSTR)tabItems[nIndex].m_sName,outputName());
			nSize=1;
		}
		addItemInfo(tabItems[nIndex].m_nUTID, tabItems[nIndex].m_sName, UT_ITEM)->addMade(nSize,regionFrom());
		if (cr().toSave())
			sMessagesList.Add(new CMessage(bResource?ICRMSG_PRODUCE_RESOURCE:ICRMSG_PRODUCE_ITEM, this, fromRegion(), NULL, NULL, NULL, NULL, CCR::German2English((LPCSTR)tabItems[nIndex].m_sName),nSize,nSize));
		return TRUE;
	}

	int nLevel = level(nsk);
	//TODO: correct bug: bWorkingAcademy can be FALSE if owner leaves
	bInBuilding = isInBuildingType(sBuildingName,&bBuildingWorking);
	if (bInBuilding && bBuildingWorking) 
		nLevel++; 
	else
		d.set(1);

	// TODO: to produce laen you must be inside a mine.
	// if (laen && !mine || !inMineWorking)
	// return _error();

	if (nLevel < nMinSkill) 
		return _error(MAKEITEMNOSKILL,(LPCSTR)tabItems[nIndex].m_sName,CCR::German2English(sSkillsEntries[nsk]));

	// resource level (iron, stone)
	if (bResource)
		nMinResourceLevel = fromRegion()->resourceLevel((eItem)nIndex);

	if (nMinResourceLevel > nLevel)
		return _error(MAKERESOURCENOSKILL,(LPCSTR)tabItems[nIndex].m_sName,CCR::German2English(sSkillsEntries[nsk]),nMinResourceLevel,(LPCSTR)tabItems[nIndex].m_sName);

	if (nMinResourceLevel==nLevel && bResource)
		_warning(MAKESOONRESOURCENOSKILL,11,(LPCSTR)tabItems[nIndex].m_sName,CCR::German2English(sSkillsEntries[nsk]),nMinResourceLevel,(LPCSTR)tabItems[nIndex].m_sName);

	int nbqf=nbRingsQuickFingers();
	int nbqfmax=min(nbqf,people());

	int nbr=0;
	int nRatio=1;
	int nbm=people();
	int nbrp=nbm;
	int nbEffetsBusybeer=min(nbm,effect(EFT_BUSYBEER));
	if (nbEffetsBusybeer>0) {
		if (nbqfmax==0) {
			nbrp=(nbEffetsBusybeer*2+(nbm-nbEffetsBusybeer));
		}
		else 
		if (nbEffetsBusybeer==nbm)
			nRatio=2;
		else
		{
			//how does it work : partial busybeer+QuickFinger?
			// for the moment ignore busybeer in max
			int hhhh=1;
		}
		addEffect(EFT_BUSYBEER,-nbEffetsBusybeer,TRUE);
	}
	int nNbMaxItems = (nRatio*(10*nbqfmax+nbrp-nbqfmax)*nLevel)/nMinSkill;


	t_cost c;
	nSize=nNbMaxItems;
	if (m_makeOrder.m_nSize > 0)
		nSize=min(nSize,m_makeOrder.m_nSize);

	CString sLackItems("");
	if (tabItems[nIndex].m_infos) tabItems[nIndex].m_infos->cost.copyTo(c); else c.set(0);
	if (!canMake(c,d,cres,nSize,sLackItems)) 
		return _error(UNITMAKENOITEM,(LPCSTR)tabItems[nIndex].m_sName,sLackItems);
	
	//ignore if just 1 not done
	if (nSize<nNbMaxItems && m_makeOrder.m_nSize <= 0)
		_warning(UNITMAKEITEM,nNbMaxItems-nSize==1?5:4,nSize,(LPCSTR)tabItems[nIndex].m_sName,nNbMaxItems,sLackItems);

	if (bResource)
		fromRegion()->onConsume(tabItems[nIndex],d,this,nLevel,nSize);

	if (nSize > 0)
	{
		TItemInfo * pit = addItemInfo(tabItems[nIndex].m_nUTID, tabItems[nIndex].m_sName, UT_ITEM);
		pit->addMade(nSize,regionFrom());
		build(cres, 1);

		if (bInBuilding && m_pBuilding)
			m_pBuilding->setUsed();

		//TODO: specify correct wanted value
		if (cr().toSave())
			sMessagesList.Add(new CMessage(bResource?ICRMSG_PRODUCE_RESOURCE:ICRMSG_PRODUCE_ITEM, this, fromRegion(), NULL, NULL, NULL, NULL, CCR::German2English((LPCSTR)tabItems[nIndex].m_sName),nSize,nSize));
	}

	return (nSize>0);
}

BOOL CUnit::onCheckMakeWeapon(eWeapon nIndex)
{
	t_cost d;
	d.set(1);
	d.iron = 2;

	if (nIndex==WP_ELVENBOW && !isElf())
		return _error(ELVENBOWERR);
	return onCheckMake(nIndex, s_weapons, SK_WEAPONSMITHING, EN_SMITHY,d);
}

BOOL CUnit::onCheckMakeArmourItem(eArmourItem nIndex)
{
	CHKUNIT
	BKPTUNIT

	t_cost d;
	d.set(1);
	d.iron = 2;

	return onCheckMake(nIndex, s_armourItems, SK_ARMOURSMITHING, EN_SMITHY,d);
}

BOOL CUnit::onCheckMakeItem()
{
	CHKUNIT
	BKPTUNIT

	int i=ID_UNKNOWN;
	if (isItem(m_makeOrder.m_nUTID,&i)) 
		return onCheckProduceItem((eItem)i);
	if (isWeapon(m_makeOrder.m_nUTID,&i)) 
		return onCheckMakeWeapon((eWeapon)i);
	if (isArmour(m_makeOrder.m_nUTID,&i)) 
		return onCheckMakeArmourItem((eArmourItem)i);
	return errormsg("onCheckMakeItem::unknown itemid %d for %s",m_makeOrder.m_nUTID,outputName());
}

BOOL CUnit::spendItem(eItem nUTid, int n)
{
	if (n > 0)
	{
		if (nUTid == IT_SILVER) 
			return spendMoney(n,TRUE); //CG - TRUE added
		addItemInfo(s_items[nUTid].m_nUTID,s_items[nUTid].m_sName,UT_ITEM,(s_items[nUTid].m_infos?s_items[nUTid].m_infos->weight():0))->spend(n);
	}
	return TRUE;
}

BOOL CUnit::canUseItem(eItem nUTid, int n, int & nUsed, int & nSize, CString & sItem)
{
	sItem="";
	nSize = -1;
	nUsed = 0;
	if (n <= 0) return TRUE;
	int nbHavings = nbUTItems(nUTid);
	nUsed = nbHavings - n;
	nSize = 0;
	if (nUsed < 0) 
	{
		sItem.Format("%d %s",-nUsed,s_items[nUTid].m_sName);
		return FALSE;
	}
	nSize = nbHavings / n;
	nUsed = nSize * n;
	return TRUE;
}

BOOL CUnit::canUseMaxItem(eItem nUTid, int n, int nMax, int nDivider, int & nUsed, int & nSize, CString & sLackItem)
{
	// pbm here ; for claymore
	CString sItem("");
	nSize = -1;
	nUsed = 0;
	if (n <= 0) return TRUE;
	int nbHavings = nbUTItems(nUTid);
	int nMax2=divr(nMax*n,nDivider);
	nUsed = min(nbHavings, nMax2/*/n*/);
	nSize = 0;
	if (nUsed >= 0) 
	{
		nSize = min((nUsed * nDivider)/n,nMax);
		if (nSize < nMax)
			sItem.Format("%d %s",nMax2-nUsed,s_items[nUTid].m_sName);
	}
	else
		sItem.Format("%d %s",-nUsed,s_items[nUTid].m_sName);

	if (!sItem.IsEmpty()) sLackItem+=sItem+" ";
	return TRUE;
}

BOOL CUnit::build(const t_cost & cost, int nSize, BOOL bIsCastle, BOOL bIsShip, BOOL bIsRoad)
{
	if (bIsCastle || bIsRoad) return spendItem(IT_STONE, nSize);
	if (bIsShip) return spendItem(IT_WOOD, nSize);
	if (cost.sum()==0) return TRUE;
	spendItem(IT_SILVER, cost.silver*nSize);
	spendItem(IT_WOOD, cost.woods*nSize);
	spendItem(IT_STONE, cost.stones*nSize);
	spendItem(IT_IRON, cost.iron*nSize);
	spendItem(IT_MALLORN, cost.mallorn*nSize);
	return spendItem(IT_LAEN, cost.laen*nSize);
}

int CUnit::maxStoneForCastleBuilder(int nNbPersons, int nCastleSize)
{
	int nCoef = e_castleCoef(nCastleSize);
	int nTotalSkill = level(SK_MASONRY)*nNbPersons;	
	int nMaxStones = nTotalSkill/nCoef;

	//check if another castle size limit is reacht
	int nNewCoef = e_castleCoef(nCastleSize+nMaxStones-1);
	if (nNewCoef > nCoef)
	{
		int nNeededStones = e_castleBoundFromCoef(nNewCoef)-nCastleSize;
		int nSkillLeft = nTotalSkill-nCoef*nNeededStones;
		nMaxStones = nNeededStones+nSkillLeft/nNewCoef;
	}
	return nMaxStones;
}

int CUnit::maxWoodForShipcrafter(const t_ship * tinf, int nNbPersons, int nCurrentSize)
{
	if (!tinf) return 0;
	int nCoef = tinf->shipcraftSkill;
	int nTotalSkill = level(SK_SHIPCRAFT)*nNbPersons;	
	return min(nTotalSkill/nCoef,tinf->size-nCurrentSize);
}

BOOL CUnit::canBuildCastle(int nCurrentSize, t_cost & result, int & nSize, CString & sLackItems)
{
	CHKUNIT
	BKPTUNIT

	int nbPersons = people();
	int nUsed;
	int nMaxStones = maxStoneForCastleBuilder(nbPersons, nCurrentSize);
	int nbHavings = nbUTItems(IT_STONE);
	
	nSize = min(nbHavings,nMaxStones);
	nUsed = nSize;

	if (nSize == 0)
	{
		sLackItems.Format("%d stones",nMaxStones);
		return FALSE;
	}
	
	if (nSize<nMaxStones)
	{
		_warning(CANBUILDMORE,6,EN_CASTLE,nMaxStones,nSize,EN_STONE);
		nbPersons--;
		int nbUnusedMen = 0;
		while (nbPersons > 0)
		{
			if (maxStoneForCastleBuilder(nbPersons, nCurrentSize) < nSize) break;
			nbUnusedMen++;
			nbPersons--;
		}
		if (nbUnusedMen > 0) 
			_warning(CANUSEBUILDERFOROTHER,6,nbUnusedMen,EN_CASTLE);
	}
	return TRUE;
}

BOOL CUnit::canBuildRoad(int nCurrentSize, t_cost & result, int & nSize, CString & sLackItems)
{
	CHKUNIT
	BKPTUNIT

	int nNbPersons = people();
	int nUsed;
	int nlvl = level(SK_ROADWORK);
	int nMaxStones = nlvl*nNbPersons;	
	int nbHavings = nbUTItems(IT_STONE);
	nSize = min(nbHavings,nMaxStones);
	nUsed = nSize;

	if (nSize == 0)
	{
		sLackItems = "some stones";
		return FALSE;
	}
	
	if (nSize<nMaxStones)
	{
		_warning(CANBUILDMORE,6,EN_ROAD,nMaxStones,nSize,EN_STONE);
		nNbPersons--;
		int nbUnusedMen = 0;
		while (nNbPersons > 0)
		{
			nMaxStones -= nlvl;
			if (nMaxStones < nSize) break;
			nbUnusedMen++;
			nNbPersons--;
		}
		if (nbUnusedMen > 0) 
			_warning(CANUSEBUILDERFOROTHER,6,nbUnusedMen,EN_ROAD);
	}
	return TRUE;
}

BOOL CUnit::canBuildShip(int nShipSize, const t_ship * tinf, int & nSize, CString & sLackItems)
{
	int nbPersons = people();
	int nUsed;
	int nMaxWoods = maxWoodForShipcrafter(tinf, nbPersons, nShipSize);
	int nbHavings = nbUTItems(IT_WOOD);
	
	nSize = min(nbHavings,nMaxWoods);
	nUsed = nSize;
	
	if (nSize == 0) 
	{
		sLackItems="some wood";
		return FALSE;
	}
	
	int nNeededSkill = tinf->shipcraftSkill * nSize;
	int nTotalSkill = level(SK_SHIPCRAFT) * nbPersons;
	if (nTotalSkill < nNeededSkill) {
		int nRestrictedSize=nTotalSkill/tinf->shipcraftSkill;
		_warning(BUILDSKILL,6,nNeededSkill-nTotalSkill,(LPCSTR)CCR::German2English(CR_SHIPCRAFT),tinf->name,nSize-nRestrictedSize);
		nSize = nRestrictedSize;
	}

	if (nSize<nMaxWoods)
	{
		_warning(CANBUILDMORE,6,tinf->name,nMaxWoods,nSize,EN_WOOD);
		nbPersons--;
		int nbUnusedMen = 0;
		while (nbPersons > 0)
		{
			if (maxWoodForShipcrafter(tinf, nbPersons, nShipSize) < nSize) 
				break;
			nbUnusedMen++;
			nbPersons--;
		}
		if (nbUnusedMen > 0) 
			_warning(CANUSEBUILDERFOROTHER,6,nbUnusedMen,"ship");
	}
	return TRUE;
}

BOOL CUnit::canBuild(int nSizeBefore, const t_cost & cost, t_cost & result, int & nSize, CString & sLackItems, BOOL bIsCastle, BOOL bIsRoad)
{
	BOOL bOK = TRUE;
	int nCurrentSize = -1;
	nSize = -1;
	CString sItem("");
	sLackItems = "";
	if (bIsCastle) return canBuildCastle(nSizeBefore, result, nSize, sLackItems);
	if (bIsRoad) return canBuildRoad(nSizeBefore, result, nSize, sLackItems);
	if (canUseItem(IT_SILVER, cost.silver, result.silver, nCurrentSize,sItem)) nSize = positiveMin(nSize,nCurrentSize); else sLackItems+=sItem+" ";
	if (canUseItem(IT_WOOD, cost.woods, result.woods, nCurrentSize,sItem)) nSize = positiveMin(nSize,nCurrentSize); else sLackItems+=sItem+" ";
	if (canUseItem(IT_STONE, cost.stones, result.stones, nCurrentSize,sItem)) nSize = positiveMin(nSize,nCurrentSize); else sLackItems+=sItem+" ";
	if (canUseItem(IT_IRON, cost.iron, result.iron, nCurrentSize,sItem)) nSize = positiveMin(nSize,nCurrentSize); else sLackItems+=sItem+" ";
	if (canUseItem(IT_MALLORN, cost.mallorn, result.mallorn, nCurrentSize,sItem)) nSize = positiveMin(nSize,nCurrentSize); else sLackItems+=sItem+" ";
	if (canUseItem(IT_LAEN, cost.laen, result.laen, nCurrentSize,sItem)) nSize = positiveMin(nSize,nCurrentSize); else sLackItems+=sItem+" ";
	if (!sLackItems.IsEmpty()) 
		nSize=0; //lacks something
	return (nSize!=0);
}

BOOL CUnit::canMake(const t_cost & cost, const t_cost &d, t_cost & result, int & nSize, CString & sLackItems)
{
	BOOL bOK = TRUE;
	int nMaxSize = nSize;
	int nCurrentSize = -1;
	nSize = -1;
	CString sItem("");
	for (int i=0;i<2;i++)
	{
		if (cost.sum() > 0)
		{
			if (canUseMaxItem(IT_WOOD, cost.woods, nMaxSize, d.woods, result.woods, nCurrentSize,sItem)) nSize = positiveMin(nSize,nCurrentSize);
			if (canUseMaxItem(IT_STONE, cost.stones, nMaxSize,d.stones, result.stones, nCurrentSize,sItem)) nSize = positiveMin(nSize,nCurrentSize);
			if (canUseMaxItem(IT_IRON, cost.iron, nMaxSize,d.iron, result.iron, nCurrentSize,sItem)) nSize = positiveMin(nSize,nCurrentSize);
			if (canUseMaxItem(IT_MALLORN, cost.mallorn, nMaxSize,d.mallorn, result.mallorn, nCurrentSize,sItem)) nSize = positiveMin(nSize,nCurrentSize);
			if (canUseMaxItem(IT_LAEN, cost.laen, nMaxSize,d.laen, result.laen, nCurrentSize,sItem)) nSize = positiveMin(nSize,nCurrentSize);
		}
		else
		{
			// if no cost, then maxsize items can be made
			// TODO: check region resources
			nSize = nMaxSize;
		}
		if (nSize == nMaxSize) break;
		sLackItems = sItem;
		nMaxSize = nSize;
	}
	return (nSize!=0);
}

////////////////////
// static methods

// magic bag taken into account
int CUnit::itemWeightOutsideMagicBag(int lbsi, const char * sItemName, int & lbsitems_in_magic_bags_max)
{
	// no stone nor horse nor cart in magicbag
	if (lbsi>0 && lbsitems_in_magic_bags_max>0 && _strcmpi(sItemName,EN_MAGIC_BAG) && _strcmpi(sItemName,EN_STONE))
	{
		int lbsm=min(lbsitems_in_magic_bags_max,lbsi);
		lbsitems_in_magic_bags_max-=lbsm;
		lbsi-=lbsm;
	}
	return lbsi;
}

int CUnit::paramIndexToCombatStatus(int i)
{
	switch (i)
	{
		case P_AGGRESSIVE	: return COMBATSTATUS_AGGRESSIVE;
		case P_FRONT		: return COMBATSTATUS_FRONT;
		case P_BEHIND		: return COMBATSTATUS_REAR;
		case P_DEFENSIVE	: return COMBATSTATUS_DEFENSIVE;
		case P_NOT			: return COMBATSTATUS_NOT;
		case P_FLEE			: return COMBATSTATUS_FLEE;
		default				: break;
	}
	return COMBATSTATUS_UNKNOWN;
}

void CUnit::onsaveCheckUnitsToRemove()
{
	POSITION pos = s_unitsToRemove.GetHeadPosition();
	while (pos) 
		s_unitsToRemove.GetNext(pos)->onsave_remove();
}

void CUnit::checkControlGivers()
{
	POSITION pos = s_controlGivers.GetHeadPosition();
	while (pos) 
		s_controlGivers.GetNext(pos)->onCheckGiveControl();
}

void CUnit::checkAttackers()
{
	POSITION pos = s_attackers.GetHeadPosition();
	while (pos) 
		s_attackers.GetNext(pos)->onCheckAttack();
}

void CUnit::checkTaxers()
{
	POSITION pos = s_taxers.GetHeadPosition();
	while (pos) 
		s_taxers.GetNext(pos)->onCheckTax();
}

void CUnit::checkCarriers()
{
	POSITION pos = s_carriers.GetHeadPosition();
	while (pos) 
		s_carriers.GetNext(pos)->onCheckCarrier();
}

void CUnit::checkPotentialReceivers()
{
	POSITION pos = s_potentialReceivers.GetHeadPosition();
	while (pos) 
		s_potentialReceivers.GetNext(pos)->onCheckPotentialReceiver();
}

void CUnit::checkTransported()
{
	POSITION pos = s_transportedByHorse.GetHeadPosition();
	while (pos) 
		s_transportedByHorse.GetNext(pos)->onCheckTransported();
}

void CUnit::checkMovers(BOOL bBeforeMove)
{
	POSITION pos = s_movers.GetHeadPosition();
	while (pos) 
		s_movers.GetNext(pos)->onCheckMove(bBeforeMove);
}

void CUnit::checkMovesOnLand()
{
	POSITION pos = s_movers.GetHeadPosition();
	while (pos) 
		s_movers.GetNext(pos)->onCheckMoveOnLand();
}

void CUnit::checkGuards()
{
	POSITION pos = s_guards.GetHeadPosition();
	while (pos) 
		s_guards.GetNext(pos)->onCheckGuard();
}

void CUnit::checkEntertainers()
{
	POSITION pos = s_entertainers.GetHeadPosition();
	while (pos)	
		s_entertainers.GetNext(pos)->onCheckEntertain();
}

void CUnit::checkTraders()
{
	POSITION pos = s_traders.GetHeadPosition();
	while (pos)	
		s_traders.GetNext(pos)->onCheckTrade();
}

void CUnit::checkWorkers()
{
	POSITION pos = s_workers.GetHeadPosition();
	while (pos)	
		s_workers.GetNext(pos)->onCheckWork();
}

void CUnit::checkRecruited()
{
	POSITION pos = s_recruited.GetHeadPosition();
	while (pos)	
		s_recruited.GetNext(pos)->onCheckRecruited();
}

void CUnit::checkTeachers()
{
	POSITION pos = s_teachers.GetHeadPosition();
	while (pos)	
		s_teachers.GetNext(pos)->onCheckTeachers();
}

void CUnit::checkLearners()
{
	POSITION pos = s_learners.GetHeadPosition();
	while (pos)	
		s_learners.GetNext(pos)->onCheckLearners();
}

void CUnit::checkHeroes()
{
	POSITION pos = s_soonHeroes.GetHeadPosition();
	while (pos)	s_soonHeroes.GetNext(pos)->onCheckHeroes();
}

void CUnit::checkMakers()
{
	POSITION pos = s_makers.GetHeadPosition();
	while (pos)	
		s_makers.GetNext(pos)->onCheckMakers();
}

void CUnit::remindMakers()
{
	POSITION pos = s_makers.GetHeadPosition();
	while (pos)	
		s_makers.GetNext(pos)->onRemindMakers();
}

void CUnit::loadIDsFrom(TUTItemInfo tabitems[], int nMax, int nFirst)
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

void CUnit::loadUTIDs()
{
	//loadIDsFrom(s_potions,PO_NBMAX);
	loadIDsFrom(s_weapons,WP_NBMAX);
	loadIDsFrom(s_armourItems,AR_NBMAX);
	loadIDsFrom(s_items,IT_NBMAX,IT_SILVER+1); //ignore IT_SILVER
}

void CUnit::before_checkings()
{
	//loadUTIDs();
	sUnitsList.beforeCheck();
}

void CUnit::check_regionMoney()
{
	POSITION pos = sUnitsList.GetHeadPosition();
	while (pos)	{
		CUnit * u = sUnitsList.GetNext(pos);
		u->moneyToRegion(FALSE);
		// Réservation < argent des unités ?  prendre l'argent d'autres unités 
		if (u->reservedSilver() > u->money()) {
			int i=u->reservedSilver() - u->money();
			POSITION pos2 = sUnitsList.GetHeadPosition();
			while (pos2 && i>0) {
				CUnit * t = sUnitsList.GetNext(pos2);
				if (t->regionTo()!=u->regionTo() || t==u) continue;
				int um=min(i,t->money()-t->reservedSilver());
				if (um>0) {
					if (t->onTransferMoney(um, u, FALSE))
						i-=um;
				}
			}
		}
	}
}

BOOL CUnit::onCarriedByHorse(CUnit * pCarrier, const TMoveInfo & moveInfos)
{
	if (!pCarrier) return errormsg("onCarriedByHorse::internal error");
	if (!m_pRider || m_pRider!=pCarrier)
		return _error(DOESNTRIDE,pCarrier->outputName());
	SetMoveStatus(MOVESTATUS_SOMEWHERE);
	int x=LOCATION_UNKNOWN_VALUE;
	int y=LOCATION_UNKNOWN_VALUE;
	if (pCarrier->isAlive())
	{
		x=pCarrier->x();
		y=pCarrier->y();
	}
	SetXY(x,y);
	if (OnMove())
	{
		if (pCarrier->isAlive() && cr().toSave())
		{
			m_moveInfos.copy(moveInfos);
			sMessagesList.Add(new CMessage(ICRMSG_TRANSPORT, pCarrier, fromRegion(), this, toRegion()));
		}
		return TRUE;
	}
	return FALSE;
}

void CUnit::spendCosts()
{
	CUnit *u;			
	POSITION pos=NULL;
	
	u=sUnitsList.Find(-1,0);
	if (u) u->empty();

	if (sRegionsList.GetCount() > 0) {
		pos = sUnitsList.GetHeadPosition();
		while (pos) {
			u = sUnitsList.GetNext(pos);
			u->SpendMaintainCost();
			u->SpendLearningCost();
		}
	}
}

void CUnit::checkCosts()
{
	CUnit *u;			
	POSITION pos=NULL;
	
	u=sUnitsList.Find(-1,0);
	if (u) u->empty();

	if (sRegionsList.GetCount() > 0) {
		pos = sUnitsList.GetHeadPosition();
		while (pos) {
			u = sUnitsList.GetNext(pos);
			if (u->isCostingBuildingOwner() || u->learningCost())
				u->checkMoney(TRUE);
		}
	}
}

void CUnit::check_money(BOOL bBeforeMove) 
{
	CUnit *u;			
	POSITION pos=NULL;
	
	u=sUnitsList.Find(-1,0);
	if (u) u->empty();

	if (sRegionsList.GetCount() > 0) {


		if (bBeforeMove)
		{
			check_regionMoney();
			CRegion::moneyCheckings();
		}
		else
		{
			// l'argent manquant du groupement d'argent prendre
			pos = sUnitsList.GetHeadPosition();
			while (pos)	{
				u = sUnitsList.GetNext(pos);
				u->checkMoney(FALSE);
			}
		}
	}

	// here is money on regions before upkeep, before move
	sRegionsList.ResetSilver();
}

void CUnit::feed() 
{
	if (isDemon())
		feedDemon();
	if (isAlive())
		moneyToRegion(TRUE); // silver of the units into the silver pool
}

void CUnit::check_units() 
{
	CUnit *u;

	POSITION pos = sUnitsList.GetHeadPosition();
	while (pos)	{
		u=sUnitsList.GetNext(pos);
		u->feed();
	}
}

void CUnit::setCurrentOrder(const char *pOrder)
{
	m_sOrder = pOrder;
}

BOOL CUnit::_warning(int nErrorNum, int nLevel, ...)
{
	CHKUNIT
	BKPTUNIT
	if (UNITTEACHTOOMUCH==nErrorNum) {
		if (m_bWarningTeachTooMuch)
			return FALSE;
		m_bWarningTeachTooMuch=TRUE;
	}
	outputSetRegionName(regionOutputName());
	va_list argList;
	va_start(argList, nLevel);
	errvalist(nLevel, nErrorNum, outputPrefix(), argList);
	va_end(argList);
	return FALSE;
}

const char * CUnit::outputName() const
{
	return (LPCSTR)m_sOutputName;
}


CString CUnit::outputPrefix() const
{
	CString sResult;
	sResult.Format("[%s] %s: ",m_pFaction?m_pFaction->name():"unknown",(LPCSTR)outputName());
	return sResult;
}

BOOL CUnit::_error(int nErrorNum,...)
{
	CHKUNIT
	BKPTUNIT

	ERTRACE("Unit::_error %d",nErrorNum);
	outputSetRegionName(regionOutputName());
	va_list argList;
	va_start(argList, nErrorNum);
	errvalist(0, nErrorNum, outputPrefix(), argList);
	va_end(argList);
	return FALSE;
}

BOOL CUnit::checkHasLongOrder()
{
	CHKUNIT
	BKPTUNIT

	if (isAlive() && HasMen() && !HasLongOrder()) {
		if (!isAquarian() && fromOcean())
			return FALSE;
		return _warning(LONGORDERMISSING,m_bAttacking?6:2);
	}
	return TRUE;
}

BOOL CUnit::cr_retrieveBasicInfos()
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
			m_sOutputName.Format("%s (%s)",(LPCSTR)UTF8_to_ansi(m_sShortName),sid());
			m_sNewName = m_sShortName;
		}

		if (cr().geti(m_cr_unit,CR_PLAYER,nPlayerID))
			m_pFaction = sFactionsList.Add(nPlayerID,CCR::German2English(sRaceName));
		else
		{
			int nCamouflagedPlayer = 0;
			if (cr().geti(m_cr_unit,CR_CAMOUFLAGED_PLAYER,nCamouflagedPlayer) && nCamouflagedPlayer==1)
			{
				// Parteitarnung
				//TODO: special faction
				m_pFaction = sFactionsList.AddCamouflaged(CCR::German2English(sRaceName),m_cr_unit);
			}
		}

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

BOOL CUnit::retrieveFaction()
{
	if (!IsTemp() && m_pFaction==NULL && cr().Loaded())
		cr_unit();
	
	return (m_cr_unit!=NULL);
}

BOOL CUnit::setInitialNbPeople(int n) 
{ 
	CHKUNIT
	BKPTUNIT

	if (m_nPeople!=0 && !m_pMenGiver)
	{
		if (isAlive()) return errormsg("setInitialNbPeople::internal error - inital people already set for unit %s",outputName());
		if (m_nPeople!=n) return errormsg("setInitialNbPeople::internal error - nbpeople different for unit %s",outputName());
		m_nPeople = 0;
	}
	m_nInitialPeople = n;
	return addPeople(n);
}

BOOL CUnit::addPeople(int n, CUnit * pMenGiver, BOOL bGiveUnit) 
{ 
	CHKUNIT
	BKPTUNIT
	
	if (pMenGiver==this)
		return FALSE;

	if (pMenGiver) 
	{
		m_pMenGiver = pMenGiver;
		if (bGiveUnit)	m_bReceivedUnit = TRUE;
	}
	m_nPeople+=n; 
	return TRUE;
}


BOOL CUnit::removePeople(int n) 
{ 
	CHKUNIT
	BKPTUNIT
	if (m_nPeople==0) return errormsg("removePeople::internal error for unit %s",outputName());
	m_nPeople-=n;
	return TRUE;
}

BOOL CUnit::beforeCheck()
{
	ProcessOrdersBySequence();
	checkHasLongOrder();
	checkMen();
	return TRUE;
}

BOOL CUnit::checkMen()
{
	CHKUNIT
	BKPTUNIT

//	CHKUNIT2
	BKPTUNIT

	if (m_bReceivedUnit && m_pMenGiver)
	{
		if (!IsMadeTemp()) 
			return _error(UNITGIVEUNITTEMP);
		//TODO: check same races - humans only can receive other races
		//TODO: check maxnumber of received men (humans/others)
		if (!m_pMenGiver->isRace(m_pFaction->raceName())) 
			return _error(UNITGIVEUNITRACE);
	}

	if (!IsTemp() && !isAlive()) // unité ou Untot/illusion étrangères: omettre, parce que son besoin en argent ne compte pas
	{
		if (!retrieveFaction())
		{
			if (m_bUnknown)
				m_pRegionFrom->RemoveUnit(this);
		}
		return TRUE;
	}

	if (IsTemp() && !IsMadeTemp())
		return _warning(5,TEMPNOTTEMP);

	if (notEnoughPeople())
		_warning(UNITHASPERSONS,3, people());

	int nbPersons = people();

	if (nbPersons==0)
	{

		if ((!IsTemp() && money()>0) || IsTemp()) 
		{
			if (IsTemp())
				_warning((money()>0?UNITHASNTPERSONS:TEMPHASNTPERSONS),2);
			else 
				/*if (no_comment<=0)*/
				_warning(UNITLOSESITEMS,3);
		}

		if (m_bReceivedSomething)
			_warning(UNITLOSESRECEIVED,3);
	}

	checkInsideWhat(); //tmp
	if (m_pFaction && nbPersons > 0)
		m_pFaction->AddNbMen(nbPersons,m_bHeroes,IsRecruited());

	if (m_pRegionFrom && m_pRegionFrom->war() && m_pInitialShip && !m_bAttacking && cr_combatStatus()!=COMBATSTATUS_NOT)
		_warning(UNITONSHIPWAR,-1);
		
	return TRUE;
}

int isparam(const char *s, int i, char print) {
	if (i != findparam(s))
		return 0;
	return 1;
}

CUnit * CUnit::unit(CFaction * pFaction, int n, int tempStatus, int rX, int rY) {

	CUnit *u=sUnitsList.Add(pFaction, n, tempStatus, rX, rY, "");
	if (u->TempStatus() < UNIT_STATUS_NOTTEMP)
		Error(ISUSEDIN2REGIONS, u->outputName(), rX, rY, u->x(), u->y());

	if (u->TempStatus() < UNIT_STATUS_TEMP)
		u->SetTempStatus(tempStatus);

	return u;
}

BOOL CUnit::canDoLongOrder(K_Order nCurrentCmd, const CString & sCurrentOrder) {
	if (fromOcean() && !isAquarian())
		return FALSE;
	if (HasLongOrder()) {
		CString s(longOrder());
		int nInd = s.Find(' ');
		if (nInd!=-1) s = s.Left(nInd);
		int i=findkeyword(s);
		switch (i) {
			case K_CAST: if (nCurrentCmd==i) return TRUE;
			case K_SELL:
			case K_BUY:
				//  several BUY/SELL per unit = possible
				if (nCurrentCmd==K_SELL || nCurrentCmd==K_BUY) return TRUE;
		}
		if ((i==K_FOLLOW  || nCurrentCmd==K_FOLLOW) && (i!=nCurrentCmd))
			return TRUE;
		return _warning(UNITALREADYHASLONGORDERS, 1, 0, longOrder());
	}
	return setLongOrder(sCurrentOrder);
}

int CUnit::order_nextUnitID(int type, CUnit **cmdUnit) {
	const char *s;
	char is_temp=0;
	int i;

	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	if (cmdUnit)
		*cmdUnit=NULL;

	s=getstr();
	if (s[0]==0) {
		if (type==NECESSARY) {
			_error(UNITMISSING);
			return GETAUNIT_FAILURE;
		}
		return 1;
	}
	switch (findparam(s)) {
		case P_PEASANT:
			//Scat(printparam(P_PEASANT));
			return GETAUNIT_THROWAWAY;

		case P_TEMP:
			//scat(" TEMP");
			s=getstr();
			i=btoi(s);
			is_temp=1;
			break;

		default:
			i=btoi(s);
			break;
	}

	if (type==42) {	//Seulement essai, si une unité vient, parce que c'est une erreur
		if (i)
			return 42;
		return GETAUNIT_FAILURE;
	}

	if (i==0) 
		return GETAUNIT_THROWAWAY; // ex GIVE 0

	if (cmdUnit)
		*cmdUnit=CUnit::unit(NULL,i,is_temp,m_pRegionFrom->x(),m_pRegionFrom->y());
	//bcat(i);
	if (is_temp)
		return GETAUNIT_TEMP;
	return GETAUNIT_ID;
}

BOOL CUnit::onProcess_order_give(const char *s, BOOL bAutomatic)
{
	int i, n;
	int nType = UTTYPE_UNKNOWN;
	CUnit *receiver_unit=NULL;

	CHKUNIT
	BKPTUNIT

	int nResult=order_nextUnitID(NECESSARY,&receiver_unit);

	s=getstr();

	if (isparam(s, P_CONTROL,1))
		return onGiveControl(receiver_unit);
	if (isparam(s, P_HERBS,1))
		return onGiveHerbs(receiver_unit,bAutomatic);
	if (isparam(s, P_UNIT,1)) 
		return onGiveUnit(receiver_unit);

	// how much given
	n=atoi(s);
	if (n<1) {
		if (findparam(s)==P_ALLES) {	// DONNER xx TOUT wasauchimmer
			n=-1;
		} else {
			// "GIVE xxx 0 silver" ignored for error
			BOOL bIgnoreGiveErr=(s && s[0]=='0' /*&& isparam(s, P_SILVER,1)*/);
			if (!bIgnoreGiveErr)
				_error(NUMMISSING);
			n=0;
		}
	}

	s=getstr2();
	if (!(*s) && n<0)	
		return onGiveAll(receiver_unit,bAutomatic); // DONNER xx TOUT

	if (!(*s))
		return _error(GIVEWHAT);

	if (n==0)
		return FALSE;

	i=findparam(s);

	if (i==P_PERSON) return onGiveMen(n, receiver_unit, bAutomatic);
	if (i==P_SILVER) return onGiveSilverOrder(n, receiver_unit, bAutomatic);

	i=finditem(s);
	if (i<0) {
		i=findherb(s);
		if (i<0) {
			i=findpotion(s);
			if (i>=0)
				nType = UT_POTION;
			else
			{
				i = K_NOTHING;
				_warning(UNRECOGNIZEDOBJECT,1,s);
			}
		} 
		else
			nType = UT_HERB;
	} 
	else 
		nType = UT_ITEM;

	if (i==UTTYPE_UNKNOWN)
		return _error(GIVEWHAT);

	return onGive(i,s,nType,n,receiver_unit,bAutomatic,FALSE,FALSE);
}

BOOL CUnit::onProcess_order_cast(const char *s, BOOL bAutomatic)
{
	s=getstr2();

	t_spell *sp;
	int x_region=0;
	int y_region=0;
	int level=1;
	BOOL bRegion=FALSE;

	if (findparam(s) == P_REGION) {
		s=getstr();
		if (!*s)
			return _error(ERRORREGIONPARAMETER);
		x_region=atoi(s);
		s=getstr();
		if (!*s)
			return _error(ERRORCOORDINATES);
		y_region=atoi(s);
		bRegion=TRUE;
		s=getstr();
	}

	if (findparam(s) == P_LEVEL) {
		s=getstr();
		if (!*s || atoi(s)<1)
			return _error(ERRORLEVELPARAMETERS);
		level=atoi(s);
		s=getstr();
	}
	CString sMagie("");
	magicSchool(sMagie);
	BOOL bInMagicSchool=FALSE;
	sp=findSpell(s,sMagie,bInMagicSchool);
	if (!sp)
		return _error(UNRECOGNIZEDSPELL,s);
	if (*s>='0' && *s<='9')
		return _error(MISSINGPARAMETERS);
	if (!bInMagicSchool && !isPet())
		return _error(NOTINMAGICSCHOOL,sp->name,(LPCSTR)sMagie);

	if (!(sp->type & SP_ZAUBER)) 
		return _warning(ISCOMBATSPELL,1,sp->name,(sp->type & SP_ZAUBER) ? _msg(ISNOTCOMBATSPELL) : "");

	if ((sp->type & SP_BATTLE) && (spell & sp->type)) {
		int nOrder=0;
		switch (sp->type) {
			case SP_POST:nOrder = POST; break;
			case SP_PRAE: nOrder = PRE; break;
		}
	}

	/*
	do {
		s=getstr();
		//if (*s) {Scat(s);}
	} while (*s);		// remaining parameters without checks spend
	*/
	return OnCastOrder(sp, 1, level, x_region, y_region, bRegion);
}

BOOL CUnit::onProcess_order_tax(const char *s, BOOL bAutomatic)
{
	// nAmount=get_i()
	s=getstr2();
	int nAmount=atoi(s);
	if (nAmount < 1) nAmount=-1;
	return onTaxOrder(nAmount);
}

BOOL CUnit::onProcess_order_entertain(const char *s, BOOL bAutomatic)
{
	// nAmount=get_i()
	s=getstr2();
	int nAmount=atoi(s);
	if (nAmount < 1) nAmount=-1;
	return onEntertainOrder(nAmount);
}

BOOL CUnit::onProcess_order_work(const char *s, BOOL bAutomatic)
{
	s_workers.AddTail(this); // mark unit as worker - checks will be done later
	return TRUE;
}

BOOL CUnit::onProcess_order_reserve(const char *s, BOOL bAutomatic)
{
	CHKUNIT
	BKPTUNIT

	int i,n;
	BOOL bEach=FALSE;
	if (IsTemp()) 
		return _warning(TEMPUNITSCANTRESERVE,5);
	s=getstr2();
	n=atoi(s);
	if (n<1) {
		// EACH
		if (findparam(s)==P_EACH) {
			bEach=TRUE;
			n=get_i();
		}
	}
	if (n<=0) 
		return _warning(RESERVE0SENSELESS,6);
	s=getstr2();
	if (!(*s)) 
		return _error(RESERVEWHAT);
	if (findparam(s)==P_SILVER)	
		return onReserveSilver(n,bEach);
	i=finditem(s);
	if (i>=0) 
		return onReserve(i,n,UT_ITEM,bEach);
	i=findpotion(s);
	if (i>=0) 
		return onReserve(i,n,UT_POTION,bEach);
	i=findherb(s);
	if (i>=0) 
		return onReserve(i,n,UT_HERB,bEach);
	return _error(RESERVEWHAT);
}

BOOL CUnit::onProcess_order_buy(const char *s, BOOL bAutomatic) 
{
	s=getstr2();
	int nAmount=atoi(s);
	if (nAmount<1) {
		if (findparam(s)==P_ALLES)
			return _error(BUYALLNOTPOSSIBLE);
		return _error(NUMLUXURIESMISSING);
	}
	s=getstr();
	int nObjID=finditem(s);
	if (ItemPrice(nObjID)<1)
		return _error(NOLUXURY);
	return onTradeOrder(nObjID,nAmount,TRUE);
}

BOOL CUnit::onProcess_order_sell(const char *s, BOOL bAutomatic) 
{
	s=getstr2();

	int nAmount=atoi(s);
	if (nAmount<1) {
		if (findparam(s)!=P_ALLES)
			return _error(NUMLUXURIESMISSING);
		nAmount = VALUE_ALL;
	} 
	s=getstr();
	int nObjID=finditem(s);
	if (ItemPrice(nObjID)<1)
		return _error(NOLUXURY);
	return onTradeOrder(nObjID,nAmount,FALSE);
}

BOOL CUnit::onProcess_order_use(const char *s, BOOL bAutomatic)
{
	CHKUNIT
	BKPTUNIT

	int nAmount=0;

	s=getstr2();
	if (isdigit(*s)) { 
		// USE amount of "article"
		nAmount = atoi(s);
		if (nAmount == 0) 
			_warning(NUMBER0SENSELESS,2);
		s=getstr2();
	}
	int nObjID=findpotion(s);
	if (nObjID>=0)
		return onUsePotion(nObjID,nAmount,bAutomatic);
	int nIDItem=finditem(s);
	if (nIDItem==K_NOTHING)
		return _error(USEWHAT,s); 
	return onUseSpecialItem(nIDItem,nAmount,bAutomatic);
}

BOOL CUnit::onProcess_order_guard(const char *s, BOOL bAutomatic) 
{
	s=getstr();
	if (findparam(s)==P_NOT) 
		m_bGuarding=FALSE; 
	else
		s_guards.AddTail(this);
	return TRUE;
}

BOOL CUnit::onProcess_order_promotion(const char *s, BOOL bAutomatic) 
{
	m_bPromotion = TRUE;
	s_soonHeroes.AddTail(this);
	return TRUE;
}

BOOL CUnit::ProcessAnOrder(COrder *pOrder, K_Order k_until)
{
	CHKUNIT
	BKPTUNIT

	if (!pOrder)
		return FALSE;
	
	if (pOrder->processed())
		return FALSE;

	if (pOrder->cmd()>k_until)
		return FALSE;

	pOrder->setProcessed(); // marked as processed

	m_sCurrentOrder = pOrder->order();
	m_nCurrentCmd = pOrder->cmd();
	BOOL bAutomatic = (m_sCurrentOrder.GetAt(0)=='@');
	if (isLongOrder(m_nCurrentCmd) && !canDoLongOrder(m_nCurrentCmd,m_sCurrentOrder))
		return FALSE;
	const char *s = igetstr((LPCSTR)m_sCurrentOrder); //extract first word
	switch (m_nCurrentCmd)
	{
		case K_RESERVE: return onProcess_order_reserve(s,bAutomatic); break;
		case K_USE:	return onProcess_order_use(s,bAutomatic); break;
		case K_GIVE: return onProcess_order_give(s,bAutomatic); break;
		case K_CAST: return onProcess_order_cast(s,bAutomatic); break;
		case K_TAX: return onProcess_order_tax(s,bAutomatic); break;
		case K_ENTERTAIN: return onProcess_order_entertain(s,bAutomatic); break;
		case K_BUY: return onProcess_order_buy(s,bAutomatic); break;
		case K_SELL: return onProcess_order_sell(s,bAutomatic); break;
		case K_GUARD: return onProcess_order_guard(s,bAutomatic); break;
		case K_PROMOTION: return onProcess_order_promotion(s,bAutomatic); break;
		default : break;
	}
	return FALSE;
}

BOOL CUnit::isLongOrder(K_Order cmd) const
{
	if (K_CAST==cmd) return TRUE;
	if (K_WORK==cmd) return TRUE;
	if (K_BUY==cmd) return TRUE;
	if (K_SELL==cmd) return TRUE;
	if (K_TAX==cmd) return TRUE;
	if (K_ENTERTAIN==cmd) return TRUE;
	return FALSE;
}

BOOL CUnit::AddOrderBySequence(const char *pOrder, K_Order cmd)
{
	CHKUNIT
	BKPTUNIT

	//TODO: GIVE MEN must be done immediatly (?) for TEMP units management
	switch (cmd)
	{
		//case K_DELIVER:
		case K_RESERVE:
		case K_USE:
		case K_GIVE:
		case K_CAST:
		case K_WORK:
		case K_BUY:
		case K_SELL:
		case K_TAX:
		case K_ENTERTAIN:
		case K_GUARD:
		case K_PROMOTION:
			//if (isLongOrder(cmd)) setLongOrder(pOrder);
		return m_orders.insert(pOrder, cmd); break;
		default: return FALSE;
	}
	return FALSE;
}

void CUnit::ProcessOrdersBySequence(K_Order k_until)
{
	POSITION pos = m_orders.GetHeadPosition();
	BOOL bProcessed = FALSE;
	while (pos)	
		ProcessAnOrder(m_orders.GetNext(pos),k_until);
}

BOOL CUnit::checkMoney(BOOL bCheckCosts)
{
	CHKUNIT
	BKPTUNIT

	int nWarnLevel = 2;
	//if (before_move) nWarnLevel = 6;
	if (notEnoughMoney() && isAlive()) {
		if (bCheckCosts)
		{
			if (maintainCost()) {
				CString sBType("");
				CString sInfo("building");
				if (inBuildingType(sBType))
					sInfo=sBType;
				if (!inUsedBuilding()) {
					nWarnLevel=5;
					sInfo="unused "+sInfo;
				}
				return _warning((CANTMAINTAINBUILDING),nWarnLevel,-money(),sInfo);
			}

			if (learningCost())
			{
				CString s("");
				if (m_pLearntSkill) s+=m_pLearntSkill->name;
				if (m_bInAcademy) s+=" inside the academy";
				return _warning(LEARNINGNEEDSSILVER,nWarnLevel,-money(),(LPCSTR)s);
			}
		}
		return _warning(UNITHASSILVER,10,"",money()); //not enough silver for give....
	}
	return TRUE;
}

BOOL CUnit::checkControlGiven(eID nGivenID, CUnit * pNewController)
{
	CHKUNIT2
	BKPTUNIT

	ASSERT(nGivenID!=ID_UNKNOWN);
	ASSERT(pNewController);
	int nID = ID_UNKNOWN;
	if (m_pShip) nID = m_pShip->id();
	if (m_pBuilding) nID = m_pBuilding->id();
	if (nID==ID_UNKNOWN) 
		return errormsg("checkControlGiven::internal error - this unit is not on a ship nor in a building");
	if (nID!=nGivenID) 
		return errormsg("checkControlGiven::internal error - this unit does not enter into %s object, but into %s",itob(nGivenID),itob(nID));

	//only if old owner has leaved
	if (m_pNewController==NULL) 
		return _warning(MUSTGIVECONTROL,4,pNewController->outputName());
	if (m_pNewController!=pNewController) 
		return _warning(GAVEBADCONTROL,4,m_pNewController->outputName(),pNewController->outputName());
	return TRUE;
}

BOOL CUnit::checkMaintainCostGiven(eID nContainerID, CUnit * pNewController)
{
	CHKUNIT2
	BKPTUNIT

	ASSERT(nContainerID!=ID_UNKNOWN);
	ASSERT(m_pInitialBuilding);
	ASSERT(pNewController);
	int nID = ID_UNKNOWN;
	if (m_pInitialBuilding) nID = m_pInitialBuilding->id();
	if (unknownID(nID)) return errormsg("checkMaintainCostGiven::internal error - unit %s is not on a ship nor in a building",outputName());
	if (nID!=nContainerID) return errormsg("checkMaintainCostGiven::internal error - unit %s does not enter into %s object, but into %s",outputName(),itob(nContainerID),itob(nID));

	if (m_bImplicitLeaving && !m_pNewController)
	{
		m_pNewController=pNewController;
		if (m_money.m_nMaintainCost > 0)
			onCheckGiveBuildingControl(m_pInitialBuilding,ChangedOfContainer());
	}
	if (m_pNewController==NULL) 
		return _warning(MUSTGIVECONTROL,4,pNewController->outputName());
	if (m_pNewController!=pNewController) return _warning(GAVEBADCONTROL,4,m_pNewController->outputName(),pNewController->outputName());
	return TRUE;
}

BOOL CUnit::onOrder_group(const char *sGroup)
{
	CHKUNIT
	BKPTUNIT

	m_sNewGroup=sGroup;
	//extractName(m_sNewGroup);
	if (m_sNewGroup.IsEmpty())
		m_sNewGroup="~to_remove~";
	return TRUE;
}

////////////////////

BOOL CUnit::updateRegionName(CString & sRegionName)
{
	if (!m_sRegionNewName.IsEmpty())
	{
		sRegionName = m_sRegionNewName;
		return TRUE;
	}
	return FALSE;
}

BOOL CUnit::updateContainerName(CString & sContainerName)
{
	if (!m_sContainerNewName.IsEmpty())
	{
		sContainerName = m_sContainerNewName;
		return TRUE;
	}
	return FALSE;
}

BOOL CUnit::updateNewName()
{
	CHKUNIT
	BKPTUNIT


	if (m_sNewName.CompareNoCase(m_sShortName))
	{
		m_sShortName = m_sNewName;
		m_sOutputName.Format("%s (%s)",(LPCSTR)UTF8_to_ansi(m_sShortName),name());
		return TRUE;
	}
	return FALSE;
}

BOOL CUnit::markCrossedRegions()
{
	CString s(outputName());
	fromRegion()->cr_addThroughUnit(s);
	int nNbMoves = m_moveInfos.m_nCount;
	for (int j=1;j<=nNbMoves;j++)
	{
		CPoint p(m_moveInfos.moves[j]);
		CRegion *pRegion = sRegionsList.Add(p.x, p.y, 0);
		if (pRegion) pRegion->cr_addThroughUnit(s);
	}

	return TRUE;
}

BOOL CUnit::nbMenChanged() const
{
	return (m_pMenGiver || IsTemp() || (m_nPeople != m_nInitialPeople)); 
}

BOOL CUnit::payMaintenance(CBuilding *pb)
{
	CHKUNIT
	BKPTUNIT

	if (!isAlive()) 
		return TRUE; 
	if (m_pBuilding!=pb) 
		return errormsg("CUnit::payMaintenance - internal error for %s", outputName());
	if (m_pBuilding->upkeepPaidByMagic()) 
		return TRUE;
	// spend building upkeep cost
	const t_upkeep & t =  pb->upkeep();
	BOOL bCanPayUpkeep = TRUE;
	CString sItem("");
	int n;
	int ns;
	//if (!canUseItem(IT_SILVER, t.silver, n, ns, sItem)) bCanPayUpkeep = FALSE; else
	if (!canUseItem(IT_WOOD, t.woods, n, ns, sItem)) bCanPayUpkeep = FALSE; else
	if (!canUseItem(IT_STONE, t.stones, n, ns, sItem)) bCanPayUpkeep = FALSE; else
	if (!canUseItem(IT_HORSE, t.horses, n, ns, sItem)) bCanPayUpkeep = FALSE;

	if (bCanPayUpkeep)
	{
		//spendItem(IT_SILVER, t.silver);
		spendItem(IT_WOOD, t.woods);
		spendItem(IT_STONE, t.stones);
		spendItem(IT_HORSE, t.horses);

		if (cr().toSave())
			sMessagesList.Add(new CMessage(ICRMSG_PAY, this, NULL, NULL, NULL, NULL, pb));
		return TRUE;
	}
	return FALSE;
}

BOOL CUnit::changedRegion() const 
{	
	return toRegion() && fromRegion()!=toRegion();
}

void CUnit::setCRConfirmedOrders() 
{
	m_bCRConfirmedOrders=TRUE;
	ASSERT(m_pFaction);
	if (m_pFaction)
		m_pFaction->addUnitWithConfirmedOrders();
}


BOOL CUnit::onsave_remove()
{
	if (people()==0 && m_cr_unit)
	{
		// retrieve useful infos in case of unit being a parent
		//cr_combatStatus();
		//cr_isInsideSomething();
		cr().onSave_remove(m_cr_unit);
		m_cr_unit = NULL;
		return TRUE;
	}

	return FALSE;
}

BOOL CUnit::setInitialGuarding()
{
	CHKUNIT
	BKPTUNIT

	m_bInitialGuard = m_bGuarding = TRUE;
	return onRegionChangeGuardsNumber(1);
}

BOOL CUnit::OnDisguise(int nOtherFactionID)
{
	// if (m_nDisguisedFactionID!=ID_UNKNOWN) // pbm
	m_nDisguisedFactionID = nOtherFactionID;
	// TODO: valid faction id ?
	return TRUE;
}

BOOL CUnit::skillLevelChangeOnTerrain(eSkill nSkillID, int & nDiff)
{
	CRegion *pRFrom = fromRegion();
	CRegion *pRTo = toRegion();

	nDiff = 0;

	if (!changedRegion() && !IsRecruited())
		return FALSE;

	if (isInsect())
	{
		// In deserts and swamps, insects get a bonus of +1 on all skills. in mountains, all their skills are reduced by 1.
		if (pRFrom->isTerrain(EN_DESERT) || pRFrom->isTerrain(EN_SWAMP)) nDiff--; else
		if (pRFrom->isTerrain(EN_MOUNTAIN)) nDiff++;
		if (pRTo)
		{
			if (pRTo->isTerrain(EN_DESERT) || pRTo->isTerrain(EN_SWAMP)) nDiff++; else
			if (pRTo->isTerrain(EN_MOUNTAIN)) nDiff--;
		}
	}
	else
	if (isDwarf() && (nSkillID == SK_TACTICS))
	{
		// In mountains and glaciers, dwarves get a bonus of +1 to their tactics skill.
		if (pRFrom->isTerrain(EN_MOUNTAIN) || pRFrom->isGlacier()) 
			nDiff--;
		if (pRTo && (pRTo->isTerrain(EN_MOUNTAIN) || pRTo->isGlacier())) 
			nDiff++;
	}
	else
	if (isElf() && (nSkillID == SK_TACTICS || nSkillID == SK_STEALTH || nSkillID == SK_PERCEPTION))
	{
		// In forest: elves have a bonus of +1 to stealth and perception, and +2 to tactics.
		if (pRFrom->isTerrain(EN_FOREST)) nDiff--;
		if (pRTo && pRTo->isTerrain(EN_FOREST)) nDiff++;
		if (nSkillID == SK_TACTICS) nDiff*=2;
	}
	return (nDiff!=0);
}

BOOL CUnit::modifyAllTalents(int nVariation)
{
	if (nVariation == 0)
		return FALSE;

	block * t = cr_talents();
	if (t)
	{
	
		entry *e = t->entries;
		while (e) {
			int nlvl=e->data.ip[2]+nVariation;
			setTalent(e->tag->name,nlvl,nVariation);
			e = e->next;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CUnit::setTalent(const char *pSkillName, int nNewLvl, int nVariation)
{
	return cr().setTalent(cr_talents(),pSkillName,nNewLvl,TRUE,nVariation);
}

BOOL CUnit::setTalent(eSkill nSkillID, int nNewLvl, int nVariation)
{
	return setTalent(sSkillsEntries[nSkillID],nNewLvl,nVariation);
}

BOOL CUnit::setNewTalent(eSkill nSkillID, int nNewLvl)
{
	return setTalent(nSkillID, nNewLvl, nNewLvl);
}

BOOL CUnit::setNewTalent(const char *pSkillName, int nNewLvl)
{
	return setTalent(pSkillName, nNewLvl, nNewLvl);
}


BOOL CUnit::modifyTalent(eSkill nSkillID, int nVariation)
{
	if (nVariation == 0)
		return FALSE; 
	if (nSkillID==SK_MAX)
		return modifyAllTalents(nVariation); // for insects only
	int nLvl = level(nSkillID);
	int nNewLvl = nLvl+nVariation;
	if (nLvl <= 0) return FALSE; // to be increased, must have at least current skill=1
	return setTalent(nSkillID,nNewLvl,nVariation);
}

BOOL CUnit::save()
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	
	BOOL bNew = IsTemp();
	int nbMen = people();
	BOOL bIsAlive = isAlive();
	BOOL bAccessibleSettings = (bIsAlive || (cr_combatStatus()!=COMBATSTATUS_UNKNOWN));
	BOOL bDbg=FALSE;

	//create unit & attach to region
	if (bNew)
	{
		if (nbMen == 0) 
			return FALSE;
		if (isUnknownUnit()) 
			return FALSE;
		m_nID = newTempID(regionTo(), "unit", m_sShortName);
		m_cr_unit = regionTo()->cr_addNewUnit(m_nID, NULL);
		m_bCombatStatusChanged = TRUE;
	}

	if (isUnknownUnit())
		return FALSE;

	if (!m_cr_unit)
	{
		if (bIsAlive)
			return errormsg("CUnit::save - no cr unit block for %s",outputName());
		return TRUE;
	}

	if (bNew)
	{
		// TEMP UNITS
		if (m_pFaction)
		{
			cr().seti(m_cr_unit,CR_PLAYER,m_pFaction->id());
			cr().sets(m_cr_unit,CR_TYPE,CCR::English2German(m_pFaction->raceName()));
		}

		// add talents (same as men giver)
		if (IsRecruited())
		{
			// RECRUITED TEMP UNITS

			m_cr_talents = cr().onSave_addBlock(m_cr_unit, NULL, DE_TALENTS, 0);

			eSkill nLearntSkill = m_eLearntSkill;
			if (m_bLearning) 
				nLearntSkill = skillnameToIndex(CCR::English2German(m_pLearntSkill->name,TRUE));
	
			if (isOrc())
			{
				//+1 when learning & taught
				int nlvlPolearm = 1;
				int nlvlMelee = 1;
				if (m_bTaught && nLearntSkill==SK_POLEARM) nlvlPolearm++;
				if (m_bTaught && nLearntSkill==SK_MELEE) nlvlMelee++;
				modifyTalent(SK_POLEARM,nlvlPolearm);
				modifyTalent(SK_MELEE,nlvlMelee);
			}

			if (m_bLearning && bIsAlive)
			{
				if (!isOrc() || (nLearntSkill!=SK_POLEARM && nLearntSkill!=SK_MELEE))
				{
					int nnsk = learntSkillLevel(TRUE);
					if (m_bTaught) nnsk++;
					setNewTalent(nLearntSkill,nnsk);
				}
			}
		}
		else
		if (m_pMenGiver)
		{
			block * t = m_pMenGiver->cr_talents();
			BOOL bLearnSkillFound = FALSE;
			CString sLearntSkillname("");
			if (bIsAlive && m_bLearning)
				sLearntSkillname=CCR::English2German(m_pLearntSkill->name,TRUE);
			if (t)
			{
				m_cr_talents = cr().onSave_addBlock(m_cr_unit, NULL, DE_TALENTS, 0);
				entry *e = t->entries;
				while (e) {
					int nlvl=e->data.ip[2];
					int nlvlAfter=nlvl;
					BOOL bLearntSkill = !sLearntSkillname.IsEmpty() && !sLearntSkillname.CompareNoCase(e->tag->name);
					if (bLearntSkill) 
					{
						bLearnSkillFound = TRUE;
						nlvlAfter = learntSkillLevel(TRUE);
						if (m_bTaught) nlvlAfter++;
					}
					setTalent(e->tag->name,nlvl,nlvlAfter);
					e = e->next;
				}
				if (m_bLearning && !bLearnSkillFound)
				{
					int	nlvl = learntSkillLevel(TRUE);
					if (m_bTaught) nlvl++;
					setNewTalent(sLearntSkillname,nlvl);
				}
			}
		}
	}
	else
		cr().onSave_removeEntry(m_cr_unit,CR_WEIGHT); // or set current weight

	if (nbMen == 0) {
		s_unitsToRemove.AddTail(this);
		return TRUE;
	}

	BOOL bNameToUpdate=bNew;
	CString sPrevShortName = m_sShortName;
	if (updateNewName())
		bNameToUpdate=TRUE;
	else
	{
		CString sID(itob(m_nID));
		m_sNewName = "Unit "+sID+"("+sID+")"; // NAME not specified
	}
	if (bNameToUpdate)
		cr().sets(m_cr_unit,CR_NAME,m_sNewName);

	if (!m_sNewGroup.IsEmpty())
	{
		if (!m_sNewGroup.CompareNoCase("~to_remove~"))
			cr().onSave_removeEntry(m_cr_unit,CR_GROUP_ID);
		else
		{
			int nGroupID=-1;
			// search group in PARTEI block
			block * bf = NULL;
			block * bfg = NULL;
			CFaction * f = faction();
			if (f)
			{
				bf = f->cr_groups();
				block * bfn = bf;
				while (bfn)
				{
					CString sGroupName("");
					cr().gets(bfn,"name",sGroupName);
					if (!sGroupName.CompareNoCase(m_sNewGroup))
					{
						bfg=bfn;
						nGroupID=bfn->ids[0];
						break;
					}
					bfn=bfn->next;
				}
			}
			if (nGroupID==-1)
			{
				nGroupID=newID();

				ASSERT(bf!=NULL);
				if (bf)
				{
					bfg=cr().onSave_addBlock(NULL,bf,CR_GROUP,newID());
					if (bfg)
						cr().sets(bfg,"name",m_sNewGroup);
				}
			}

			ASSERT(bfg!=NULL);

			if (bfg)
				cr().seti(m_cr_unit,CR_GROUP_ID,nGroupID);
		}
	}
	for (int k = 0;k<EFT_MAX;k++) {
		if (m_bSpendEffects[k])
			modifyEffect(sEffectNames[k],m_nEffects[k]);
	}

	// TODO: for illusion & camouflaged, should save:
	// - silverbag instead of silver
	// - herbbag instead of herbs
	// - vial instead of potions

	// save items possession
	TItemInfo * pItem = m_items;
	while (pItem) 
	{
		int nb = pItem->number();
		if (!cr().setItemsNumber(m_cr_unit, pItem->m_sName,nb) && pItem->initialNumber()>0)
			errormsg("error saving %s items number for unit %s",pItem->m_sName,outputName());
		pItem = pItem->m_pNext;
	}

	// resolve leave orders
	CString sLeavingContainer("");

	if (m_pInitialShip && !m_pShip) sLeavingContainer = CR_SHIP; else
	if (m_pInitialBuilding && !m_pBuilding) sLeavingContainer=CR_CASTLE;

	if (!m_bLeaving)
	{
		// implicit leave by entering somewhere else
		if (m_pInitialShip && m_pBuilding) sLeavingContainer = CR_SHIP; else
		if (m_pInitialBuilding && m_pShip)	sLeavingContainer = CR_CASTLE; else
		if (changedRegion())
		{
			if (m_pInitialBuilding && (m_pInitialBuilding==m_pBuilding)) sLeavingContainer = CR_CASTLE;
		}
	}

	if (!sLeavingContainer.IsEmpty())
		cr().onSave_removeEntry(m_cr_unit,sLeavingContainer);

	// resolve enter orders
	if (m_pShip && (m_pShip!=m_pInitialShip || bNew))	cr().seti(m_cr_unit,CR_SHIP,m_pShip->id());
	if (m_pBuilding && (m_pBuilding!=m_pInitialBuilding || bNew))	cr().seti(m_cr_unit,CR_CASTLE,m_pBuilding->id());

	// combat status update
	if (m_bCombatStatusChanged)
		cr().seti(m_cr_unit,CR_COMBATSTATUS,cr_combatStatus());

	if (m_bPromotion)
		cr().seti(m_cr_unit,CR_HERO,1);

	block *bt = cr_talents();
	if (bIsAlive && !bNew)
	{
		cr().removeTalentsDiff(m_cr_unit,bt);

		if (changedRegion())
		{

			int nAllSkillsDiff = 0;
			int nTacticsSkillDiff = 0;
			int nStealthSkillDiff = 0;
			int nPerceptionSkillDiff = 0;
			if (isInsect())
				skillLevelChangeOnTerrain(SK_MAX, nAllSkillsDiff);
			else 
			if (isDwarf())
				skillLevelChangeOnTerrain(SK_TACTICS, nTacticsSkillDiff);
			else 
			if (isElf())
			{
				skillLevelChangeOnTerrain(SK_TACTICS, nTacticsSkillDiff);
				skillLevelChangeOnTerrain(SK_PERCEPTION, nPerceptionSkillDiff);
				skillLevelChangeOnTerrain(SK_STEALTH, nStealthSkillDiff);
			}

			// change talent values
			modifyTalent(SK_TACTICS,nTacticsSkillDiff);
			modifyTalent(SK_PERCEPTION,nPerceptionSkillDiff);
			modifyTalent(SK_STEALTH,nStealthSkillDiff);			
			modifyTalent(SK_MAX,nAllSkillsDiff);
		}
	}

	if (m_nDisguisedFactionID!=ID_UNKNOWN)
	{
		if (m_pFaction && m_pFaction->id()==m_nDisguisedFactionID)
		{
			// undisguise
			cr().onSave_removeEntry(m_cr_unit,CR_DISGUISED);

		}
		else
		{
			//disguise
			cr().seti(m_cr_unit,CR_DISGUISED,m_nDisguisedFactionID);

		}
	}

	if (bIsAlive && !bNew && m_bLearning)
	{
		// inc skill level for taught men
		int nnsprev = learntSkillLevel(FALSE);
		int nnsk = learntSkillLevel(TRUE);
		if (m_bTaught) nnsk++;
		if (nnsk > nnsprev && nnsk>0)
		{
		
			int nns[3]={0, nnsk, nnsk-nnsprev};
			size_t nSize=0;
			const int * pp = NULL;
			CString sSkillname(CCR::English2German(m_pLearntSkill->name,TRUE));
			if (cr().getints(m_cr_talents,sSkillname,&pp,&nSize) && pp)
			{
				nns[0]=pp[0];
				nns[1]=pp[1];
				if (m_bTaught) (nns[1])++;
			}
			cr().setints(m_cr_talents,sSkillname,nns,3);
		}
	}	

	// reset commands
	block * bcmd = cr().cmds(m_cr_unit);
	//CString sUnitInfo(outputName());
	if (bcmd)
	{
		entry * e = bcmd->entries;
		entry * eprev = NULL;
		BOOL bMadeTemp = FALSE;
		while (e) {
			BOOL bModified = FALSE;
			CString s = e->data.cp;
			if (!bMadeTemp && isKeepingOrder(s,bModified,bMadeTemp))
			{
				if (bModified)
				{
					free(e->data.cp);
					e->data.cp = (char*)malloc(1+strlen(s)); 
					strcpy_s(e->data.cp, 1+strlen(s), s);
				}
				eprev = e;
				e = e->next;
			}
			else
			{
				entry * enext = e->next;
				if (eprev) eprev->next=enext; else bcmd->entries=enext;
				free(e->data.cp);
				free(e);
				e = enext;
			}
		}
	}
	else
	{
		if (bNew)
		{
			bcmd = cr().onSave_addBlock(m_cr_unit, NULL, CR_CMDS, 0);
			entry * e = bcmd->entries;
			// CG TODO: not only the long order but all needed orders (@ orders)
			CString s (longOrder());
			BOOL bm1, bm2;
			if (!isKeepingOrder(s,bm1,bm2))
				s="WORK";
			if (!s.IsEmpty()) s+= " ; TMP"; else s="// TMP";
			if (m_pRecruiter) s=s+" recruit"; else
			if (m_pMenGiver) s=s+" made";
			/*
			if (!s.IsEmpty()) s+= " ; temp"; else s="// TEMP";
			if (m_pRecruiter) s=s+" recruited by "+ m_pRecruiter->outputName(); else
			if (m_pMenGiver) s=s+" from "+ m_pMenGiver->outputName();
			*/
			cr().sets(bcmd,NULL,s);
			POSITION pos = m_orders.GetHeadPosition();
			while (pos)	
			{
				COrder *pOrder = m_orders.GetNext(pos);
				s=pOrder->order();
				if (isKeepingOrder(s,bm1,bm2))
					cr().sets(bcmd,NULL,s);
			}
		}
		// !alive or extra orders units	
	}

	//TODO: check luxuries

	//TODO: NAME SHIP...

	if (!bNew && m_bInitialGuard)
	{
		// remove guarding feature if guarding status reseted
		if (!m_bGuarding || nbWeapons() == 0)	
			cr().onSave_removeEntry(m_cr_unit,CR_GUARDED); // cannot guard anymore
	}

	// new guards
	if (m_bGuarding && !m_bInitialGuard) cr().seti(m_cr_unit,CR_GUARDED,1); 

	if (changedRegion())
	{
		// has moved
		if (!IsOnShip())
			markCrossedRegions();

		if (!bNew)
			cr().onSave_move(m_cr_unit, fromRegion()->cr_region(), toRegion()->cr_region());
	}

	
	// update nbmen
	if (nbMenChanged()) 
		cr().seti(m_cr_unit,CR_NBMEN,nbMen);

	cr().seti(m_cr_unit,CR_ORDERSCONFIRMED,HasToConfirmOrders()?1:0);

	// save silvers
	if (bAccessibleSettings)
	{
		int nSilver= max(0,m_money.m_nMoney);
		// TODO: take into account : upkeepCost()
		if (m_money.m_nInitialMoney != nSilver)
			cr().setItemsNumber(m_cr_unit, EN_SILVER, nSilver);
	}
	else
	{
		// TODO: save as silverbag if money given and more than xxx
	}

	// temp id management for later checkorders actions
	if (bNew)
	{
		CString sTurn("");
		CString sKeyName("");
		sTurn.Format("%d",cr().turn());
		sKeyName.Format("%s.unit.%s",regionTo()->outputName(),(LPCSTR)sPrevShortName);
		 ::WritePrivateProfileString(sTurn,(LPCSTR)sKeyName,itob(m_nID),".\\eressea_tmp.ini");
		 ::WritePrivateProfileString(sTurn,(LPCSTR)sKeyName,itob(m_nID),".\\eressea_ngm.ini");
		//if (bNew) output("%15s:%10s saved as: %s\n",regionTo()->outputName(),(LPCSTR)sPrevShortName,outputName());
	}

	return TRUE;
}

////////////////////

BOOL CUnit::HasToConfirmOrders() const
{
	if (!m_bCRConfirmedOrders) 
		return FALSE;
	if (!m_pFaction) 
		return FALSE;
	return m_pFaction->outConfirm();
}

BOOL CUnit::OnOrdersEnded() 
{
	// ALL orders until GIVE (included) are processed now because GIVE MEN have to be resolved
	ProcessOrdersBySequence(K_GIVE);
	return TRUE;
}

void CUnit::checkings()
{
	ERTRACE("CUnit::checkings...");

	s_bCheckings = TRUE;

	before_checkings();

	checkAttackers();

	//checkGivers();
	checkControlGivers();
	sShipsList.resolveOwners();

	//checkPotentialAddedWeights();
	checkMovers(TRUE);

	remindMakers();

	checkRecruited();

	checkTeachers();
	checkLearners();

	sBuildingsList.resolveOwners();

	spendCosts();

	checkCosts();
	
	checkTaxers();
	checkEntertainers();
	checkTraders();
	checkWorkers();

	checkMakers();

	//checkMovesOnLand();
	
	checkTransported();
	checkCarriers();
	checkHeroes();

	check_money(TRUE);

	checkPotentialReceivers();

	// after move
	checkMovers(FALSE);

	checkGuards();

	sShipsList.checkings();
	sBuildingsList.checkings();

	check_money(FALSE);	
	check_units();

	sRegionsList.onCheckAll();

	ERTRACE("CUnit::checkings - end");
}

////////////////////

int CUnit::cr_familiarMageID()
{
	if (m_nFamiliarMageID==UFT_UNKNOWN_VALUE)
		cr().geti(cr_unit(),CR_FAMILIARMAGE,m_nFamiliarMageID);
	return (m_nFamiliarMageID);
}

BOOL CUnit::isPet()
{
	return (cr_familiarMageID()!=0);
}

BOOL CUnit::OnCastOrder(t_spell *sp, int reallycast, int level, int x_region, int y_region, BOOL bRegion)
{
	CHKUNIT
	BKPTUNIT

	CHKUNIT2
	BKPTUNIT

	// TODO: check syntax (region...)
	BOOL bOK = TRUE;
	int nUTID = K_NOTHING;
	CString sMadeItem("");
	int nbAuras=cr_aura();
	if (sp->cost.a+sp->cost.al*level+sp->cost.pa>nbAuras)
		return _error(NOTENOUGHTOCAST,"aura",sp->name);

	if (sp->cost.silver>0 && sp->cost.silver>money())
		bOK = _error(UNITCASTNEEDITEM,sp->name,sp->cost.silver-money(),EN_SILVER);

	// parse and check whether the unit has each of the needed items to cast the spell
	if (sp->cost.other_items)
	{
		CString sItem("");
		int i = 0;
		int nb = 1;
		char *nexttoken;
		char *s=sp->cost.other_items;
		char seps[]=SPELL_SEPARATOR_NEEDED_ITEMS;
		char *p = strtok_s(s,seps,&nexttoken);
		while (p)
		{
			sItem=(const char *)p;
			int nInd = sItem.Find(':');
			if (nInd!=-1) 
			{
				CString sNumber(sItem.Mid(nInd+1).MakeUpper());
				int nbi = atoi(sNumber);
				if (nbi!=0) nb=nbi;
				if (sNumber.GetAt(sNumber.GetLength()-1)=='L')
					nb*=level;
				sItem = sItem.Left(nInd);
			}
			sItem.TrimLeft();
			sItem.TrimRight();

			// TODO: check peasant on region in case of some peasants needed
			if (!sItem.IsEmpty() && sItem.CompareNoCase(EN_PEASANT))
			{
				int nNeededItemID=K_NOTHING;
				int nNeededItemUTID=K_NOTHING;
				int nbHaving=0;
				if (itemKind(sItem, nNeededItemID, nNeededItemUTID))
					nbHaving = nbElements(nNeededItemID,sItem,nNeededItemUTID);
				if (nbHaving < nb)
					bOK = _error(UNITCASTNEEDITEM,sp->name,nb-nbHaving,sItem);
			}
			p = strtok_s(NULL,seps,&nexttoken);
		}
	}

	if (!bOK)
		return FALSE;

	if (!_strcmpi(sp->name,EN_SPELL_SUMMON_STORM_ELEMENTAL))
	{
		int shipsID[20];
		int nbShips=getmoreShips(shipsID, 20);
		CShip * pShip=NULL;
		for (int i=0;i<nbShips;i++)
		{
			int nShipID=shipsID[i];
			if (invalidID(nShipID)) 
				_error(UNKNOWNSHIP,"<unknown>");
			else
			if (!sShipsList.ship(pShip,regionFrom(),nShipID)) 
				_error(UNKNOWNSHIP,itob(nShipID));
			else
				pShip->SetSpeedUpByMagicSummonStormElemental();
		}
	}

	if (!_strcmpi(sp->name,EN_SPELL_CREATE_AN_AMULET_OF_TRUE_SIGHT)) sMadeItem=EN_AMULET_OF_TRUE_SEEING; else
	if (!_strcmpi(sp->name,EN_CREATE_A_RING_OF_INVISIBILITY)) sMadeItem=EN_RING_OF_INVISIBILITY; else
	if (!_strcmpi(sp->name,EN_SPELL_QUICK_FINGERS)) sMadeItem=EN_RING_OF_QUICK_FINGERS; else
	if (!_strcmpi(sp->name,EN_SPELL_CREATE_A_SPHERE_OF_INVISIBILITY)) sMadeItem=EN_SPHERE_OF_INVISIBILITY; else
	if (!_strcmpi(sp->name,EN_SPELL_CREATE_A_MAGICAL_HERB_POUCH)) sMadeItem=EN_BAG_OF_CONSERVATION; else
	if (!_strcmpi(sp->name,EN_SPELL_CREATE_AN_ANTIMAGIC_CRYSTAL)) sMadeItem=EN_ANTIMAGIC_CRYSTAL; else
	if (!_strcmpi(sp->name,EN_SPELL_CREATE_A_RING_OF_POWER)) sMadeItem=EN_RING_OF_POWER; else
	if (!_strcmpi(sp->name,EN_SPELL_CREATE_A_BAG_OF_HOLDING)) sMadeItem=EN_MAGIC_BAG; else
	if (!_strcmpi(sp->name,EN_SPELL_CREATE_A_BELT_OF_TROLL_STRENGTH)) sMadeItem=EN_TROLLBELT; else
	if (!_strcmpi(sp->name,EN_SPELL_CREATE_A_FLAMESWORD)) sMadeItem=EN_FLAMING_SWORD; else
	if (!_strcmpi(sp->name,EN_SPELL_BLESS_STONE_CIRCLE)) {;} else
	if (!_strcmpi(sp->name,EN_SPELL_SUMMON_FAMILIAR)) {;} else
	if (!_strcmpi(sp->name,EN_SPELL_BIND_FAMILIAR)) {;} else
	if (!_strcmpi(sp->name,EN_SPELL_CALL_FAMILIAR)) {;} else
	{;}

	if (!sMadeItem.IsEmpty())
	{
		nUTID = finditem(sMadeItem);
		if (nUTID!=K_NOTHING)
		{
			TItemInfo * pit = addItemInfo(nUTID, sMadeItem, UT_ITEM);
			if (pit)
				pit->addMade(1,regionFrom());
		}
	}

	spell |= sp->type*reallycast;
	spendMoney(sp->cost.silver,TRUE);
	return TRUE;
}

void CUnit::magicSchool(CString & sMagicSchool)
{
	sMagicSchool="";
	CFaction *pf = faction();
	if (pf)
		pf->magicSchool(sMagicSchool);
}

int CUnit::getmoreShips(int *shipsID, int nbmax) {
	const char *s;
	int count=0;

	for (count=0;count<nbmax;) {
		s=getstr();
		if (!(*s))
			break;
		shipsID[count]=btoi(s);
		count++;
	}
	return count;
}

int CUnit::studycost(const t_skills *talent) {
	if (!talent) 
		return errormsg("studycost:: talent is NULL");
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

BOOL CUnit::isInsect()		const { return (m_pFaction?m_pFaction->isRace(EN_INSECTS):FALSE);}
BOOL CUnit::isOrc()			const { return (m_pFaction?m_pFaction->isRace(EN_ORCS):FALSE);}
BOOL CUnit::isAquarian()	const { return (m_pFaction?m_pFaction->isRace(EN_AQUARIANS):FALSE);}
BOOL CUnit::isCat()			const { return (m_pFaction?m_pFaction->isRace(EN_CATS):FALSE);}
BOOL CUnit::isHuman()		const { return (m_pFaction?m_pFaction->isRace(EN_HUMANS):FALSE);}
BOOL CUnit::isElf()			const { return (m_pFaction?m_pFaction->isRace(EN_ELVES):FALSE);}
BOOL CUnit::isDwarf()		const { return (m_pFaction?m_pFaction->isRace(EN_DWARVES):FALSE);}
BOOL CUnit::isTroll()		const { return (m_pFaction?m_pFaction->isRace(EN_TROLLS):FALSE);}
BOOL CUnit::isDemon()		const { return (m_pFaction?m_pFaction->isRace(EN_DEMONS):FALSE);}

BOOL CUnit::isRace(const char *pRaceName)	const { return (m_pFaction?m_pFaction->isRace(pRaceName):FALSE);}

