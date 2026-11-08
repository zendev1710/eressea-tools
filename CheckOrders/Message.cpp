// Message.cpp: implementation of the CMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EchkMgr.h"
#include "crnames.h"
#include "CR.h"
#include "MyEresseaCR.h"
#include "Region.h"
#include "Unit.h"
#include "Container.h"
#include "Messages.h"
#include "Message.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define CR_MODE_MOVE_BY_HORSE	(1)
#define CR_MODE_MOVE_BY_FOOT	(2)

#define CR_MODE_EARN_WORK			(0)
#define CR_MODE_EARN_ENTERTAIN		(1)
#define CR_MODE_EARN_TAX			(2)
#define CR_MODE_EARN_BY_SELL		(3)
#define CR_MODE_EARN_REGION_OWNER	(4)

////////////

static block * s_lastMsgBlock = NULL;
static int s_nFirstMessageID = 372567920;

//TODO: addmsg in CUnit: donatessilver 

BOOL donatessilver_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("donatessilver_msg_attr:: null block");
	if (!pMsg) 	return errormsg("donatessilver_msg_attr:: null msg");
	if (!pi) 	return errormsg("donatessilver_msg_attr:: null msginfo");
	if (!pMsg->m_pUnit1) 	return errormsg("donatessilver_msg_attr:: null unit1");
	if (!pMsg->m_pUnit2) 	return errormsg("donatessilver_msg_attr:: null unit2");

	CFaction * f1 = pMsg->m_pUnit1->faction();
	CFaction * f2 = pMsg->m_pUnit2->faction();

	if (!f1) 	return errormsg("donatessilver_msg_attr:: null faction1");
	if (!f2) 	return errormsg("donatessilver_msg_attr:: null faction2");

	CString sPlayer1Name(f1->outputName());
	CString sPlayer2Name(f2->outputName());

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",_msg(pi->m_nTxtIndex,(LPCSTR)sPlayer1Name,pMsg->m_nNumber,(LPCSTR)sPlayer1Name));
	cr().seti(b,"from",f1->id());
	cr().seti(b,"amount",pMsg->m_nNumber);
	cr().seti(b,"to",f2->id());

	return FALSE;
}

BOOL move_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi, int nMode)
{
	if (!b)	return errormsg("move_msg_attr:: null block");
	if (!pMsg) 	return errormsg("move_msg_attr:: null msg");
	if (!pi) 	return errormsg("move_msg_attr:: null msginfo");
	if (!pMsg->m_pRegion1) 	return errormsg("move_msg_attr:: null region1");
	if (!pMsg->m_pRegion2) 	return errormsg("move_msg_attr:: null region2");
	if (!pMsg->m_pUnit1) 	return errormsg("move_msg_attr:: null unit1");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sReg1Name(pMsg->m_pRegion1->outputName(TRUE));
	CString sReg2Name(pMsg->m_pRegion2->outputName(TRUE));
	CString sReg3Name("");
	CString sRegionsCrossed(".");
	CString sRegionsCoords("");

	int ni1[3], ni2[3],	ni3[3];
	for (int i=0;i<3;i++) {	ni1[i]=0; ni2[i]=0;	ni3[i]=0;}
	ni1[0]=pMsg->m_pRegion1->x(); ni1[1]=pMsg->m_pRegion1->y();
	ni2[0]=pMsg->m_pRegion2->x(); ni2[1]=pMsg->m_pRegion2->y();

	if (pMsg->m_pRegion3) 
	{
		//TODO:modif if 4 moves
		sReg3Name= pMsg->m_pRegion3->outputName(TRUE);
		sRegionsCrossed.Format(" by way of %s and %s.",sReg3Name,sReg2Name);
		ni3[0]=pMsg->m_pRegion3->x();
		ni3[1]=pMsg->m_pRegion3->y();
		sRegionsCoords.Format("%d %d %d, %d %d %d",ni3[0],ni3[1],ni3[2],ni2[0],ni2[1],ni2[2]);
	}
	sReg2Name+=sRegionsCrossed;
	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",_msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,(LPCSTR)sReg1Name,(LPCSTR)sReg2Name));
	cr().seti(b,"unit",pMsg->m_pUnit1->id());
	cr().seti(b,"mode",nMode);
	cr().setints(b,"start",ni1,3);
	cr().setints(b,"end",ni2,3);
	cr().sets(b,"regions",sRegionsCoords);

	return TRUE;
}


