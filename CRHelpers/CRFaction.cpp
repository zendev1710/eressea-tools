// CRFaction.cpp: implementation of the CCRFaction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "CR.h"
#include "CRRace.h"
#include "CRRaces.h"
#include "CRFaction.h"
#include "CRUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCRRaces CCRFaction::s_races;

int sRacesModifiers[NBMAXRACES][SK_MAX]=
{
{0,-1,0,0,0,0,0,0,0,0,0,0,-1,0,-2,0,0,0,0,-1,3,3,0,0,0,0,2,0},
{-1,-1,0,0,-1,0,0,0,2,0,1,0,-1,0,-2,2,0,-1,0,-1,-2,-1,1,0,0,1,0,0},
{2,0,0,-2,0,0,0,-3,0,1,-3,1,0,1,0,1,1,0,-1,0,-1,-1,1,-1,-3,1,-3,1},
{0,2,-1,0,2,0,0,-1,0,-1,-2,-2,2,1,2,0,0,2,-2,2,-2,-1,-1,0,-2,1,1,2},
{-1,-1,2,0,-2,0,0,0,0,0,2,1,-1,0,-2,1,0,-1,0,-1,-1,-1,1,0,1,0,0,0},
{0,0,-1,2,-1,1,0,1,1,0,2,0,1,-1,1,1,-1,0,-1,1,-2,-1,1,0,-1,-1,2,0},
{0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0},
{1,0,0,-1,1,0,0,-1,0,0,0,-1,1,0,1,0,0,0,0,-2,-2,-2,1,-2,0,0,-1,0},
{0,2,-2,0,0,1,0,-2,0,1,1,0,2,-1,1,1,1,0,-3,-1,0,0,-1,-1,-3,0,-1,0},
{1,1,0,-1,0,0,0,-2,-1,1,-2,-1,1,0,1,0,0,1,0,0,-1,-1,0,1,-1,1,-3,2},
{0,2,-2,0,2,0,0,-1,-3,0,-1,0,2,1,2,-1,0,2,-2,2,-1,-1,-3,-1,-1,1,0,0}
};

