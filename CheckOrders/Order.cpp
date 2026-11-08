// Order.cpp: implementation of the COrder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Order.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// must respect K_Order enum order
CString COrder::s_keywords[MAXKEYWORDS]={
	"GROUP",
	"[MAKE TEMP]",			// MAKETEMP - not a real command - based on MAKE cmd
	"NAME",
	"DESCRIBE",
	"[GUARD NOT]",			// GUARDNOT - not a command - based on GUARD cmd
	"HELP",
	"COMBAT",
	"COMBATSPELL",
	"HIDE",
	"ORIGIN",
	"SHOW",
	"BANNER",
	"EMAIL",
	"SCHOOL",
	"OPTION",
	"PASSWORD",
	"MESSAGE",
	"USE",
	"ENTER",
	"CONTACT",
	"LEAVE",
	"ATTACK",
	"RESERVE",
	"SIEGE",
	"[ENTER SECOND ATTEMPT]", // K_ENTER_SECOND_ATTEMPT - not a command
	"[CONTACT SECOND ATTEMPT]", // K_CONTACT_SECOND_ATTEMPT - not a command
	"FOLLOW",
	"GIVE",
	"FORGET",
	"DESTROY",
	"RECRUIT",
	"RESTART",
	"QUIT",
	"CAST",
	"TEACH",
	"LEARN",
	"WORK",
	"STEAL",
	"BUY",
	"SELL",
	"MAKE",
	"SABOTAGE",
	"SPY",
	"TAX",
	"ENTERTAIN",
	"GROW",
	"CARRY",
	"RIDE",
	"MOVE",
	"ROUTE",
	"GUARD",
	"DEFAULT",
	"SORT",
	"NUMBER",
	"[ENTER THIRD ATTEMPT]", // K_ENTER_THIRD_ATTEMPT - not a command
	"[CONTACT THIRD ATTEMPT]", // K_CONTACT_THIRD_ATTEMPT - not a command
	"PROMOTION",
	"END",
	"RESEARCH",
	"FIGHT",
	"DELIVER",
	"REPORT",
	"COMMENT",
	"BREED",
	"OPINION",
	"PIRACY",
	"PREFIX",
	"CLAIM",
	"PLANT"
};

int COrder::findKeyword(const char *s)
{
	int i=0;
	for (i=0; i<MAXKEYWORDS; i++)
		if (COrder::s_keywords[i].CompareNoCase(s)==0)
			return i;
	return -1;		
}

int COrder::compareBySequence(COrder* b1, COrder* b2) 
{ 
	if (!b1) return 0;
	if (!b2) return 0;
	return (b2->m_cmd>b1->m_cmd);
}


COrder::COrder(CString sOrder, K_Order cmd):m_bProcessed(FALSE)
{
	m_sOrder=sOrder;
	m_cmd=cmd;
}


COrder::~COrder()
{

}