BOOL give_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("give_msg_attr:: null block");
	if (!pMsg) 	return errormsg("give_msg_attr:: null msg");
	if (!pi) 	return errormsg("give_msg_attr:: null msginfo");
	if (!pMsg->m_pUnit1) return errormsg("give_msg_attr:: null unit1");
	if (!pMsg->m_pUnit2) return errormsg("give_msg_attr:: null unit2");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sUnit2Name(pMsg->m_pUnit2->outputName());
	const char *ppMsg=_msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,pMsg->m_nNumber,(LPCSTR)CCR::German2English(pMsg->m_sObjectName),(LPCSTR)sUnit2Name);
	
	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",ppMsg);
	cr().seti(b,"unit",pMsg->m_pUnit1->id());
	cr().seti(b,"amount",pMsg->m_nNumber);
	cr().sets(b,"resource",CCR::English2German(pMsg->m_sObjectName));
	cr().seti(b,"target",pMsg->m_pUnit2->id());
	return TRUE;
}

BOOL receive_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("receive_msg_attr:: null block");
	if (!pMsg) 	return errormsg("receive_msg_attr:: null msg");
	if (!pi) 	return errormsg("receive_msg_attr:: null msginfo");
	if (!pMsg->m_pUnit1) return errormsg("receive_msg_attr:: null unit1");
	if (!pMsg->m_pUnit2) return errormsg("receive_msg_attr:: null unit2");

	CString sReceiverName(pMsg->m_pUnit1->outputName());
	CString sGiverName(pMsg->m_pUnit2->outputName());
	const char *ppMsg=_msg(pi->m_nTxtIndex,(LPCSTR)sGiverName,pMsg->m_nNumber,(LPCSTR)CCR::German2English(pMsg->m_sObjectName),(LPCSTR)sReceiverName);

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",ppMsg);
	cr().seti(b,"unit",pMsg->m_pUnit2->id());
	cr().seti(b,"amount",pMsg->m_nNumber);
	cr().sets(b,"resource",CCR::English2German(pMsg->m_sObjectName));
	cr().seti(b,"target",pMsg->m_pUnit1->id());
	return TRUE;
}


BOOL earn_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi, int nMode)
{
	if (!b)	return errormsg("earn_msg_attr:: null block");
	if (!pMsg) 	return errormsg("earn_msg_attr:: null msg");
	if (!pi) 	return errormsg("earn_msg_attr:: null msginfo");
	if (!pMsg->m_pRegion1) 	return errormsg("earn_msg_attr:: null region1");
	if (!pMsg->m_pUnit1) 	return errormsg("earn_msg_attr:: null unit1");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sReg1Name(pMsg->m_pRegion1->outputName(TRUE));
	CString sMsg("");
	if (pMsg->m_nNumber == pMsg->m_nNumberWanted)
		sMsg = _msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,pMsg->m_nNumber,(LPCSTR)sReg1Name);
	else
		sMsg = _msg(CRMSGOF_EARN,(LPCSTR)sUnit1Name,pMsg->m_nNumber,pMsg->m_nNumberWanted,(LPCSTR)sReg1Name);

	int ni1[3];
	for (int i=0;i<3;i++) ni1[i]=0;
	ni1[0]=pMsg->m_pRegion1->x(); ni1[1]=pMsg->m_pRegion1->y();

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",sMsg);
	cr().seti(b,"unit",pMsg->m_pUnit1->id());
	cr().setints(b,"region",ni1,3);
	cr().seti(b,"amount",pMsg->m_nNumber);
	cr().seti(b,"wanted",pMsg->m_nNumberWanted);
	cr().seti(b,"mode",nMode); // 1 for entertain - 2 for tax/entertain - 3 for sell
	return TRUE;
}

