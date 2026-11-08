// CRUnit.h: interface for the CCRUnit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRUNIT_H__BE85D52A_BBC6_46D8_A384_7A5197A4DE9D__INCLUDED_)
#define AFX_CRUNIT_H__BE85D52A_BBC6_46D8_A384_7A5197A4DE9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "CRTypes.h"

//class CCRUnit;
class CCRBuilding;
class CCRShip;
class CCRFaction;
class CCRRegion;
class CCR;

//////////

#define ALIVE_USER_CRUNIT			(1)
#define ALIVE_UNKNOWN			(0)
#define ALIVE_ILLUSION			(-1)

#define CRUNIT_STATUS_NOTTEMP		(0)
#define CRUNIT_STATUS_TEMP		(42)

#define MOVESTATUS_CARRIED		(-1)
#define MOVESTATUS_NOMOVE		(0)
#define MOVESTATUS_SOMEWHERE	(1)
#define MOVESTATUS_BYHIMSELF	(2)

#define LEVEL_UNKNOWN			(VALUE_UNKNOWN)

#define WEAPONTYPE_MELEE		(1)
#define WEAPONTYPE_POLEARM		(2)
#define WEAPONTYPE_CROSSBOW		(4)
#define WEAPONTYPE_BOW			(8)
#define WEAPONTYPE_CATAPULT		(16)

#define ITEMID_NONE				(-1)
#define UFT_UNKNOWN_VALUE		(-1)

typedef enum eWeapon
{
	WP_SWORD=0,
	WP_AXE,
	WP_ADAMANTIUM_AXE,
	WP_CLAYMORE,
	WP_RUSTY_SWORD,
	WP_LAEN_SWORD,
	WP_FLAMING_SWORD,
	WP_SPEAR,
	WP_HALBERD,
	WP_LANCE,
	WP_MALLORNSPEAR,
	WP_CROSSBOW,
	WP_MALLORNCROSSBOW,
	WP_BOW,
	WP_MALLORNBOW,
	WP_ELVENBOW,
	WP_CATAPULT,
	WP_NBMAX
} eWeapon;

typedef enum eArmourItem
{
	AR_CHAINMAIL=0,
	AR_PLATEMAIL,
	AR_SHIELD,
	AR_LAENMAIL,
	AR_LAENSHIELD,
	AR_NBMAX
} eArmourItem;

#define WP_FIRST_MELEE		WP_SWORD
#define WP_LAST_MELEE		WP_FLAMING_SWORD
#define WP_FIRST_POLEARM	WP_SPEAR
#define WP_LAST_POLEARM		WP_MALLORNSPEAR
#define WP_FIRST_CROSSBOW	WP_CROSSBOW
#define WP_LAST_CROSSBOW	WP_MALLORNCROSSBOW
#define WP_FIRST_BOW		WP_BOW
#define WP_LAST_BOW			WP_ELVENBOW

////////////

#define ILRN_SKILL_BEFORE (0)
#define ILRN_SKILL_AFTER  (1)

///////////

class CCRUnit : public CObject 
{
public:
	//CCRUnit(CCRFaction *pFaction, eID nID, int nTempStatus, int xRegion, int yRegion, const char *sOrder, int nbPeople=0);
	CCRUnit(CCR & ocr, block * crCRUnit, CCRRegion * pRegion);

	virtual ~CCRUnit();

	BOOL isUnknownUnit();

	BOOL isUnknown() const { return m_bUnknown;}

	///////////////////
	// money management

	BOOL setInitialNbPeople(int n); 

	BOOL setFaction(CCRFaction *pFaction);

	void setMaintainCost(int n);
	void addMaintainCost(int n);
	void setMoney(int n);

	BOOL notEnoughMoney() const;
	BOOL moneyToRegion(BOOL bConsumeUpkeep=TRUE) const;

//	int maintainCost() const { return m_money.m_nMaintainCost;}
//	int learningCost() const { return m_money.m_nLearningCost;}
//	int money() const;

	///////////////////
	
	CCRFaction * faction() const { return m_pFaction;}

	void reset();

	void SetAliveStatus(int n);

	BOOL isAlive() const { return m_nLives >= ALIVE_USER_CRUNIT;}

	BOOL isInsect()		const;// { return (m_pFaction?m_pFaction->isRace(EN_INSECTS):FALSE);}
	BOOL isOrc()		const;// { return (m_pFaction?m_pFaction->isRace(EN_ORCS):FALSE);}
	BOOL isAquarian()	const;// { return (m_pFaction?m_pFaction->isRace(EN_AQUARIANS):FALSE);}
	BOOL isCat()		const;// { return (m_pFaction?m_pFaction->isRace(EN_CATS):FALSE);}
	BOOL isHuman()		const;// { return (m_pFaction?m_pFaction->isRace(EN_HUMANS):FALSE);}
	BOOL isElf()		const;// { return (m_pFaction?m_pFaction->isRace(EN_ELVES):FALSE);}
	BOOL isDwarf()		const;// { return (m_pFaction?m_pFaction->isRace(EN_DWARVES):FALSE);}
	BOOL isTroll()		const;// { return (m_pFaction?m_pFaction->isRace(EN_TROLLS):FALSE);}

	BOOL isRace(const char *pRaceName)	const;// { return (m_pFaction?m_pFaction->isRace(pRaceName):FALSE);}

	void setMother(CCRUnit *pMother);

	///////////

	const char * regionName(/*BOOL bFirstRegion=TRUE*/); 
	const char * regionOutputName(/*BOOL bFirstRegion=TRUE*/);

	CCRRegion * regionFrom() const { return fromRegion();}

	BOOL fromOcean() const;
	int people() const {return m_nInitialPeople;}

	int id() const {return m_nID;}

