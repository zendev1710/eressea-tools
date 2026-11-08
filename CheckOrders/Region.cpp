// Region.cpp: implementation of the CRegion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CR.h"
#include "Eressea.h"
#include "MyEresseaCR.h"
#include "kToTrace.h"
#include "EcheckTypes.h"
#include "Faction.h"
#include "Unit.h"
#include "Units.h"
#include "Region.h"
#include "Regions.h"
#include "Building.h"
#include "Buildings.h"
#include "Ship.h"
#include "Ships.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define CHECKREGION if (!m_sName.CompareNoCase("Sorne"))
#define CHECKREGION if (!m_sName.CompareNoCase("Rulsil"))
//#define CHECKREGION if (0)
#define BKPTREGION {int zzzzzz=1;}

static CRegions s_withReservedMoney(FALSE);

int CRegion::s_nItemsID[NBLUXURIES]={VALUE_UNKNOWN,VALUE_UNKNOWN,VALUE_UNKNOWN,VALUE_UNKNOWN,VALUE_UNKNOWN,VALUE_UNKNOWN,VALUE_UNKNOWN};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CRegion::s_bNameWithCoords = FALSE;

CRegion::CRegion(int x, int y, int pers, const char *pName): 
	m_nNbPersons(pers),
	m_nX(x),
	m_nY(y),
	m_sName((pName?pName:REGION_VALUE_UNKNOWN)),
	m_cr_region(NULL),
	m_cr_castle(NULL),
	m_cr_resources(NULL),
	m_cr_luxuries(NULL),
	//m_cr_guards(NULL),
	m_cr_roads(NULL),
	m_cr_buildings(NULL),
	m_cr_academy(NULL),
	m_cr_units(NULL),
	m_items(NULL),
	m_demons(NULL),
	m_nDefaultWage(WAGE_MIN_VALUE),
	m_nNbBoughtItems(0),
	m_nNbSoldItems(0),
	m_nNbRecruits(0),
	m_bHasOrderedUnits(FALSE),
	m_bAcademyChecked(FALSE),
	m_bMallorn(FALSE),
	m_bWar(FALSE),
	m_units(NULL),
	m_sHerbName(""),
	m_sType(""),
	m_nBuyableLuxuryItemIndex(VALUE_UNKNOWN),
	m_nBuyableLuxuryItemID(VALUE_UNKNOWN),
	m_nNbAlliedGuards(0),
	m_nNbOtherGuards(0),
	m_bMoneyWarning(FALSE)
{
	int i;
	for (i = 0;i<RFT_MAX;i++)
		m_nFeatures[i] = VALUE_UNKNOWN;

	for (i = 0;i<IT_NBMAX;i++)
		m_nNewResourceLevels[i] = VALUE_UNKNOWN;

	m_money.reset();
	if (pName && (!_strcmpi(pName,REGIONNAME_ECHECK_NULLVALUE) || !_strcmpi(pName,"")))
		m_sName=REGION_VALUE_UNKNOWN;

	if (cr().Loaded() && !m_sName.CompareNoCase(REGION_VALUE_UNKNOWN))
		cr_resolveName();
}

CRegion::~CRegion()
{
	TItemInfo *pi=m_items;
	TItemInfo *pinext=NULL;
	TDemonInfos * pDem = m_demons;
	TDemonInfos * pNext = NULL;
	while (pDem) {
		pNext = pDem->m_pNext;
		delete (pDem);
		pDem = pNext;
	}
	while (pi) {
		pinext = pi->m_pNext;
		delete (pi);
		pi = pinext;
	}

}

BOOL CRegion::updateNewName(CUnit * pInitialOwner)
{
	if (pInitialOwner && pInitialOwner->updateRegionName(m_sNewName) && m_sNewName.CompareNoCase(m_sName))
	{
		m_sName = m_sNewName;
		return TRUE;
	}
	return FALSE;
}

block * CRegion::cr_units()
{
	if (m_cr_units) 
		return m_cr_units;
	m_cr_units = cr().units(cr_region());
	return m_cr_units;
}

BOOL CRegion::cr_hasUnit(eID nUnitID)
{
	block * crunit = cr().units(cr_region(),&nUnitID);
	return crunit!=NULL;
}

BOOL CRegion::AddUnit(CUnit *pUnit, BOOL bUnitHasOrders)
{
	CHECKREGION
	BKPTREGION

	m_units.InsertBySortedFaction(pUnit);
//	if (!m_units.Has(pUnit)) 
//		m_units.AddHead(pUnit);
	if (bUnitHasOrders) 
		m_bHasOrderedUnits = TRUE;
	return TRUE;
}

BOOL CRegion::RemoveUnit(CUnit *pUnit)
{
	BOOL bRemoved=FALSE;
	int nNb=0;
	if (pUnit) 
	{
		int nNb = pUnit->people();
		if (m_units.Remove(pUnit))
		{
			m_nNbPersons -= nNb;
			bRemoved = TRUE;
		}
	}
	return bRemoved;
}

BOOL CRegion::hasUnalliedUnits()
{
	// TODO
	return TRUE;
}

int CRegion::resourceLevel(eItem nIndex)
{
	if (nIndex==IT_STONE) 
		return feature(RFT_STONE_LEVEL,cr_resource(CR_STONES));
	if (nIndex==IT_IRON) 
		return feature(RFT_IRON_LEVEL,cr_resource(CR_IRONS));
	if (nIndex==IT_LAEN) 
		return feature(RFT_LAEN_LEVEL,cr_resource(CR_LAEN));
	return 1;
}

void CRegion::addGuards(BOOL bAllied, int nb)
{
	(bAllied?m_nNbAlliedGuards+=nb:m_nNbOtherGuards+=nb);
}

