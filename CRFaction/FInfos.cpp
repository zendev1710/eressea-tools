// FactionInfos.cpp: implementation of the CFInfos class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "crnames.h"
#include "wlizard.h"
#include "CR.h"
#include "FInfos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFInfos::CFInfos(int nID):
m_sName(""),
m_nID(nID),
m_sRaceName("unknown"),
m_bOutConfirm(FALSE),
m_nNbTotalMen(0),
m_cr_faction(NULL),
m_sPswd("")
{
	init();
}


CFInfos::CFInfos(block *bfaction):
m_sName(""),
m_nID(0),
m_sRaceName(""),
m_bOutConfirm(FALSE),
m_nNbTotalMen(0),
m_cr_faction(bfaction),
m_sPswd("")
{
	if (bfaction) m_nID = bfaction->ids[0];
	init();
}

void CFInfos::init()
{
/*	if (cr_faction())
	{
		if (m_sName.IsEmpty())
			cr().gets(m_cr_faction,CR_PLAYERNAME,m_sName);
		//if (!m_pRace)
		{
			CString sRaceName("");
			if (cr().gets(m_cr_faction,CR_TYPE,sRaceName))
			{
			}
		}

	}
*/
	m_sOutputName.Format("%s (%s)",(LPCSTR)m_sName,sid());
}

BOOL CFInfos::isUnknownRace() const
{
	if (m_sRaceName.IsEmpty()) return TRUE;
	if (!m_sRaceName.CompareNoCase("unknown")) return TRUE;
	return FALSE;
}

//parm must be english
BOOL CFInfos::setRaceName(const char *pRaceName)
{
	if (isUnknownRace())
		m_sRaceName = pRaceName;
	return TRUE;
}

block * CFInfos::cr_faction()
{
//	if (m_cr_faction) return m_cr_faction;
//	m_cr_faction = cr().factions(&m_nID);
	return m_cr_faction;
}

CFInfos::~CFInfos()
{
}

void CFInfos::AddNbMen(int nb) 
{ 
	m_nNbTotalMen += nb; 
}

BOOL CFInfos::isRace(const char *pRaceName) const
{
	return !m_sRaceName.CompareNoCase(pRaceName);
}

const char * CFInfos::outputName() const
{
	return (LPCSTR)m_sOutputName;
}

const char * CFInfos::sid() const
{
	return itob(m_nID);
}


