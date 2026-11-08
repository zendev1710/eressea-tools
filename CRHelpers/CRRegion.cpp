// CRRegion.cpp: implementation of the CCRRegion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "CRTypes.h"
#include "Eressea.h"
#include "CR.h"
#include "CRRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CHECKREGION if (!m_sName.CompareNoCase("Píbut"))
//#define CHECKREGION if (0)
#define CHECKPIPO {int zzzzzz=1;}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRRegion::CCRRegion(CCR & cr, block *r, const char *pName): 
	m_cr(cr),
	m_nNbPersons(0),
//	m_nX(x),
//	m_nY(y),
	m_sName((pName?pName:REGION_VALUE_UNKNOWN)),
	m_cr_region(r),
	m_cr_castle(NULL),
	m_cr_resources(NULL),
	m_cr_luxuries(NULL),
	m_cr_roads(NULL),
	m_cr_buildings(NULL),
	m_cr_academy(NULL),
	m_cr_units(NULL),
	//m_items(NULL),
//	m_nDefaultWage(WAGE_MIN_VALUE),
//	m_bMallorn(FALSE),
	m_sHerbName(""),
	m_sType(""),
	m_units(FALSE)
{
	B_REGION_XY(m_nX,m_nY,r);
	cr_resolveName();
}

BOOL CCRRegion::s_bNameWithCoords = FALSE;

/*
CCRRegion::CCRRegion(int x, int y, int pers, const char *pName): 
	m_nNbPersons(pers),
	m_nX(x),
	m_nY(y),
	m_sName((pName?pName:REGION_VALUE_UNKNOWN)),
	m_cr_region(NULL),
	m_cr_castle(NULL),
	m_cr_resources(NULL),
	m_cr_luxuries(NULL),
	m_cr_roads(NULL),
	m_cr_buildings(NULL),
	m_cr_academy(NULL),
	m_cr_units(NULL),
	//m_items(NULL),
//	m_nDefaultWage(WAGE_MIN_VALUE),
//	m_bMallorn(FALSE),
	m_sHerbName(""),
	m_sType("")
{
//	int i;
	for (i = 0;i<RFT_MAX;i++)
		m_nFeatures[i] = VALUE_UNKNOWN;

	for (i = 0;i<IT_NBMAX;i++)
		m_nNewResourceLevels[i] = VALUE_UNKNOWN;

	if (pName && (!strcmpi(pName,REGIONNAME_ECHECK_NULLVALUE) || !strcmpi(pName,"")))
		m_sName=REGION_VALUE_UNKNOWN;

	if (cr().Loaded() && !m_sName.CompareNoCase(REGION_VALUE_UNKNOWN))
		cr_resolveName();
}
*/

CCRRegion::~CCRRegion()
{
}

block * CCRRegion::cr_units()
{
	if (m_cr_units) return m_cr_units;
	m_cr_units = cr().units(cr_region());
	return m_cr_units;
}

BOOL CCRRegion::cr_hasUnit(eID nUnitID)
{
	block * crunit = cr().units(cr_region(),&nUnitID);
	return crunit!=NULL;
}

BOOL CCRRegion::AddUnit(CCRUnit *pUnit)
{
	if (!m_units.Has(pUnit)) 
		m_units.AddHead(pUnit);
	return TRUE;
}

BOOL CCRRegion::hasUnalliedUnits()
{
	// TODO
	return TRUE;
}

BOOL CCRRegion::hasKnownMen()
{
	CCRUnit *u = NULL;
	POSITION pos = m_units.GetHeadPosition();
	while (pos)	{
		u = m_units.GetNext(pos);
		CCRFaction *pf = u->faction();
		if (pf && pf->HasKnownMen())
			return TRUE;
	}
	return TRUE;
}
/*
int CCRRegion::resourceLevel(eItem nIndex)
{
	if (nIndex==IT_STONE) return feature(RFT_STONE_LEVEL,cr_resource(CR_STONES));
	if (nIndex==IT_IRON) return feature(RFT_IRON_LEVEL,cr_resource(CR_IRONS));
	if (nIndex==IT_LAEN) return feature(RFT_LAEN_LEVEL,cr_resource(CR_LAEN));
	return 1;
}
*/

BOOL CCRRegion::isNeighbour(int nx, int ny) const
{
	if (m_nX==nx && m_nY==ny) return FALSE;
	if (abs (abs(m_nX)-abs(nx)) > 1) return FALSE;
	if (abs (abs(m_nY)-abs(ny)) > 1) return FALSE;
	return TRUE;
}

BOOL CCRRegion::isNeighbour(CCRRegion * pRegion) const
{
	if (!pRegion) return FALSE;
	if (pRegion==this) return FALSE;
	return isNeighbour(pRegion->x(), pRegion->y());
}

BOOL CCRRegion::isLand()
{
	CString sterrain = terrain();
	if (!sterrain.CompareNoCase(EN_OCEAN) || !sterrain.CompareNoCase(EN_FIREPROOF_WALL))
		return FALSE;
	return TRUE;
}

CString CCRRegion::terrain()
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

BOOL CCRRegion::cr_resolveName()
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

BOOL CCRRegion::isOcean() const
{
	// TODO (?)
	// CString st = terrain();
	if (!m_sType.CompareNoCase(EN_OCEAN)) return TRUE;
	if (!m_sName.CompareNoCase(EN_OCEAN)) return TRUE;
	return FALSE;
}

block * CCRRegion::cr_region()
{
	if (m_cr_region) return m_cr_region;
//	m_cr_region = cr().region(this);
	return m_cr_region;
}

block * CCRRegion::cr_luxuries()
{
	if (m_cr_luxuries) return m_cr_luxuries;
	block * r = cr_region();
	if (!r) return NULL;
	m_cr_luxuries = cr().luxuries(r);
	return m_cr_luxuries;
}

