// Messages.h: interface for the CMessages class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGES_H__47165D89_56DF_40C2_9B37_06D545D8D1E0__INCLUDED_)
#define AFX_MESSAGES_H__47165D89_56DF_40C2_9B37_06D545D8D1E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Message.h"

class CMessages : public CMessageList  
{
public:
	CMessages();
	virtual ~CMessages();

	BOOL Add(CMessage * pMsg);

	void save();

	static TMsgInfo * cr_msg(int nIndex); 

private:
	static TMsgInfo *s_cr_msgs[IMAX_CRMSG];
};

extern CMessages sMessagesList;

#endif // !defined(AFX_MESSAGES_H__47165D89_56DF_40C2_9B37_06D545D8D1E0__INCLUDED_)
