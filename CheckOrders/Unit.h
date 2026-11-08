// Unit.h: interface for the CUnit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNIT_H__BE85D52A_BBC6_46D8_A384_7A5197A4DE9D__INCLUDED_)
#define AFX_UNIT_H__BE85D52A_BBC6_46D8_A384_7A5197A4DE9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wLizard.h"
#include "SortableObList.h"
#include "Echecktypes.h"
#include "CR.h"
#include "CRTypes.h"
#include "Faction.h"
#include "Order.h"
#include "Orders.h"
#include "CRUnit.h"

class CFaction;
class CBuilding;
class CRoad;
class CShip;
class CRegion;
class CUnits;

struct TTeachInfo;

//////////

#define ALIVE_USER_UNIT			(1)
#define ALIVE_UNKNOWN			(0)
#define ALIVE_ILLUSION			(-1)

#define UNIT_STATUS_NOTTEMP		(0)
#define UNIT_STATUS_TEMP		(42)

#define MOVESTATUS_CARRIED		(-1)
#define MOVESTATUS_NOMOVE		(0)
#define MOVESTATUS_SOMEWHERE	(1)
#define MOVESTATUS_BYHIMSELF	(2)

#define LEVEL_UNKNOWN			(VALUE_UNKNOWN)
#define EFFECT_UNKNOWN			(-9999)

#define WEAPONTYPE_MELEE		(1)
#define WEAPONTYPE_POLEARM		(2)
#define WEAPONTYPE_CROSSBOW		(4)
#define WEAPONTYPE_BOW			(8)
#define WEAPONTYPE_CATAPULT		(16)

#define ITEMID_NONE				(-1)
#define UFT_UNKNOWN_VALUE		(-1)

////////////

class CUnit;

typedef struct TMakeOrder
{
	PParam m_nParam;
	int m_nSize;
	int m_nUTType;
	int m_nUTID;
	eID m_nID;

	TMakeOrder():m_nSize(VALUE_UNKNOWN),m_nUTType(UTTYPE_UNKNOWN),m_nUTID(IDUT_UNKNOWN),m_nID(ID_UNKNOWN){}

} TMakeOrder;

////////////

typedef struct TUTItemInfo
{ 
	CString  m_sName;
	int		 m_nUTID;
	eSkill	 m_nSkillID;
	t_item * m_infos;
	TUTItemInfo(const char * pName,eSkill nSkillID=(eSkill)-1):m_sName(pName),m_nSkillID(nSkillID),m_nUTID(-1),m_infos(NULL){}
} TUTItemInfo;

typedef TUTItemInfo TUTItemInfoTab[];

///////////

typedef struct TTradeItem
{
	int		m_nItemID;
	int		m_nNb;
	int		m_nPrice;
	CString m_sName;
	TTradeItem * m_pNext;

	TTradeItem::TTradeItem(int nID, int nb, const char *pName, int nPrice, TTradeItem ** pNext) {
		m_nItemID = nID;
		m_nNb = nb;
		m_sName = pName;
		m_nPrice = nPrice;
		if (pNext)
		{
			m_pNext = *pNext;
			*pNext=this; // always as first item
		}
	}

	int cost(int b)
	{
		int p = -m_nPrice;
		int t = m_nNb;
		if (p==0) m_nNb=0;
		if (b==0) m_nNb=0;
		if (m_nNb==0) return 0;
		// pbm (probably round)
		int a = t/b;
		int k=(p*(a+1)*(2*t-a*b))/2;
		return k;
	}

	BOOL setCost(int b, int & silver)
	{
		// TODO: use a formula instead (faster)
		int k=cost(b);
		if (silver<=0) m_nNb=0;
		while (m_nNb > 0 && k > silver)
		{
			m_nNb--;
			k = cost(b);
		}
		silver = k;
		return m_nNb > 0;
	}

} TTradeItem;

///////////

class CItemReceivedInfo : public  CObject
{
private:
	CUnit * m_pGiver;
	int m_nNb;
	CString m_sName;
	int m_nID;
	int m_nType;

public:
	CItemReceivedInfo(int iItem, const char * pName, int nType, int nNb, CUnit * pGiver);
	virtual ~CItemReceivedInfo(){}

	CUnit *u() { return m_pGiver;}
	int nb() const { return m_nNb;}
	const char *name() const { return m_sName;}
	int itemID() const { return m_nID;}
	int itemType() const { return m_nType;}
} ;

