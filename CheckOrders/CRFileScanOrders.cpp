// CRFileScanOrders.cpp: implementation of the CCRFileScanOrders class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "kToTrace.h"
#include "eCheckTypes.h"
#include "crnames.h"
#include "CR.h"
#include "Faction.h"
#include "Unit.h"
#include "Units.h"
#include "Regions.h"
#include "CRFileScanOrders.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define NO_LINE			(0)
#define REGION_LINE		(1)
#define UNIT_LINE		(2)
#define COMMAND_LINE	(3)

#define CHKUNIT if (!_strcmpi(m_unitInfos.m_sID,"q42n"))
#define CHKUNIT2 if (!_strcmpi(m_unitInfos.m_sID,"1v54"))
#define BKPTUNIT {int zzzzzz=1;}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRFileScanOrders::CCRFileScanOrders():CCREresseaFile(NULL),
m_pCurrentRegion(NULL),
m_pCurrentUnit(NULL),
m_pRegions(NULL),
m_nLineType(NO_LINE)
{
	m_unitInfos.m_nFactionID = 0;
}

CCRFileScanOrders::CCRFileScanOrders(const char *pFilename):
CCREresseaFile(pFilename),
m_pCurrentRegion(NULL),
m_pCurrentUnit(NULL),
m_pRegions(NULL)
{
	m_unitInfos.m_nFactionID = 0;
}

CCRFileScanOrders::~CCRFileScanOrders()
{
}

CString CCRFileScanOrders::ResultFilename()
{
	CString stc;
	CString stn;
	CString s = filename();
	int nTurn = m_cr.turn();
	int nTurnNext = nTurn+1;
	stc.Format("%d",nTurn);
	stn.Format("%d",nTurnNext);
	int nind = s.Replace(stc,stn);
	if (nind==0) 
	{
		stn="_"+stn;
		int n=s.ReverseFind('.');
		if (n!=-1) s = s.Left(n)+stn+".cr"; else s+=stn;
	}
	return s;
}

BOOL CCRFileScanOrders::nextRegion()
{
	if (!m_pCurrentRegion) 
		return errormsg("nextRegion::null region");
	m_pCurrentRegion = m_pCurrentRegion->next;
	return regionWithOrder(m_pCurrentRegion);
}

BOOL CCRFileScanOrders::nextUnit()
{
	if (!m_pCurrentUnit) 
		return errormsg("nextUnit::null unit");
	block * pUnit = m_pCurrentUnit->next;
	if (!unitWithOrder(pUnit))
		return nextRegion();
	return TRUE;
}

BOOL CCRFileScanOrders::nextCmd()
{
	if (!m_unitInfos.m_cmdpos) 
		return nextUnit();
	m_unitInfos.m_sCurrentCmd = m_unitInfos.m_sCmds.GetNext(m_unitInfos.m_cmdpos);
	return cmdToBuffer();
}

BOOL CCRFileScanOrders::process()
{
	outputln(PROCESSINGFILE,filename());

	m_pRegions = m_cr.regions();
	if (!regionWithOrder(m_pRegions)) 
		return FALSE;

	int nFactionID=0;

	// Find the first player, and afterwards process up to the last player
	while (!eof()) {
		const char * pBuf=buffer();	// current order line
		switch (igetparam(pBuf)) {

			case P_REGION: 
			STDebugOutput("%s\n",pBuf); 
			check_region();
			if (!nextOrder()) 
				return FALSE;
			break;

			case P_UNIT:
				STDebugOutput("%s\n",pBuf); 
				nFactionID = m_unitInfos.m_nFactionID;
				if (nFactionID) {
					//scat(buffer());
					if (!readaunit(&m_unitInfos)) 
						nextOrder();
					incNbUnits();
				}
				else 
					nextOrder();
				break;

				// If in READ unit one breaks off, stand there either a new player, a new unit or the file end. 
				// The SWITCH is again gone through, and called the appropriate function. 
				// One may not overwrite order_buf in any case! 
				// With all other entries here must again be filled order_buf, since 
				// the information concerned is located in only one line, and now the next must be read

			default: 
				check_otherParam(pBuf,nFactionID);
				if (!nextOrder()) 
					return FALSE;
				break;
		}
	} // end while !eof()

	return TRUE;
}

BOOL CCRFileScanOrders::nextOrder()
{
	BOOL bRes = FALSE;
	if (m_nLineType == COMMAND_LINE) bRes = nextCmd(); else
	if (m_nLineType == UNIT_LINE) bRes = cmdToBuffer(); else
	if (m_nLineType == REGION_LINE) bRes = unitToBuffer(m_pCurrentUnit);
	if (!bRes) 
		setEOF();
	return bRes;
}

BOOL CCRFileScanOrders::Open(const char *pCRFilename, const char *pHierarchyFilename)
{
	if (!CCR::setHierarchyFilename(pHierarchyFilename))
		return FALSE;
	setFilename(pCRFilename);
	return open();
}

