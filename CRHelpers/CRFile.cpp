  // CRFile.cpp: implementation of the CCRFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "kToTrace.h"

#include "CR.h"
#include "CRFaction.h"
#include "CRUnit.h"
#include "CRReport.h"
#include "CRRegion.h"
#include "CRFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SPACE_REPLACEMENT		'~'
#define BUFSIZE	8192
#define SPACE								' '
#define ESCAPE_CHAR					'\\'

typedef enum CRPParam {
	P_ALLES,
	P_PEASANT,
	P_CASTLE,
	P_BUILDING,
	P_UNIT,
	P_FLEE,
	P_BEHIND,
	P_FRONT,
	P_CONTROL,
	P_HERBS,
	P_SEED,
	P_NOT,
	P_NEXT,
	P_FACTION,
	P_PERSON,
	P_REGION,
	P_SHIP,
	P_SILVER,
	P_ROAD,
	P_TEMP,
	P_PRIVAT,
	P_FIGHT,
	P_OBSERVE,
	P_GIVE,
	P_GUARD,
	P_FACTIONSTEALTH,
	P_WARN,
	P_LEVEL,
	P_HORSE,
	P_FOREIGN,
	P_AGGRESSIVE,
	P_DEFENSIVE,
	P_NUMBER,
	P_LOCALE,
	MAXPARAMS
} CRPParam;


#define NO_LINE			(0)
#define REGION_LINE		(1)
#define UNIT_LINE		(2)
//#define COMMAND_LINE	(3)

#define getstr()		igetstr(NULL)
#define igetparam(s)	findparam(igetstr2(s))

int findparam(const char *s) {
	if (!*s)
		return -1;
	if (!strcmpi(s,"UNIT")) return P_UNIT;
	if (!strcmpi(s,"REGION")) return P_REGION;
/*	if (!strcmpi(s,"UNIT")) return P_UNIT;
	if (!strcmpi(s,"UNIT")) return P_UNIT;
	if (!strcmpi(s,"UNIT")) return P_UNIT;
	if (!strcmpi(s,"UNIT")) return P_UNIT;
	if (!strcmpi(s,"UNIT")) return P_UNIT;
	if (!strcmpi(s,"UNIT")) return P_UNIT;
	if (!strcmpi(s,"UNIT")) return P_UNIT;
	if (!strcmpi(s,"UNIT")) return P_UNIT;*/

	return -1;
}

const char * extractName(CString & sName)
{
	int n1=-1;
	int n2=-1;
	n1 = sName.Find("\\\"");
	if (n1!=-1) n2 = sName.Find("\\\"",n1+2);
	if (n1!=-1 && n2!=-1 && n1 < n2)
	{
		CString sn = sName.Left(n1)+sName.Mid(n1+2,n2-n1-2)+sName.Mid(n2+2);
		sName = sn;
	}
	n1 = sName.Find('"');
	if (n1!=-1) n2 = sName.Find('"',n1+1); else n2=-1;
	if (n1!=-1 && n2!=-1 && n1 < n2)
	{
		CString sn = sName.Left(n1)+sName.Mid(n1+1,n2-n1-1)+sName.Mid(n2+1);
		sName = sn;
	}
	return sName;
}

CString wordAfter(char sep, const char *buff)
{
	CString sRes("");	
	char *x=strchr(buff, sep);
	if (x) {
		x++;
		while (isspace(*x)) x++;
		char *z=strchr(x,'\n');
		if (z) *z=0;
		sRes=(LPCSTR)(x);
	}
	return sRes;
}

const char * igetstr(const char *s1) {
	int i;
	static const char *s;
	static char buf[BUFSIZE];

	if (s1)
		s=s1;
	while (*s==SPACE)
		s++;

	for (i=0; *s && *s!=SPACE && i<(int)sizeof(buf); i++, s++) {
		buf[i]=*s;
		//C.G. 30/09/06 tabs replaced by spaces
		//if (*s=='\t') buf[i]=SPACE;
		if (*s==SPACE_REPLACEMENT) {
			if (i>0 && buf[i-1]==ESCAPE_CHAR)
				buf[--i]=SPACE_REPLACEMENT;
			else
				buf[i]=SPACE;
		}
	}
	buf[i]=0;
	if (i>0 && buf[i-1]==';')
			// A semicolon stands directly in the back to, this deletes
		buf[i-1]=0;

	return buf;
}