DECLARE_LIST(CItemReceivedInfo);

typedef struct TItemInfo {

	void addReceived(int n, BOOL bByAttack=FALSE) {if (bByAttack) m_nNbWon+=n;else m_nNbReceived+=n;}
	void addMade(int n, CRegion *pRegion=NULL);

	void spend(int n) {if (n > 0) {m_nNbSpent+=n; dec(n);}}
	void give(int n) {if (n > 0) {m_nNbGiven+=n; dec(n);}}
	void reserve(int n) {if (n > 0) {m_nNbReserved+=n; dec(n);}}

	int initialNumber() const { return m_nNbHaving+m_nNbGiven+m_nNbSpent+m_nNbReserved;}
	int number(BOOL bWithMade=TRUE) const{ return m_nNbReserved+m_nNbHaving+m_nNbReceived+m_nNbWon+(bWithMade?m_nNbMade:0);}
	int totalWeight() const { return m_nWeight*number();}
	TItemInfo(int id, const char *pName, int nNbHaving, int nWeight, int nType);
	
	int nbGiven() const { return m_nNbGiven;}
	int nbMade() const { return m_nNbMade;}
	int nbReserved() const { return m_nNbReserved;}
	int nbLeft() const { return m_nNbHaving;}
	int nbWon() const { return m_nNbWon;}
	int nbSpent() const { return m_nNbSpent;}
	int basicWeight() const { return m_nWeight;}

	TItemInfo *m_pNext;
	CString m_sName;
	int m_nID;
	int m_nType;
	CUnit * m_pAllReceiver;	

private:
	int m_nWeight;
	int m_nNbHaving;
	int m_nNbGiven;
	int m_nNbSpent;
	int m_nNbReserved;
	int m_nNbReceived;
	int m_nNbWon;
	int m_nNbMade;

	int dec(int n) {m_nNbHaving-=n; /*m_nNbHaving=max(0,m_nNbHaving);*/ return m_nNbHaving;}
	
} TItemInfo;

///////////

class CFaction;
//class CShip;
//class CBuilding;
//class CUnits;
//class CRoad;

struct block;
//struct TTeachInfo;

#define ILRN_SKILL_BEFORE (0)
#define ILRN_SKILL_AFTER  (1)

#define GETAUNIT_FAILURE		(0)
#define GETAUNIT_ID				(1)
#define GETAUNIT_THROWAWAY		(2)
#define GETAUNIT_TEMP			(3)
#define GETAUNIT_ANOTHERUNIT	(42)

///////////

class CUnit : public CObject 
{
public:
	CUnit(CFaction *pFaction, eID nID, int nTempStatus, int xRegion, int yRegion, const char *sOrder, int nbPeople=0);
	CUnit(block * crUnit, eID nID, CRegion * pRegion);

	static BOOL s_bShow_lbs_details;

	virtual ~CUnit();

	BOOL isUnknownUnit();

	BOOL isUnknown() const { return m_bUnknown;}
	BOOL OnEnter(int nType, int nID, BOOL bAsOwner = FALSE);
	
	BOOL OnLeave(BOOL bOrderSpecified = FALSE);
	BOOL OnMove();
	BOOL OnShipMoved(const CShip * pShip, CRegion *pRegionTo);
	BOOL OnMoveOrder(const TMoveInfo & moveinfos);

	BOOL OnCarriedBy(CUnit *pCarrier);
	BOOL OnRideOrder(CUnit *pRider);

	BOOL OnCastOrder(t_spell *sp, int reallycast, int level, int x_region, int y_region, BOOL bRegion);

	///////////////////
	// money management

	BOOL setStartOfOrder(const char *pOrder);

	BOOL payMaintenance(CBuilding *pb);

	BOOL SpendMaintainCost();
	BOOL SpendLearningCost();

	BOOL SpendUpkeepCost(int & nNeededMoney);

	void setInitialMoney(int nVariation); 
	void addMoney(int nVariation, BOOL bFromReservedAlso=FALSE);
	BOOL spendMoney(int nVariation, BOOL bFromReservedAlso=FALSE);
	BOOL onReserveSilver(int n, BOOL bEach);
	BOOL onReserve(int nThing, int n, int nUTType, BOOL bEach);

	void emptyMoney();
	void empty();

	void feed();

	BOOL setInitialNbPeople(int n); 

	BOOL setFaction(CFaction *pFaction);

	void setMaintainCost(int n);
	void setMoney(int n);