BOOL transport_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("transport_msg_attr:: null block");
	if (!pMsg) 	return errormsg("transport_msg_attr:: null msg");
	if (!pi) 	return errormsg("transport_msg_attr:: null msginfo");
	if (!pMsg->m_pRegion1) 	return errormsg("transport_msg_attr:: null region1");
	if (!pMsg->m_pRegion2) 	return errormsg("transport_msg_attr:: null region2");
	if (!pMsg->m_pUnit1) 	return errormsg("transport_msg_attr:: null unit1");
	if (!pMsg->m_pUnit2) 	return errormsg("transport_msg_attr:: null unit2");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sUnit2Name(pMsg->m_pUnit2->outputName());
	CString sReg1Name(pMsg->m_pRegion1->outputName(TRUE));
	CString sReg2Name(pMsg->m_pRegion2->outputName(TRUE));
	int ni1[3], ni2[3];
	for (int i=0;i<3;i++) {	ni1[i]=0; ni2[i]=0; }
	ni1[0]=pMsg->m_pRegion1->x(); ni1[1]=pMsg->m_pRegion1->y();
	ni2[0]=pMsg->m_pRegion2->x(); ni2[1]=pMsg->m_pRegion2->y();

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",_msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,(LPCSTR)sUnit2Name,(LPCSTR)sReg1Name,(LPCSTR)sReg2Name));
	cr().seti(b,"unit",pMsg->m_pUnit1->id());
	cr().seti(b,"target",pMsg->m_pUnit2->id());
	cr().setints(b,"start",ni1,3);
	cr().setints(b,"end",ni2,3);

	return TRUE;
}

BOOL study_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("study_msg_attr:: null block");
	if (!pMsg) 	return errormsg("study_msg_attr:: null msg");
	if (!pi) 	return errormsg("study_msg_attr:: null msginfo");
	if (!pMsg->m_pRegion1) 	return errormsg("study_msg_attr:: null region1");
	if (!pMsg->m_pUnit1) 	return errormsg("study_msg_attr:: null unit1");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sReg1Name(pMsg->m_pRegion1->outputName(TRUE));

	int ni1[3];
	for (int i=0;i<3;i++) ni1[i]=0;
	ni1[0]=pMsg->m_pRegion1->x(); ni1[1]=pMsg->m_pRegion1->y();

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",_msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,pMsg->m_nNumber,(LPCSTR)sReg1Name,(LPCSTR)CCR::German2English(pMsg->m_sObjectName)));
	cr().seti(b,"unit",pMsg->m_pUnit1->id());
	cr().setints(b,"region",ni1,3);
	cr().seti(b,"cost",pMsg->m_nNumber);
	cr().sets(b,"skill",CCR::English2German(pMsg->m_sObjectName,TRUE));

	return TRUE;
}

BOOL teach_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("teach_msg_attr:: null block");
	if (!pMsg) 	return errormsg("teach_msg_attr:: null msg");
	if (!pi) 	return errormsg("teach_msg_attr:: null msginfo");
	if (!pMsg->m_pUnit1) 	return errormsg("teach_msg_attr:: null unit1");
	if (!pMsg->m_pUnit2) 	return errormsg("teach_msg_attr:: null unit2");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sUnit2Name(pMsg->m_pUnit2->outputName());

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",_msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,(LPCSTR)sUnit2Name,(LPCSTR)CCR::German2English(pMsg->m_sObjectName),pMsg->m_nNumber));
	cr().seti(b,"teacher",pMsg->m_pUnit1->id());
	cr().seti(b,"student",pMsg->m_pUnit2->id());
	cr().sets(b,"skill",CCR::English2German(pMsg->m_sObjectName,TRUE));
	cr().seti(b,"level",pMsg->m_nNumber);

	return TRUE;
}

BOOL taught_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("taught_msg_attr:: null block");
	if (!pMsg) 	return errormsg("taught_msg_attr:: null msg");
	if (!pi) 	return errormsg("taught_msg_attr:: null msginfo");
	if (!pMsg->m_pUnit1) 	return errormsg("taught_msg_attr:: null unit1");
	if (!pMsg->m_pUnit2) 	return errormsg("taught_msg_attr:: null unit2");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sUnit2Name(pMsg->m_pUnit2->outputName());

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",_msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,(LPCSTR)sUnit2Name,(LPCSTR)CCR::German2English(pMsg->m_sObjectName)));
	cr().seti(b,"teacher",pMsg->m_pUnit1->id());
	cr().seti(b,"student",pMsg->m_pUnit2->id());
	cr().sets(b,"skill",CCR::English2German(pMsg->m_sObjectName,TRUE));

	return TRUE;
}