void CCRFaction::loadRaces()
{
	s_races.AddTail(new CCRRace(EN_AQUARIANS,80,sRacesModifiers[0]));
	s_races.AddTail(new CCRRace(EN_CATS,90,sRacesModifiers[1]));
	s_races.AddTail(new CCRRace(EN_DEMONS,150,sRacesModifiers[2]));
	s_races.AddTail(new CCRRace(EN_DWARVES,110,sRacesModifiers[3]));
	s_races.AddTail(new CCRRace(EN_ELVES,130,sRacesModifiers[4]));
	s_races.AddTail(new CCRRace(EN_HALFLINGS,80,sRacesModifiers[5]));
	s_races.AddTail(new CCRRace(EN_HUMANS,75,sRacesModifiers[6]));
	s_races.AddTail(new CCRRace(EN_INSECTS,80,sRacesModifiers[7]));
	s_races.AddTail(new CCRRace(EN_GOBLINS,50,sRacesModifiers[8],6,440));
	s_races.AddTail(new CCRRace(EN_ORCS,70,sRacesModifiers[9]));
	s_races.AddTail(new CCRRace(EN_TROLLS,90,sRacesModifiers[10],20,1080));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRFaction::CCRFaction(CCR & cr, block *b):
m_cr(cr),
m_sName(""),
m_nID(0),
//m_nRecruitCost(g_nRecruitCost),
m_sRaceName(""),
m_nKerubinStatus(KST_NEUTRAL),
m_bOutConfirm(FALSE),
m_pRace(NULL),
m_nNbMenGivenUnits(0),
m_nNbTotalMen(0),
m_nNbRecruitedMen(0),
m_nNbExistingHeroes(0),
m_nNbNewHeroes(0),
m_cr_faction(b),
m_sPswd(""),
m_nNbKnownMen(0)
{
	m_nID = b->ids[0];
	init();
}

CCR & CCRFaction::cr() { return m_cr;}

void CCRFaction::init()
{
	m_nNbUnitsWithConfirmedOrders = 0;
	if (!m_sPswd.IsEmpty())
		m_nKerubinStatus = KST_HAS_PSWD;
	if (cr_faction())
	{

		if (cr().geti(m_cr_faction,CR_KNOWN_NBMEN,m_nNbKnownMen))
		{
			int hhh=1;
		}

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

BOOL CCRFaction::isUnknownRace() const
{
	if (m_sRaceName.IsEmpty()) return TRUE;
	if (!m_sRaceName.CompareNoCase(S_VALUE_UNKNOWN)) return TRUE;
	if (!m_pRace) return TRUE;
	return FALSE;
}

//parm must be english
BOOL CCRFaction::setRaceName(const char *pRaceName)
{
	if (!isUnknownRace()) return FALSE;//errormsg("setRaceName::race already defined to %s",(LPCSTR)m_sRaceName);
	m_sRaceName = pRaceName;
	if (!m_pRace) m_pRace = s_races.getByName(m_sRaceName);
	return TRUE;
}

block * CCRFaction::cr_faction()
{
	if (m_cr_faction) return m_cr_faction;
	m_cr_faction = cr().factions(&m_nID);
	return m_cr_faction;
}

CCRFaction::~CCRFaction()
{

}

void CCRFaction::AddNewHeroes(int nb)
{
	m_nNbNewHeroes+=nb;
}

void CCRFaction::AddNbMen(int nb, BOOL bHeroes, BOOL bRecruited) 
{ 
	m_nNbTotalMen += nb; 
	if (bHeroes) m_nNbExistingHeroes+=nb;
	if (bRecruited) m_nNbRecruitedMen+=nb;
}

void CCRFaction::AddNbMenGivenUnits(int nb) 
{ 
	m_nNbMenGivenUnits += nb; 
}

int CCRFaction::nbmaxHeroes() const 
{
	//TODO: which function ? float? double?
	return log10((long double)nbTotalMen()/50)*20;
} 

BOOL CCRFaction::isRace(const char *pRaceName) const
{
	return !m_sRaceName.CompareNoCase(pRaceName);
}

int CCRFaction::raceModifier(eSkill n) const { 
	//ASSERT(m_pRace);
	if (!m_pRace) return 0;
	return m_pRace->modifier(n);
}

const TSkillsTab &  CCRFaction::raceModifiers() const { 
	ASSERT(m_pRace);
	return m_pRace->modifiers();
}


BOOL CCRFaction::Stats()
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
		CString sFactionName(m_sName);
		int nbMaxHeroes = nbmaxHeroes();
		if (nbMaxHeroes > 0)
			sNbMaxHeroes.Format("/%d",nbMaxHeroes);
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
			sFactionName = m_sName.Left(26-3)+"...";

		if (m_nNbExistingHeroes > 0 && m_nNbNewHeroes==0 && ((nbMaxHeroes-m_nNbExistingHeroes)>0))
		{
			int nbMenToRecruit = pow(10,(float)(m_nNbExistingHeroes+10)/20.0F)*50.0F-nbTotalMen();
			if (nbMenToRecruit > 0)
				sHeroInfo.Format(" ; +10 heroes = %4d recruits [promo:%d recrut:%d]",nbMenToRecruit,(nbTotalMen()+nbMenToRecruit)*10,nbMenToRecruit*nCost);
			else
				sHeroInfo.Format(" ; +10 heroes [promo:%d]",nbTotalMen()*10);
		}
//		STOutput("%-26s | %-10s | %-11s  | %s%s%s",(LPCSTR)sFactionName,(LPCSTR)m_sRaceName,(LPCSTR)sNbMen,(LPCSTR)sNbHeroes,(LPCSTR)sNbMaxHeroes,(LPCSTR)sHeroInfo);
		return TRUE;
	}
	return FALSE;
}

const char * CCRFaction::outputName() const
{
	return (LPCSTR)m_sOutputName;
}

const char * CCRFaction::sid() const
{
	return itob(m_nID);
}


//CCR & CCRFaction::cr() { return CCR::s_currentCR;}
