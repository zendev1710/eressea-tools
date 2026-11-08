// EresseaCR.h: interface for the CMyEresseaCR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERESSEACR_H__73DC631E_E2AF_4356_B466_40E522495569__INCLUDED_)
#define AFX_ERESSEACR_H__73DC631E_E2AF_4356_B466_40E522495569__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CR.h"

#include "crdata.h"
#include "crparse.h"
#include "hierarchy.h"

class CUnit;
class CRegion;

class CMyEresseaCR : public CCR 
{
public:
	CMyEresseaCR();
	virtual ~CMyEresseaCR();
	
	boolean OnGiveItem(CUnit *order_unit, CUnit *cmd_unit, int n, const char *itemName);

	int nbItems(CUnit *unit, const char *itemName);

	block *	region(CRegion *pRegion, BOOL bSetAsCurrent=TRUE);
	block *	region(int x, int y, BOOL bSetAsCurrent=TRUE);

	BOOL	isOcean(int x, int y);

	BOOL setItemsNumber(block *s_unit, const char *itemName, int nItems);

private:
	
	block * m_pCurrentRegion;

	boolean give(block *s_unit,  block *t_unit, int n, const char *itemName);
	boolean warn(const char *format,...);

};

#endif // !defined(AFX_ERESSEACR_H__73DC631E_E2AF_4356_B466_40E522495569__INCLUDED_)
