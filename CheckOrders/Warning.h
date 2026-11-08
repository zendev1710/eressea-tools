// Warning.h: interface for the CWarning class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WARNING_H__4E2D7C2E_FE73_4D51_AC13_0D420B7A3EE3__INCLUDED_)
#define AFX_WARNING_H__4E2D7C2E_FE73_4D51_AC13_0D420B7A3EE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wLizard.h"

class CUnit;
class CRegion;
class CFaction;

/////////

class CWarning: public CObject  
{
public:
	CWarning();
	virtual ~CWarning();

private:

	CUnit * m_pUnit;
	CRegion * m_pRegion;
	CFaction * m_pFaction;
	int nLevel;
};

DECLARE_LIST(CWarning)

#endif // !defined(AFX_WARNING_H__4E2D7C2E_FE73_4D51_AC13_0D420B7A3EE3__INCLUDED_)
