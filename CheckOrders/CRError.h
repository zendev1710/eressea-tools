// CRError.h: interface for the CCRError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRERROR_H__694513DC_899F_425A_B929_B79971108CAF__INCLUDED_)
#define AFX_CRERROR_H__694513DC_899F_425A_B929_B79971108CAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "crdata.h"
#include "wLizard.h"
#include "SortableObList.h"

///////////

class CCRError : public CObject 
{
public:
	
	CCRError(int nLevel, int nErrorID, const char * pzTxt, const char *pzOrder);
	virtual ~CCRError();

	BOOL write();

	int level() const { return m_nLevel;}

	static int CCRError::compareOnLevel(CCRError* b1, CCRError* b2); 

private:

	CString m_sTxt;
	CString m_sOrder;
	int m_nLevel;
	int m_nErrorID;
};

DECLARE_SORTABLE_LIST(CCRError)

///////////

class CCRErrors : public CCRErrorList 
{
public:
	
	CCRErrors();
	virtual ~CCRErrors();

	void Show(/*int nSortFlag*/); 

	static int s_bBasicErrors;

};

///////////

extern CCRErrors sErrorsList;

#endif // !defined(AFX_CRERROR_H__694513DC_899F_425A_B929_B79971108CAF__INCLUDED_)
