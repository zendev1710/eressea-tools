// Token.h: interface for the CToken class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOKEN_H__A2664DE9_6FC8_4EF1_A587_8584A81955F8__INCLUDED_)
#define AFX_TOKEN_H__A2664DE9_6FC8_4EF1_A587_8584A81955F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wLizard.h"

class CToken : public CObject  
{
public:
	CToken();
	virtual ~CToken();

	static void parse(char *s, const char *seps, int nbmax, char **pValues);
};

DECLARE_LIST(CToken)

#endif // !defined(AFX_TOKEN_H__A2664DE9_6FC8_4EF1_A587_8584A81955F8__INCLUDED_)
