// RegionStatsDoc.cpp : implementation of the CRegionStatsDoc class
//

#include "stdafx.h"
#include "RegionStats.h"

#include "RegionStatsDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegionStatsDoc

IMPLEMENT_DYNCREATE(CRegionStatsDoc, CDocument)

BEGIN_MESSAGE_MAP(CRegionStatsDoc, CDocument)
	//{{AFX_MSG_MAP(CRegionStatsDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegionStatsDoc construction/destruction

CRegionStatsDoc::CRegionStatsDoc()
{
	// TODO: add one-time construction code here

}

CRegionStatsDoc::~CRegionStatsDoc()
{
}

BOOL CRegionStatsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CRegionStatsDoc serialization

void CRegionStatsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRegionStatsDoc diagnostics

#ifdef _DEBUG
void CRegionStatsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRegionStatsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRegionStatsDoc commands
