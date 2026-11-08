// RegionsList.cpp: implementation of the CCRRegions class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "CR.h"
#include "CRRegion.h"
#include "CRRegions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRRegions::CCRRegions(BOOL bDestroyable)/*:m_madeItems(NULL)*//*,CRegionList(bDestroyable)*/
{

}

CCRRegions::~CCRRegions()
{
	POSITION pos=GetHeadPosition();
	while  (pos) delete (GetNext(pos));
	RemoveAll();
}

CCRRegion * CCRRegions::Add(CCR & cr, block *r, const char * name) 
{
	CCRRegion * pr = new CCRRegion(cr,r,name);
	AddTail(pr);
	return pr;
}
/*
//Add a region, or only pers if region already exists
CCRRegion * CCRRegions::Add(int x, int y, int pers, const char * name) 
{
	CCRRegion *r = NULL;
	CCRRegion *R = NULL;

	//if (name) r = FromName(name);	
	if (!r) r = FromCoords(x,y);

	if (!r)
	{
		r= new CCRRegion(x,y,pers,name);
		AddTail(r);
	}
	else
		r->AddNbPersons(pers);
	return r;
}
*/
BOOL CCRRegions::FindUnit(eID nUnitID, CCRRegion *& pRegion) const
{
	CCRRegion *r=NULL;
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
/*
void CCRRegions::onCheckAll() const
{
	CCRRegion *r=NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	{
		r = GetNext(pos);
		r->onCheck();
	}
}*/

/*
void CCRRegions::CheckSilver(BOOL bReserved) const
{
	POSITION pos = GetHeadPosition();
	while (pos)	GetNext(pos)->checkSilver(bReserved);
}

void CCRRegions::ResetSilver() const
{
	CCRRegion *r=NULL;
	POSITION pos = GetHeadPosition();
	// directly money is moved with the units, 
	// therefore the pool empty and after the movement again to fill
	while (pos)	GetNext(pos)->ResetSilver();	
}
*/

CCRRegion * CCRRegions::FromName(const char * name) 
{
	CCRRegion *r=NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	{
		r = GetNext(pos);
		if (!_strcmpi(r->name(),name))
			return r;
	}
	return NULL;
}

CCRRegion * CCRRegions::FromCoords(int x, int y) 
{
	CCRRegion *r=NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	{
		r = GetNext(pos);
		if (r->PositionIs(x,y))
			return r;
	}
	return NULL;
}
/*
void CCRRegions::Stats(int nSortFlag) 
{

	if (nSortFlag == SORT_ON_POOLSILVER) Sort(CCRRegion::compareOnSilver); else
	if (nSortFlag == SORT_ON_MEN) Sort(CCRRegion::compareOnMen);

	output("\n================================\n");
	output("\nRegions after orders:\n");
	output("--------------------------------------------------------------------------------------------\n");
	output("     region     | coord  | men  |    tax       |  entertain  | silver |     | produced items\n");
	output("--------------------------------------------------------------------------------------------\n");

	int nTotalTax=0;
	int nTotalEntertain=0;
	int nTotalSilver=0;
	
	POSITION pos = GetHeadPosition();
	while (pos)	
	{
		CCRRegion *r=GetNext(pos);
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

	output("-----------------------------------------------------------------------------\n");
	output("Total:                          | %-11s  | %-11s | %5d\n\n",(LPCSTR)sTax,(LPCSTR)sEntertain,nTotalSilver);

	CString s("");
	if (CCRRegion::madeItemsToStr(m_madeItems,s,ITT_WEAPON))	output("made weapons    : %s\n",(LPCSTR)s);
	if (CCRRegion::madeItemsToStr(m_madeItems,s,ITT_ARMOUR))	output("made armours    : %s\n",(LPCSTR)s);
	if (CCRRegion::madeItemsToStr(m_madeItems,s,ITT_HERB))	output("made herbs      : %s\n",(LPCSTR)s);
	if (CCRRegion::madeItemsToStr(m_madeItems,s,ITT_POTION))	output("made potions    : %s\n",(LPCSTR)s);
	if (CCRRegion::madeItemsToStr(m_madeItems,s,ITT_LUXURY))	output("made luxuries   : %s\n",(LPCSTR)s);
	if (CCRRegion::madeItemsToStr(m_madeItems,s,ITT_RESOURCE))output("made resources  : %s\n",(LPCSTR)s);
	if (CCRRegion::madeItemsToStr(m_madeItems,s,ITT_ITEM))	output("made other items: %s\n",(LPCSTR)s);
}		

void CCRRegions::save()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->save();
}
*/