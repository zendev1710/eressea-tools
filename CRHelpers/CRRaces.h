// CRRaces.h: interface for the CCRRaces class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRRACES_H__82C7DFAD_E531_4D47_8F96_2ADC44FD6E91__INCLUDED_)
#define AFX_CRRACES_H__82C7DFAD_E531_4D47_8F96_2ADC44FD6E91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CRRace.h"

#define NBMAXRACES	(11)

class CCRRaces : public CCRRaceList
{
public:
	CCRRaces(BOOL bDestroyable=TRUE);
	virtual ~CCRRaces();

	CCRRace * getByName(const char *pName) const;

};

#endif // !defined(AFX_CRRACES_H__82C7DFAD_E531_4D47_8F96_2ADC44FD6E91__INCLUDED_)