block * CCRRegion::cr_castle()
{
	if (m_cr_castle) return m_cr_castle;
	block * r = cr_region();
	if (!r) return NULL;
	m_cr_castle = cr().castle(r);
	return m_cr_castle;
}

block * CCRRegion::cr_resource(const CString & sWantedType)
{
	CHECKREGION
	CHECKPIPO

	CString sType("");
	if (!m_cr_resources)
	{
		block * r = cr_region();
		if (!r) return NULL;
		// NULL for some regions (Dustarborvon)
		m_cr_resources = cr().resources(r);
//		if (!m_cr_resources) errormsg("CCRRegion::cr_resource - null resource for %s",(LPCSTR)m_sName);
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
//	if (!rs) errormsg("CCRRegion::cr_resource - %s resource not found on %s",(LPCSTR)sWantedType,(LPCSTR)m_sName);
	return rs;
}

block * CCRRegion::cr_roads()
{
	if (m_cr_roads) return m_cr_roads;
	block * r = cr_region();
	if (!r) return NULL;
	m_cr_roads = cr().roads(r);
	return m_cr_roads;
}

block * CCRRegion::cr_buildings()
{
	if (m_cr_buildings) return m_cr_buildings;
	block * r = cr_region();
	if (!r) return NULL;
	m_cr_buildings = cr().buildings(r);
	return m_cr_buildings;
}
/*
block * CCRRegion::cr_academy()
{
	if (!m_cr_academy && !m_bAcademyChecked) m_cr_academy = cr_building(EN_ACADEMY);
	m_bAcademyChecked = TRUE;
	return m_cr_academy;
}
*/
block * CCRRegion::cr_road(int nDirection)
{
	block * bs = cr_roads();
	return cr().roadWithDirection(bs,nDirection);
}

block * CCRRegion::cr_building(const char *pType)
{
	block * bs = cr_buildings();
	CString sType = CCR::English2German(pType);
	if (CCR::isCastleType(sType)) return cr_castle();
	return cr().buildingWithType(bs,sType);
}

block * CCRRegion::cr_building(eID nID)
{
	block * r = cr_region();
	if (!r) return NULL;
	return cr().buildings(r,&nID);
}

block * CCRRegion::cr_ship(eID nID)
{
	block * r = cr_region();
	if (!r) return NULL;
	return cr().ships(r,&nID);
}

block * CCRRegion::throughJourneyUnits()
{
	return cr().throughJourneyUnits(cr_region());
}

/*
BOOL CCRRegion::cr_hasAcademy()
{
	return cr_academy()!=NULL;
}
*/

/*
int CCRRegion::feature(eRegionFeature n, block *b)
{
	if (!b) b = cr_region();
	if (m_nFeatures[n] == VALUE_UNKNOWN)
		cr().reg_i(n,b, m_nFeatures[n]);
	return m_nFeatures[n];
}
*/
CString CCRRegion::herbName()
{
	block * b = cr_region();
	if (m_sHerbName.IsEmpty())
	{
		cr().reg_s(RFT_HERB, b, m_sHerbName);
		m_sHerbName = CCR::German2English(m_sHerbName);
		if (m_sHerbName.IsEmpty()) m_sHerbName=S_VALUE_UNKNOWN_HERB;
	}
	return m_sHerbName;
}

//int CCRRegion::cr_wage() { return max(feature(RFT_WAGE),m_nDefaultWage);}
//int CCRRegion::cr_castleSize() { return max(feature(RFT_CASTLESIZE,cr_castle()),0);}
/*
int CCRRegion::cr_buildingSize(int nID, BOOL bIsCastle, block * & pCRBuilding)
{ 
	int nSize=0;
	if (bIsCastle) return cr_castleSize();
	if (!pCRBuilding) pCRBuilding = cr_building(nID);
	cr().reg_i(RFT_CASTLESIZE, pCRBuilding, nSize);
	return nSize;
}

int CCRRegion::cr_shipSize(int nID, block * & p_cr_ship)
{ 
	int nSize=0;
	if (!p_cr_ship) p_cr_ship = cr_ship(nID);
	cr().reg_i(RFT_CASTLESIZE, p_cr_ship, nSize);
	return nSize;
}
*/
/*
BOOL CCRRegion::cr_hasBuilding(const char *pTypeName)
{
	return cr_building(pTypeName)!=NULL;
}

BOOL CCRRegion::cr_hasShip(const char *pNameType)
{
	//TODO

	return FALSE;
}


BOOL CCRRegion::cr_hasShip(eID nID)
{
	if (invalidID(nID))	return FALSE;
	return (cr_ship(nID)!=NULL);
}

int CCRRegion::cr_wageForRace(BOOL bIsOrc)
{
	int n =cr_wage();
	if (bIsOrc) n-=e_orcWageDiff(cr_castleSize());
	return n;
}
*/
const char *CCRRegion::outputName(BOOL bWithCoords) const
{
	if (!_strcmpi(name(),EN_OCEAN)) bWithCoords = TRUE;

	if (s_bNameWithCoords || bWithCoords)
	{
		static CString sOutputName("");
		sOutputName.Format("%s (%d,%d)",name(),m_nX,m_nY);
		return (LPCSTR)sOutputName;
	}
	return name();
}

void CCRRegion::AddNbPersons(int nNbPersons)
{
	m_nNbPersons += nNbPersons;
}

/*
CUnit * CCRRegion::owner()
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

CUnit * CCRRegion::initialOwner()
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
*/
/*
BOOL CCRRegion::cr_hasAnHarbour()
{
	return cr_hasBuilding(EN_HARBOUR);
}
*/
////////////////////

CCR & CCRRegion::cr() { return m_cr;}