BOOL CRegion::onConsume(const TUTItemInfo & item, const t_cost & d, CUnit * pConsumer, int nSkillLevel, int & nb)
{
	CHECKREGION
	BKPTREGION

	// TODO:pbms if several extraction of same resource
	// TODO:laen
	// feature(xxx) should be updated each time

	//trolls: only 75% of the stones quarried are substracted from the amount of stones available in a region.
	//dwarves: only 75% of the iron they mine is substracted from the iron available in a region
	int nbConsumed = 0;
	int nbExtracted = nb;
	if (!item.m_sName.CompareNoCase(EN_WOOD)) {
		if (d.woods!=1) 
			nbExtracted = (nb+1)/d.woods;
		nbConsumed = min(feature(RFT_TREES),nbExtracted);
		if (nbConsumed<nbExtracted) 
			nbConsumed += min(feature(RFT_YOUNGTREES),nbExtracted-nbConsumed);
		if (nbConsumed<nbExtracted) 
			nb = nbConsumed*d.woods;
	}
	else
	if (!item.m_sName.CompareNoCase(EN_IRON)) {
		if (d.iron!=1) 
			nbExtracted = (nb+1)/d.iron;
		if (pConsumer->isDwarf()) 
			nbExtracted = (75*nbExtracted)/100;
		nbConsumed = min(feature(RFT_IRON),nbExtracted);
		if (nbConsumed<nbExtracted) 
		{
			int nRSLevel = resourceLevel(IT_IRON)+1;
			m_nNewResourceLevels[IT_IRON] = nRSLevel;
			_warning(RESOURCELEVELINC, 5, item.m_sName, nRSLevel);
			if (nRSLevel <= nSkillLevel)
				nbConsumed = nbExtracted;
			else
			{
				//TODO: Warning("resource level more than men skill level");
				nb = nbConsumed;
				if (pConsumer->isDwarf()) 
					nb = (125*nbConsumed)/100;
				nb*=d.iron;
			}
		}
	}
	else
	if (!item.m_sName.CompareNoCase(EN_STONE)) {
		if (d.stones!=1) 
			nbExtracted = (nb+1)/d.stones;
		if (pConsumer->isTroll()) 
			nbExtracted = (75*nbExtracted)/100;
		nbConsumed = min(feature(RFT_STONES),nbExtracted);
		if (nbConsumed<nbExtracted) 
		{
			int nRSLevel = resourceLevel(IT_STONE)+1;
			m_nNewResourceLevels[IT_STONE] = nRSLevel;
			_warning(RESOURCELEVELINC, 5, item.m_sName, nRSLevel);
			if (nRSLevel <= nSkillLevel)
				nbConsumed = nbExtracted;
			else
			{
				//TODO: Warning("resource level more than men skill level");
				nb = nbConsumed;
				if (pConsumer->isTroll()) 
					nb = (125*nbConsumed)/100;
				nb*=d.stones;
			}
		}
	}
	else
	{
		if (!item.m_sName.CompareNoCase(EN_HORSE))
		{
			nbConsumed = min(feature(RFT_HORSES),nbExtracted);
			// TODO: if all extracted, warning 4: 0 horse on region
			// TODO: if nbConsumed < nbExtracted, warning 6:taming only x of y (nbmen needed...)
			nb = nbConsumed;
		}
	}

	// mark consumed resource for CR saving
	if (cr().toSave())
		addConsumedItem(m_items, item.m_nUTID, item.m_sName, UT_ITEM, nbConsumed);

	return TRUE;
}

TDemonInfos * CRegion::demon(TDemonInfos * pList, CFaction * pFaction)
{
	TDemonInfos * pDem = pList;
	while (pDem) {
		if (pDem->is(pFaction)) 
			return pDem; 
		pDem = pDem->m_pNext;
	}
	return NULL;
}

BOOL CRegion::addDemonFeed(TDemonInfos * & pList, CFaction * pFaction, int nbFeed)
{
	CHECKREGION
	BKPTREGION

	TDemonInfos * pDem = demon(pList,pFaction);
	if (!pDem) 
	{
		pDem = new TDemonInfos(pFaction);
		addlist(&pList,pDem);
	}

	pDem->addFeed(nbFeed);
	return TRUE;
}

BOOL CRegion::onMadeItem(TItemInfo * pItemInfo, int nb)
{
	CHECKREGION
	BKPTREGION

	if (!pItemInfo) 
		return errormsg("CRegion::onMadeItem: internal error - null infos");
	addMadeItem(m_items,pItemInfo->m_nID,pItemInfo->m_sName, pItemInfo->m_nType, nb);
	return TRUE;
}

TItemInfo * CRegion::item(TItemInfo * pList, int iItem, int nType)
{
	TItemInfo * pItem = pList;
	while (pItem) 
	{
		if (pItem->m_nID==iItem && pItem->m_nType==nType) 
			return pItem; 
		pItem = pItem->m_pNext;
	}
	return NULL;
}

BOOL CRegion::addMadeItem(TItemInfo * & pList, int iItem, const char *pName, int nType, int nbMade)
{
	TItemInfo * pItem = item(pList,iItem,nType);
	if (!pItem) 
	{
		// TODO: memory leak on this pointer
		pItem = new TItemInfo(iItem,pName,0,NULL,nType);
		addlist(&pList,pItem);
	}

	pItem->addMade(nbMade);
	return TRUE;
}

BOOL CRegion::addConsumedItem(TItemInfo * & pList, int iItem, const char *pName, int nType, int nbConsumed)
{
	CHECKREGION
	BKPTREGION

	TItemInfo * pItem = item(pList, iItem, nType);
	if (!pItem) 
	{
		pItem = new TItemInfo(iItem,pName,0,0,nType);
		addlist(&pList,pItem);
	}

	pItem->spend(nbConsumed);
	return TRUE;
}

