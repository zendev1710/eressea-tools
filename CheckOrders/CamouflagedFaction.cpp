// CamouflagedFaction.cpp: implementation of the CCamouflagedFaction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CheckOrders.h"
#include "CamouflagedFaction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCamouflagedFaction::CCamouflagedFaction(const char *pRaceName, block *bUnit):
m_cr_unit(bUnit),
CFaction(CAMOUFLAGED_PLAYER_ID,pRaceName)
{
	
}

CCamouflagedFaction::~CCamouflagedFaction()
{

}

block *CCamouflagedFaction::saveMessage(int nID) 
{
	return NULL;
}
