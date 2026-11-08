// Orders.cpp: implementation of the COrders class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Order.h"
#include "Orders.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


COrders::COrders()
{
}

COrders::~COrders()
{
	POSITION pos=GetHeadPosition();
	while  (pos) delete (GetNext(pos));
	RemoveAll();
}

BOOL COrders::insert(CString sOrder, K_Order cmd)
{
	COrder * b = NULL;
	POSITION posPrev = NULL;
	POSITION pos = GetHeadPosition();
	while (pos)	{
		posPrev = pos;
		b = GetNext(pos);
		if (cmd < b->cmd())
		{
			InsertBefore(posPrev,new COrder(sOrder,cmd));
			return TRUE;
		}
	}
	AddTail(new COrder(sOrder,cmd));
	return TRUE;
}