BOOL CRegion::madeItemsToStr(const TItemInfo * pItems, CString & sMadeItems, eItemType nFlag, TItemInfo ** pList)
{
	const TItemInfo * pItem = pItems;
	sMadeItems="";
	while (pItem) 
	{
		if (pItem->nbMade() > 0)
		{
			CString sItem("");
			CString sName(pItem->m_sName);
			BOOL bInclude=FALSE;
			if (nFlag == ITT_ALL)
				bInclude=TRUE;
			else
			{
				int nITID=-1;
				if (nFlag & ITT_HERB) bInclude=(pItem->m_nType==UT_HERB); else 
				if (nFlag & ITT_POTION) bInclude=(pItem->m_nType==UT_POTION); else
				if (pItem->m_nType==UT_ITEM)
				{ 
					BOOL bWeapon=FALSE;
					BOOL bArmour=FALSE;
					BOOL bLuxury=FALSE;
					BOOL bResource=FALSE;
					BOOL bOtherItem=FALSE;

					if (CUnit::isWeapon(pItem->m_nID)) bWeapon=TRUE; else
					if (CUnit::isArmour(pItem->m_nID)) bArmour=TRUE; else
					if (ItemPrice(pItem->m_nID) > 0) bLuxury=TRUE; else
					if (CUnit::isItem(pItem->m_nID,&nITID)) {  if (nITID>=0 && nITID<IT_LASTCOST) bResource=TRUE; else bOtherItem=TRUE;}

					if (nFlag & ITT_WEAPON) bInclude=bWeapon; else
					if (nFlag & ITT_ARMOUR) bInclude=bArmour; else
					if (nFlag & ITT_LUXURY) bInclude=bLuxury; else 
					if (nFlag & ITT_RESOURCE) bInclude=bResource; else 
					if (nFlag & ITT_ITEM) bInclude=bOtherItem;
				}
			}

			if (bInclude)
			{
				sItem.Format("%d %s",pItem->nbMade(),(LPCSTR)sName);
				if (!sMadeItems.IsEmpty()) 
					sMadeItems+=", ";
				sMadeItems+=sItem;
			}
			if (pList)
				addMadeItem(*pList, pItem->m_nID, pItem->m_sName, pItem->m_nType, pItem->nbMade());
		}
		pItem = pItem->m_pNext;
	}
	return !sMadeItems.IsEmpty();
}

BOOL CRegion::isNeighbour(int nx, int ny) const
{
	if (m_nX==nx && m_nY==ny) 
		return FALSE;
	if (abs (abs(m_nX)-abs(nx)) > 1) 
		return FALSE;
	if (abs (abs(m_nY)-abs(ny)) > 1) 
		return FALSE;
	return TRUE;
}

BOOL CRegion::isNeighbour(CRegion * pRegion) const
{
	if (!pRegion) 
		return FALSE;
	if (pRegion==this) 
		return FALSE;
	return isNeighbour(pRegion->x(), pRegion->y());
}

CString CRegion::terrain()
{
	if (m_sType.IsEmpty())
	{
		block * r = cr_region();
		if (r)
		{
			if (cr().gets(r,CR_TERRAIN,m_sType))
				m_sType = CCR::German2English(m_sType);
		}
		else
			m_sType = EN_OCEAN; // new regions
	}
	return m_sType;
}

BOOL CRegion::cr_resolveName()
{
	if (!_strcmpi((LPCSTR)m_sName,REGION_VALUE_UNKNOWN))
	{
		CString st = terrain();
	
		if (!st.CompareNoCase(EN_OCEAN))
		{
			m_sName = st;
			return TRUE;
		}

		if (!st.IsEmpty())
			return cr().gets(cr_region(),CR_NAME,m_sName);

		return FALSE;
	}
	return TRUE;
}

BOOL CRegion::isOcean() const
{
	// TODO (?)
	// CString st = terrain();
	if (!m_sType.CompareNoCase(EN_OCEAN)) 
		return TRUE;
	if (!m_sName.CompareNoCase(EN_OCEAN)) 
		return TRUE;
	return FALSE;
}

void CRegion::AddNbBoughtItems(int nb)
{
	m_nNbBoughtItems+=nb;
}

void CRegion::AddNbSoldItems(int nb)
{
	m_nNbSoldItems+=nb;
}

void CRegion::setDefaultWage(int nDefWage)
{
	m_nDefaultWage = max(nDefWage,WAGE_MIN_VALUE);
}


block * CRegion::cr_region()
{
	if (m_cr_region) 
		return m_cr_region;
	m_cr_region = cr().region(this);
	return m_cr_region;
}

block * CRegion::cr_luxuries()
{
	if (m_cr_luxuries) 
		return m_cr_luxuries;
	block * r = cr_region();
	if (!r) 
		return NULL;
	m_cr_luxuries = cr().luxuries(r);
	return m_cr_luxuries;
}

/*
block * CRegion::cr_guards()
{
	if (m_cr_guards) return m_cr_guards;
	block * r = cr_region();
	if (!r) return NULL;
	m_cr_guards = cr().guards(r);
	return m_cr_guards;
}
*/

block * CRegion::cr_castle()
{
	if (m_cr_castle) 
		return m_cr_castle;
	block * r = cr_region();
	if (!r) 
		return NULL;
	m_cr_castle = cr().castle(r);
	return m_cr_castle;
}

block * CRegion::cr_resource(const CString & sWantedType)
{
	CHECKREGION
	BKPTREGION

	CString sType("");
	if (!m_cr_resources)
	{
		block * r = cr_region();
		if (!r) 
			return NULL;
		// NULL for some regions (Dustarborvon)
		m_cr_resources = cr().resources(r);
		if (!m_cr_resources)
			return NULL;
			//if (!cr().geti(r,sWantedType,nNb))
			//	errormsg("CRegion::cr_resource - null resource for %s while searching %s",(LPCSTR)m_sName,sWantedType);
	}
	// does not work by find with id
	block * rs = m_cr_resources;
	while (rs)
	{
		cr().gets(rs, CR_RESOURCE_TYPE, sType);
		if (!sType.CompareNoCase(sWantedType))
			break;
		// if (cr().gets(rs, CR_RESOURCE_TYPE, sType) && !sType.CompareNoCase(sWantedType))	break;
		rs = rs->next;
	}		
	if (!rs) 
		return NULL;
//		errormsg("CRegion::cr_resource - %s resource not found on %s",(LPCSTR)sWantedType,(LPCSTR)m_sName);
	return rs;
}

block * CRegion::cr_roads()
{
	if (m_cr_roads) 
		return m_cr_roads;
	block * r = cr_region();
	if (!r) 
		return NULL;
	m_cr_roads = cr().roads(r);
	return m_cr_roads;
}