	BOOL notEnoughMoney() const;
	BOOL moneyToRegion(BOOL bConsumeUpkeep) const;
	BOOL consumeUpkeep() const;

	BOOL onOrder_group(const char *sGroup);

	int maintainCost() const { return m_money.m_nMaintainCost;}
	int learningCost() const { return m_money.m_nLearningCost;}
	int money() const;
	int reservedSilver() const;

	BOOL Entering() const { return m_bEntering;}

	BOOL onTaxOrder(int nAmount);
	BOOL onEntertainOrder(int nAmount);
	BOOL onTradeOrder(int nItemID, int nb, BOOL bBuy);

	BOOL checkControlGiven(eID nID, CUnit * pNewController);
	BOOL checkMaintainCostGiven(eID nContainerID, CUnit * pNewController);
	BOOL feedDemon();

	///////////////////
	
	CFaction * faction() const { return m_pFaction;}

	void reset();

	void SetAliveStatus(int n);

	BOOL isAlive() const { return m_nLives >= ALIVE_USER_UNIT;}
	BOOL hasOrder() const {return !m_sOrder.IsEmpty();}
	BOOL hasStartOrder() const {return !m_sStartOfOrders.IsEmpty();}

	BOOL isInsect()		const;// { return (m_pFaction?m_pFaction->isRace(EN_INSECTS):FALSE);}
	BOOL isOrc()		const;// { return (m_pFaction?m_pFaction->isRace(EN_ORCS):FALSE);}
	BOOL isAquarian()	const;// { return (m_pFaction?m_pFaction->isRace(EN_AQUARIANS):FALSE);}
	BOOL isCat()		const;// { return (m_pFaction?m_pFaction->isRace(EN_CATS):FALSE);}
	BOOL isHuman()		const;// { return (m_pFaction?m_pFaction->isRace(EN_HUMANS):FALSE);}
	BOOL isElf()		const;// { return (m_pFaction?m_pFaction->isRace(EN_ELVES):FALSE);}
	BOOL isDwarf()		const;// { return (m_pFaction?m_pFaction->isRace(EN_DWARVES):FALSE);}
	BOOL isTroll()		const;// { return (m_pFaction?m_pFaction->isRace(EN_TROLLS):FALSE);}
	BOOL isDemon()		const;// { return (m_pFaction?m_pFaction->isRace(EN_DEMONS):FALSE);}

	BOOL isRace(const char *pRaceName)	const;// { return (m_pFaction?m_pFaction->isRace(pRaceName):FALSE);}

	BOOL notEnoughPeople() const { return m_nPeople<0;}
	BOOL HasLongOrder() const { return !m_sLongOrder.IsEmpty();}
	BOOL HasMen() const { return m_nPeople > 0;}
	BOOL HasMoved() const { return m_nMoveStatus != MOVESTATUS_NOMOVE;}
	BOOL HasMovedSomewhere() const { return m_nMoveStatus >= MOVESTATUS_SOMEWHERE;}
	BOOL HasMovedByHimself() const { return m_nMoveStatus == MOVESTATUS_BYHIMSELF;}
	void SetMoveStatus(int nStatus);
	void SetTempStatus(int nTemp);
	BOOL IsTemp() const {return m_nTemp != UNIT_STATUS_NOTTEMP;}
	BOOL IsMadeTemp() const {return abs(m_nTemp) == UNIT_STATUS_TEMP;}
	BOOL IsRecruited() const { return m_pRecruiter!=NULL;}

	int MoveStatus() const { return m_nMoveStatus;}
	int TempStatus() const {return m_nTemp;}
	void RemoveTempStatus(CFaction *pFaction);

	void setMother(CUnit *pMother);

	///////////

	BOOL updateContainerName(CString & sContainerName);
	BOOL updateRegionName(CString & sRegionName);

	BOOL save();

	BOOL OnRename(int nType, const char *pname);

	const char * regionName(BOOL bFirstRegion=TRUE); 
	const char * regionOutputName(BOOL bFirstRegion=TRUE);

	const char * longOrder() const { return (LPCSTR)m_sLongOrder; }

	CRegion * regionFrom() const { return region(TRUE);}
	CRegion * regionTo() const;

	BOOL setLongOrder(const char *sLongOrder);

	BOOL fromOcean() const;
	int people() const {return m_nPeople;}

	int id() const {return m_nID;}

	int x() const { return m_nNewX;}
	int y() const { return m_nNewY;}

	void CopyTo(CUnit *to) const;

