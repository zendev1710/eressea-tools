// Orders.h: interface for the COrders class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ORDERS_H__7416E0FD_A3C5_4CC9_86E1_84665DA4694E__INCLUDED_)
#define AFX_ORDERS_H__7416E0FD_A3C5_4CC9_86E1_84665DA4694E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Order.h"

//////////

class COrders: public COrderList
{
public:
	COrders();
	virtual ~COrders();

	BOOL insert(CString sOrder, K_Order cmd);
};

#endif // !defined(AFX_ORDERS_H__7416E0FD_A3C5_4CC9_86E1_84665DA4694E__INCLUDED_)
