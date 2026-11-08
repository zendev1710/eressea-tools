// Race.cpp: implementation of the CRace class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CheckOrders.h"
#include "wLizard.h"
#include "Race.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRace::CRace(CString sName, int nCost, TSkillsTab & modifiers, int nManWeight, int nCarryCapacity):
m_sName(sName),
m_nCost(nCost),
m_modifiers(modifiers),
m_nManWeight(nManWeight),
m_nCarryCapacity(nCarryCapacity)
{

}

CRace::~CRace()
{

}