	int nbMenToTeach() const { return m_nNbMenToTeach;}

	BOOL IsCaptain() const;
	BOOL IsOnShip(eID nID=ID_UNKNOWN) const;
	BOOL IsInBuilding(eID nID=ID_UNKNOWN) const;

	BOOL isAllied() const;

	boolean isOnRegion(CRegion *pRegion) const { return pRegion==regionTo();}
	boolean isFromRegion(CRegion *pRegion) const { return pRegion==regionFrom();}

	BOOL setIsOnShip(eID nShipID, BOOL bAsCaptain=FALSE);
	BOOL setIsInBuilding(eID nBuildingID,int nMaintainCost=0);
	BOOL setInBuilding(eID nBuildingID, BOOL bAsOwner=FALSE);
	BOOL setOnShip(int nShipID, BOOL bAsCaptain=FALSE);
	BOOL onGive(int iItem, const char *pName, int nType, int & n, CUnit * pReceiver, BOOL bAutomatic, BOOL bFromGiveAll, BOOL bFromUse);
	BOOL onLoose(int iItem, const char *pName, int nType, int n, CUnit * pReceiver);
	BOOL onReceive(int iItem, const char *pName, int nType, int n, BOOL bByAttacking=FALSE);

	BOOL onPotentialReceive(int iItem, const char * pName, int nType, int n, CUnit *pGiver);

	BOOL onMakeShipOrder(t_ship * pInfos, eID nShipID, int nAmount=VALUE_NOTSPECIFIED);
	BOOL onMakeBuildingOrder(t_building * pInfos, eID nBuildingID, int nAmount=VALUE_NOTSPECIFIED);
	BOOL onMakeHerbOrder(int nAmount=VALUE_NOTSPECIFIED);
	BOOL onResearchHerbOrder();

	BOOL onMakePotionOrder(int nPotionID, int nAmount=VALUE_NOTSPECIFIED);
	BOOL onMakeRoadOrder(eCoast nCoast, int nAmount=VALUE_NOTSPECIFIED);
	BOOL onMakeItemOrder(int nItemID, int nAmount=VALUE_NOTSPECIFIED);

	BOOL onUsePotion(int nPotionID, int nAmount=VALUE_NOTSPECIFIED, BOOL bAutomatic=FALSE);
	BOOL onUseSpecialItem(int nItemID, int nAmount=VALUE_NOTSPECIFIED, BOOL bAutomatic=FALSE);

	BOOL onGiveMen(int n, CUnit * pReceiver, BOOL bAutomatic, BOOL bGiveUnit=FALSE);
	BOOL onGiveControl(CUnit * toUnit);
	BOOL onGiveUnit(CUnit * toUnit);
	
	BOOL onGiveHerbs(CUnit * toUnit, BOOL bAutomatic);
	BOOL onGiveAll(CUnit * toUnit, BOOL bAutomatic);

	BOOL onGiveSilverOrder(int n, CUnit *pReceiver, BOOL bAutomatic);
	BOOL onGiveMaxMoney(int & nSilver, CUnit *pReceiver, BOOL bFromSameFaction);

	BOOL onAttackedBy(CUnit * toUnit);
	BOOL onAttackOrder(CUnit *attacked_unit);
	BOOL onCombatStatus(int i);
	BOOL onRecruit(int nbPersons, CUnit *pRecruiter);
	BOOL onTeachOrder(int nUnitID, int temp);
	BOOL onLearnOrder(t_skills * sk);

	BOOL OnEnterBuilding(eID nBuildingID, BOOL bAsOwner = FALSE);
	BOOL OnEnterShip(eID nShipID, BOOL bAsOwner = FALSE);

	BOOL OnDisguise(int nOtherFactionID);

	BOOL checkOrder(const char *sOrder, int this_command);

	BOOL isBuildingOwner() const;
	const char * currentOrder() const { return (LPCSTR)m_sOrder;}

	void SetXY(int x, int y);

	void setCurrentOrder(const char *pOrder);
	BOOL checkHasLongOrder();

	BOOL _warning(int nErrorNum, int nLevel,...);
	BOOL _error(int nErrorNum,...);

	BOOL onReceiveBuildingControl(eID nBuildingID, BOOL bGiverChangedOfContainer, int nCost);
	BOOL onReceiveShipControl(eID nShipID, BOOL bGiverEnteredSomewhereElse);

	int finalSailingSkill();

