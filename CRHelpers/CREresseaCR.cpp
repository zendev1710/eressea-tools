// EresseaCR.cpp: implementation of the CEresseaCR class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "crnames.h"
//#include "CheckOrders.h"
//#include "Unit.h"
//#include "kToTrace.h"
#include "crdata.h"
#include "crparse.h"
#include "hierarchy.h"
#include "CR.h"
//#include "Region.h"
#include "EresseaCR.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEresseaCR::CEresseaCR():
m_pCurrentRegion(NULL)
{
}

CEresseaCR::~CEresseaCR()
{

}

boolean CEresseaCR::OnGiveItem(CUnit *order_unit, CUnit *cmd_unit, int n, const char *itemName)
{
	if (m_bLoaded)
	{
		block *r = region(order_unit->regionFrom());
		if (!r) return warn("OnGiveItem::region %s not found", order_unit->regionName());
		
		// rechercher unités ds région courante
		const report_interface * blocks = m_data->parser->ireport;
		int nIDs = order_unit->id();
		int nIDt = cmd_unit->id();
		block * s = (block*)blocks->find(m_data, r, CR_UNIT, &nIDs, 1);

		// TODO - manage temp units
		block * t = (block*)blocks->find(m_data, r, CR_UNIT, &nIDt, 1);
		if (!s) return warn("OnGiveItem::unit source %s not found", order_unit->outputName());
		if (!t) return warn("OnGiveItem::unit target %s not found", cmd_unit->outputName());
		
		return give(s,t,n,itemName);
	}
	return false;
}

int CEresseaCR::nbItems(CUnit *unit, const char *itemName)
{
	int nb = 0;
	if (m_bLoaded && unit)
	{
		block *r = region(unit->regionFrom());
		if (!r) 
			warn("nbItems::region %s not found", order_unit->regionName());
		else
		{
			// rechercher unités ds région courante
			const report_interface * blocks = m_data->parser->ireport;
			int nIDs = unit->id();
			block * s = (block*)blocks->find(m_data, r, CR_UNIT, &nIDs, 1);
			if (s) nb = itemsNumber(s,itemName);
		}
	}
	return nb;
}


BOOL CEresseaCR::setItemsNumber(block *s_unit, const char *itemName, int nItems)
{
	block *	t = (block*)m_parser->ireport->find(m_data, s_unit, CR_ITEMS, NULL, 0);
	if (!t)
	{ 
		if (nItems == 0) return TRUE;
		block * tl = (block*)m_parser->ireport->find(m_data, s_unit, DE_TALENTS, NULL, 0);
		t = onSave_addBlock(s_unit, tl, CR_ITEMS, 0);
		if (!t)	return FALSE;
	}
	if (nItems==0)
	{
		int n=block_entry_remove(m_data, t, English2German(itemName));
		if (n==0)
		{
			n=0;
		}
		return (n==1);
	}
	m_parser->iblock->set_int(m_data,t,English2German(itemName),nItems);
	return TRUE;
}

boolean CEresseaCR::warn(const char *lpszFormat,...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	STOutputV(lpszFormat,argList,TRUE);
	va_end(argList);

	return false;
}

boolean CEresseaCR::give(block *s_unit,  block *t_unit, int n, const char *itemName)
{
	// int n = s_unit->entries();
	//s_unit->
	int nSrcItems = 0;
	int nDstItems = 0;
	block *	t = (block*)m_parser->ireport->find(m_data, s_unit, CR_ITEMS, NULL, 0);
	if (!t) return warn("give::unit %s has no item",itob(s_unit->ids[0]));
	m_parser->iblock->get_int(m_data,t,English2German(itemName),&nSrcItems);
	if (!nSrcItems) return warn("give::unit %s has no %s",itob(s_unit->ids[0]),itemName);
	if (nSrcItems < n) warn("unit %s has only %d %s - cannot give %d",itob(s_unit->ids[0]),nSrcItems,itemName,n);
//
	return false;
}

block *	CEresseaCR::region(CRegion * pRegion, BOOL bSetAsCurrent)
{
	if (!pRegion) return NULL;
	return region(pRegion->x(),pRegion->y());
}

block *	CEresseaCR::region(int x, int y, BOOL bSetAsCurrent)
{
	if (!m_bLoaded) return NULL;
	int nIDs[2];
	nIDs[0]=x;
	nIDs[1]=y;

	if (m_pCurrentRegion && m_pCurrentRegion->ids[0]==nIDs[0] && m_pCurrentRegion->ids[1]==nIDs[1]) return m_pCurrentRegion;
	block * r = (block*)m_parser->ireport->find(m_data, m_data->blocks, DE_REGION, nIDs, 2);
	if (bSetAsCurrent) m_pCurrentRegion=r;
	return r;
}

BOOL CEresseaCR::isOcean(int x, int y)
{
	block * r = region(x, y, FALSE);
	if (!r) return TRUE;
	CString sType("");
	if (gets(r,CR_TERRAIN,sType))
	{
		sType = CCR::German2English(sType);
		if (!sType.CompareNoCase(EN_OCEAN))
			return TRUE;
	}
	return FALSE;
}
