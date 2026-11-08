// Races.h: interface for the CRaces class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RACES_H__82C7DFAD_E531_4D47_8F96_2ADC44FD6E91__INCLUDED_)
#define AFX_RACES_H__82C7DFAD_E531_4D47_8F96_2ADC44FD6E91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Race.h"

#define NBMAXRACES	(11)

class CRaces : public CRaceList
{
public:
	CRaces(BOOL bDestroyable=TRUE);
	virtual ~CRaces();

	CRace * getByName(const char *pName) const;

};

#endif // !defined(AFX_RACES_H__82C7DFAD_E531_4D47_8F96_2ADC44FD6E91__INCLUDED_)
