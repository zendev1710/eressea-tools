// CRRace.h: interface for the CCRRace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRRACE_H__24F7294D_8199_4DE2_BC5E_A222CFEAF6F2__INCLUDED_)
#define AFX_CRRACE_H__24F7294D_8199_4DE2_BC5E_A222CFEAF6F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wLizard.h"
#include "CRTypes.h"

class CCRRace : public CObject 
{
public:
	CCRRace(CString sName, int nCost, TSkillsTab & modifiers, int nManWeight=10, int fCarryCapacity100=540);
	virtual ~CCRRace();

	const char * name() const { return (LPCSTR)m_sName;}
	int cost() const { return m_nCost;}
	int manWeight() const { return m_nManWeight;}

	int modifier(eSkill n) const { if (n >= SK_MAX) return 0; return m_modifiers[n];}
	const TSkillsTab & modifiers() const { return m_modifiers;}

	int carryCapacity100() const { return m_nCarryCapacity100;}

private:

	TSkillsTab & m_modifiers;
	CString m_sName;
	int m_nCost;
	int m_nManWeight;
	int m_nCarryCapacity100;
};

DECLARE_LIST(CCRRace)

#endif // !defined(AFX_CRRACE_H__24F7294D_8199_4DE2_BC5E_A222CFEAF6F2__INCLUDED_)
