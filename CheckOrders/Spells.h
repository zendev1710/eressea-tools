// SpellsList.h: interface for the CSpells class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPELLSLIST_H__315B9E67_F640_4651_AB17_3205EF0150DB__INCLUDED_)
#define AFX_SPELLSLIST_H__315B9E67_F640_4651_AB17_3205EF0150DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wLizard.h"
#include "EcheckTypes.h"
#include "Token.h"

#define SPELL_SEPARATOR_NEEDED_ITEMS		","

#define SP_ZAUBER   1
#define SP_KAMPF	2
#define SP_POST		4
#define SP_PRAE		8
#define SP_BATTLE	14	// 2+4+8
#define SP_ALL		15

class CSpells : public CTokenList  
{
public:
	CSpells();
	virtual ~CSpells();

	void Add(char *s);
	t_spell * Find(const char *s, const char *sMagie, BOOL & bInMagicSchool);
	void toTokens();

private:
		t_spell *m_pFirst;
};

#endif // !defined(AFX_SPELLSLIST_H__315B9E67_F640_4651_AB17_3205EF0150DB__INCLUDED_)
