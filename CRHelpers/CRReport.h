// CRReport.h: interface for the CCRReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRREPORT_H__73DC631E_E2AF_4356_B466_40E522495569__INCLUDED_)
#define AFX_CRREPORT_H__73DC631E_E2AF_4356_B466_40E522495569__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CRShips.h"
#include "CRBuildings.h"
#include "CRFactions.h"
#include "CRRegions.h"
#include "CRUnits.h"

///////////

#define OPT_REGIONS			(1)
#define OPT_UNITS			(2)
#define OPT_SHIPS			(4)
#define OPT_BUILDINGS		(8)
#define OPT_MESSAGES		(16)
#define OPT_FACTIONS		(32)

class CCR; 

class CCRReport  
{
public:

	CCRReport(CCR & cr);
	virtual ~CCRReport();
	
	void fill(int nOptions=OPT_REGIONS|OPT_FACTIONS|OPT_UNITS);

	CCRBuildings & allBuildings() { return m_allBuildings;}
	CCRShips & allShips() { return m_allShips;}
	CCRRegions & allRegions() { return m_allRegions;}
	CCRUnits & allUnits() { return m_allUnits;}

private:

	void fillRegions(int nOptions);
	void fillUnits(int nOptions, CCRRegion * pr);
	void fillShips(int nOptions, CCRRegion * pr);

	void fillFactions(int nOptions);

	CCRBuildings		m_allBuildings;
	CCRShips			m_allShips;
	CCRRegions			m_allRegions;
	CCRUnits			m_allUnits;
	CCRFactions			m_allFactions;

	CCR &				m_cr;

};

#endif // !defined(AFX_CRREPORT_H__73DC631E_E2AF_4356_B466_40E522495569__INCLUDED_)
