// Region.h: interface for the CRegion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGION_H__4BA0A37E_87B0_43D7_BB41_2694C8A8077D__INCLUDED_)
#define AFX_REGION_H__4BA0A37E_87B0_43D7_BB41_2694C8A8077D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wLizard.h"
#include "SortableObList.h"
#include "Unit.h"
#include "crdata.h"
#include "EcheckTypes.h"
#include "CRTypes.h"
#include "Units.h"
#include "CRRegion.h"


struct TMoney;
struct TTradeItem;
struct block;

class CFaction;
class CRoad;

/////////

/*
#define RS_TREES_ID			(1951692551)
#define RS_YOUNGTREES_ID	(-1534895839)
#define RS_MALLORN_ID		(1551731767)
#define RS_YOUNGMALLORN_ID	(322259672)
#define RS_STONE_ID			(1992739472)
#define RS_IRON_ID			(235898859)
#define RS_LAEN_ID			(1658234158)
*/

typedef enum EResourceValueKind
{
	VRS_AMOUNT=0,
	VRS_SKILL=1,
} EResourceValueKind;

#define VRS_NB	(VRS_SKILL+1)

typedef	enum eRegionResource
{
	RS_TREES=0,
	RS_YOUNGTREES,
	RS_MALLORN,
	RS_YOUNGMALLORN,
	RS_STONE,
	RS_IRON,
	RS_LAEN,
	RS_HERB,
	RS_NBMAX
} eRegionResource;



///////////

class CRegion : public CObject 
{

public:

    static int compareOnSilver(CRegion* r1, CRegion* r2) { return r1->allSilver() > r2->allSilver();}
    static int compareOnMen(CRegion* r1, CRegion* r2) { return r2->m_nNbPersons > r1->m_nNbPersons;}

	static BOOL loadLuxuriesIDs();
	static BOOL luxuryName(int nItemID, CString & sResult, BOOL bEnglish=TRUE);

	///////////////////
	// money management

	void setWar() { m_bWar = TRUE;}

	BOOL hasOrderedUnits() const { return m_bHasOrderedUnits;}
	BOOL war() const { return m_bWar;}

	void setDefaultWage(int nDefWage);

	void AddSilver(const TMoney & money, BOOL bUnitHasMoved);
	void AddReservedSilver(int nReservedSilver);
	void SpendSilver(int nSilver, BOOL bAsUpkeep);
	void SpendUpkeep();
	void ResetSilver();

	int resourceLevel(eItem nIndex);

	int reservedSilver() const;
	int allSilver() const;

	BOOL checkSilver(BOOL bReservedSilver) const; 
	BOOL onBuy(TTradeItem * pBuy);
	BOOL onSell(TTradeItem * pSell);

	BOOL onMadeItem(TItemInfo * pItemInfo, int nb);
	BOOL onConsume(const TUTItemInfo & item, const t_cost & d, CUnit * pConsumer, int nSkillLevel, int & nb);

	void runUpkeep();

	/////////

	void AddNbDemonFeed(int nNb, CFaction *pFaction);

	void AddNbPersons(int nNbPersons, BOOL bAsRecruited=FALSE);

	CRegion(int x, int y, int pers, const char *pName);

	virtual ~CRegion();
	
	BOOL Stats(TItemInfo * & pItemsList);
	void onCheck();

	BOOL isOcean() const;
	BOOL isNeighbour(int nx, int ny) const;
	BOOL isNeighbour(CRegion * pRegion) const;

	BOOL PositionIs(int x, int y) const { return m_nX==x && m_nY==y;}

	//BOOL IsGuardedByAllied(int nPlayerID);
	//BOOL IsGuardedByNotAllied(int nPlayerID);

	//BOOL IsGuardedByAllied(CFaction * pFaction=NULL);
	//BOOL IsGuardedByNotAllied(CFaction * pFaction=NULL);

	BOOL canBuyLuxury(int nItemID);
	BOOL canSellLuxury(int nItemID);

	BOOL isTerrain(const CString & sName) {return (!terrain().CompareNoCase(sName));}
	BOOL isGlacier() { return isTerrain(EN_GLACIER);}

	BOOL luxuryInfos(int nItemID, CString & sName, int & nPrice, BOOL bEnglish=TRUE);
	CString buyableLuxuryName(BOOL bEnglish=TRUE);

	BOOL hasUnalliedUnits();

	int nbMaxLuxuriesToSell();
	BOOL canBuy(TTradeItem * pBuy, int & ns);
	BOOL canSell(TTradeItem * pSell);
	// accessors

	int x() const { return m_nX;}
	int y() const { return m_nY;}

	const char *name() const { return (LPCSTR)m_sName;}
	const char *outputName(BOOL bWithCoords=FALSE) const;

	int cr_wageForRace(BOOL bIsOrc);

	int cr_wage();
	int cr_castleSize();
	int cr_buildingSize(int nID, BOOL bIsCastle, block * & p_cr_building);
	int cr_shipSize(int nID, block * & p_cr_ship);

	BOOL cr_hasAcademy();
	BOOL cr_resolveName();
	BOOL cr_hasBuilding(const char *pNameType);
	BOOL cr_hasShip(eID nID);
	BOOL cr_hasShip(const char *pNameType);
	BOOL cr_hasUnit(eID nUnitID);

