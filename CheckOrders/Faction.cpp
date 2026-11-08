// Faction.cpp: implementation of the CFaction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "crnames.h"
#include "wlizard.h"
#include "kToTrace.h"
#include "EcheckTypes.h"
#include "MyEresseaCR.h"
#include "Race.h"
#include "Races.h"
#include "Faction.h"
#include "Unit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRaces CFaction::s_races;

int s_RacesModifiers[NBMAXRACES][SK_MAX]=
{
{0,-1,0,0,0,0,0,0,0,0,0,0,-1,0,-2,0,0,0,0,-1,3,3,0,0,0,0,2,0}, //aquarians
{-1,-1,0,0,-1,0,0,0,2,0,1,0,-1,0,-2,2,0,-1,0,-1,-2,-1,1,0,0,1,0,0}, //cats
{2,0,0,-2,0,0,0,-3,0,1,-3,1,0,1,0,1,1,0,-1,0,-1,-1,1,-1,-3,1,-3,1}, //demons
{0,2,-1,0,2,0,0,-1,0,-1,-2,-2,2,1,2,0,0,2,-2,2,-2,-1,-1,0,-2,1,1,2}, //dwarves
{-1,-1,2,0,-2,0,0,0,0,0,2,1,-1,0,-2,1,0,-1,0,-1,-1,-1,1,0,1,0,0,0}, //elves
{0,0,-1,2,-1,1,0,1,1,0,2,0,1,-1,1,1,-1,0,-1,1,-2,-1,1,0,-1,-1,2,0}, //halflings
{0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0}, //humans
{1,0,0,-1,1,0,0,-1,0,0,0,-1,1,0,1,0,0,0,0,-2,-2,-2,1,-2,0,0,-1,0}, //goblins
{0,2,-2,0,0,1,0,-2,0,1,1,0,2,-1,1,1,1,0,-3,-1,0,0,-1,-1,-3,0,-1,0}, //insects
{1,1,0,-1,0,0,0,-2,-1,1,-2,-1,1,0,1,0,0,1,0,0,-1,-1,0,1,-1,1,-3,2}, //orks
{0,2,-2,0,2,0,0,-1,-3,0,-1,0,2,1,2,-1,0,2,-2,2,-1,-1,-3,-1,-1,1,0,0} //trolls
};