block * CRegion::cr_buildings()
{
	if (m_cr_buildings) 
		return m_cr_buildings;
	block * r = cr_region();
	if (!r) return NULL;
	m_cr_buildings = cr().buildings(r);
	return m_cr_buildings;
}

block * CRegion::cr_addNewBuilding(CBuilding * pb)
{
	block * bs = cr_buildings();
	if (!bs) 
		bs = m_cr_region->children; // unuseful ?
	return  cr().onSave_addBlock(m_cr_region, bs, CR_CASTLE, pb->id());
}

block * CRegion::cr_addNewRoad(CRoad * pr)
{
	CHECKREGION
	BKPTREGION
	int nbRoads=0;
	block * bs = cr_roads();
	if (!bs) 
		bs = m_cr_region->children; // unuseful ?
	else
	{
		block * bb=bs;
		while (bb) {
			if (_strcmpi(bb->type->name,CR_BORDER))
				break;
			nbRoads++;
			bs = bb; //last road as brother
			bb = bb->next;
		}
	}
	nbRoads++;
	return  cr().onSave_addBlock(m_cr_region, bs, CR_BORDER, nbRoads);
}

block * CRegion::cr_addNewUnit(int nID, block * bMotherUnit)
{
	return  cr().onSave_addBlock(m_cr_region, bMotherUnit, CR_UNIT, nID);
}

BOOL CRegion::cr_addThroughUnit(const char *pUnitName)
{
	block * r = cr_region();
	if (!r) 
		return errormsg("cr_addThroughUnit::null cr region block");
	block * bs = cr().throughJourneyUnits(r);
	if (!bs) 
		bs = cr().onSave_addBlock(r, bs, CR_THROUGH_JOURNEY, 0);
	if (!bs) 
		return errormsg("cr_addThroughUnit::failed to create block");
	return cr().sets(bs,NULL,pUnitName);
}

BOOL CRegion::cr_addThroughShip(const char *pShipName)
{
	block * bs = NULL;
	block * r = cr_region();
	if (!r) 
	{
		// new region
		if (!save())
			return errormsg("cr_addThroughShip::null cr region block");
		r = m_cr_region;
	}
	else
		bs = cr().throughJourneyShips(r);
	if (!bs) 
		bs = cr().onSave_addBlock(r, bs, CR_THROUGH_SAILING, 0);
	if (!bs) 
		return errormsg("cr_addThroughShip::failed to create block");
	return cr().sets(bs,NULL,pShipName);
}

block * CRegion::cr_addNewShip(CShip * pb)
{
	block * bs = cr_ship(0);
	if (!bs) 
		bs = m_cr_region->children; // unuseful ?
	return  cr().onSave_addBlock(m_cr_region, bs, CR_SHIP, pb->id());
}


block * CRegion::cr_academy()
{
	if (!m_cr_academy && !m_bAcademyChecked) 
		m_cr_academy = cr_building(EN_ACADEMY);
	m_bAcademyChecked = TRUE;
	return m_cr_academy;
}

block * CRegion::cr_road(int nDirection)
{
	block * bs = cr_roads();
	return cr().roadWithDirection(bs,nDirection);
}

block * CRegion::cr_building(const char *pType)
{
	block * bs = cr_buildings();
	CString sType = CCR::English2German(pType);
	if (CCR::isCastleType(sType)) 
		return cr_castle();
	return cr().buildingWithType(bs,sType);
}

block * CRegion::cr_building(eID nID)
{
	block * r = cr_region();
	if (!r) return NULL;
	return cr().buildings(r,&nID);
}

block * CRegion::cr_ship(eID nID)
{
	block * r = cr_region();
	if (!r) return NULL;
	return cr().ships(r,&nID);
}

BOOL CRegion::cr_hasAcademy()
{
	return cr_academy()!=NULL;
}

int CRegion::feature(eRegionFeature n, block *b)
{
	CHECKREGION
	BKPTREGION

	block *bf=b;
	if (!b) 
		bf = cr_region();
	if (m_nFeatures[n] == VALUE_UNKNOWN) {
		if (!cr().reg_i(n,bf,m_nFeatures[n])) {
			m_nFeatures[n]=0;
			if (n!=RFT_TREES && n!=RFT_YOUNGTREES && n!=RFT_CASTLESIZE && n!=RFT_PEASANTS && n!=RFT_HORSES)
				errormsg("feature %s not found on %s",CCR::reg_featureInEnglish(n),m_sName);
		}
	}
	return m_nFeatures[n];
}

CString CRegion::herbName()
{
	block * b = cr_region();
	if (m_sHerbName.IsEmpty())
	{
		cr().reg_s(RFT_HERB, b, m_sHerbName);
		m_sHerbName = CCR::German2English(m_sHerbName);
		if (m_sHerbName.IsEmpty()) 
			m_sHerbName=S_VALUE_UNKNOWN_HERB;
	}
	return m_sHerbName;
}

int CRegion::cr_wage() { return max(feature(RFT_WAGE),m_nDefaultWage);}
int CRegion::cr_castleSize() { return max(feature(RFT_CASTLESIZE,cr_castle()),0);}

int CRegion::cr_buildingSize(int nID, BOOL bIsCastle, block * & pCRBuilding)
{ 
	int nSize=0;
	if (bIsCastle) 
		return cr_castleSize();
	if (!pCRBuilding) 
		pCRBuilding = cr_building(nID);
	cr().reg_i(RFT_CASTLESIZE, pCRBuilding, nSize);
	return nSize;
}

int CRegion::cr_shipSize(int nID, block * & p_cr_ship)
{ 
	int nSize=0;
	if (!p_cr_ship) 
		p_cr_ship = cr_ship(nID);
	cr().reg_i(RFT_CASTLESIZE, p_cr_ship, nSize);
	return nSize;
}

BOOL CRegion::cr_hasBuilding(const char *pTypeName)
{
	return cr_building(pTypeName)!=NULL;
}

BOOL CRegion::cr_hasShip(const char *pNameType)
{
	//TODO
	return FALSE;
}

BOOL CRegion::cr_hasShip(eID nID)
{
	if (invalidID(nID))	
		return FALSE;
	return (cr_ship(nID)!=NULL);
}

