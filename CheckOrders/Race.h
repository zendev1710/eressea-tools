// Race.h: interface for the CRace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RACE_H__24F7294D_8199_4DE2_BC5E_A222CFEAF6F2__INCLUDED_)
#define AFX_RACE_H__24F7294D_8199_4DE2_BC5E_A222CFEAF6F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wLizard.h"
#include "CRTypes.h"

class CRace : public CObject 
{
public:
	CRace(CString sName, int nCost, TSkillsTab & modifiers, int nManWeight=MAN_WEIGHT, int fCarryCapacity=MAN_CAPACITY);
	virtual ~CRace();

	const char * name() const { return (LPCSTR)m_sName;}
	int cost() const { return m_nCost;}
	int manWeight() const { return m_nManWeight;}

	int modifier(eSkill n) const { if (n >= SK_MAX) return 0; return m_modifiers[n];}
	const TSkillsTab & modifiers() const { return m_modifiers;}

	int carryCapacity() const { return m_nCarryCapacity;}

private:

	TSkillsTab & m_modifiers;
	CString m_sName;
	int m_nCost;
	int m_nManWeight;
	int m_nCarryCapacity;
};

DECLARE_LIST(CRace)

#endif // !defined(AFX_RACE_H__24F7294D_8199_4DE2_BC5E_A222CFEAF6F2__INCLUDED_)
