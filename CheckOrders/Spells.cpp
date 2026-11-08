// SpellsList.cpp: implementation of the CSpells class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Spells.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpells::CSpells()
{

}

CSpells::~CSpells()
{
	while (m_pFirst) { 
		t_spell *sp = m_pFirst->m_pNext; 
		free(m_pFirst->name); 
		if (m_pFirst->cost.other_items)
			free(m_pFirst->cost.other_items);
		free(m_pFirst); 
		m_pFirst = sp;
	}
}

char * tokenParam(const char * p, BOOL bLwr=FALSE)
{
	if (!p) return NULL;
	CString sh(p);
	int nInd=sh.Find('#');
	if (nInd!=-1)
		sh=sh.Mid(0,nInd);
	sh.TrimLeft();
	sh.TrimRight();
	if (bLwr) 
		sh.MakeLower();
	return _strdup((LPCSTR)sh);
}

void CSpells::Add(char *s) 
{
	t_spell *sp;
	char *params[16];
	char *parmsCost[10];
	int i;
	const BOOL LOWERCASE=TRUE;
	for (i=0;i<16;i++)
		params[i] = NULL;

	for (i=0;i<10;i++)
		parmsCost[i] = NULL;

	sp=(t_spell *)cmalloc(sizeof(t_spell));
	CToken::parse(s,";",16,params);
	sp->name = tokenParam(params[0]);

	sp->type=(char)Pow(myatoi(params[2]));
	sp->lvl = myatoi(params[3]);
	sp->rank = myatoi(params[4]);

	sp->category = 0;
	char *s_categ=tokenParam(params[5],LOWERCASE);
	if (s_categ)
	{
		if (strchr(s_categ,'s')) sp->category |= SPELL_CATEGORY_SHIP;
		if (strchr(s_categ,'d')) sp->category |= SPELL_CATEGORY_DISTANCE;
	}

	sp->secondname=NULL;
	char *sSecondName=params[6];
	if (sSecondName)
		sp->secondname = tokenParam(sSecondName);

	sp->magic_schools=NULL;
	char *sSchools=params[7];
	if (sSchools)
		sp->magic_schools = tokenParam(sSchools,LOWERCASE);

	CToken::parse(params[1],",",10,parmsCost);

	sp->cost.silver  = myatoi(parmsCost[0]);
	sp->cost.a = myatoi(parmsCost[1]);
	sp->cost.al = myatoi(parmsCost[2]);
	sp->cost.pa = myatoi(parmsCost[3]);
	sp->cost.other_items=NULL;

	CString sOtherItems("");
	for(int k=4;k<10;k++)
	{
		if (!parmsCost[k])
			break;
		if (!sOtherItems.IsEmpty())
			sOtherItems+=SPELL_SEPARATOR_NEEDED_ITEMS;
		sOtherItems+=tokenParam(parmsCost[k]);
	}
	if (!sOtherItems.IsEmpty())
		sp->cost.other_items = tokenParam(sOtherItems);

	addlist2Reversed(sp);
}

t_spell * CSpells::Find(const char *s, const char *sMagie, BOOL & bInMagicSchool) {
	t_spell *sp;
	t_spell *sp1=NULL;
	bInMagicSchool=FALSE;
	if (!s[0])
		return NULL;
	CString ss(s);
	ss.Replace("\\\"","");

	for (sp=m_pFirst; sp; sp=sp->m_pNext)
		if (sp->name && !strncasecmp(sp->name, (LPCSTR)ss, ss.GetLength()))
		{
			if (sp && _strcmpi(sMagie,"") && _strcmpi(sp->magic_schools,"") && _strcmpi(sp->magic_schools,"*"))
			{
				if (!strstr(sp->magic_schools,sMagie))
				{
					sp1=sp;
					continue;
				}
			}
			bInMagicSchool=TRUE;
			return sp;
		}
	return sp1;
}

void CSpells::toTokens() {
	int i;
	t_spell *sp;
	for (i=0,sp=m_pFirst; sp; sp=sp->m_pNext,++i)
		addtoken(&tokens[UT_SPELL], sp->name, i);
}