int CRegion::cr_wageForRace(BOOL bIsOrc)
{
	int n =cr_wage();
	if (bIsOrc) 
		n-=e_orcWageDiff(cr_castleSize());
	return n;
}

void CRegion::ResetSilver() 
{
	CHECKREGION
	BKPTREGION

	m_money.reset();
}

const char *CRegion::outputName(BOOL bWithCoords) const
{
	static CString sAnsiName("");
	sAnsiName = UTF8_to_ansi(m_sName);
	if (!_strcmpi(name(),EN_OCEAN))
		bWithCoords = TRUE;

	if (s_bNameWithCoords || bWithCoords)
	{
		static CString sOutputName("");

		sOutputName.Format("%s (%d,%d)",(LPCSTR)sAnsiName,m_nX,m_nY);
		return (LPCSTR)sOutputName;
	}
	return (LPCSTR)sAnsiName;
}

void CRegion::AddNbPersons(int nNbPersons, BOOL bAsRecruited)
{
	m_nNbPersons += nNbPersons;
	if (bAsRecruited) m_nNbRecruits+=nNbPersons;
}

void CRegion::AddNbDemonFeed(int nNb, CFaction *pFaction)
{
	CHECKREGION
	BKPTREGION

	//by faction
	addDemonFeed(m_demons, pFaction, nNb);
}

void CRegion::AddReservedSilver(int nSilver)
{
	CHECKREGION
	BKPTREGION
	if (m_money.m_nReserved == 0 && nSilver > 0)
		s_withReservedMoney.AddTail(this);
	m_money.m_nReserved+= nSilver;
}

void CRegion::AddSilver(const TMoney & money, BOOL bUnitHasMoved)
{

	if (!money.empty())
	{
		CHECKREGION
		BKPTREGION

		m_money.m_nMoney += money.m_nMoney;

		if (!bUnitHasMoved)
		{
			m_money.m_nByEntertain += money.m_nByEntertain;
			m_money.m_nByTax += money.m_nByTax;
			m_money.m_nByWork += money.m_nByWork;
			m_money.m_nBySell += money.m_nBySell;
			m_money.m_nByBuy += money.m_nByBuy;
		}
	}
}

void CRegion::SpendSilver(int nSilver, BOOL bAsUpkeep)
{
	CHECKREGION
	BKPTREGION
	m_money.m_nMoney-= nSilver;
	if (bAsUpkeep) 
		m_money.m_nUpkeep+=nSilver;
}

BOOL CRegion::checkSilver(BOOL bReservedSilver) const 
{ 
	CHECKREGION
	BKPTREGION

	// TODO/TO KNOW:on which money RESERVE order can be taken ?
	// RESERVE, or silver checking
	if (allSilver()<(bReservedSilver?m_money.m_nReserved:0))
	{
		outputSetRegionName(outputName());
		int nWarnLevel=6;
		if (!m_bHasOrderedUnits) 
			nWarnLevel=10;
		if (bReservedSilver) 
			return _warning(RESERVEDTOOMUCH, nWarnLevel, "silver",m_money.m_nReserved, allSilver());
		return _warning(REGIONMISSSILVER,nWarnLevel,-allSilver());
	}
	return TRUE;
}

int CRegion::reservedSilver() const 
{ 
	return m_money.m_nReserved;
}

int CRegion::allSilver() const
{
	// TODO - debug (bad value there - not enough)
	CHECKREGION
	BKPTREGION

	return m_money.m_nMoney;
}

/*
BOOL CRegion::isGuarded()
{
	if (!cr().Loaded()) return TRUE;
	block * b = cr_guards();
	return (b!=NULL);
}
*/

////////////////////////////////

/*
BOOL CRegion::isGuardedByAlliance()
{
	// TODO
	if (!cr().Loaded()) return TRUE;
	block * b = cr_guards();

	return (b!=NULL);
}
*/

////////////////////////////////

int CRegion::maxEntertain(BOOL bOnlyWhenEntertaining)
{
	int *pf = &m_nFeatures[RFT_MAXENTERTAIN];
	if (*pf == VALUE_UNKNOWN)
	{
		*pf = 0;
		if (m_money.m_nByEntertain > 0 || !bOnlyWhenEntertaining) 
		{
			int silver = feature(RFT_SILVER);
			if (silver > 0)	
				*pf = silver/20;
		}
	}
	return 	*pf;
}

int CRegion::maxTax()
{
	int *pf = &m_nFeatures[RFT_MAXTAX];
	if (*pf == VALUE_UNKNOWN)
	{
		*pf = 0;
		if (!isOcean())
		{
			int nbpeasants = feature(RFT_PEASANTS);
			if (nbpeasants > 0)	
				*pf = nbpeasants * e_taxationCoef(cr_castleSize());
			if (*pf<0) 
				*pf=0;
		}
	}
	return *pf;
}

int CRegion::maxRecruits(BOOL bOnlyWhenRecruiting)
{
	if (m_nNbRecruits > 0 || bOnlyWhenRecruiting)
		return feature(RFT_MAXRECRUITS);
	return VALUE_UNKNOWN;
}