	block * cr_units();

	block * cr_addNewRoad(CRoad * pb);
	block * cr_addNewBuilding(CBuilding * pb);
	block * cr_addNewShip(CShip * pb);
	block * cr_addNewUnit(int nID, block * bMotherUnit);

	BOOL cr_addThroughUnit(const char *pUnitName);
	BOOL cr_addThroughShip(const char *pShipName);

	block * cr_region();
	block * cr_castle();
	block * cr_building(const char *pType);
	block * cr_building(eID nID);
	block * cr_ship(eID nID);
	block * cr_buildings();
	block * cr_academy();

	block * cr_road(int nDirection);
	block * cr_roads();

	block * cr_luxuries();

	static BOOL s_bNameWithCoords;

	int price(int nItemID, int *pIndex=NULL);

	BOOL onCheckTrade();
	BOOL onCheckWork();

	void AddNbBoughtItems(int nb);
	void AddNbSoldItems(int nb);

	//BOOL isGuarded();

	BOOL AddUnit(CUnit *pUnit, BOOL bUnitHasOrders = FALSE);
	BOOL RemoveUnit(CUnit *pUnit);

	int amountTax() const { return m_money.m_nByTax;}
	int amountEntertain() const { return m_money.m_nByEntertain;}

	CString terrain();

	CString herbName();

	BOOL save();

	static BOOL madeItemsToStr(const TItemInfo * pItems, CString & sMadeItems, eItemType nFlag=ITT_ALL, TItemInfo ** pList=NULL);

	static void moneyCheckings();

	BOOL cr_hasAnHarbour();

	int nbPersons() const { return m_nNbPersons;}

	void addGuards(BOOL bAllied, int nb);

	BOOL IsGuardedByUs() const{	return (m_nNbAlliedGuards > 0);}
	BOOL IsGuardedByOther(BOOL *bWar) const { if (bWar) *bWar = war(); return (m_nNbOtherGuards > 0);}

private:

	TDemonInfos * demon(TDemonInfos * pList, CFaction * pFaction);
	BOOL addDemonFeed(TDemonInfos * & pList, CFaction * pFaction, int nbFeed);

	static TItemInfo * item(TItemInfo * pList, int iItem, int nType);
	static BOOL addMadeItem(TItemInfo * & pList, int iItem, const char *pName, int nType, int nb);
	
	BOOL addConsumedItem(TItemInfo * & pList, int iItem, const char *pName, int nType, int nbConsumed);

	static void checkWithReservedMoney();

	//block * cr_guards();

	block * cr_resource(const CString & sWantedType);

	BOOL _warning(int nErrorNum, int nLevel, ...) const;
	BOOL _error(int nErrorNum, ...) const;

	BOOL updateNewName(CUnit * pInitialOwner);
	CUnit * owner();
	CUnit * initialOwner();

	//BOOL AddGuardingFaction(int nFactionID, CFaction * & pFaction);

	BOOL onCheckEntertain();
	BOOL onCheckTax();
	BOOL onCheckMaxSilver();
	BOOL onCheckEnoughSilver();
	BOOL onCheckRecruits();
	BOOL onCheckBelongers();

	int feature(eRegionFeature n, block *b = NULL);
	int maxEntertain(BOOL bOnlyWhenEntertaining = TRUE);
	int maxTax();
	int maxRecruits(BOOL bOnlyWhenRecruiting = TRUE);

	int buyableLuxuryItemID();
	BOOL retrieveLuxuriesInfos();

	TItemInfo * m_items; // made, consumed (given by region)

	int m_resources[RS_NBMAX][VRS_NB];

	int m_nNbRecruits;
	int m_nNbPersons;
	int m_nDefaultWage;
	int m_nX;
	int m_nY;

	int m_nBuyableLuxuryItemID;
	int m_nBuyableLuxuryItemIndex;
	int m_nNbBoughtItems;
	int m_nNbSoldItems;
	int m_nNbLuxuries;

	// int m_nNbDemonFeed;
	TDemonInfos * m_demons;

	int m_nNewResourceLevels[IT_NBMAX];

	int m_nNbAlliedGuards;
	int m_nNbOtherGuards;

	BOOL m_bMoneyWarning;

	BOOL m_bHasOrderedUnits;
	BOOL m_bAcademyChecked;
	BOOL m_bWar;
	BOOL m_bMallorn;

	CUnits m_units;

	CString m_sNewName;
	CString m_sName;
	CString m_sType;
	CString m_sHerbName;

	block *m_cr_region;
	block *m_cr_castle;
	block *m_cr_luxuries;
	//block *m_cr_guards;
	block *m_cr_roads;
	block *m_cr_buildings;
	block *m_cr_academy;
	block *m_cr_units;
	block *m_cr_resources;

	TMoney m_money;
	int m_nFeatures[RFT_MAX];

	//////////
	
	static int s_nItemsID[NBLUXURIES];
	//static eID s_nResourcesID[RS_NBMAX];

	//////////
	
};

DECLARE_SORTABLE_LIST(CRegion)

#endif // !defined(AFX_REGION_H__4BA0A37E_87B0_43D7_BB41_2694C8A8077D__INCLUDED_)
