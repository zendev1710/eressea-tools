// CamouflagedFaction.h: interface for the CCamouflagedFaction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMOUFLAGEDFACTION_H__E1BE0843_C838_43C0_B026_FAC717D614C2__INCLUDED_)
#define AFX_CAMOUFLAGEDFACTION_H__E1BE0843_C838_43C0_B026_FAC717D614C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Faction.h"

class CCamouflagedFaction : public CFaction  
{
public:
	CCamouflagedFaction(const char *pRaceName, block *bUnit);
	virtual ~CCamouflagedFaction();

	virtual block *saveMessage(int nID);

private:
	block * m_cr_unit;
};

#endif // !defined(AFX_CAMOUFLAGEDFACTION_H__E1BE0843_C838_43C0_B026_FAC717D614C2__INCLUDED_)
