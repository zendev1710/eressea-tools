// CRFileScanOrders.h: interface for the CCRFileScanOrders class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRFILESCANORDERS_H__10222D43_A81A_456A_891D_B22433DF5D14__INCLUDED_)
#define AFX_CRFILESCANORDERS_H__10222D43_A81A_456A_891D_B22433DF5D14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CREresseaFile.h"
#include "crdata.h"
#include "CR.h"
#include "MyEresseaCR.h"

typedef struct TCRUnit
{
	BOOL		m_bConfirmedOrders;
	BOOL		m_bInitialGuarding;
	CString		m_sName;
	CString		m_sID;

	CStringList	m_sCmds;
	CString		m_sCurrentCmd;
	POSITION	m_cmdpos;
	 
	int			m_nFactionID;
	int			m_nNbMen;
	int			m_nSilver;
} TCRUnit;

//////////

class CCRFileScanOrders : public CCREresseaFile 
{
public:
	CCRFileScanOrders();
	CCRFileScanOrders(const char *pFilename);
	virtual ~CCRFileScanOrders();

	//////////
	// overriden
	
	BOOL open();
	BOOL nextEntry() { return nextOrder();}

	//////////
	// specific

	BOOL nextOrder();

	BOOL process();
	BOOL Open(const char *pCRFilename, const char *pHierarchyFilename = NULL);
	CMyEresseaCR & cr() { return m_cr;}
	
	BOOL checkCmds();

	CString ResultFilename();

private:

	BOOL AddUnitWithoutOrder(block *pUnit, int nFactionID, BOOL bGuarding, CFaction * pf = NULL);

	BOOL unitWithOrder(block * pUnit);
	BOOL regionWithOrder(block * pRegion);
	BOOL regionToBuffer(block * pRegion);
	BOOL unitToBuffer(block *pUnit);
	BOOL cmdToBuffer();

	char * getbuf();

	BOOL nextRegion();
	BOOL nextUnit();
	BOOL nextCmd();

	//////////

	BOOL unitIsOnShip(block *bUnit, int & nContainerID, BOOL & bCaptain);
	BOOL unitIsInBuilding(block *bUnit, int & nContainerID, BOOL & bOwner);

	CMyEresseaCR m_cr;

	block * m_pCurrentRegion;
	block * m_pCurrentUnit;
	block * m_pRegions;
	TCRUnit m_unitInfos;

	int m_nLineType;

};

#endif // !defined(AFX_CRFILESCANORDERS_H__10222D43_A81A_456A_891D_B22433DF5D14__INCLUDED_)
