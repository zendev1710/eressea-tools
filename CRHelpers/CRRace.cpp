// CRRace.cpp: implementation of the CCRRace class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wLizard.h"
#include "CRRace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRRace::CCRRace(CString sName, int nCost, TSkillsTab & modifiers, int nManWeight, int nCarryCapacity100):
m_sName(sName),
m_nCost(nCost),
m_modifiers(modifiers),
m_nManWeight(nManWeight),
m_nCarryCapacity100(nCarryCapacity100)
{

}

CCRRace::~CCRRace()
{

}
