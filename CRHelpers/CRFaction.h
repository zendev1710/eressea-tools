// CRFaction.h: interface for the CCRFaction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRFACTION_H__E205F96C_74BF_4913_BC4A_4DF36DFDC583__INCLUDED_)
#define AFX_CRFACTION_H__E205F96C_74BF_4913_BC4A_4DF36DFDC583__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CRTypes.h"
#include "CRRace.h"
#include "CRRaces.h"

#define KST_HAS_PSWD			(30) 
#define KST_BELONGS_FRENCH		(20) // Asm...
#define KST_AS_IF_BELONGS		(15) // Grincheux...
#define KST_BELONGS				(12) // Wolf...
#define KST_FRIEND				(10) // Black Phantom...
#define KST_NEUTRAL				(5)  // Herren der Winde...
#define KST_ENEMY				(0)  // Alazaar

class CCR;

struct block;

////////////

class CCRFaction : public CObject  
{
public:

	CCRFaction(CCR & cr, block *b);

    static int compareOnKerubinstatus(CCRFaction* f1, CCRFaction* f2) { return f1->m_nKerubinStatus < f2->m_nKerubinStatus;}

	virtual ~CCRFaction();

	int RecruitCost() const { return m_nRecruitCost;}
	BOOL isRace(const char *pRaceName) const;
	//BOOL isHelpGuardedWith(int nPlayerID);
	int id() const {return m_nID;}

	int nbMenGivenUnits() const { return m_nNbMenGivenUnits;}
	
	int nbTotalMen() const { return m_nNbTotalMen;}
	int nbTotalHeroes() const { return m_nNbExistingHeroes+m_nNbNewHeroes;}
	int nbmaxHeroes() const;

	int KerubinStatus() const {return m_nKerubinStatus;}
	int outConfirm() const {return m_bOutConfirm;}

	//int promotionCost() const { return nbMen()*;}
	
	void AddNbMenGivenUnits(int nb);
	void AddNbMen(int nb, BOOL bHeroes, BOOL bRecruited);
	void AddNewHeroes(int nb);

	BOOL HasKnownMen() const { return m_nNbKnownMen > 0;}
		
	BOOL Stats();

	BOOL isAllied() const { return m_nKerubinStatus > KST_NEUTRAL;}

	int raceModifier(eSkill n) const;
	static void loadRaces();

	BOOL isUnknownRace() const ;

	const char * raceName() const { return m_sRaceName;	}
	const char * outputName() const;

	const TSkillsTab & raceModifiers() const;

	block * cr_faction();

	BOOL setRaceName(const char *pRaceName);

	BOOL hasPassword() const { return !m_sPswd.IsEmpty();}

	void addUnitWithConfirmedOrders() { m_nNbUnitsWithConfirmedOrders++;}

	int nbUnitsWithConfirmedOrders() const { return m_nNbUnitsWithConfirmedOrders;}

	void setKerubinStatus(int nStatus) { m_nKerubinStatus = nStatus;	}
	void setOutConfirm(int nStatus) { m_bOutConfirm = (nStatus!=0);	}

	const CCRRace * race() const { return m_pRace;}

	const char * name() const { return m_sName;}

private:

	void init();
	const char * sid() const;
	CCR & cr();

	CString m_sPswd;
	CString m_sOutputName;
	CString m_sName;
	CString m_sRaceName;
	eID m_nID;

	int m_nNbMenGivenUnits;
	int m_nNbKnownMen;
	int m_nNbTotalMen;
	int m_nNbRecruitedMen;
	int m_nNbExistingHeroes;
	int m_nNbNewHeroes;
	int m_nRecruitCost;
	int m_nKerubinStatus;
	BOOL m_bOutConfirm;
	CCRRace *m_pRace;

	block * m_cr_faction;
	CCR & m_cr;

	int m_nNbUnitsWithConfirmedOrders;

	static CCRRaces s_races;
};

DECLARE_SORTABLE_LIST(CCRFaction)

#endif // !defined(AFX_CRFACTION_H__E205F96C_74BF_4913_BC4A_4DF36DFDC583__INCLUDED_)
