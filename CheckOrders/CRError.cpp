// CRError.cpp: implementation of the CCRError class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CRError.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern void output(const char * pzFormat, ...);
extern const char * _formatFromCompileOption();
extern const char * iToString(int n);

CCRErrors sErrorsList;
int CCRErrors::s_bBasicErrors=0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCRError::CCRError(int nLevel, int nErrorID, const char * pzTxt, const char *pzOrder):
m_sTxt(""),
m_sOrder(""),
m_nLevel (nLevel),
m_nErrorID(nErrorID)
{
	if (pzTxt) m_sTxt=pzTxt;
	if (pzOrder) m_sOrder=pzOrder;
}

int CCRError::compareOnLevel(CCRError* b1, CCRError* b2) 
{ 
	if (b1->m_nLevel == b2->m_nLevel)
	{
		if (b1->m_nErrorID==b2->m_nErrorID)
			return b1->m_sTxt.Compare(b2->m_sTxt);
		return (b1->m_nErrorID > b2->m_nErrorID);
	}
	return (b1->m_nLevel > b2->m_nLevel);
}

BOOL CCRError::write()
{
	const char * pzFormat = _formatFromCompileOption();
	output(pzFormat,"",m_nLevel==-1?"":iToString(m_nLevel),(LPCSTR)m_sTxt,(LPCSTR)m_sOrder);
	return TRUE;
}

CCRError::~CCRError()
{

}

////////////////

CCRErrors::CCRErrors()
{
}

CCRErrors::~CCRErrors()
{
	POSITION pos=GetHeadPosition();
	while  (pos) delete (GetNext(pos));
	RemoveAll();
}

void CCRErrors::Show(/*int nSortFlag*/) 
{
	/*if (nSortFlag == SORT_ON_LEVEL)*/ Sort(CCRError::compareOnLevel);
	//if (nSortFlag == SORT_ON_REGION) Sort(CBuilding::compareOnRegion);

	POSITION pos = GetHeadPosition();
	while (pos)	GetNext(pos)->write();
}