	BOOL isInBuildingType(const char * sBuildingType, BOOL * pbWorking=NULL) const;

	int upkeepCost() const { return people()*FEEDSILVER_PER_MAN;}

	BOOL setRegionFrom(int Rx, int Ry);

	BOOL beforeCheck();

	void setCRConfirmedOrders();
	
	BOOL CRConfirmedOrders() const { return m_bCRConfirmedOrders;}

	BOOL setInitialGuarding();

	int weight(CString & sResults, BOOL bDetails);

	////////////////

	const char * outputName() const;

	static void checkings();
	static BOOL existingUnit(CUnit *& pUnit, eID nUnitID, CRegion * pRegion=NULL);
	static BOOL somewhere(eID nUnitID, CRegion *& pRegion);

	static void onsaveCheckUnitsToRemove();

	static BOOL isWeapon(int ni, int *p=NULL);
	static BOOL isArmour(int ni, int *p=NULL);
	static BOOL isItem(int ni, int *p=NULL);

	static void loadUTIDs();

public:

	BOOL AddOrderBySequence(const char *pOrder, K_Order ikeyword);
	
	void ProcessOrdersBySequence(K_Order k_until=MAXKEYWORDS);

	BOOL OnOrdersEnded(); 
	BOOL ChangedOfContainer() const;

	BOOL checkPeacefulMen(const CRegion *pRegion);

	BOOL isPet();

	int spell;		// Bit-Map: 2^Spell-Typ

	void magicSchool(CString & sMagicSchool);

private:

	COrders m_orders;
	CString m_sCurrentOrder;
	K_Order m_nCurrentCmd;

	int m_nPotentialAddedWeight;

	//BOOL m_bFromOcean;

	eID m_nDisguisedFactionID;

	CUnit * m_pCarrier;
	CUnit * m_pRider;

	CString m_sLongOrder;
	CString m_sStartOfOrders;
	CString m_sNewGroup;

	BOOL m_bWarningSilver;

	BOOL m_bCombatStatusChanged;
	BOOL m_bInAcademy;
	BOOL m_bUnknown;
	BOOL m_bHeroes;
	BOOL m_bPromotion;
	BOOL m_bOrderNeeded;
	BOOL m_bCRConfirmedOrders;
	BOOL m_bUnknownReceiver;
	int m_nLives;

	BOOL m_bInitialGuard;

	CUnits * m_carried;

	static BOOL s_bCheckings;

	static CString sEffectNames[EFT_MAX];
	static CString sSkillsEntries[SK_MAX];
	static TUTItemInfo s_weapons[WP_NBMAX];
	static TUTItemInfo s_armourItems[AR_NBMAX];
	static TUTItemInfo s_items[IT_NBMAX];
	//static TUTItemInfo s_potions[PO_NBMAX];

	static int itemWeightOutsideMagicBag(int lbsi, const char * sItemName, int & lbsitems_in_magic_bags_max);

	static int paramIndexToCombatStatus(int i);
	static eSkill skillnameToIndex(const char * pName);
	static eEffect effectnameToIndex(const char * pName);

	static CUnit * unit(CFaction * pFaction, int n, int tempStatus, int rX, int rY);


	static void checkCosts();
	static void spendCosts();

	static void remindMakers();
	static void checkMakers();
	static void checkHeroes();
	static void checkTaxers();
	static void checkEntertainers();
	static void checkTraders();
	static void checkWorkers();
	static void checkControlGivers();
	static void checkAttackers();
	static void checkRecruited();
	static void checkTeachers();
	static void checkLearners();
	static void checkGuards();
	static void checkMovers(BOOL bBeforeMove);
	static void checkMovesOnLand();
	static void checkCarriers();
	static void checkTransported();

	static void checkPotentialReceivers();

	static void before_checkings();
	static void check_regionMoney();
	static void check_money(BOOL bBeforeMove);
	static void check_units();
 
	static void loadIDsFrom(TUTItemInfo tabitems[], int nMax, int nFirst = 0);

	//////////

	BOOL isLongOrder(K_Order cmd) const;

	BOOL ProcessAnOrder(COrder *pOrder, K_Order k_until);

