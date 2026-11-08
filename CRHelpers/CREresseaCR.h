// EresseaCR.h: interface for the CEresseaCR class.
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

class CCRUnit;
class CCRRegion;

class CEresseaCR : public CCR 
{
public:
	CEresseaCR();
	virtual ~CEresseaCR();
	
	int nbItems(CCRUnit *unit, const char *itemName);

	block *	region(CCRRegion *pRegion, BOOL bSetAsCurrent=TRUE);
	block *	region(int x, int y, BOOL bSetAsCurrent=TRUE);

	BOOL	isOcean(int x, int y);

	BOOL setItemsNumber(block *s_unit, const char *itemName, int nItems);

private:
	
	block * m_pCurrentRegion;

	boolean warn(const char *format,...);

};

#endif // !defined(AFX_ERESSEACR_H__73DC631E_E2AF_4356_B466_40E522495569__INCLUDED_)
