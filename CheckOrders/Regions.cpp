// RegionsList.cpp: implementation of the CRegions class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EcheckTypes.h"
#include "Unit.h"
#include "Region.h"
#include "Units.h"
#include "Regions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegions::CRegions(BOOL bDestroyable):m_madeItems(NULL), m_bDestroyable(bDestroyable)
{

}

CRegions::~CRegions()
{
	TItemInfo *pi=m_madeItems;
	TItemInfo *pinext=NULL;
	while (pi) {
		pinext = pi->m_pNext;
		delete (pi);
		pi = pinext;
	}

	if (m_bDestroyable) {
		POSITION pos=GetHeadPosition();
		while  (pos) delete (GetNext(pos));
	}
	RemoveAll();
}

//Add a region, or only pers if region already exists
CRegion * CRegions::Add(int x, int y, int pers, const char * name) 
{
	CRegion *r = NULL;
	CRegion *R = NULL;

	//if (name) r = FromName(name);	
	if (!r) 
		r = FromCoords(x,y);

	if (!r)
	{
		r= new CRegion(x,y,pers,name);
		AddTail(r);
	}
	else
		r->AddNbPersons(pers);
	return r;
}

BOOL CRegions::FindUnit(eID nUnitID, CRegion *& pRegion) const
{
	CRegion *r=NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	{
		r = GetNext(pos);
		if (r->cr_hasUnit(nUnitID))
		{
			pRegion = r;
			return TRUE;
		}
	}
	return FALSE;
}

void CRegions::onCheckAll() const
{
	CRegion *r=NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	{
		r = GetNext(pos);
		r->onCheck();
	}
}

void CRegions::CheckSilver(BOOL bReserved) const
{
	POSITION pos = GetHeadPosition();
	while (pos)	GetNext(pos)->checkSilver(bReserved);
}

void CRegions::ResetSilver() const
{
	CRegion *r=NULL;
	POSITION pos = GetHeadPosition();
	// directly money is moved with the units, 
	// therefore the pool empty and after the movement again to fill
	while (pos)	GetNext(pos)->ResetSilver();	
}


CRegion * CRegions::FromName(const char * name) 
{
	CRegion *r=NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	{
		r = GetNext(pos);
		if (!_strcmpi(r->name(),name))
			return r;
	}
	return NULL;
}

CRegion * CRegions::FromCoords(int x, int y) 
{
	CRegion *r=NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	{
		r = GetNext(pos);
		if (r->PositionIs(x,y))
			return r;
	}
	return NULL;
}

void CRegions::Stats(int nSortFlag) 
{
	ERTRACE("region statistics...");

	if (nSortFlag == SORT_ON_POOLSILVER) Sort(CRegion::compareOnSilver); else
	if (nSortFlag == SORT_ON_MEN) Sort(CRegion::compareOnMen);

	output("\n================================\n");
	output("\nRegions after orders:\n");
	output("--------------------------------------------------------------------------------------------------\n");
	output("     region     |   coords  |  men  | tax     [max] | entertain max |   pool  | g | produced items\n");
	output("--------------------------------------------------------------------------------------------------\n");

	int nTotalTax=0;
	int nTotalEntertain=0;
	int nTotalSilver=0;
	
	POSITION pos = GetHeadPosition();
	while (pos)	
	{
		CRegion *r=GetNext(pos);
		if (r->Stats(m_madeItems))
		{
			nTotalTax+=r->amountTax();
			nTotalEntertain+=r->amountEntertain();
			nTotalSilver+=r->allSilver();
		}
	}
	CString sTax("");
	CString sEntertain("");

	sTax.Format("%d",nTotalTax);
	sEntertain.Format("%d",nTotalEntertain);

	output("-----------------------------------------------------------------------------------\n");
	output("Total:                              | %-13s | %-13s | %5d\n\n",(LPCSTR)sTax,(LPCSTR)sEntertain,nTotalSilver);

	CString s("");
	if (CRegion::madeItemsToStr(m_madeItems,s,ITT_WEAPON))	output("made weapons    : %s\n",(LPCSTR)s);
	if (CRegion::madeItemsToStr(m_madeItems,s,ITT_ARMOUR))	output("made armours    : %s\n",(LPCSTR)s);
	if (CRegion::madeItemsToStr(m_madeItems,s,ITT_HERB))	output("made herbs      : %s\n",(LPCSTR)s);
	if (CRegion::madeItemsToStr(m_madeItems,s,ITT_POTION))	output("made potions    : %s\n",(LPCSTR)s);
	if (CRegion::madeItemsToStr(m_madeItems,s,ITT_LUXURY))	output("made luxuries   : %s\n",(LPCSTR)s);
	if (CRegion::madeItemsToStr(m_madeItems,s,ITT_RESOURCE))output("made resources  : %s\n",(LPCSTR)s);
	if (CRegion::madeItemsToStr(m_madeItems,s,ITT_ITEM))	output("made other items: %s\n",(LPCSTR)s);
	ERTRACE("end region statistics");

}		

void CRegions::save()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->save();
}
