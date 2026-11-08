// RegionStatsView.h : interface of the CRegionStatsView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGIONSTATSVIEW_H__E92C0263_2FD2_4D03_8F58_E14CBB278EA7__INCLUDED_)
#define AFX_REGIONSTATSVIEW_H__E92C0263_2FD2_4D03_8F58_E14CBB278EA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CRegionStatsView : public CView
{
protected: // create from serialization only
	CRegionStatsView();
	DECLARE_DYNCREATE(CRegionStatsView)

// Attributes
public:
	CRegionStatsDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegionStatsView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRegionStatsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CRegionStatsView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in RegionStatsView.cpp
inline CRegionStatsDoc* CRegionStatsView::GetDocument()
   { return (CRegionStatsDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIONSTATSVIEW_H__E92C0263_2FD2_4D03_8F58_E14CBB278EA7__INCLUDED_)