BOOL CRegion::Stats(TItemInfo * & pItemsList)
{
	// nb men, final silver pool
	if (nbPersons() > 0)
	{
	CHECKREGION
	BKPTREGION

		int nMax=15;
		CString sTax("");
		CString sEntertain("");
		CString sRegion("");
		CString sNbPersons;
		CString st("");
		CString se("");

		// only nb persons for all factions with password

		sNbPersons.Format("%d",m_nNbPersons);
		CString sRegionOutputName = outputName();
		CString sRegionName(sRegionOutputName.Left(nMax));

		if (sRegionOutputName.GetLength()>nMax)
			sRegionName=sRegionOutputName.Left(nMax-3)+"...";
		CString sCoords("");
		if (m_nX==LOCATION_UNKNOWN_VALUE) 
			sCoords="?,?"; 
		else 
			sCoords.Format("%+d,%+d",m_nX,m_nY);
		sRegion.Format("%-*s | %s",nMax,(LPCSTR)sRegionName,(LPCSTR)sCoords);
		int nmaxTax = maxTax();
		int nmaxEntertain = maxEntertain();
		if (nmaxTax > 0) st.Format("[%d]",nmaxTax);
		if (m_money.m_nByTax > 0) 
			sTax.Format("%d %7s",m_money.m_nByTax,(LPCSTR)st);
		else 
			sTax=st;
		if (nmaxEntertain > 0) 
			se.Format("[%d]",nmaxEntertain);
		if (m_money.m_nByEntertain > 0)
			sEntertain.Format("%d %7s",m_money.m_nByEntertain,(LPCSTR)se);
		else
			sEntertain=se;
		CString sMadeItems("");
		madeItemsToStr(m_items,sMadeItems,ITT_ALL,&pItemsList);
		char cGuardedStatus = ' ';
		if (!isOcean())
		{
			// -, =, o
			BOOL bGuardedByUs = (m_nNbAlliedGuards > 0);
			BOOL bGuardedByNotAllied = (m_nNbOtherGuards > 0);
			if (bGuardedByNotAllied && !bGuardedByUs) 
				cGuardedStatus = '-';
			if (bGuardedByNotAllied && bGuardedByUs) 
				cGuardedStatus = '=';
			if (!bGuardedByNotAllied && !bGuardedByUs) 
				cGuardedStatus = 'o';
		}
		else
		{
			// show boats on this ocean
			CShip * pShip = NULL;
			if (sShipsList.find(pShip,ID_UNKNOWN,this))
			{
				CString sShip;
				sShip.Format("[%s]",pShip->fullName());
				sMadeItems += sShip;
			}

		}
		STOutput("%-27s | %5s | %13s | %13s | %7d | %c | %s",(LPCSTR)sRegion,(LPCSTR)sNbPersons,(LPCSTR)sTax,(LPCSTR)sEntertain,allSilver(),cGuardedStatus,(LPCSTR)sMadeItems);
		return TRUE;
	}
	return FALSE;
}

/*
BOOL CRegion::IsGuardedByNotAllied(int nPlayerID)
{
	block * b = cr_guards();
	if (!b) return FALSE;
	int nPrevID=-1;
	int nFactionID = -1;
	while (b) {
		cr().geti(b, CR_PLAYER, nFactionID);
		if (nFactionID!=nPrevID)
		{
			nPrevID = nFactionID;
			CFaction * pFaction = NULL;
			if (AddGuardingFaction(nFactionID,pFaction) && !pFaction->isAllied()) return TRUE;
		}
	}
	return FALSE;
}
*/
/*
BOOL CRegion::AddGuardingFaction(int nFactionID, CFaction * & pFaction)
{
	//TODO
	CString sFactionID = itob(nFactionID);
	return FALSE;
}
*/

BOOL CRegion::canBuyLuxury(int nItemID)
{
	return (nItemID==buyableLuxuryItemID());
}

BOOL CRegion::retrieveLuxuriesInfos()
{
	BOOL bOK = FALSE;
	block *b = cr_luxuries();
	if (!b) 
		return bOK;
	for (int i = 0; i < NBLUXURIES; i++)
	{
		int n = feature((eRegionFeature)i,b);
		if (n < 0) 
		{
			if (bOK) 
				return errormsg("luxuries CR internal error - two buyable items !");
			m_nBuyableLuxuryItemID = s_nItemsID[i];
			m_nBuyableLuxuryItemIndex = i;
			bOK = TRUE;
		}
	}
	return bOK;
}

int CRegion::price(int nItemID, int *pIndex)
{
	for (int i=0;i<NBLUXURIES;i++)
		if (s_nItemsID[i]==nItemID)
		{
			if (pIndex) 
				*pIndex=i;
			return feature((eRegionFeature)i,cr_luxuries());
		}
	return 0;
}

BOOL CRegion::luxuryName(int nItemID, CString & sResult, BOOL bEnglish)
{
	sResult = "";
	for (int i=0;i<NBLUXURIES;i++)
	{
		if (s_nItemsID[i]==nItemID)
		{
			sResult=CCR::reg_feature(eRegionFeature(i));
			if (bEnglish) 
				sResult=CCR::German2English(sResult);
			return TRUE;
		}
	}
	return FALSE;	
}

int CRegion::buyableLuxuryItemID()
{
	if (m_nBuyableLuxuryItemID==VALUE_UNKNOWN)
		retrieveLuxuriesInfos();
	return m_nBuyableLuxuryItemID;
}

BOOL CRegion::canSellLuxury(int nItemID)
{
	// can sell all what it cannot be bought
	return (nItemID!=buyableLuxuryItemID());
}

BOOL CRegion::canBuy(TTradeItem * pBuy, int & ns)
{
	if (!pBuy) return FALSE;
	if (pBuy->m_nItemID!=m_nBuyableLuxuryItemID) 
		return FALSE;
	if (pBuy->m_nPrice >= 0)
		return FALSE;
	ns = nbMaxLuxuriesToSell();
	return TRUE;
}

BOOL CRegion::onBuy(TTradeItem * pBuy)
{
	if (!pBuy) return FALSE;
	m_nNbBoughtItems += pBuy->m_nNb;
	return TRUE;
}

CString CRegion::buyableLuxuryName(BOOL bEnglish)
{
	CString sName("luxury");
	int nPrice;
	int nIndex = -1;
	int nItemID=buyableLuxuryItemID();
	if (nItemID==VALUE_UNKNOWN) return sName;
	luxuryInfos(nItemID,sName,nPrice,bEnglish);
	return sName;
}

BOOL CRegion::luxuryInfos(int nItemID, CString & sName, int & nPrice, BOOL bEnglish)
{
	int nIndex = -1;
	nPrice = price(nItemID,&nIndex);
	if (nIndex==-1) 
		return FALSE;
	sName=CCR::reg_feature(eRegionFeature(nIndex));
	if (bEnglish) 
		sName=CCR::German2English(sName);
	return TRUE;
}

BOOL CRegion::onSell(TTradeItem * pSell)
{
	if (canSell(pSell))
	{
		m_nNbSoldItems+=pSell->m_nNb;
		return TRUE;
	}
	return FALSE;
}