	int x() const { return m_nNewX;}
	int y() const { return m_nNewY;}

	BOOL IsCaptain() const;
	BOOL IsOnShip(eID nID=ID_UNKNOWN) const;
	BOOL IsInBuilding(eID nID=ID_UNKNOWN) const;

	BOOL isAllied() const;

	boolean isOnRegion(CCRRegion *pRegion) const { return isFromRegion(pRegion);}
	boolean isFromRegion(CCRRegion *pRegion) const { return pRegion==regionFrom();}

	BOOL setIsOnShip(eID nShipID, BOOL bAsCaptain=FALSE);
	BOOL setIsInBuilding(eID nBuildingID,int nMaintainCost=0);
	BOOL setInBuilding(eID nBuildingID, BOOL bAsOwner=FALSE);
	BOOL setOnShip(int nShipID, BOOL bAsCaptain=FALSE);

	BOOL isBuildingOwner() const;

	void SetXY(int x, int y);

	int finalSailingSkill();

	BOOL isInBuilding(const char * sBuildingType, BOOL * pbWorking=NULL) const;

//	int upkeepCost() const { return people()*FEEDSILVER_PER_MAN;}

	BOOL setRegionFrom(int Rx, int Ry);

	void setCRConfirmedOrders();
	
	BOOL setInitialGuarding();

	int totalWeight100();

	////////////////

	const char * outputName() const;

	static void checkings();
	static BOOL existingUnit(CCRUnit *& pCRUnit, eID nCRUnitID, CCRRegion * pRegion=NULL);
	static BOOL somewhere(eID nCRUnitID, CCRRegion *& pRegion);

	static BOOL isWeapon(int ni, int *p=NULL);
	static BOOL isArmour(int ni, int *p=NULL);
	static BOOL isItem(int ni, int *p=NULL);


public:

	int spell;		// Bit-Map: 2^Spell-Typ

private:

	eID m_nDisguisedFactionID;

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

	static BOOL s_bCheckings;

	static CString sSkillsEntries[SK_MAX];

	static int paramIndexToCombatStatus(int i);
	static eSkill skillnameToIndex(const char * pName);

	//////////

//	int studycost(const t_skills *talent);

	BOOL skillLevelChangeOnTerrain(eSkill nSkillID, int & nDiff);
	BOOL modifyTalent(eSkill nSkillID, int nVariation);
	BOOL modifyAllTalents(int nVariation);

	BOOL checkMoney(BOOL bCheckCosts);

	BOOL addPeople(int n, CCRUnit * pMenGiver=NULL, BOOL bGiveCRUnit=FALSE);

	void checkInsideWhat();

	//int nbHorses();
	//int nbCarts();

	int nbMaxHorses(int nNbMoves, BOOL bMovingOnRoad);
	int nbMaxCarts(int nNbMoves, BOOL bMovingOnRoad);

	const char * sid() const;
	const char * name() const;

	CCRUnit * parent() const;

	CCRRegion * region() const;

	BOOL setInitialMaintainCost(int nMaintainCost);

	BOOL cr_isInsideSomething(BOOL * pInBuilding=NULL, BOOL * pInShip=NULL);

	int nSkills[SK_MAX];

	CCRFaction *m_pFaction;
	CCRRegion *m_pRegionFrom;

	int m_nNewX, m_nNewY;
	int m_nID;

	int m_nNbHorses;
	int m_nNbCarts;

	CCRBuilding * m_pBuilding;
	CCRBuilding * m_pInitialBuilding;
	CCRShip * m_pInitialShip;
	CCRShip * m_pShip;
		
//	TMoney m_money;

	int m_nInitialPeople;
	int m_nCombatStatus;

	CString m_sShortName;
	CString m_sOutputName;

	void setLearningCost(int n); 

	int level(const char * pName, int & nValue);
	int level(eSkill n, BOOL bAfterLearn=FALSE);
	int nbWeapons(int nType=0, BOOL bWhenAttack=FALSE);

	block *m_cr_talents;
	block *m_cr_unit;
	CCR & m_cr;

	block * cr_talents();
	block * cr_unit();
	block * cr_region();

	int cr_nbItems(const char *pName);

	int nbItems(int nID, const char *pName=NULL);
	int nbHerbs(int nID, const char *pName=NULL);
	int nbPotions(int nID, const char *pName=NULL);
//	int nbElements(int nID, const char *pName=NULL,int nType=UT_ITEM);

	int nbUTItems(eItem nUTID);

	BOOL canUseItem(eItem nUTid, int n, int & nUsed, int & nSize, CString & sItem);
	BOOL canUseMaxItem(eItem nUTid, int n, int nMax, int nDivider, int & nUsed, int & nSize, CString & sItem);
	BOOL spendItem(eItem nUTid, int n);

	int cr_combatStatus();
	BOOL addTeacher(CCRUnit * pTeacher, int & nbTaught, int & nStudentSkillLevel, int & nTeacherSkillLevel);

	BOOL retrieveFaction();
	
	CString outputPrefix() const;

	BOOL cr_retrieveBasicInfos();

	void init();

	int skillLevelAfterRecruit(const char * pName) const;
	int defaultSkillLevel(eSkill nSkill) const;

	int learntSkillLevel(BOOL bAfterLearn);

	BOOL nbMenChanged() const;

	CCRRegion * SetRegionFrom(CCRRegion * pRegionFrom);

	CCRRegion * fromRegion() const; 
 
	int capacity100(int nNbMoves, BOOL bOnRoad);
	int transportedWeight100();

	/////////////////

	CCR & cr();

};

///////////

#endif // !defined(AFX_CRUNIT_H__BE85D52A_BBC6_46D8_A384_7A5197A4DE9D__INCLUDED_)
