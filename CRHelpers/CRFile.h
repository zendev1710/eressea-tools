// CRFile.h: interface for the CCRFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRFILE_H__10222D43_A81A_456A_891D_B22433DF5D14__INCLUDED_)
#define AFX_CRFILE_H__10222D43_A81A_456A_891D_B22433DF5D14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CREresseaFile.h"
#include "crdata.h"
#include "CR.h"
//#include "EresseaCR.h"

class CCRFaction;
class CCRReport;

typedef struct TCRUnit
{
	//BOOL		m_bConfirmedOrders;
	BOOL		m_bInitialGuarding;
	CString		m_sName;
	CString		m_sID;

	//CStringList	m_sCmds;
	//CString		m_sCurrentCmd;
	POSITION	m_cmdpos;
	 
	int			m_nFactionID;
	int			m_nNbMen;
	int			m_nSilver;
} TCRUnit;

//////////

class CCRFile : public CCREresseaFile 
{
public:
	CCRFile();
	CCRFile(const char *pFilename);
	virtual ~CCRFile();

	//////////
	// overriden
	
	BOOL open();
	BOOL nextEntry();

	//////////
	// specific

	BOOL process();
	BOOL Open(const char *pCRFilename, const char *pHierarchyFilename = NULL);
	CCR & cr() { return m_cr;}
	
//	BOOL checkCmds();

	CString ResultFilename();

private:

	BOOL check_region();
	BOOL check_unit();
	BOOL check_otherParam(const char *buffer, int nFactionID);

	CCRFaction * findFaction(int nID, const char *pswd);
	BOOL setCurrentFaction(int nID, const char *pswd);

	BOOL AddUnitWithoutOrder(block *pUnit, int nFactionID, BOOL bGuarding, CCRFaction * pf = NULL);

	BOOL unitWithOrder(block * pUnit);
	BOOL regionWithOrder(block * pRegion);
	BOOL regionToBuffer(block * pRegion);
	BOOL unitToBuffer(block *pUnit);

	char * getbuf();

	BOOL nextRegion();
	BOOL nextUnit();

	//////////

	BOOL unitIsOnShip(block *bUnit, int & nContainerID, BOOL & bCaptain);
	BOOL unitIsInBuilding(block *bUnit, int & nContainerID, BOOL & bOwner);

	CCR m_cr;

	block * m_pCurrentRegion;
	block * m_pCurrentUnit;
	block * m_pRegions;
	TCRUnit m_unitInfos;
	CCRFaction * m_pCurrentFaction;

	CCRReport * m_crReport;
	int m_nLineType;

};

#endif // !defined(AFX_CRFILE_H__10222D43_A81A_456A_891D_B22433DF5D14__INCLUDED_)