void CFaction::loadRaces()
{
	s_races.AddTail(new CRace(EN_AQUARIANS,RACE_RECRUIT_COST_AQUARIAN,s_RacesModifiers[0]));
	s_races.AddTail(new CRace(EN_CATS,RACE_RECRUIT_COST_CAT,s_RacesModifiers[1]));
	s_races.AddTail(new CRace(EN_DEMONS,RACE_RECRUIT_COST_DEMON,s_RacesModifiers[2]));
	s_races.AddTail(new CRace(EN_DWARVES,RACE_RECRUIT_COST_DWARF,s_RacesModifiers[3]));
	s_races.AddTail(new CRace(EN_ELVES,RACE_RECRUIT_COST_ELF,s_RacesModifiers[4]));
	s_races.AddTail(new CRace(EN_HALFLINGS,RACE_RECRUIT_COST_HALFLING,s_RacesModifiers[5]));
	s_races.AddTail(new CRace(EN_HUMANS,RACE_RECRUIT_COST_HUMAN,s_RacesModifiers[6]));
	s_races.AddTail(new CRace(EN_GOBLINS,RACE_RECRUIT_COST_GOBLIN,s_RacesModifiers[7],GOBLIN_WEIGHT,GOBLIN_CAPACITY));
	s_races.AddTail(new CRace(EN_INSECTS,RACE_RECRUIT_COST_INSECT,s_RacesModifiers[8]));
	s_races.AddTail(new CRace(EN_ORCS,RACE_RECRUIT_COST_ORC,s_RacesModifiers[9]));
	s_races.AddTail(new CRace(EN_TROLLS,RACE_RECRUIT_COST_TROLL,s_RacesModifiers[10],TROLL_WEIGHT,TROLL_CAPACITY));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFaction::CFaction(eID nID, const char *pswd, const t_faction *f):
m_sName(""),
m_sMagicSchool(""),
m_nID(nID),
m_nRecruitCost(g_nRecruitCost),
m_sRaceName(S_VALUE_UNKNOWN),
m_nKerubinStatus(KST_NEUTRAL),
m_bOutConfirm(FALSE),
m_pRace(NULL),
m_nNbMenGivenUnits(0),
m_nNbTotalMen(0),
m_nNbRecruitedMen(0),
m_nNbExistingHeroes(0),
m_nNbNewHeroes(0),
m_cr_faction(NULL),
m_sPswd("")
{
	if (f)
	{
		m_bOutConfirm = (f->nOutConfirm!=0);
		m_nKerubinStatus = f->kerubinStatus;
		m_pRace = s_races.getByName(f->race);
	}
	init();
}

CFaction::CFaction(eID nID, const char *pRaceName):
m_sName(""),
m_sMagicSchool(""),
m_nID(nID),
m_nRecruitCost(g_nRecruitCost),
m_sRaceName(pRaceName),
m_nKerubinStatus(KST_NEUTRAL),
m_bOutConfirm(FALSE),
m_pRace(NULL),
m_nNbMenGivenUnits(0),
m_nNbTotalMen(0),
m_nNbRecruitedMen(0),
m_nNbExistingHeroes(0),
m_nNbNewHeroes(0),
m_cr_faction(NULL),
m_sPswd("")
{
	m_pRace = s_races.getByName(m_sRaceName);
	init();
}

CFaction::CFaction(block *bfaction, const char * sPswd):
m_sName(""),
m_nID(FACTION_ID_UNKNOWN),
m_nRecruitCost(g_nRecruitCost),
m_sRaceName(""),
m_sMagicSchool(""),
m_nKerubinStatus(KST_NEUTRAL),
m_bOutConfirm(FALSE),
m_pRace(NULL),
m_nNbMenGivenUnits(0),
m_nNbTotalMen(0),
m_nNbRecruitedMen(0),
m_nNbExistingHeroes(0),
m_nNbNewHeroes(0),
m_cr_faction(bfaction),
m_sPswd(sPswd)
{
	if (bfaction) m_nID = bfaction->ids[0];
	init();
}

void CFaction::init()
{
	m_pLastMessage=NULL;
	m_nNbUnitsWithConfirmedOrders = 0;
	if (!m_sPswd.IsEmpty())
		m_nKerubinStatus = KST_HAS_PSWD;
	if (cr_faction())
	{
		// cr().gets(m_cr_faction,CR_MAGICSCHOOL,m_sMagicSchool);
		if (m_sName.IsEmpty())
			cr().gets(m_cr_faction,CR_PLAYERNAME,m_sName);
		if (!m_pRace)
		{
			CString sRaceName("");
			if (cr().gets(m_cr_faction,CR_TYPE,sRaceName))
				m_pRace = s_races.getByName(CCR::German2English(sRaceName));
		}

	}

	if (m_pRace) {
		m_sRaceName = m_pRace->name();
		m_nRecruitCost = m_pRace->cost();
	}
	m_sOutputName.Format("%s (%s)",(LPCSTR)m_sName,sid());
}

void CFaction::magicSchool(CString & sMagicSchool)
{
	if (m_sMagicSchool.IsEmpty())
		cr().gets(m_cr_faction,CR_MAGICSCHOOL,m_sMagicSchool);
	sMagicSchool=m_sMagicSchool;
}


BOOL CFaction::isUnknownRace() const
{
	if (m_sRaceName.IsEmpty()) return TRUE;
	if (!m_sRaceName.CompareNoCase(S_VALUE_UNKNOWN)) return TRUE;
	if (!m_pRace) return TRUE;
	return FALSE;
}

//parm must be english
BOOL CFaction::setRaceName(const char *pRaceName)
{
	if (!isUnknownRace()) return errormsg("setRaceName::race already defined to %s",(LPCSTR)m_sRaceName);
	m_sRaceName = pRaceName;
	if (!m_pRace) m_pRace = s_races.getByName(m_sRaceName);
	return TRUE;
}

block * CFaction::cr_faction()
{
	if (m_cr_faction) 
		return m_cr_faction;
	m_cr_faction = cr().factions(&m_nID);
	return m_cr_faction;
}

block * CFaction::cr_groups()
{
	block * bf=cr_faction();
	if (bf)
		return cr().groups(bf);
	return NULL;
}

CFaction::~CFaction()
{

}

void CFaction::AddNewHeroes(int nb)
{
	m_nNbNewHeroes+=nb;
}

void CFaction::AddNbMen(int nb, BOOL bHeroes, BOOL bRecruited) 
{ 
	m_nNbTotalMen += nb; 
	if (bHeroes) m_nNbExistingHeroes+=nb;
	if (bRecruited) m_nNbRecruitedMen+=nb;
}

void CFaction::AddNbMenGivenUnits(int nb) 
{ 
	m_nNbMenGivenUnits += nb; 
}

int CFaction::nbmaxHeroes() const
{	
	//TODO: which function to use? float? long double?
	return log10((long double)(nbTotalMen()/50))*20;
}


BOOL CFaction::isRace(const char *pRaceName) const
{
	return !m_sRaceName.CompareNoCase(pRaceName);
}

int CFaction::raceModifier(eSkill n) const { 
	//ASSERT(m_pRace);
	if (!m_pRace) return 0;
	return m_pRace->modifier(n);
}

const TSkillsTab &  CFaction::raceModifiers() const { 
	ASSERT(m_pRace);
	return m_pRace->modifiers();
}


BOOL CFaction::Stats()
{
	if (nbTotalMen()>0)
	{
		//int zz=1;
		int nCost=(m_pRace?m_pRace->cost():50);
		CString sNbMen("");
		CString sNbNewMen("");
		CString sNbHeroes("");
		CString sNbNewHeroes("");
		CString sNbMaxHeroes("");
		CString sHeroInfo("");
		CString sFactionNameANSI = UTF8_to_ansi(m_sName);
		CString sFactionName(sFactionNameANSI);
		int nbMaxHeroes = nbmaxHeroes();
		if (nbMaxHeroes > 0)
			sNbMaxHeroes.Format("/%-3d",nbMaxHeroes);
		if (m_nNbRecruitedMen > 0) sNbNewMen.Format("[%+d]",m_nNbRecruitedMen);
		if (m_nNbNewHeroes > 0) sNbNewHeroes.Format("[%+d]",m_nNbNewHeroes);
		sNbMen.Format("%4d %s",m_nNbTotalMen,(LPCSTR)sNbNewMen);
		if (m_nNbExistingHeroes+m_nNbNewHeroes > 0)
			sNbHeroes.Format("%3d%s",m_nNbExistingHeroes+m_nNbNewHeroes,(LPCSTR)sNbNewHeroes);
		else
		{
			if (m_nKerubinStatus >= KST_AS_IF_BELONGS)
				sNbHeroes="  0";
			else
				sNbHeroes="  x";
		}
		if (sFactionName.GetLength()>26)
			sFactionName = sFactionNameANSI.Left(26-3)+"...";

		if (nbMaxHeroes-m_nNbExistingHeroes>0 && m_nKerubinStatus >= KST_AS_IF_BELONGS)
		{
			int nbMenToRecruit=0;
			if (m_nNbNewHeroes==0)
				nbMenToRecruit = pow(10,(float)(m_nNbExistingHeroes+10)/20.0F)*50.0F-nbTotalMen();
			if (nbMenToRecruit > 0)
				sHeroInfo.Format("; +10 heroes = %d recruits [promo silver:%d. recruit silver:%d]",nbMenToRecruit,(nbTotalMen()+nbMenToRecruit)*10,nbMenToRecruit*nCost);
			else
				sHeroInfo.Format("; +10 heroes [promo silver:%d]",nbTotalMen()*10);
		}
		STOutput("%-26s | %-10s | %-11s  | %s%s%s",(LPCSTR)sFactionName,(LPCSTR)m_sRaceName,(LPCSTR)sNbMen,(LPCSTR)sNbHeroes,(LPCSTR)sNbMaxHeroes,(LPCSTR)sHeroInfo);
		return TRUE;
	}
	return FALSE;
}

block *CFaction::saveMessage(int nID)
{
	block *brother = m_pLastMessage;
	block *parent = NULL;
	if (!brother)
	{
		// retrieve PARTEI block as parent
		parent = cr_faction();
		if (parent)
		{
			block *msgs=cr().messages(parent);
			block *balz=cr().alliances(parent);
			if (balz) balz=cr().lastBrother(balz,CR_ALLIANCE);
			if (balz) brother = balz;
		}
	}

	m_pLastMessage = cr().onSave_addBlock(parent, brother, CR_MESSAGE, m_nID);
	//STTRACE_RELEASEMODE("msg de type %s",m_pLastMessage->btype);
	return m_pLastMessage;
}

const char * CFaction::outputName() const
{
	return (LPCSTR)m_sOutputName;
}

const char * CFaction::sid() const
{
	return itob(m_nID);
}