BOOL CRegion::canSell(TTradeItem * pSell)
{
	if (!pSell) 
		return FALSE;
	if (pSell->m_nItemID==m_nBuyableLuxuryItemID) 
		return FALSE;
	int pr = price(pSell->m_nItemID);
	if (pSell->m_nPrice > 0)
		return TRUE;
	return FALSE;
}

int CRegion::nbMaxLuxuriesToSell()
{
	// trade not possible if castlesize<2
	if (cr_castleSize() < CASTLESIZE_TRADEPOST) 
		return 0;

	return feature(RFT_PEASANTS)/100;
}

BOOL CRegion::onCheckTrade()
{
	if (m_nNbSoldItems == 0 && m_nNbBoughtItems==0) 
		return TRUE;
	int nbMaxTrade = nbMaxLuxuriesToSell();
	if (nbMaxTrade==0) 
		return FALSE;

	//it's possible to buy more than nbmaxTrade
	if (m_nNbBoughtItems > nbMaxTrade) 
		return _error(REGIONSELLTOOMUCH,m_nNbBoughtItems,nbMaxTrade);
	//TODO: transfer money (%age according to castle size) from the sellers to the castle owner 
	//int nCastleSize = cr_castleSize();

	return TRUE;
}

BOOL CRegion::onCheckWork()
{
	//TODO:check max workers according to region type & nbtrees
	//1 tree= -8 for maxworker
	return TRUE;
}

BOOL CRegion::onCheckEntertain()
{
	int nLevel=4;
	if (m_money.m_nByEntertain > 0)
	{
		int nmaxEntertain = maxEntertain();
		if (nmaxEntertain > 0 && m_money.m_nByEntertain > nmaxEntertain) 
		{
			m_bMoneyWarning=TRUE;
			return _warning(REGIONENTERTAINTOOMUCH,nLevel,m_money.m_nByEntertain,nmaxEntertain);
		}
	}
	return TRUE;
}

BOOL CRegion::onCheckTax()
{
	int nLevel=4;
	if (m_money.m_nByTax > 0)
	{
		int nmaxTax = maxTax();
		if (m_money.m_nByTax > nmaxTax) 
		{
			m_bMoneyWarning=TRUE;
			return _warning(REGIONTAXTOOMUCH,nLevel,m_money.m_nByTax,nmaxTax);
		}
	}
	return TRUE;
}

BOOL CRegion::onCheckMaxSilver()
{
	int nLevel=5;
	//int nmax = maxTax()+maxEntertain();
	int nmax = maxTax();
	int nmaxEntertain = maxEntertain();
	int nspent=min(m_money.m_nByTax,nmax)+min(m_money.m_nByEntertain,nmaxEntertain);
	if (nmax > 0 && nspent > nmax) 
	{
		if (m_bMoneyWarning || (nmax>nmaxEntertain)) 
			nLevel = 6;
		m_bMoneyWarning = TRUE;
		return _warning(REGIONSPENDTOOMUCH,-nLevel,nspent-nmax);
	}
	return TRUE;
}

BOOL CRegion::onCheckBelongers()
{
	CHECKREGION
	BKPTREGION

	if (m_bWar)
	{
		POSITION pos = m_units.GetHeadPosition();
		while (pos) 
			m_units.GetNext(pos)->checkPeacefulMen(this);
	}

	if (m_bHasOrderedUnits) {
	//TODO: final peasant blood to verify
		TDemonInfos * pDem = m_demons;
		while (pDem) {
			int nbFeed=pDem->feed();
			if (nbFeed<-9) {
				CString sName(pDem->faction()->outputName());
				ERTRACE("f demon=%s",(LPCSTR)sName);
				_warning(REGIONPEASANTSEATEN,-1,(LPCSTR)sName,-nbFeed/10);
			}
			pDem = pDem->m_pNext;
		}
	}
	return TRUE;
}

BOOL CRegion::onCheckRecruits()
{
	if (m_nNbRecruits > 0)
	{
		int nmaxRecruits = maxRecruits();
		if (nmaxRecruits > 0 && m_nNbRecruits > nmaxRecruits) 
			return _error(REGIONRECRUITSTOOMUCH,m_nNbRecruits,nmaxRecruits);
	}
	return TRUE;
}

BOOL CRegion::onCheckEnoughSilver()
{
	return checkSilver(FALSE);
}

void CRegion::onCheck()
{
	CHECKREGION
	BKPTREGION

	onCheckTax();
	onCheckEntertain();
	onCheckMaxSilver();
	onCheckEnoughSilver();
	onCheckRecruits();
	onCheckBelongers();
}

BOOL CRegion::loadLuxuriesIDs()
{
	if (s_nItemsID[0]!=VALUE_UNKNOWN) 
		return TRUE;
	for (int i= 0; i < NBLUXURIES; i++) {
		s_nItemsID[i]=finditem(CCR::German2English(CCR::reg_feature(eRegionFeature(i))));
		if (s_nItemsID[i]==-1)
			errormsg("luxury item %d not found in CR",i);
	}
	return (s_nItemsID[0]!=VALUE_UNKNOWN);
}

BOOL CRegion::_warning(int nErrorNum, int nLevel, ...) const
{
	outputSetRegionName(outputName());
	va_list argList;
	va_start(argList, nLevel);
	region_error(nLevel,nErrorNum,argList);
	va_end(argList);
	return FALSE;
}

BOOL CRegion::_error(int nErrorNum, ...) const
{
	outputSetRegionName(outputName());
	va_list argList;
	va_start(argList, nErrorNum);
	region_error(0,nErrorNum,argList);
	va_end(argList);
	return FALSE;
}