	BOOL onProcess_order_reserve(const char *s, BOOL bAutomatic);
	BOOL onProcess_order_use(const char *s, BOOL bAutomatic);
	BOOL onProcess_order_cast(const char *s, BOOL bAutomatic);
	BOOL onProcess_order_give(const char *s, BOOL bAutomatic);
	BOOL onProcess_order_buy(const char *s, BOOL bAutomatic); 
	BOOL onProcess_order_sell(const char *s, BOOL bAutomatic);
	BOOL onProcess_order_tax(const char *s, BOOL bAutomatic);
	BOOL onProcess_order_entertain(const char *s, BOOL bAutomatic);
	BOOL onProcess_order_work(const char *s, BOOL bAutomatic);
	BOOL onProcess_order_guard(const char *s, BOOL bAutomatic);
	BOOL onProcess_order_promotion(const char *s, BOOL bAutomatic);

	BOOL canDoLongOrder(K_Order nCurrentCmd, const CString & sCurrentOrder);

	int order_nextUnitID(int type, CUnit **cmdUnit);

	BOOL checkMen();

	int getmoreShips(int *shipsID, int nbmax);

	int studycost(const t_skills *talent);

	BOOL skillLevelChangeOnTerrain(eSkill nSkillID, int & nDiff);

	BOOL AddMakeOrder(int nAmount, int nUTType, eID nID=VALUE_UNKNOWN, int nUTID=VALUE_UNKNOWN);

	BOOL setNewTalent(const char *pSkillName, int nNewLvl);
	BOOL setNewTalent(eSkill nSkillID, int nNewLvl);
	BOOL setTalent(const char *pSkillName, int nNewLvl, int nVariation);
	BOOL setTalent(eSkill nSkillID, int nNewLvl, int nVariation);
	BOOL modifyTalent(eSkill nSkillID, int nVariation);
	BOOL modifyAllTalents(int nVariation);

	BOOL checkMoney(BOOL bCheckCosts);

	void setReceivedSomething() {m_bReceivedSomething = TRUE;}

	BOOL AddCarried(CUnit * pCarried);
	BOOL RemovedCarried(CUnit * pCarried);

	BOOL addPeople(int n, CUnit * pMenGiver=NULL, BOOL bGiveUnit=FALSE);
	BOOL removePeople(int n);

	void checkInsideWhat();

	int nbHorses();
	int nbTrollBelts();
	int nbElvenHorses();
	int nbCarts();
	int nbMagicBags();
	int nbRingsQuickFingers();

	int nbMaxHorses(int nNbMoves, BOOL bMovingOnRoad);
	int nbMaxCarts(int nNbMoves, BOOL bMovingOnRoad);

	int decreaseNbMoves();

	int nbEffects(const char *pEffectName); 
	BOOL modifyEffect(const char *pEffectName, int nNb); 


	int nbMoves() const;

	const char * sid() const;
	const char * name() const { return itob(m_nID);}

	CUnit * parent() const;

	CRegion * region(BOOL bFrom) const;
	TItemInfo * itemInfo(int iItem, int nType) const;
	TItemInfo * addItemInfo(int iItem, const char *pName, int nType, int nWeight=0);

	BOOL setInitialMaintainCost(int nMaintainCost);

	BOOL cr_isInsideSomething(BOOL * pInBuilding=NULL, BOOL * pInShip=NULL);

	BOOL canBuildShip(int nSizeBefore, const t_ship * tinf, int & nSize, CString & sLackItems);
	BOOL canBuildCastle(int nSizeBefore, t_cost & result, int & nSize, CString & sLackItems);
	BOOL canBuildRoad(int nSizeBefore, t_cost & result, int & nSize, CString & sLackItems);
	BOOL canBuild(int nSizeBefore, const t_cost & cost, t_cost & result, int & nSize, CString & sLackItems, BOOL bIsCastle=FALSE, BOOL bIsRoad=FALSE);
	BOOL build(const t_cost & cost, int nSize, BOOL bIsCastle=FALSE, BOOL bIsShip=FALSE, BOOL bIsRoad=FALSE);

	BOOL canMake(const t_cost & cost, const t_cost &divider, t_cost & result, int & nSize, CString & sLackItems);

	BOOL onCarriedByHorse(CUnit * pCarrier,	const TMoveInfo & moveInfos);

	BOOL changedRegion() const; 
	BOOL inUsedBuilding() const;
	BOOL isCostingBuildingOwner() const;

	int m_nSkills[SK_MAX];
	int m_nEffects[EFT_MAX];
	BOOL m_bSpendEffects[EFT_MAX];

	CFaction *m_pFaction;
	CRegion *m_pRegionFrom;
	CRegion *m_pRegionTo;
	CUnit * m_pNewController;
	CUnit * m_pMenGiver;
	CUnit * m_pRecruiter;
	CUnit * m_pMother;
	t_skills * m_pLearntSkill;
	int m_nSpecifiedLearntCost;

