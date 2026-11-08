// Messages.cpp: implementation of the CMessages class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EchkMgr.h"
#include "Message.h"
#include "Messages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TMsgInfo * CMessages::s_cr_msgs[IMAX_CRMSG]=
{
	// tobe deleted at the end of the program
	new TMsgInfo(CRMSG_DONATESSILVER,1682429624,&donatessilver_msg_attr,FALSE),
	new TMsgInfo(CRMSG_GIVE,5281483,&give_msg_attr),
	new TMsgInfo(CRMSG_GIVE,5281483,&receive_msg_attr),
	new TMsgInfo(CRMSG_EARN,771334452,&earn_msg_attr_work),
	new TMsgInfo(CRMSG_EARN,771334452,&earn_msg_attr_tax),
	new TMsgInfo(CRMSG_EARN,771334452,&earn_msg_attr_entertain),
	new TMsgInfo(CRMSG_EARN,771334452,&earn_msg_attr_region_owner),
	new TMsgInfo(CRMSG_EARN,771334452,&earn_msg_attr_sell),
	new TMsgInfo(CRMSG_WALK,1242100855,&walk_msg_attr),
	new TMsgInfo(CRMSG_RIDE,1242100855,ride_msg_attr),
	new TMsgInfo(CRMSG_TRANSPORT,891175669,&transport_msg_attr),
	new TMsgInfo(CRMSG_STUDY,443066738,&study_msg_attr),
	new TMsgInfo(CRMSG_TEACH,1109016161,&teach_msg_attr),
	new TMsgInfo(CRMSG_TAUGHT,200588432,&taught_msg_attr),
	new TMsgInfo(CRMSG_RESEARCH_HERB,1349776898,&research_msg_attr),
	new TMsgInfo(CRMSG_MAKE_HERB,1511758069,&makeherb_msg_attr),
	new TMsgInfo(CRMSG_BUILD,1750373495,&build_msg_attr),
	new TMsgInfo(CRMSG_BUILD,1670080073,&build_msg_attr),
	new TMsgInfo(CRMSG_SAIL,2026874001,&sail_msg_attr),
	new TMsgInfo(CRMSG_PRODUCE,2087428775,&produce_msg_attr),
	new TMsgInfo(CRMSG_PRODUCE,704703128,&produce_msg_attr),
	new TMsgInfo(CRMSG_PAYBUY,170076,&paybuy_msg_attr),
	new TMsgInfo(CRMSG_PAY,107552268,&pay_msg_attr),
	new TMsgInfo(CRMSG_BUY,1549031288,&buy_msg_attr),
	new TMsgInfo(CRMSG_SELL,1478912224,&sell_msg_attr),
	new TMsgInfo(CRMSG_PROMOTE,370981872,&promote_msg_attr),
	new TMsgInfo(CRMSG_USES,2146680486,&uses_msg_attr)
} ;

CMessages::CMessages()
{

}

CMessages::~CMessages()
{
	POSITION pos=GetHeadPosition();
	while  (pos) delete (GetNext(pos));
	RemoveAll();
	for (int i=0;i<IMAX_CRMSG;i++)
		if (s_cr_msgs[i]) {
			delete s_cr_msgs[i];
			s_cr_msgs[i]=NULL;
		}
}

TMsgInfo * CMessages::cr_msg(int nIndex) 
{
	if (nIndex >=0 && nIndex < IMAX_CRMSG)
		return s_cr_msgs[nIndex];
	return NULL;
}

BOOL CMessages::Add(CMessage *pMsg) 
{
	if (pMsg)
	{
		AddTail(pMsg);
		return TRUE;
	}
	return FALSE;
}

void CMessages::save()
{
	POSITION pos=GetHeadPosition();
	while  (pos) GetNext(pos)->save();
}
