// Container.h: interface for the CContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTAINER_H__6582789C_1D9E_4277_AD4D_D9C59ACB312A__INCLUDED_)
#define AFX_CONTAINER_H__6582789C_1D9E_4277_AD4D_D9C59ACB312A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wLizard.h"
#include "crdata.h"
#include "SortableObList.h"
#include "EcheckTypes.h"
#include "Units.h"

class CRegion;
//class CUnit;

class CContainer : public CObject
{
public:
	CContainer(eID nContainerID, CRegion *pRegion, BOOL bNew);
	virtual ~CContainer();

	int extendedSize() const { return m_nExtendedSize;}
	int id() const { return m_nID;}

	const char * fullName() const {return (LPCSTR)m_sFullName;}
	BOOL isNew() const { return m_bNew;}

	int currentSize() const { return m_nExtendedSize+m_nInitialSize;}
	int initialSize() const { return m_nInitialSize;}

	BOOL RemoveUnit(CUnit *pUnit);

	BOOL SetOwner(CUnit * pUnit);

	BOOL hasOwner(const CUnit *pUnit = NULL) const;

	void addExtendedSize(int n) { m_nExtendedSize+=n;}


	int nbUnits() const { return m_units.GetCount();}

	CRegion * regionFrom() const { return m_pRegionFrom;}
	CUnit * owner() const { return m_pOwner;}
	CUnit * initialOwner() const { return m_pInitialOwner;}

	//////////////
	// overridable

	virtual BOOL SetInitialOwner(CUnit * pUnit);
	virtual BOOL AddUnit(CUnit *pUnit, BOOL bAsOwner = FALSE);

	virtual BOOL isBuilding() const { return TRUE;}

	const char * outputName() const {return (LPCSTR)m_sOutputName;}

	BOOL resolveOwner();


protected:

	block * cr_effects();

	BOOL updateNewName(CUnit * pOwner);

	const char * sid() const;

	CString m_sShortName;
	CString m_sNewName;
	CString m_sOutputName;
	CString m_sFullName;

	CUnit * m_pOwner;
	CUnit * m_pInitialOwner;

	CRegion * m_pRegionFrom;

	CUnits m_units;

	int m_nID;

	int m_nExtendedSize;
	int m_nInitialSize;

	int m_nInitialOwnerID;

	BOOL m_bNew;

	block * m_cr_container;
	block *m_cr_effects;
};

#endif // !defined(AFX_CONTAINER_H__6582789C_1D9E_4277_AD4D_D9C59ACB312A__INCLUDED_)
