// Message.h: interface for the CMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGE_H__694513DC_899F_425A_B929_B79971108CAF__INCLUDED_)
#define AFX_MESSAGE_H__694513DC_899F_425A_B929_B79971108CAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "crdata.h"
#include "wLizard.h"
#include "SortableObList.h"

class CMessage;

struct TMsgInfo;

typedef BOOL func_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi);

extern func_msg_attr donatessilver_msg_attr;
extern func_msg_attr move_msg_attr;
extern func_msg_attr give_msg_attr;
extern func_msg_attr receive_msg_attr;
extern func_msg_attr earn_msg_attr;
extern func_msg_attr transport_msg_attr;
extern func_msg_attr study_msg_attr;
extern func_msg_attr teach_msg_attr;
extern func_msg_attr taught_msg_attr;
extern func_msg_attr research_msg_attr;
extern func_msg_attr makeherb_msg_attr;
extern func_msg_attr build_msg_attr;
extern func_msg_attr sail_msg_attr;
extern func_msg_attr produce_msg_attr;
extern func_msg_attr pay_msg_attr;
extern func_msg_attr buy_msg_attr;
extern func_msg_attr sell_msg_attr;
extern func_msg_attr promote_msg_attr;
extern func_msg_attr paybuy_msg_attr;
extern func_msg_attr uses_msg_attr;

extern func_msg_attr walk_msg_attr;
extern func_msg_attr ride_msg_attr;
extern func_msg_attr earn_msg_attr_work;
extern func_msg_attr earn_msg_attr_tax;
extern func_msg_attr earn_msg_attr_entertain;
extern func_msg_attr earn_msg_attr_region_owner;
extern func_msg_attr earn_msg_attr_sell;

///////////

typedef enum eMessageType
{
	ICRMSG_GIVESILVER,
	ICRMSG_GIVE,
	ICRMSG_RECEIVE,
	ICRMSG_EARN_WORK,
	ICRMSG_EARN_TAX,
	ICRMSG_EARN_ENTERTAIN,
	ICRMSG_EARN_REGION_OWNER,
	ICRMSG_EARN_SELL,
	ICRMSG_WALK,
	ICRMSG_RIDE,
	ICRMSG_TRANSPORT,
	ICRMSG_STUDY,
	ICRMSG_TEACH,
	ICRMSG_TAUGHT,
	ICRMSG_RESEARCH_HERB,
	ICRMSG_MAKE_HERB,
	ICRMSG_BUILD_BUILDING,
	ICRMSG_BUILD_SHIP,
	ICRMSG_SAIL,
	ICRMSG_PRODUCE_RESOURCE,
	ICRMSG_PRODUCE_ITEM,
	ICRMSG_PAYBUY,
	ICRMSG_PAY,
	ICRMSG_BUY,
	ICRMSG_SELL,
	ICRMSG_PROMOTE,
	ICRMSG_USES,
	IMAX_CRMSG
} eMessageType;

///////////

typedef struct TMsgInfo
{
	int	m_nTypeID;
	int	m_nTxtIndex;
	BOOL m_bWithinPlayerSection;
	func_msg_attr *m_func;
	
	TMsgInfo::TMsgInfo(int nTxtIndex, int nTypeID, func_msg_attr *f, BOOL bWithinPlayerSection=TRUE)
	{
		m_nTxtIndex=nTxtIndex;
		m_nTypeID=nTypeID;
		m_bWithinPlayerSection=bWithinPlayerSection;
		m_func = f;
	}
} TMsgInfo;

//////////

class CUnit;
class CRegion;
class CContainer;

//////////

class CMessage : public CObject 
{
public:
	
	CMessage(int nIndex, CUnit * pUnit1, CRegion * pRegion1, CUnit * pUnit2 = NULL, CRegion * pRegion2 = NULL, CRegion * pRegion3 = NULL, CContainer * pContainer = NULL, const char * sObjectName = "", int nNumber=0, int nNumberWanted=0);
	virtual ~CMessage();

	BOOL save();

public:

	CUnit * m_pUnit1;
	CUnit * m_pUnit2;
	CRegion * m_pRegion1;
	CRegion * m_pRegion2;
	CRegion * m_pRegion3;
	CContainer * m_pContainer;
	CString m_sObjectName;
	int m_nNumber;
	int m_nNumberWanted;

private:

	int m_nIndex;
	int m_nID;
	TMsgInfo * m_pMsgInfo;

};

DECLARE_SORTABLE_LIST(CMessage)

#endif // !defined(AFX_MESSAGE_H__694513DC_899F_425A_B929_B79971108CAF__INCLUDED_)
