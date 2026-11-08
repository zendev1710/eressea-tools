// RegionStatsDoc.h : interface of the CRegionStatsDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGIONSTATSDOC_H__E74BA9BC_066C_4746_8983_37086B953A62__INCLUDED_)
#define AFX_REGIONSTATSDOC_H__E74BA9BC_066C_4746_8983_37086B953A62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CRegionStatsDoc : public CDocument
{
protected: // create from serialization only
	CRegionStatsDoc();
	DECLARE_DYNCREATE(CRegionStatsDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegionStatsDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRegionStatsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CRegionStatsDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIONSTATSDOC_H__E74BA9BC_066C_4746_8983_37086B953A62__INCLUDED_)