BOOL CCRFileScanOrders::open()
{
	return m_cr.load(filename(),NULL,&crdata_ireport_echeck);
}

///////////

BOOL CCRFileScanOrders::regionWithOrder(block * pRegion)
{
	while (pRegion) 
	{
		if (!CCR::b_isRegion(pRegion)) 
		{
			pRegion = NULL;
			continue;
		}

		if (regionToBuffer(pRegion))
			break;

		pRegion = pRegion->next;
	}
	return (pRegion!=NULL);
}

BOOL CCRFileScanOrders::unitWithOrder(block * pUnit)
{
	while (pUnit && !unitToBuffer(pUnit)) 
		pUnit = pUnit->next;
	return pUnit!=NULL;
}

///////////

BOOL CCRFileScanOrders::regionToBuffer(block * pRegion)
{
	if (!pRegion) 
		return errormsg("regionToBuffer:: null region");

	block * pUnits = m_cr.units(pRegion);
	block * pUnitWithOrder = pUnits;
	m_pCurrentRegion = pRegion;

	if (unitWithOrder(pUnitWithOrder))
	{
		CString sName("");
		m_cr.gets(pRegion,CR_NAME,sName);
		CString sBuffer;
		int nx = pRegion->ids[0];
		int ny = pRegion->ids[1];
		sBuffer.Format("REGION %d,%d ; %s",nx,ny,(LPCSTR)sName);
		setBuffer(sBuffer);
		ERTRACE("%s...",(LPCSTR)sBuffer);
		m_nLineType = REGION_LINE;
		m_pCurrentRegion = pRegion;
		return TRUE;
	}

	//ignore regions without at least one with-orders unit
	return FALSE;
}

///////////

BOOL CCRFileScanOrders::unitIsOnShip(block *bUnit, int & nContainerID, BOOL & bCaptain)
{
	if (m_cr.geti(bUnit,CR_SHIP,nContainerID))
	{
		// ON SHIP
		block *bc = m_cr.ships(m_pCurrentRegion,&nContainerID);
		if (bc)
		{
			int nOwnerID=0;
			m_cr.geti(bc, CR_CAPTAIN, nOwnerID);
			bCaptain = (nOwnerID==bUnit->ids[0]);
			return TRUE;
		}
	}
	return  FALSE;
}

BOOL CCRFileScanOrders::unitIsInBuilding(block *bUnit, int & nContainerID, BOOL & bOwner)
{
	if (m_cr.geti(bUnit,CR_CASTLE,nContainerID))
	{
		// IN BUILDING
		block *bc = m_cr.buildings(m_pCurrentRegion,&nContainerID);
		if (bc)
		{
			int nOwnerID=0;
			m_cr.geti(bc, CR_OWNER, nOwnerID);
			bOwner = (nOwnerID==bUnit->ids[0]);
			return TRUE;
		}
	}
	return  FALSE;
}

BOOL CCRFileScanOrders::AddUnitWithoutOrder(block *bUnit, int nFactionID, BOOL bGuarding, CFaction * pf)
{
	if (!pf) 
		pf = findFaction(nFactionID,"");

	if (pf)
	{
		if (pf->isUnknownRace())
		{
			CString sRaceName("");
			if (m_cr.gets(bUnit,CR_TYPE,sRaceName))
				pf->setRaceName(CCR::German2English(sRaceName));
		}
		int nbmen = 0;
		int nHeroStatus = 0;
		BOOL bHeroes = FALSE;
		m_cr.geti(bUnit,CR_NBMEN,nbmen);
		if (pf->KerubinStatus() > KST_BELONGS)
			if (m_cr.geti(bUnit,CR_HERO,nHeroStatus) && nHeroStatus==1) 
				bHeroes=TRUE;
		pf->AddNbMen(nbmen,bHeroes,FALSE);

		if (m_pCurrentRegion)
		{

			CUnit *pUnit = NULL;
			int nContainerID = 0;
			BOOL bOwner = FALSE;
			BOOL bOnShip = FALSE;
			BOOL bInBuilding = FALSE;
			if (unitIsOnShip(bUnit, nContainerID, bOwner)) bOnShip = TRUE; else
			if (unitIsInBuilding(bUnit, nContainerID, bOwner)) bInBuilding = TRUE;

			// do not create other units
			if (bGuarding || bOnShip || bInBuilding)
			{
				int nUnitID = bUnit->ids[0];
				int xr = m_pCurrentRegion->ids[0];
				int yr = m_pCurrentRegion->ids[1];
				sRegionsList.Add(xr, yr, 0);
				if (!pUnit) pUnit = sUnitsList.Find(nUnitID,0,xr,yr);
				if (!pUnit)	sUnitsList.Add(pUnit=new CUnit(pf,nUnitID,0,xr,yr,"",nbmen));
				if (bOnShip) pUnit->setIsOnShip(nContainerID, bOwner); else
				if (bInBuilding) pUnit->setIsInBuilding(nContainerID);
				if (bGuarding) pUnit->setInitialGuarding();
			}
		}
	}
	return FALSE; //must always return FALSE because m_pCurrentUnit not modified
}