const char * igetstr2(const char *s1) {
	static CString s;
	s = igetstr(s1);
	return extractName(s);
}

BOOL CCRFile::check_unit()
{
	//TODO
	//to use: m_unitInfos
	return TRUE;
}

BOOL CCRFile::check_region()
{
	CString sRegionName("");
	const char *x;
	int Rx, Ry;
	// Marks the temp flag of units of the last region.
	// wrong: temp numbers with GIVES or is noticeable in such a way
//	if (combine_factions==0 && sRegionsList.GetCount()>0)
//		sUnitsList.RemoveTempStatus(NULL/*sCurrentFaction*/);
/*	if (echo_it) {
		fputs(sCurrentFile->buffer(),fOUT);
		putc('\n',fOUT);
	}*/
	x = getstr();
	if (*x) {
		if (!isdigit(*x) && (*x!='-'))
			Rx=Ry=LOCATION_UNKNOWN_VALUE; // REGION without coordinates - e.g. branch ral level
		else {
			Rx=atoi(x);
			x=strchr(buffer(), ',');
			if (!x) {
				x=strchr(buffer(), ' ');
				if (x) 
					x=strchr(++x, ' ');		// 2. Space is disconnecting switch?
				else 
					x=getstr();
			}
			if (x && *(++x))
				Ry=atoi(x);
			else
				Ry=LOCATION_BAD_VALUE;
		}
	} 
	else
		Rx=Ry=LOCATION_BAD_VALUE;
	if (Rx<LOCATION_UNKNOWN_VALUE || Ry<LOCATION_UNKNOWN_VALUE)
		Warning(ERRORREGION,0);
	sRegionName = wordAfter(';',buffer());
	//pCurrentRegion=sRegionsList.Add(Rx, Ry, 0, sRegionName);
	//outputSetRegionName(pCurrentRegion->outputName());
	return TRUE;
}

BOOL CCRFile::check_otherParam(const char *buffer, int nFactionID)
{
	if (buffer[0]==';'){}
/*		check_comment();*/
	else 
	{
		if (nFactionID && buffer[0])
			return FALSE;//Warning(NOTEXECUTED,1);
	}
	return TRUE;
}

CCRFaction * CCRFile::findFaction(int nID, const char *pswd)
{
//	t_faction * f = findFactionFromDescribed(nID, pswd);
//	return sFactionsList.Add(nID,pswd,f);
return NULL;
}