	int m_nNewX, m_nNewY;

	int m_nTemp;
	int m_nMoveStatus;
	int m_nID;

	TMakeOrder m_makeOrder;
	TMoveInfo m_moveInfos;

	CUnit * m_pLastTeacher;

	BOOL m_bReceivedUnit;
	BOOL m_bTaught;
	BOOL m_bAttacking;
	BOOL m_bLearning;
	BOOL m_bEntering;
	BOOL m_bLeaving;
	BOOL m_bImplicitLeaving;
	BOOL m_bGuarding;
	BOOL m_bReceivedSomething;
	BOOL m_bWarningTeachTooMuch;
	int m_nNbMenToTeach;
	int m_nNbMenToBeTaught;
	int m_nNbTeachers;
	int m_nNbHorses;
	int m_nNbTrollBelts;
	int m_nNbElvenHorses;
	int m_nNbCarts;
	int m_nNbMagicBags;
	int m_nNbRingsQuickFingers;

	CBuilding * m_pBuilding;
	CBuilding * m_pInitialBuilding;
	CBuilding * m_pMadeBuilding;
	CRoad * m_pMadeRoad;

	CShip * m_pMadeShip;
	CShip * m_pShip;
	CShip * m_pInitialShip;
		
	TMoney m_money;
	TTradeItem * m_buys;
	TTradeItem * m_sells;
	TItemInfo * m_items;
	TTeachInfo * m_teachs;

	CItemReceivedInfoList m_potentialReceives;


	int m_nNbBoughtLuxuries;
	int m_nNbMaxTradeItems;
	int m_nNbSoldItems;
	int m_nNbBoughtItems;
	int m_nPeople;
	int m_nInitialPeople;
	int m_nCombatStatus;
	int m_nAura;
	int m_nFamiliarMageID;
	int m_nStarvingStatus;
	int m_nLearntSkillLevels[2];

	eSkill m_eLearntSkill;

	int m_nEntertainmentAmount;
	int m_nTaxAmount;

	CString m_sShortName;
	CString m_sContainerNewName;
	CString m_sRegionNewName;
	CString m_sNewName;
	CString m_sOutputName;

	CString m_sOrder;

	void setLearningCost(int n); 

	BOOL onLeaveShip(BOOL bOrderSpecified=FALSE);
	BOOL onLeaveBuilding(BOOL bOrderSpecified=FALSE);

	BOOL onSell(int nItemID, int nb);
	BOOL onBuy(int nItemID, int nb);

	int effect(eEffect n);
	int addEffect(eEffect nEff, int n, BOOL bMinZero);

	int level(const char * pName, int & nValue);
	int level(eSkill n, BOOL bAfterLearn=FALSE);
	int nbWeapons(int nType=0, BOOL bWhenAttack=FALSE);

	BOOL buy(TTradeItem * pBuy, CRegion *r);
	BOOL sell(TTradeItem * pSell, CRegion *r, int & nbAvailableTrades);

	BOOL hasTradeItems(int nItemID = ITEMID_NONE, TTradeItem ** pItem = NULL) const;
	BOOL addTradeItems(int nItemID, int & nb, BOOL bBuy);

	BOOL hasStudent(int nUnitID=-1, int bIsTemp=0, TTeachInfo ** pTeach = NULL) const;
	BOOL addStudent(int nUnitID, int bIsTemp);

	BOOL hasTeacher() const;

	BOOL CUnit::isMovingToOcean(int & nNbMoves) const;

	block *m_cr_talents;
	block *m_cr_unit;
	block *m_cr_effects;

	block * cr_effects();;
	block * cr_talents();
	block * cr_unit();
	block * cr_region();

	int cr_familiarMageID();

	int cr_nbItems(const char *pName);

	int nbItems(int nID, const char *pName=NULL);
	int nbHerbs(int nID, const char *pName=NULL);
	int nbPotions(int nID, const char *pName=NULL);
	int nbElements(int nID, const char *pName=NULL,int nType=UT_ITEM);

	int nbUTItems(eItem nUTID);

	int maxStoneForCastleBuilder(int nNbPersons, int nCastleSize);
	int maxWoodForShipcrafter(const t_ship * tinf, int nNbPersons, int nCurrentSize);