BOOL CCRFileScanOrders::unitToBuffer(block *pUnit)
{
	if (!pUnit) 
		return errormsg("unitToBuffer::null unit");
	int nConfirmed=0;
	m_cr.geti(pUnit,CR_ORDERSCONFIRMED,nConfirmed);
	BOOL bOrdersConfirmed=(nConfirmed==1);
	BOOL bGuarding=FALSE;

	int nFactionID = 0;
	int nGuardingStatus = 0;
	m_cr.geti(pUnit,CR_PLAYER,nFactionID);
	if (m_cr.geti(pUnit,CR_GUARDED,nGuardingStatus) && nGuardingStatus==1) bGuarding=TRUE;

	m_unitInfos.m_bConfirmedOrders = FALSE;
	m_unitInfos.m_bInitialGuarding = bGuarding;

	if (!bOrdersConfirmed)
		return AddUnitWithoutOrder(pUnit,nFactionID,bGuarding);

	m_unitInfos.m_sCurrentCmd = "";
	m_unitInfos.m_cmdpos = NULL;
	m_unitInfos.m_sCmds.RemoveAll();
	//CString sUnitInfo(itob(pUnit->ids[0]));
	// does not work (old infos in m_unitInfos)
	//sUnitInfo.Format("%s (%s)",(LPCSTR)m_unitInfos.m_sName,(LPCSTR)m_unitInfos.m_sID);	
	block *pCmd = m_cr.cmds(pUnit); //the first only is correct, second is talents
	if (pCmd)
	{
		entry * e = pCmd->entries;
		while (e) {
			CString s = e->data.cp;
			if (isValidOrder(s))
				m_unitInfos.m_sCmds.AddTail(s);
			e = e->next;
		}
	}
	else
	{
		//confirmed for other factions ?
		CFaction * pf = findFaction(nFactionID,"");
		if (!pf || 	!m_cr.isUserFaction(nFactionID))
			return AddUnitWithoutOrder(pUnit,nFactionID,bGuarding,pf);
	}

	m_unitInfos.m_bConfirmedOrders = TRUE;
	if (m_unitInfos.m_sCmds.GetCount()==0)
	{
		m_unitInfos.m_bConfirmedOrders=FALSE;
		m_unitInfos.m_sCmds.AddTail("// "); // to treat empty order confirmed unit
	}	

	m_unitInfos.m_cmdpos = m_unitInfos.m_sCmds.GetHeadPosition();
	m_unitInfos.m_sCurrentCmd = m_unitInfos.m_sCmds.GetNext(m_unitInfos.m_cmdpos);

	CString sBuffer;
	CString sSuffix("");
	int nContainerID=0;
	int nBuildingID=0;
	int nOwnerID=0;
	BOOL bOwner = FALSE;
	m_unitInfos.m_sID = itob(pUnit->ids[0]);
	
	CHKUNIT
	BKPTUNIT

	m_cr.geti(pUnit,CR_NBMEN,m_unitInfos.m_nNbMen);
	m_cr.gets(pUnit,CR_NAME,m_unitInfos.m_sName);

	if (unitIsOnShip(pUnit, nContainerID, bOwner))
	{
		// captain ?
		CString ss;
		char c='s';
		if (bOwner) c='S';			
		ss.Format(",%c%s",c,itob(nContainerID));
		sSuffix+=ss;
	}
	else
	if (unitIsInBuilding(pUnit, nContainerID, bOwner))
		sSuffix+=",U0";

	m_unitInfos.m_nSilver = m_cr.itemsNumber(pUnit, CR_SILVER);
		
	if (m_unitInfos.m_nFactionID!=nFactionID)
	{
		m_unitInfos.m_nFactionID = nFactionID;
		setCurrentFaction(m_unitInfos.m_nFactionID,"");
	}

	sBuffer.Format("UNIT %s; %s [%d,%d$%s]",(LPCSTR)m_unitInfos.m_sID,(LPCSTR)m_unitInfos.m_sName,m_unitInfos.m_nNbMen,m_unitInfos.m_nSilver,(LPCSTR)sSuffix);
	setBuffer(sBuffer);

	m_nLineType = UNIT_LINE;
	m_pCurrentUnit = pUnit;
	return TRUE;
}

BOOL CCRFileScanOrders::cmdToBuffer()
{
	STDebugOutput("%s\n",(LPCSTR)m_unitInfos.m_sCurrentCmd); 
	setBuffer(m_unitInfos.m_sCurrentCmd);
	m_nLineType = COMMAND_LINE;
	return TRUE;
}

///////////