BOOL CCRFile::setCurrentFaction(int nID, const char *pswd)
{
	m_pCurrentFaction = findFaction(nID, pswd);
	return m_pCurrentFaction!=NULL;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRFile::CCRFile():CCREresseaFile(NULL),
m_pCurrentRegion(NULL),
m_pCurrentUnit(NULL),
m_pRegions(NULL),
m_nLineType(NO_LINE)
{
	m_unitInfos.m_nFactionID = 0;
}

CCRFile::CCRFile(const char *pFilename):
CCREresseaFile(pFilename),
m_pCurrentRegion(NULL),
m_pCurrentUnit(NULL),
m_pRegions(NULL)
{
	m_unitInfos.m_nFactionID = 0;
}

CCRFile::~CCRFile()
{
}

CString CCRFile::ResultFilename()
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

BOOL CCRFile::nextRegion()
{
	if (!m_pCurrentRegion) 
		return FALSE;//errormsg("nextRegion::null region");
	m_pCurrentRegion = m_pCurrentRegion->next;
	return regionWithOrder(m_pCurrentRegion);
}

BOOL CCRFile::nextUnit()
{
	if (!m_pCurrentUnit) 
		return FALSE;//errormsg("nextUnit::null unit");
	block * pUnit = m_pCurrentUnit->next;
	if (!unitWithOrder(pUnit))
		return nextRegion();
	return TRUE;
}

BOOL CCRFile::process()
{
//	outputln(PROCESSINGFILE,filename());

	m_pRegions = m_cr.regions();
	if (!regionWithOrder(m_pRegions)) 
		return FALSE;

	int nFactionID=0;

	// Find the first player, and afterwards process up to the last player
	while (!eof()) {
		const char * pBuf=buffer();	// current order line
		switch (igetparam(pBuf)) {

			case P_REGION: 
			//STDebugOutput("%s\n",pBuf); 
			check_region();
			if (!nextEntry()) return FALSE;
			break;

			case P_UNIT:
				//STDebugOutput("%s\n",pBuf); 
				nFactionID = m_unitInfos.m_nFactionID;
				if (nFactionID) {
					//scat(buffer());
					if (!check_unit()) 
						nextEntry();
					incNbUnits();
				}
				else 
					nextEntry();
				break;

				// If in READ unit one breaks off, stand there either a new player, a new unit or the file end. 
				// The SWITCH is again gone through, and called the appropriate function. 
				// One may not overwrite order_buf in any case! 
				// With all other entries here must again be filled order_buf, since 
				// the information concerned is located in only one line, and now the next must be read

			default: 
				check_otherParam(pBuf,nFactionID);
				if (!nextEntry()) return FALSE;
				break;
		}
	} // end while !eof()

	return TRUE;
}

BOOL CCRFile::nextEntry()
{
	BOOL bRes = FALSE;
	if (m_nLineType == UNIT_LINE) bRes = nextUnit(); else
	if (m_nLineType == REGION_LINE) bRes = unitToBuffer(m_pCurrentUnit);
	if (!bRes) 
		setEOF();
	return bRes;
}

BOOL CCRFile::Open(const char *pCRFilename, const char *pHierarchyFilename)
{
	if (!CCR::setHierarchyFilename(pHierarchyFilename))
		return FALSE;
	setFilename(pCRFilename);
	return open();
}

BOOL CCRFile::open()
{
	return m_cr.load(filename(),NULL,&crdata_ireport_echeck);
}

///////////

BOOL CCRFile::regionWithOrder(block * pRegion)
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

BOOL CCRFile::unitWithOrder(block * pUnit)
{
	while (pUnit && !unitToBuffer(pUnit)) 
		pUnit = pUnit->next;
	return pUnit!=NULL;
}

///////////

BOOL CCRFile::regionToBuffer(block * pRegion)
{
	if (!pRegion) 
		return FALSE;//errormsg("regionToBuffer:: null region");

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
		//ERTRACE("%s...",(LPCSTR)sBuffer);
		m_nLineType = REGION_LINE;
		m_pCurrentRegion = pRegion;
		return TRUE;
	}

	//ignore regions without at least one with-orders unit
	return FALSE;
}

///////////

BOOL CCRFile::unitIsOnShip(block *bUnit, int & nContainerID, BOOL & bCaptain)
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

BOOL CCRFile::unitIsInBuilding(block *bUnit, int & nContainerID, BOOL & bOwner)
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

BOOL CCRFile::AddUnitWithoutOrder(block *bUnit, int nFactionID, BOOL bGuarding, CCRFaction * pf)
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

			CCRUnit *pUnit = NULL;
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
				m_crReport->allRegions().Add(xr, yr, 0);
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

BOOL CCRFile::unitToBuffer(block *pUnit)
{
	if (!pUnit) 
		return FALSE;//errormsg("unitToBuffer::null unit");
	int nConfirmed=0;
	m_cr.geti(pUnit,CR_ORDERSCONFIRMED,nConfirmed);
	BOOL bOrdersConfirmed=(nConfirmed==1);
	BOOL bGuarding=FALSE;

	int nFactionID = 0;
	int nGuardingStatus = 0;
	m_cr.geti(pUnit,CR_PLAYER,nFactionID);
	if (m_cr.geti(pUnit,CR_GUARDED,nGuardingStatus) && nGuardingStatus==1) bGuarding=TRUE;

//	m_unitInfos.m_bConfirmedOrders = FALSE;
	m_unitInfos.m_bInitialGuarding = bGuarding;

	if (!bOrdersConfirmed)
		return AddUnitWithoutOrder(pUnit,nFactionID,bGuarding);

//	m_unitInfos.m_sCurrentCmd = "";
//	m_unitInfos.m_cmdpos = NULL;
//	m_unitInfos.m_sCmds.RemoveAll();

	/*		
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
	else*/
	{
		//confirmed for other factions ?
		CFaction * pf = findFaction(nFactionID,"");
		if (!pf || 	!m_cr.isUserFaction(nFactionID))
			return AddUnitWithoutOrder(pUnit,nFactionID,bGuarding,pf);
	}


	CString sBuffer;
	CString sSuffix("");
	int nContainerID=0;
	int nBuildingID=0;
	int nOwnerID=0;
	BOOL bOwner = FALSE;
	m_unitInfos.m_sID = itob(pUnit->ids[0]);
	
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

///////////
