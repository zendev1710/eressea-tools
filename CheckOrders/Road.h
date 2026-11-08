// Road.h: interface for the CRoad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROAD_H__39D6F085_5DA5_42F2_A728_B90798DA874A__INCLUDED_)
#define AFX_ROAD_H__39D6F085_5DA5_42F2_A728_B90798DA874A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wLizard.h"
#include "crdata.h"
#include "SortableObList.h"
#include "EcheckTypes.h"

class CRegion;

///////////

class CRoad : public CObject
{
public:

	CRoad(CRegion *pRegion, eCoast nCoast, block * p_cr_road = NULL, BOOL bNew = FALSE);

	virtual ~CRoad();

    static int compareOnRegion(CRoad* b1, CRoad* b2);

	BOOL checkings();
	BOOL save();

	BOOL isNew() const { return m_bNew;}

	int maxSize() const { return m_nMaxSize;}
	int currentSize() const { return m_nExtendedSize+m_nInitialSize;}
	int extendedSize() const { return m_nExtendedSize;}
	int currentPercent() const;

	void addExtendedSize(int n) { m_nExtendedSize+=n;}

	eCoast coast() const { return m_nCoast;}
	CRegion * region() const { return m_pRegion;}

protected:

private:

	CRegion * m_pRegion;
	eCoast m_nCoast;
	int m_nMaxSize;
	int m_nExtendedSize;
	int m_nInitialSize;
	BOOL m_bNew;
	block * m_cr_road;
};

DECLARE_SORTABLE_LIST(CRoad)

#endif // !defined(AFX_ROAD_H__39D6F085_5DA5_42F2_A728_B90798DA874A__INCLUDED_)