BOOL research_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("research_msg_attr:: null block");
	if (!pMsg) 	return errormsg("research_msg_attr:: null msg");
	if (!pi) 	return errormsg("research_msg_attr:: null msginfo");
	if (!pMsg->m_pUnit1) return errormsg("research_msg_attr:: null unit1");
	if (!pMsg->m_pRegion1) return errormsg("research_msg_attr:: null region1");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sRegionName(pMsg->m_pRegion1->outputName(TRUE));
	CString sHerbName("");
	sHerbName.Format("beaucoup de %s",(LPCSTR)CCR::German2English(pMsg->m_sObjectName));

	int ni1[3];
	for (int i=0;i<3;i++) ni1[i]=0;
	ni1[0]=pMsg->m_pRegion1->x(); ni1[1]=pMsg->m_pRegion1->y();

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",_msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,(LPCSTR)sHerbName,(LPCSTR)sRegionName));
	cr().seti(b,"unit",pMsg->m_pUnit1->id());
	cr().sets(b,"amount","sehr viele"); //bcp de - pipo
	cr().sets(b,"herb",(LPCSTR)CCR::English2German(pMsg->m_sObjectName));
	cr().setints(b,"region",ni1,3);
	return TRUE;
}

BOOL makeherb_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("makeherb_msg_attr:: null block");
	if (!pMsg) 	return errormsg("makeherb_msg_attr:: null msg");
	if (!pi) 	return errormsg("makeherb_msg_attr:: null msginfo");
	if (!pMsg->m_pUnit1) return errormsg("makeherb_msg_attr:: null unit1");
	if (!pMsg->m_pRegion1) return errormsg("makeherb_msg_attr:: null region1");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sRegionName(pMsg->m_pRegion1->outputName(TRUE));

	int ni1[3];
	for (int i=0;i<3;i++) ni1[i]=0;
	ni1[0]=pMsg->m_pRegion1->x(); ni1[1]=pMsg->m_pRegion1->y();

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",_msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,(LPCSTR)sRegionName,pMsg->m_nNumber,(LPCSTR)CCR::German2English(pMsg->m_sObjectName)));
	cr().seti(b,"unit",pMsg->m_pUnit1->id());
	cr().setints(b,"region",ni1,3);
	cr().seti(b,"amount",pMsg->m_nNumber);
	cr().sets(b,"herb",(LPCSTR)CCR::English2German(pMsg->m_sObjectName));
	return TRUE;
}

BOOL build_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("build_ship_msg_attr:: null block");
	if (!pMsg) 	return errormsg("build_ship_msg_attr:: null msg");
	if (!pi) 	return errormsg("build_ship_msg_attr:: null msginfo");
	if (!pMsg->m_pUnit1) return errormsg("build_ship_msg_attr:: null unit1");
	if (!pMsg->m_pContainer) return errormsg("build_ship_msg_attr:: null container");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sContainerName(pMsg->m_pContainer->outputName()); //TO MODIFY
	CString sType("building");
	if (!pMsg->m_pContainer->isBuilding()) sType="ship";

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",_msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,pMsg->m_nNumber,(LPCSTR)sContainerName));
	cr().seti(b,"unit",pMsg->m_pUnit1->id());
	cr().seti(b,"size",pMsg->m_nNumber);
	cr().seti(b,sType,pMsg->m_pContainer->id());
	return TRUE;
}

BOOL sail_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("sail_msg_attr:: null block");
	if (!pMsg) 	return errormsg("sail_msg_attr:: null msg");
	if (!pi) 	return errormsg("sail_msg_attr:: null msginfo");
	if (!pMsg->m_pRegion1) 	return errormsg("sail_msg_attr:: null region1");
	if (!pMsg->m_pRegion2) 	return errormsg("sail_msg_attr:: null region2");
	if (!pMsg->m_pContainer) 	return errormsg("sail_msg_attr:: null container");

	CString sContainerName(pMsg->m_pContainer->outputName());
	CString sReg1Name(pMsg->m_pRegion1->outputName(TRUE));
	CString sReg2Name(pMsg->m_pRegion2->outputName(TRUE));

	int ni1[3], ni2[3];
	for (int i=0;i<3;i++) {	ni1[i]=0; ni2[i]=0;}
	ni1[0]=pMsg->m_pRegion1->x(); ni1[1]=pMsg->m_pRegion1->y();
	ni2[0]=pMsg->m_pRegion2->x(); ni2[1]=pMsg->m_pRegion2->y();

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",_msg(pi->m_nTxtIndex,(LPCSTR)sContainerName,(LPCSTR)sReg1Name,(LPCSTR)sReg2Name));
	cr().seti(b,"ship",pMsg->m_pContainer->id());
	cr().setints(b,"from",ni1,3);
	cr().setints(b,"to",ni2,3);
	return TRUE;
}

