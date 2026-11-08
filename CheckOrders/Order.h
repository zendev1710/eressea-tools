// Order.h: interface for the COrder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ORDER_H__39D6F085_5DA5_42F2_A728_B90798DA874A__INCLUDED_)
#define AFX_ORDER_H__39D6F085_5DA5_42F2_A728_B90798DA874A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SortableObList.h"

///////////

typedef enum K_Order
{
	K_NO_CMD = -1,
	K_GROUP,
	K_MAKETEMP,
	K_NAME,
	K_DESCRIBE,
	K_GUARDNOT,
	K_HELP,
	K_COMBAT,
	K_COMBATSPELL,
	K_HIDE,
	K_ORIGIN,
	K_SHOW,
	K_BANNER,
	K_EMAIL,
	K_SCHOOL,
	K_OPTION,
	K_PASSWORD,
	K_MESSAGE,
	K_USE,
	K_ENTER,
	K_CONTACT,
	K_LEAVE,
	K_ATTACK,
	K_RESERVE,
	K_SIEGE,
	K_ENTER_SECOND_ATTEMPT,
	K_CONTACT_SECOND_ATTEMPT,
	K_FOLLOW,
	K_GIVE,
	K_FORGET,
	K_DESTROY,
	K_RECRUIT,
	// buildings upkeep first attempt
	K_RESTART,
	K_QUIT,
	K_CAST,
	K_TEACH,
	K_LEARN, // K_STUDY
	K_WORK,
	K_STEAL,
	K_BUY,
	K_SELL,
	K_MAKE,
	K_SABOTAGE,
	K_SPY,
	K_TAX,
	K_ENTERTAIN,
	K_GROW,
	// 1. gathering of resources (piece by piece)
	// 2. entertainment (piece by piece)
	// 3. work (piece by piece)
	// 4. taxation (in portions of 10 pieces of silver)
	// 5. buy (piece by piece)
	// 6. sell (piece by piece)
	// 7. steal (piece by piece)
	// 22. Ships with too small a crew suffer damage
	K_CARRY,
	K_RIDE,
	K_MOVE,
	K_ROUTE,
	K_GUARD,
	K_DEFAULT,
	K_SORT,
	K_NUMBER,
	K_ENTER_THIRD_ATTEMPT,
	K_CONTACT_THIRD_ATTEMPT,
	K_PROMOTION,
	// K8 under are not part of sequence orders
	K_END,
	K_RESEARCH,
	K_FIGHT,
	K_DELIVER,
	K_REPORT,
	K_COMMENT,
	K_BREED,
	K_OPINION,
	K_PIRACY,
	K_PREFIX,
	K_CLAIM,
	K_PLANT,
	MAXKEYWORDS
} K_Order;


class COrder: public CObject
{
public:

	COrder(CString sOrder, K_Order cmd);
	virtual ~COrder();

	int compareBySequence(COrder* b1, COrder* b2); 

	K_Order cmd() const { return m_cmd;}
	CString order() const { return m_sOrder;}

	BOOL processed() const { return m_bProcessed; }

	void setProcessed() { m_bProcessed = TRUE;}

	static int findKeyword(const char *s);

private:

	K_Order m_cmd;
	CString m_sOrder;
	BOOL m_bProcessed;

	static CString s_keywords[MAXKEYWORDS];

};

DECLARE_SORTABLE_LIST(COrder);

#endif // !defined(AFX_ORDER_H__39D6F085_5DA5_42F2_A728_B90798DA874A__INCLUDED_)