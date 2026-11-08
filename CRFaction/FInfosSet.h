// FInfosSet.h: interface for the CFactionsList class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wLizard.h"
#include "FInfos.h"

/////////

class CFInfosSet: public CFInfosList 
{
public:
	CFInfosSet(BOOL bDestroyable=TRUE);
	virtual ~CFInfosSet();

	CFInfos * Add(int nID);
	CFInfos * Add(block * bfaction); 

	BOOL find(CFInfos * & pFaction, int nID);

};


