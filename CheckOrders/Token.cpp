// Token.cpp: implementation of the CToken class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Token.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToken::CToken()
{

}

CToken::~CToken()
{

}

void CToken::parse(char *s, const char *seps, int nbmax, char **pValues)
{
	int i = 0;
	if (s==NULL) 
		return;
	char *nexttoken;
	pValues[0] = strtok_s(s,seps,&nexttoken);
	while (pValues[i] && i < nbmax-1)
	{
		char *pp=strchr(pValues[i],'#');
		if (pp) 
			*pp='\0';
		pValues[++i] = strtok_s(NULL,seps,&nexttoken);
	}
}
