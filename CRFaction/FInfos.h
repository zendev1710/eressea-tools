// FInfos.h: interface for the CFInfos class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include "wLizard.h"
#include "crdata.h"

/////////////

class CFInfos : public CObject 
{
public:
	CFInfos(int nID);
	CFInfos(block *bfaction);

	virtual ~CFInfos();

	BOOL isRace(const char *pRaceName) const;
	int id() const {return m_nID;}

	int nbTotalMen() const { return m_nNbTotalMen;}

	int outConfirm() const {return m_bOutConfirm;}

	void AddNbMen(int nb);
	
	BOOL isUnknownRace() const;

	const char * raceName() const { return m_sRaceName;	}
	const char * outputName() const;

	block * cr_faction();

	BOOL setRaceName(const char *pRaceName);

	BOOL hasPassword() const { return !m_sPswd.IsEmpty();}

	const char * name() const { return m_sName;}

private:

	void init();
	const char * sid() const;

	CString m_sPswd;
	CString m_sOutputName;
	CString m_sName;
	CString m_sRaceName;
	int m_nID;

	int m_nNbTotalMen;
	BOOL m_bOutConfirm;

	block * m_cr_faction;
};

DECLARE_LIST(CFInfos)

