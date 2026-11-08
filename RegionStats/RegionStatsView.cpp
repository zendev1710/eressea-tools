// RegionStatsView.cpp : implementation of the CRegionStatsView class
//

#include "stdafx.h"
#include "RegionStats.h"

#include "RegionStatsDoc.h"
#include "RegionStatsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegionStatsView

IMPLEMENT_DYNCREATE(CRegionStatsView, CView)

BEGIN_MESSAGE_MAP(CRegionStatsView, CView)
	//{{AFX_MSG_MAP(CRegionStatsView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegionStatsView construction/destruction

CRegionStatsView::CRegionStatsView()
{
	// TODO: add construction code here

}

CRegionStatsView::~CRegionStatsView()
{
}

BOOL CRegionStatsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CRegionStatsView drawing

void CRegionStatsView::OnDraw(CDC* pDC)
{
	CRegionStatsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CRegionStatsView diagnostics

#ifdef _DEBUG
void CRegionStatsView::AssertValid() const
{
	CView::AssertValid();
}

void CRegionStatsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRegionStatsDoc* CRegionStatsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRegionStatsDoc)));
	return (CRegionStatsDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRegionStatsView message handlers
