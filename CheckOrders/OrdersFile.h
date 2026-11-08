// OrdersFile.h: interface for the COrdersFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ORDERSFILE_H__10222D43_A81A_456A_891D_B22433DF5D14__INCLUDED_)
#define AFX_ORDERSFILE_H__10222D43_A81A_456A_891D_B22433DF5D14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CREresseaFile.h"

class COrdersFile : public CCREresseaFile 
{
public:
	COrdersFile();
	COrdersFile(const char *pFilename);

	virtual ~COrdersFile();

	///////////////////
	// overriden

	BOOL open();
	BOOL nextEntry() { return nextOrder();}

	BOOL checkHeader() { return checkOPTIONS();}

	BOOL process();

	///////////////////
	// specific

	BOOL nextOrder();

private:

	BOOL checkOPTIONS();
	const char * getbuf();

	FILE * m_file;
	//CString m_sWarningBuffer;
};

#endif // !defined(AFX_ORDERSFILE_H__10222D43_A81A_456A_891D_B22433DF5D14__INCLUDED_)
