// /------------------------------------------------------------
// | PapirusIncludeSource
// | 
// | File Name    : wSTCVInPlaceEdit.h
// | Created By   : cguillon
// | Creation Date: Monday, September 04, 2000
// | 
// | Purpose      : 
// | 
//  ------------------------------------------------------------
#ifndef _WSTCVINPLACEEDIT_H_
#define _WSTCVINPLACEEDIT_H_


/////////////////////////////////////////////////////////////////////////////
// CSTCVInPlaceEdit window

class CSTCVInPlaceEdit : public CEdit
{
// Construction
public:
	CSTCVInPlaceEdit(int iItem, int iSubItem, CString sInitText);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSTCVInPlaceEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSTCVInPlaceEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSTCVInPlaceEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_iItem;
	int m_iSubItem;
	CString m_sInitText;
	BOOL    m_bESC;	 	// To indicate whether ESC key was pressed
};
#endif //_WSTCVINPLACEEDIT_H_
//  ------------------------------------------------------------