BOOL produce_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("produce_msg_attr:: null block");
	if (!pMsg) 	return errormsg("produce_msg_attr:: null msg");
	if (!pi) 	return errormsg("produce_msg_attr:: null msginfo");
	if (!pMsg->m_pUnit1) 	return errormsg("produce_msg_attr:: null unit1");
	if (!pMsg->m_pRegion1) 	return errormsg("produce_msg_attr:: null region1");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sReg1Name(pMsg->m_pRegion1->outputName(TRUE));
	CString sMsg("");
	if (pMsg->m_nNumber==pMsg->m_nNumberWanted)
		sMsg = _msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,(LPCSTR)sReg1Name,pMsg->m_nNumber,(LPCSTR)CCR::German2English(pMsg->m_sObjectName));
	else
		sMsg = _msg(CRMSGOF_PRODUCE,(LPCSTR)sUnit1Name,(LPCSTR)sReg1Name,pMsg->m_nNumber,pMsg->m_nNumberWanted,(LPCSTR)CCR::German2English(pMsg->m_sObjectName));

	int ni1[3];
	for (int i=0;i<3;i++) ni1[i]=0;
	ni1[0]=pMsg->m_pRegion1->x(); ni1[1]=pMsg->m_pRegion1->y();

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",sMsg);
	cr().seti(b,"unit",pMsg->m_pUnit1->id());
	cr().setints(b,"region",ni1,3);
	cr().seti(b,"amount",pMsg->m_nNumber);
	cr().seti(b,"wanted",pMsg->m_nNumberWanted);
	cr().sets(b,"resource",CCR::English2German(pMsg->m_sObjectName));

	return TRUE;
}

BOOL pay_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("pay_msg_attr:: null block");
	if (!pMsg) 	return errormsg("pay_msg_attr:: null msg");
	if (!pi) 	return errormsg("pay_msg_attr:: null msginfo");
	if (!pMsg->m_pUnit1) return errormsg("pay_msg_attr:: null unit1");
	if (!pMsg->m_pContainer) return errormsg("pay_msg_attr:: null container");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sBuildingName(pMsg->m_pContainer->outputName());
	CString sMsg(_msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,(LPCSTR)sBuildingName));

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",sMsg);
	cr().seti(b,"unit",pMsg->m_pUnit1->id());
	cr().seti(b,"building",pMsg->m_pContainer->id());
	return TRUE;
}

BOOL buy_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("buy_msg_attr:: null block");
	if (!pMsg) 	return errormsg("buy_msg_attr:: null msg");
	if (!pi) 	return errormsg("buy_msg_attr:: null msginfo");
	if (!pMsg->m_pUnit1) 	return errormsg("buy_msg_attr:: null unit1");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sMsg(_msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,pMsg->m_nNumber,(LPCSTR)CCR::German2English(pMsg->m_sObjectName)));

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",sMsg);
	cr().seti(b,"unit",pMsg->m_pUnit1->id());
	cr().seti(b,"amount",pMsg->m_nNumber);
	cr().sets(b,"resource",CCR::English2German(pMsg->m_sObjectName));
	return TRUE;
}

BOOL sell_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("sell_msg_attr:: null block");
	if (!pMsg) 	return errormsg("sell_msg_attr:: null msg");
	if (!pi) 	return errormsg("sell_msg_attr:: null msginfo");
	if (!pMsg->m_pUnit1) 	return errormsg("sell_msg_attr:: null unit1");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sMsg(_msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,pMsg->m_nNumber,(LPCSTR)CCR::German2English(pMsg->m_sObjectName)));

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",sMsg);
	cr().seti(b,"unit",pMsg->m_pUnit1->id());
	cr().seti(b,"amount",pMsg->m_nNumber);
	cr().sets(b,"resource",CCR::English2German(pMsg->m_sObjectName));
	return TRUE;
}

BOOL promote_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("promote_msg_attr:: null block");
	if (!pMsg) 	return errormsg("promote_msg_attr:: null msg");
	if (!pi) 	return errormsg("promote_msg_attr:: null msginfo");
	if (!pMsg->m_pUnit1) 	return errormsg("promote_msg_attr:: null unit1");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sMsg(_msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,pMsg->m_nNumber));

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",sMsg);
	cr().seti(b,"unit",pMsg->m_pUnit1->id());
	cr().seti(b,"cost",pMsg->m_nNumber);
	return TRUE;
}

