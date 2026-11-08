// CRContainer.h: interface for the CCRContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRCONTAINER_H__6582789C_1D9E_4277_AD4D_D9C59ACB312A__INCLUDED_)
#define AFX_CRCONTAINER_H__6582789C_1D9E_4277_AD4D_D9C59ACB312A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CRTypes.h"
#include "CRUnits.h"

class CCRRegion;
class CCRUnit;
class CCR;

struct block;

//////////

class CCRContainer : public CObject
{
public:
	CCRContainer(CCR & cr, block *b, CCRRegion * pRegion);
	virtual ~CCRContainer();

	int extendedSize() const { return m_nExtendedSize;}
	int id() const { return m_nID;}

	const char * fullName() const {return (LPCSTR)m_sFullName;}

	int currentSize() const { return m_nExtendedSize+m_nInitialSize;}
	int initialSize() const { return m_nInitialSize;}

	//BOOL RemoveUnit(CCRUnit *pUnit);

	BOOL SetOwner(CCRUnit * pUnit);

	BOOL hasOwner(const CCRUnit *pUnit = NULL) const;

	void addExtendedSize(int n) { m_nExtendedSize+=n;}


	int nbUnits() const ;

	CCRRegion * regionFrom() const { return m_pRegionFrom;}
	CCRUnit * owner() const { return m_pOwner;}
	CCRUnit * initialOwner() const { return m_pInitialOwner;}

	//////////////
	// overridable

	virtual BOOL SetInitialOwner(CCRUnit * pUnit);
	virtual BOOL AddUnit(CCRUnit *pUnit, BOOL bAsOwner = FALSE);

	virtual BOOL isBuilding() const { return TRUE;}

	const char * outputName() const {return (LPCSTR)m_sOutputName;}


protected:

//	BOOL updateNewName(CUnit * pOwner);

	const char * sid() const;

	CString m_sShortName;
	CString m_sNewName;
	CString m_sOutputName;
	CString m_sFullName;

	CCRUnit * m_pOwner;
	CCRUnit * m_pInitialOwner;
	CCRRegion * m_pRegionFrom;
	CCRUnits m_units;

	int m_nID;

	int m_nExtendedSize;
	int m_nInitialSize;

	int m_nInitialOwnerID;

	block * cr_container();
	CCR & cr();

private:

	CCR & m_cr;
	block * m_cr_container;

};

#endif // !defined(AFX_CRCONTAINER_H__6582789C_1D9E_4277_AD4D_D9C59ACB312A__INCLUDED_)