	BOOL canUseItem(eItem nUTid, int n, int & nUsed, int & nSize, CString & sItem);
	BOOL canUseMaxItem(eItem nUTid, int n, int nMax, int nDivider, int & nUsed, int & nSize, CString & sItem);
	BOOL spendItem(eItem nUTid, int n);
	BOOL isStarving();

	int cr_starvingStatus();
	int cr_combatStatus();
	int cr_aura();
	BOOL addTeacher(CUnit * pTeacher, int & nbTaught, int & nStudentSkillLevel, int & nTeacherSkillLevel);

	BOOL retrieveFaction();

	BOOL onCheckBuys(CRegion * r);
	BOOL onCheckSells(CRegion * r);
	
	BOOL onCheckEntertain();
	BOOL onCheckTax();
	BOOL onCheckTrade();
	BOOL onCheckWork();
	BOOL onCheckAttack();
	BOOL onCheckRecruited();
	BOOL onCheckTeachers();
	BOOL onCheckLearners();
	BOOL onCheckHeroes();
	BOOL onCheckMakers();
	BOOL onCheckGuard();
	BOOL onCheckMove(BOOL bBeforeMove);
	BOOL onCheckMoveOnLand();
	BOOL onCheckCarrier();
	BOOL onCheckTransported();
	BOOL onCheckBeforeMove();
	BOOL onCheckAfterMove();

	BOOL onCheckPotentialReceiver();
	BOOL onPotentialAddedWeights();

	BOOL onRemindMakers();

	BOOL onCheckMakeRoad();
	BOOL onCheckMakeBuilding();
	BOOL onCheckMakeShip();
	BOOL onCheckMakeHerb();
	BOOL onCheckMakePotion();
	BOOL onCheckMakeItem();
	BOOL onCheckProduceItem(eItem nIndex);
	BOOL onCheckMakeWeapon(eWeapon nIndex);
	BOOL onCheckMakeArmourItem(eArmourItem nIndex);

	BOOL onCheckMake(int nIndex, const TUTItemInfo *tabItems, eSkill nsk, const char * sBuildingName, t_cost & d,  BOOL bResource=FALSE);

	void onCheckGiveControl();
	void onCheckGiveShipControl(const CShip *pShip, BOOL bChangedOfContainer);
	void onCheckGiveBuildingControl(const CBuilding * pBuilding, BOOL bChangedOfContainer);

	BOOL onEarnMoney(eSkill nSkill);

	CString outputPrefix() const;

	BOOL cr_retrieveBasicInfos();

	void init();

	int skillLevelAfterRecruit(const char * pName) const;
	int defaultSkillLevel(eSkill nSkill) const;

	int learntSkillLevel(BOOL bAfterLearn);

	BOOL updateNewName();
	BOOL nbMenChanged() const;

	CRegion * SetRegionTo(CRegion * pRegionTo);
	CRegion * SetRegionFrom(CRegion * pRegionFrom);

	CRegion * toRegion() const;
	CRegion * fromRegion() const; 
 
	BOOL markCrossedRegions();

	BOOL moveUsingRoads(int nNbMoves);

	BOOL onsave_remove();

	BOOL onReceiveMoney(int n, BOOL bFromReservedAlso, BOOL bExplicitGive);
	BOOL onTransferMoney(int n, CUnit *pReceiver, BOOL bFromReservedAlso=TRUE, BOOL bExplicitGive=FALSE, BOOL bAutomatic=FALSE);

	BOOL onRegionChangeGuardsNumber(int nb);

	int capacity(int nNbMoves, BOOL bOnRoad);
	int transported_weight(CString & sResults, BOOL bDetails);

	BOOL HasToConfirmOrders() const;

	BOOL inBuildingType(CString &sType) const;


	/////////////////

};

typedef struct TTeachInfo
{
	int		m_nUnitID;
	int		m_bIsTemp;
	CUnit	* m_pUnit;
	TTeachInfo * m_pNext;

	TTeachInfo::TTeachInfo(int nUnitID, int bIsTemp, TTeachInfo ** pNext) {
		m_nUnitID = nUnitID;
		m_bIsTemp = bIsTemp;
		m_pUnit = NULL;
		if (pNext)
		{
			m_pNext = *pNext;
			*pNext=this; // always as first item
		}
	}
} TTeachInfo;

///////////

DECLARE_LIST(CUnit);

#endif // !defined(AFX_UNIT_H__BE85D52A_BBC6_46D8_A384_7A5197A4DE9D__INCLUDED_)