BOOL paybuy_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("paybuy_msg_attr:: null block");
	if (!pMsg) 	return errormsg("paybuy_msg_attr:: null msg");
	if (!pi) 	return errormsg("paybuy_msg_attr:: null msginfo");
	if (!pMsg->m_pUnit1) return errormsg("paybuy_msg_attr:: null unit1");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sMsg(_msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,pMsg->m_nNumber));

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",sMsg);
	cr().seti(b,"unit",pMsg->m_pUnit1->id());
	cr().seti(b,"money",pMsg->m_nNumber);
	return TRUE;
}

BOOL uses_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	if (!b)	return errormsg("uses_msg_attr:: null block");
	if (!pMsg) 	return errormsg("uses_msg_attr:: null msg");
	if (!pi) 	return errormsg("uses_msg_attr:: null msginfo");
	if (!pMsg->m_pUnit1) return errormsg("uses_msg_attr:: null unit1");

	CString sUnit1Name(pMsg->m_pUnit1->outputName());
	CString sMsg(_msg(pi->m_nTxtIndex,(LPCSTR)sUnit1Name,CCR::German2English(pMsg->m_sObjectName)));

	cr().seti(b,"type",pi->m_nTypeID);
	cr().sets(b,"rendered",sMsg);
	cr().seti(b,"unit",pMsg->m_pUnit1->id());
	cr().sets(b,"potion",CCR::English2German(pMsg->m_sObjectName));
	return TRUE;
}

BOOL walk_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	return move_msg_attr(b,pMsg,pi,CR_MODE_MOVE_BY_FOOT);
}

BOOL ride_msg_attr(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	return move_msg_attr(b,pMsg,pi,CR_MODE_MOVE_BY_HORSE);
}

BOOL earn_msg_attr_work(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	return earn_msg_attr(b, pMsg, pi, CR_MODE_EARN_WORK);
}

BOOL earn_msg_attr_tax(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	return earn_msg_attr(b, pMsg, pi, CR_MODE_EARN_TAX);
}

BOOL earn_msg_attr_entertain(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	return earn_msg_attr(b, pMsg, pi, CR_MODE_EARN_ENTERTAIN);
}

BOOL earn_msg_attr_region_owner(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	return earn_msg_attr(b, pMsg, pi, CR_MODE_EARN_REGION_OWNER);
}

BOOL earn_msg_attr_sell(block *b, CMessage * pMsg, TMsgInfo * pi)
{
	return earn_msg_attr(b, pMsg, pi, CR_MODE_EARN_BY_SELL);
}

///////////////

CMessage::CMessage(int nIndex, CUnit * pUnit1, CRegion * pRegion1, CUnit * pUnit2, CRegion * pRegion2, CRegion * pRegion3, CContainer * pContainer, const char * sObjectName, int nNumber, int nNumberWanted):
	m_pUnit1(pUnit1),
	m_pRegion1(pRegion1),
	m_pUnit2(pUnit2),
	m_pRegion2(pRegion2),
	m_pRegion3(pRegion3),
	m_pContainer(pContainer),
	m_sObjectName(sObjectName),
	m_nNumber(nNumber),
	m_nNumberWanted(nNumberWanted),
	m_pMsgInfo(NULL),
	m_nIndex(nIndex)
{
		m_pMsgInfo = CMessages::cr_msg(nIndex);
}

BOOL CMessage::save()
{
	if (!m_pMsgInfo) 
		return errormsg("CMessage::save - null info");
	if (!m_pMsgInfo->m_func) 
		return errormsg("CMessage::save - null func");

	m_nID = s_nFirstMessageID;
	s_nFirstMessageID++;
	if (!m_pMsgInfo->m_bWithinPlayerSection)
	{
		//TODO - attach msg to region
		int yyy=1;
		return FALSE;
	}

	if (!m_pUnit1) 
		return errormsg("Message[%d]::save - null unit",m_nIndex);
	CFaction  * pf = m_pUnit1->faction();
	if (!pf) 
	{
		if (m_pUnit1->isUnknownUnit()) 
			return TRUE; //ignore message
		return errormsg("Message[%d]::save - null faction for %s",m_nIndex,m_pUnit1->outputName());
	}

	block * bm = pf->saveMessage(m_nID);
	if (bm && !(*m_pMsgInfo->m_func)(bm,this,m_pMsgInfo))
	{
		cr().onSave_remove(bm);
		return errormsg("CMessage::save - failed to write block");
	}
	return TRUE;
}

CMessage::~CMessage()
{

}

/*
 *	
 MESSAGE 344853208
2146680486;type
"Merlin (vzv2) uses potion of truth.";rendered
1492814;unit
"Trank der Wahrheit";potion
 */