BOOL CRegion::save()
{
	CHECKREGION
	BKPTREGION

	SpendUpkeep();

	block * r = cr_region();
	BOOL bNew = (r==NULL);
	// unknown regions set as new Ocean
	if (bNew)
	{
		if (!cr().onSave_addRegionBlock(m_cr_region,x(),y()))
			return errormsg("failed to save region %s",outputName(TRUE));
		m_sType=EN_OCEAN;
		m_sName=EN_OCEAN;
	}

	// update resources - comsumed items are extracted
	TItemInfo * pItem = m_items;
	while (pItem) 
	{
		int nb = pItem->nbSpent();
		if (nb > 0)
		{
			int nInitialLevel=0;
			int nNewLevel=0;
			block *br=NULL;
			int nNbInitial=0;
			int nNb=0;
			BOOL bTrees=FALSE;
			CString sResourceGermanName("");
			CString sPrevSuffix(CR_PREV);
			//TODO:laen
			if (!pItem->m_sName.CompareNoCase(EN_WOOD))
			{
				nNbInitial = feature(RFT_TREES);
				nNb = nNbInitial - nb;
				sResourceGermanName = CR_TREES;
				bTrees=TRUE;
			}
			else
			if (!pItem->m_sName.CompareNoCase(EN_IRON))
			{
				nNbInitial = feature(RFT_IRON);
				nInitialLevel = resourceLevel(IT_IRON);
				nNewLevel =  nInitialLevel;
				nNb = nNbInitial - nb;
				sResourceGermanName = CR_IRONS;
				sPrevSuffix=CR_PREVS;
			}
			else
			if (!pItem->m_sName.CompareNoCase(EN_STONE))
			{
				nNbInitial = feature(RFT_STONES);
				nInitialLevel = resourceLevel(IT_STONE);
				nNewLevel =  nInitialLevel;
				nNb = nNbInitial - nb;
				sResourceGermanName = CR_STONES;
			}
			else
			if (!pItem->m_sName.CompareNoCase(EN_HORSE))
			{
				nNbInitial = feature(RFT_HORSES);
				nNb = max(0,nNbInitial-nb);
				sResourceGermanName = CR_HORSES;
			}

			if (nNb!=nNbInitial)
			{
				// no RESOURCE block for horses
				if (sResourceGermanName.CompareNoCase(CR_HORSES))
					br=cr_resource(bTrees?DE_RESOURCETYPE_TREES:sResourceGermanName);
				int nbYoungTreesToRemove = 0;
				if (nNb < 0) 
				{
					if (bTrees) 
					{
						nbYoungTreesToRemove+=-nNb;
						nNb=0;
					}
					else
					{
						nNb = 9999; 
						nNewLevel++;
						if (br)
							cr().seti(br,"skill",nNewLevel);
					}
				}
				//TODO:pbm on prev value
				CString sPrevName;
				sPrevName.Format("%s%s",sPrevSuffix,sResourceGermanName);
				if (br)
					cr().seti(br,"number",nNb);
				//if (nNb!=0) cr().seti(br,"number",nNb); else cr().onSave_remove(br); /for trees? or all?
				cr().seti(m_cr_region,sResourceGermanName,nNb);
				cr().seti(m_cr_region,sPrevName,nNbInitial);

				if (bTrees && nbYoungTreesToRemove>0) 
				{
					int nbInitialYoungTrees = feature(RFT_YOUNGTREES);
					int nbYoungTrees = max(0,nbInitialYoungTrees-nbYoungTreesToRemove);
					br=cr_resource(DE_RESOURCETYPE_YOUNGTREES);
					cr().seti(m_cr_region,CR_YOUNGTREES,nbYoungTrees);
					cr().seti(m_cr_region,CR_YOUNGTREES_PREV,nbInitialYoungTrees);
				}
			}
		}
		pItem = pItem->m_pNext;
	}

	if (!bNew)
	{
		CUnit * pInitialOwner = initialOwner();
		if (pInitialOwner && updateNewName(pInitialOwner))
			cr().sets(m_cr_region,CR_NAME,m_sNewName);
	}

	return TRUE;
}

CUnit * CRegion::owner()
{
	block * bct = cr_castle();
	if (bct)
	{
		eID nCastleID = bct->ids[0];
		CBuilding * pBuilding = NULL;
		// TODO: doesnt work everytime
		if (sBuildingsList.find(pBuilding, nCastleID, this) && pBuilding)
			return pBuilding->owner();
	}
	return NULL;
}

CUnit * CRegion::initialOwner()
{
	block * bct = cr_castle();
	if (bct)
	{
		eID nCastleID = bct->ids[0];
		CBuilding * pBuilding = NULL;
		// TODO: doesnt work everytime
		if (sBuildingsList.find(pBuilding, nCastleID, this) && pBuilding)
			return pBuilding->initialOwner();
	}
	return NULL;
}

void CRegion::checkWithReservedMoney()
{
	POSITION pos = s_withReservedMoney.GetHeadPosition();
	while (pos)	
		s_withReservedMoney.GetNext(pos)->checkSilver(TRUE);
}

void CRegion::SpendUpkeep()
{
	CHECKREGION
	BKPTREGION 

	CString sName(outputName());
	if (!sName.CompareNoCase("ocean (-5,-9)"))
		int hhhjjj=1;

	// units are sorted by faction name
	BOOL bOnlyFromSameFaction = TRUE;
	POSITION pos = m_units.GetHeadPosition();
	while (pos) 
	{
		CUnit * pUnit = m_units.GetNext(pos);
		int nSilver = 0;
		if (!pUnit->SpendUpkeepCost(nSilver))
		{
			// not enough ùmoney
		}
	}

	for (int i=1; i<=2; i++)
	{
		pos = m_units.GetHeadPosition();
		while (pos) 
		{
			CUnit * pUnit = m_units.GetNext(pos);
			if (pUnit->notEnoughMoney() && pUnit->isAllied())
			{
				int nSilver = -pUnit->money();
				if (nSilver > 0)
				{
					//find nSilver from other unit, any faction
					POSITION pos2 = m_units.GetHeadPosition();
					while (pos2 && (nSilver > 0)) 
					{
						CUnit * pGiver = m_units.GetNext(pos2);
						if (pGiver!=pUnit)
							pGiver->onGiveMaxMoney(nSilver, pUnit, bOnlyFromSameFaction);
					}
				}
			}
		}
		bOnlyFromSameFaction=FALSE;
	}
}

void CRegion::moneyCheckings()
{
	checkWithReservedMoney();
}

BOOL CRegion::cr_hasAnHarbour()
{
	return cr_hasBuilding(EN_HARBOUR);
}

////////////////////
