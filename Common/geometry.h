// GEOMETRY - template library to move/resize child controls due to their parent resize
// (C) 1998 Eugen Paval

// V1.0 - 2/19/1998 (single inheritance version)
// V1.5 - 2/23/1998 x, y, MX, MY, MCX, MCY options added
// V2.0 - 3/22/1998 (multiple inheritance version)
// V3.0 - 9/15/1998 (template library version)
// V3.1 - 9/17/1998 bug fixes and minor changes
// V3.5 - 11/4/1998 added support for columns/rows of controls

#if !defined(_GEOMETRY_H_)
#define _GEOMETRY_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if !defined(__AFXTEMPL_H__)
#include <afxtempl.h>
#endif
#if !defined(__AFXPRIV_H__)
#include <afxpriv.h>
#endif

#define USE_GEOMETRYWINDOWS	using namespace Geometry
#define USE_GEOMETRY_TEMPLATES	using namespace Geometry

#if _MSC_VER < 1200													// prior to VC++ 6.0
#define bool		BOOL
#define true		TRUE
#define false		FALSE
#define typename	class
#endif

namespace Geometry
{

struct SResizeCtl
{
	UINT	m_nID;													// id of the control
	CRect	m_iRect;												// the initial control's rect (relative to parent)

	SResizeCtl() {};
	
	SResizeCtl(UINT nID,CRect iRect)
	{
		m_nID = nID;
		m_iRect = iRect;
	}

	SResizeCtl(UINT nID);
};
	
class CConstraint													// contains resizeable controls
{
public:

	short			m_options;										// positioning options
	float			m_xratio;										// total X/m_xratio
	float			m_yratio;										// total Y/m_yratio
	float			m_cxratio;										// total CX/m_cxratio
	float			m_cyratio;										// total CY/m_cyratio

	BOOL ExtractOptions(LPCSTR lpCaption);							// helper functions
	void CalcRatio(char* p,float& ratio);

	CConstraint() : m_options(0), m_xratio(0), m_yratio(0), m_cxratio(0), m_cyratio(0) {}

	CConstraint(LPCSTR lpOptions)									// options given as an ANSI string
	{
		m_options = 0;
		m_xratio = 0;
		m_yratio = 0;
		m_cxratio = 0;
		m_cyratio = 0;
		
		ExtractOptions(lpOptions);
	}

	CConstraint(CConstraint& cstr)									// copy constructor
	{
		m_options = cstr.m_options;
		m_xratio = cstr.m_xratio;
		m_yratio = cstr.m_yratio;
		m_cxratio = cstr.m_cxratio;
		m_cyratio = cstr.m_cyratio;
	}

	CConstraint(float x,float y=0,float cx=0,float cy=0)			// runtime calculated values
	{																// use 1 for full move/resize
		m_xratio = x;												// use 0 for freezing movement/resizing
		m_yratio = y;												// caution: movement+resizing on one axis cannot exceed 1
		m_cxratio = cx;
		m_cyratio = cy;

		m_options = eMoveX | eMoveY | eResizeCX | eResizeCY;
	}

	enum
	{
		eNothing = 0x00, 
		eResizeCX = 0x01,											// caution with eResizeX 
		eResizeCY = 0x02,											// caution eResizeY
		eMoveX = 0x04,
		eMoveY = 0x08,
		eCenterX = 0x10,											// mutually exclusive with all the others
		eCenterY = 0x20,											// mutually exclusive with all the others
		eMaintainRatio = 0x8000,									// maintain initial ratio
		eRatioCX = 0x0100,											// ratio cx
		eRatioCY = 0x0200,											// ratio cy
		eRatioX  = 0x0400,											// ratio x
		eRatioY  = 0x0800											// ratio y
	};
};

struct CBindCtlToConstraint											// maps a control to a constraint
{
	SResizeCtl	m_rsctl;
	CConstraint	m_constraint;
	
	CBindCtlToConstraint(UINT nID,CConstraint& constr)
	{
		m_rsctl.m_nID = nID;
		m_constraint = constr;
	}

	void ResizeControl(HWND hwndParent,CSize pExt,CSize mExt);
	void SetRect(CRect rect)										// set the initial control rect
	{
		m_rsctl.m_iRect = rect;
	}
};

typedef CList<CBindCtlToConstraint*,CBindCtlToConstraint*&> CtlToCstr;

class CResizeCtlList												// contains the mappings frames-controls
{
	CtlToCstr	m_gCtl;												// list of tuples(IDctl,Constraint)

public:
	CResizeCtlList() {};
	virtual ~CResizeCtlList()
	{
		while (!m_gCtl.IsEmpty())
			delete m_gCtl.RemoveHead();
	}

	bool AddConstraint(UINT nID,CConstraint& cstr,CWnd* pParent = NULL);
	void RemoveConstraint(UINT nID);

	void ResizeControls(HWND hwndParent,CSize pExt,CSize mExt);
	void SetRects(HWND hwndParent);									// establishes the initial controls' rects
};

void InvalidateRects(CWnd* pWnd,CRect irect,CRect frect);

#define DECLARE_GEOMETRY_RTSUPPORT()														\
public:																						\
	bool AddConstraint(UINT nID,CConstraint& cstr) { return AddConstraint(nID,cstr,this); };\
	virtual bool AddConstraint(UINT nID,CConstraint& cstr,CWnd* pParent) { ASSERT(FALSE); return true; };\
	virtual void RemoveConstraint(UINT nID) { ASSERT(FALSE); };

// The class template itself
template <typename T> class CGeometryWnd : public T
{
	CRect			m_originalRect;									// the client rect before resizing
	CRect			m_minRect;										// the initial window rect
	CResizeCtlList	m_cList;										// resizeable control list

public:
	// Constructors - no default parameters due to ambiguous definitions
	// CDialog
	CGeometryWnd() : 
				m_originalRect(0,0,0,0), m_minRect(0,0,0,0), T() {};
	CGeometryWnd(UINT nIDTemplate,CWnd* pParent) : 
				m_originalRect(0,0,0,0), m_minRect(0,0,0,0), T(nIDTemplate,pParent) {};
	CGeometryWnd(LPCTSTR lpszTemplateName,CWnd* pParent) : 
				m_originalRect(0,0,0,0), m_minRect(0,0,0,0), T(lpszTemplateName,pParent) {};
	
	// CFormView
	CGeometryWnd(UINT nIDTemplate) : 
				m_originalRect(0,0,0,0), m_minRect(0,0,0,0), T(nIDTemplate) {};
	CGeometryWnd(LPCTSTR lpszTemplateName) : 
				m_originalRect(0,0,0,0), m_minRect(0,0,0,0), T(lpszTemplateName) {};

	// CPropertyPage
	CGeometryWnd(UINT nIDTemplate,UINT nIDCaption) :
				m_originalRect(0,0,0,0), m_minRect(0,0,0,0), T(nIDTemplate,nIDCaption) {};
	CGeometryWnd(LPCTSTR lpszTemplateName,UINT nIDCaption) :
				m_originalRect(0,0,0,0), m_minRect(0,0,0,0), T(lpszTemplateName,nIDCaption) {};

	// CPropertySheet
	CGeometryWnd(UINT nIDCaption,CWnd* pParent,UINT iSelectPage) : 
				m_originalRect(0,0,0,0), m_minRect(0,0,0,0), T(nIDCaption,pParent,iSelectPage) {};
	CGeometryWnd(LPCTSTR lpszCaption,CWnd* pParent,UINT iSelectPage) : 
				m_originalRect(0,0,0,0), m_minRect(0,0,0,0), T(lpszCaption,pParent,iSelectPage) {};

	virtual bool AddConstraint(UINT nID,CConstraint& cstr,CWnd* pParent) 
		{ return m_cList.AddConstraint(nID,cstr,pParent); };
	virtual void RemoveConstraint(UINT nID) 
		{ m_cList.RemoveConstraint(nID); };

protected:
	virtual const AFX_MSGMAP* GetMessageMap() const;
	virtual BOOL OnWndMsg(UINT message,WPARAM wParam,LPARAM lParam,LRESULT* pResult);
	afx_msg BOOL _OnInitDialog();
	afx_msg void _OnSize(UINT nType, int cx, int cy);
	afx_msg void _OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual BOOL OnNotify(WPARAM wParam,LPARAM lParam,LRESULT* pResult);

	// CFormView
	// CPropertyPage
	void _OnPPSize(UINT nType, int cx, int cy);
	void _OnPPGetMinMaxInfo(MINMAXINFO* lpMMI);

	// CPropertySheet
	afx_msg BOOL _OnPSInitDialog();
	int _OnPSCreate(LPCREATESTRUCT lpcs);
	void _OnPSorFVSize(UINT nType, int cx, int cy);					// includes form view resizing
	void _OnPSGetMinMaxInfo(MINMAXINFO* lpMMI);

	// V4.0
	void GeometryInitialize();
	void BuildGeometryList();
	bool IsInFrame(HWND hFrame,HWND hCtl);

	// helper functions
	static bool IsGeometryCtl(HWND hwnd);
};

// -------------------------------------------------------------------------------
// CGeometryDynCreateWnd - template class for geometry windows
// Helper macros simulating the DECLARE_DYNCREATE/IMPLEMENT_DYNCREATE counterparts
#ifdef _AFXDLL
#define IMPLEMENT_GEOMETRY_DYNCREATE(baseClass)																\
	CRuntimeClass classCGeometry##baseClass =																\
	{																										\
		"CGeometry##baseClass##",sizeof(CGeometryDynCreateWnd<baseClass>),0xFFFF,						\
		CGeometryDynCreateWnd<baseClass>::CreateObject,														\
		CGeometryDynCreateWnd<baseClass>::_GetBaseClass,NULL												\
	};																										\
	CRuntimeClass* CGeometryDynCreateWnd<##baseClass##>::GetRuntimeClass() const							\
	{																										\
		return &classCGeometry##baseClass;																	\
	}																										\
	CRuntimeClass* PASCAL CGeometryDynCreateWnd<baseClass>::_GetBaseClass()									\
	{																										\
		return RUNTIME_CLASS(baseClass);																	\
	}																										\
	CRuntimeClass* GetRT##baseClass##()																		\
	{																										\
		return &classCGeometry##baseClass;																	\
	}																										\

#else
#define IMPLEMENT_GEOMETRY_DYNCREATE(baseClass)																\
	CRuntimeClass classCGeometry##baseClass =																\
	{																										\
		"CGeometryDynCreateWnd",sizeof(CGeometryDynCreateWnd<baseClass>),0xFFFF,						\
		CGeometryDynCreateWnd<baseClass>::CreateObject,														\
		RUNTIME_CLASS(baseClass),NULL																		\
	};																										\
	CRuntimeClass* CGeometryDynCreateWnd<##baseClass##>::GetRuntimeClass() const							\
	{																										\
		return &classCGeometry##baseClass;																	\
	}																										\
	CRuntimeClass* GetRT##baseClass##()																		\
	{																										\
		return &classCGeometry##baseClass;																	\
	}																										\

#endif

#define DECLARE_GEOMETRY_DYNCREATE(baseClass)																\
	CRuntimeClass* GetRT##baseClass##();																	\

#define GEOMETRY_RUNTIME_CLASS(baseClass)	GetRT##baseClass##()

// This template is for dynamic creatable classes (implements the Runtime class)
// It has to be used in conjunction with IMPLEMENT_GEOMETRY_DYNCREATE() macro
template <typename T> class CGeometryDynCreateWnd : public CGeometryWnd<T>
{
public:																										
	static CObject* PASCAL CreateObject() { return new CGeometryDynCreateWnd<T>; };
	virtual CRuntimeClass* GetRuntimeClass() const;

#ifdef _AFXDLL
public:																									
	static CRuntimeClass* PASCAL _GetBaseClass();
#endif

public:
	// Constructors - no default parameters due to ambiguous definitions
	// CDialog
	CGeometryDynCreateWnd() : CGeometryWnd<T>() {};
 };


//Inlines

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// Caption must be in the following format:
// <options>
// <options> =
//						CX		CY		X		Y		x		y		MX		MY
//				MX		*		ok		ok		ok		*		*		*		ok
//				MY		ok		*		ok		ok		*		*		ok		*
//				CX		*		ok		*		ok		*		*		*		ok
//				CY		ok		*		ok		*		*		*		ok		*
//				X		*		ok		*		ok		*		*		ok		ok
//				Y		ok		*		ok		*		*		*		ok		ok
//				x		*		ok		*		ok		*		ok		*		*
//				y		ok		*		ok		*		ok		*		*		*
// in case of illegal use it retains only the first option

inline void CConstraint::CalcRatio(char* p,float& ratio)
{
	if (*p == 0 || *p != '[')
	{
		ratio = 1.0;
		return ;
	}

	p++;
	ratio = (float)atof(p);
}

inline BOOL CConstraint::ExtractOptions(LPCSTR lpCaption)
{
	char	cOptionsString[256]; 
	char*	p;
	char *nexttoken;
	short	n;

	strcpy_s(cOptionsString,256,lpCaption);
	p = strtok_s(cOptionsString,"+",&nexttoken);
	while (p)
	{
		if (*p == 'M')												// maintain ratio
		{
			m_options |= eMaintainRatio;
			p++;
		}
		switch (*p)
		{
		case 'C':
			switch (*(p+1))
			{
			case 'X':
				m_options |= eResizeCX;
				CalcRatio(p+2,m_cxratio);
				break;

			case 'Y':
				m_options |= eResizeCY;
				CalcRatio(p+2,m_cyratio);
				break;

			default:
				return FALSE;
			}
			break;

		case 'X':
			m_options |= eMoveX;
			CalcRatio(p+1,m_xratio);
			break;

		case 'Y':
			m_options |= eMoveY;
			CalcRatio(p+1,m_yratio);
			break;

		case 'x':
			m_options |= eCenterX;
			break;

		case 'y':
			m_options |= eCenterY;
			break;

		default:
			break;
		}

		if (m_options & eMaintainRatio)								// isolate Mxx options
		{															// move them in the first byte
			n = (m_options & 0x000F) << 8;
			m_options |= n;
			m_options &= ~eMaintainRatio;							// clear maintain ratio bit
		}

		p = strtok_s(NULL,"+",&nexttoken);
	}

//	n = (m_options & 0x0F00) >> 8;									// clear the move & resize bits if 
//	m_options ^= n;													// maintain ratio is enforced

	if (m_options & 0x0500)											// should maintain ratio CX,X
		m_options &= 0xFFEF;										// clear center x bit
	if (m_options & 0x0A00)											// should maintain ratio CY,Y
		m_options &= 0xFFDF;										// clear center y bit

	n = m_options & 0x0030;											// center x,y bits
	m_options &= ~(n >> 2);											// exclude move x,y
	m_options &= ~(n >> 4);											// exclude resize x,y

//	n = m_options & 0x0001;											// should resize cx
//	m_options &= ~(n << 2);											// exclude move x

//	n = m_options & 0x0002;											// should resize cy
//	m_options &= ~(n << 2);											// exclude move y

	return TRUE;
};

inline void CBindCtlToConstraint::ResizeControl(HWND hwndParent,CSize pExt,CSize mExt)
{
	CRect rCtl;
	CWnd* pParent = CWnd::FromHandle(hwndParent);
	HWND hwnd = GetDlgItem(hwndParent,m_rsctl.m_nID);
	ASSERT(hwnd != NULL);
		
	::GetWindowRect(hwnd,rCtl);
	pParent->ScreenToClient(rCtl);

	// pExt is the new size of the parent rect
	// mExt is the initial size of the parent rect
	int	x  = m_rsctl.m_iRect.left;
	int y  = m_rsctl.m_iRect.top;
	int	cx = m_rsctl.m_iRect.Width();
	int	cy = m_rsctl.m_iRect.Height();								// the new coordinates of the control rect

	if (m_constraint.m_options & CConstraint::eRatioX)
		x  = pExt.cx*m_rsctl.m_iRect.left/mExt.cx;
	if (m_constraint.m_options & CConstraint::eRatioCX)
		cx = pExt.cx*m_rsctl.m_iRect.Width()/mExt.cx;
	if (m_constraint.m_options & CConstraint::eRatioY)
		y  = pExt.cy*m_rsctl.m_iRect.top/mExt.cy;
	if (m_constraint.m_options & CConstraint::eRatioCY)
		cy = pExt.cy*m_rsctl.m_iRect.Height()/mExt.cy;
	if (m_constraint.m_options & CConstraint::eCenterX)
		x += (pExt.cx-mExt.cx)/2;
	if (m_constraint.m_options & CConstraint::eCenterY)
		y += (pExt.cy-mExt.cy)/2;
	if (m_constraint.m_options & CConstraint::eResizeCX)
		cx += (int)((pExt.cx-mExt.cx) * m_constraint.m_cxratio);
	if (m_constraint.m_options & CConstraint::eResizeCY)
		cy += (int)((pExt.cy-mExt.cy) * m_constraint.m_cyratio);
	if (m_constraint.m_options & CConstraint::eMoveX)
		x += (int)((pExt.cx-mExt.cx) * m_constraint.m_xratio);
	if (m_constraint.m_options & CConstraint::eMoveY)
		y += (int)((pExt.cy-mExt.cy) * m_constraint.m_yratio);

	if (x < m_rsctl.m_iRect.left)
		x = m_rsctl.m_iRect.left;
	if (y < m_rsctl.m_iRect.top)
		y = m_rsctl.m_iRect.top;
	if (cx < m_rsctl.m_iRect.Width())
		cx = m_rsctl.m_iRect.Width();
	if (cy < m_rsctl.m_iRect.Height())
		cy = m_rsctl.m_iRect.Height();

	UINT nFlags = SWP_NOZORDER | ((GetWindowLong(hwnd,GWL_STYLE) & WS_VISIBLE)? SWP_SHOWWINDOW : 0);
	//ShowWindow(hwnd,SW_HIDE);

	SetWindowPos(hwnd,NULL,x,y,cx,cy,nFlags);
	
//	InvalidateRects(pParent,rCtl,CRect(x,y,x+cx,y+cy));
}

inline void CResizeCtlList::ResizeControls(HWND hwndParent,CSize pExt,CSize mExt)
{
	POSITION	pos = m_gCtl.GetHeadPosition();

	while (pos)
	{
		CBindCtlToConstraint* pcbc = m_gCtl.GetNext(pos);
		pcbc->ResizeControl(hwndParent,pExt,mExt);
	}
}

inline void CResizeCtlList::SetRects(HWND hParent)
{
	CRect		rect;
	POSITION	pos = m_gCtl.GetHeadPosition();

	while (pos)
	{
		CBindCtlToConstraint* pcbc = m_gCtl.GetNext(pos);
		HWND hCtl = GetDlgItem(hParent,pcbc->m_rsctl.m_nID);
		GetWindowRect(hCtl,rect);

		CWnd::FromHandle(hParent)->ScreenToClient(rect);
		pcbc->SetRect(rect);
	}
}

inline bool CResizeCtlList::AddConstraint(UINT nID,CConstraint& cstr,CWnd* pParent)
{
	POSITION	pos = m_gCtl.GetHeadPosition();
	CBindCtlToConstraint* pNewcbc;

	while (pos)
	{
		CBindCtlToConstraint* pcbc = m_gCtl.GetNext(pos);
		if (pcbc->m_rsctl.m_nID == nID)
		{
			pcbc->m_constraint = cstr;
			return true;
		}
	}
	pNewcbc = new CBindCtlToConstraint(nID,cstr);

	if (pParent && pParent->GetSafeHwnd())
	{
		CRect rect;
		pParent->GetDlgItem(nID)->GetWindowRect(rect);
		pParent->ScreenToClient(rect);
		pNewcbc->SetRect(rect);
	}

	m_gCtl.AddTail((CBindCtlToConstraint*&)pNewcbc);

	return true;
}

inline void CResizeCtlList::RemoveConstraint(UINT nID)
{
	POSITION	pos = m_gCtl.GetHeadPosition();
	POSITION	pos2;

	while (pos)
	{
		pos2 = pos;
		CBindCtlToConstraint* pcbc = m_gCtl.GetNext(pos);
		if (pcbc->m_rsctl.m_nID == nID)
		{
			m_gCtl.RemoveAt(pos2);
			delete pcbc;
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////
// CGeometryWnd

template <typename T> inline bool CGeometryWnd<T>::IsGeometryCtl(HWND hwnd)
{
	TCHAR			className[128];
	TCHAR			wndCaption[256];
	long			classStyle;

	 ::GetClassName(hwnd,className,sizeof(className));
	 if (_tcscmp(className,_T("Button")) != 0)
		 return false;
	 
	 classStyle = ::GetWindowLong(hwnd,GWL_STYLE);
	 if ((classStyle & BS_GROUPBOX) == 0 ||
		 (((classStyle & BS_GROUPBOX) != 0) && (classStyle & WS_VISIBLE) != 0))
		 return false;

	 ::GetWindowText(hwnd,wndCaption,sizeof(wndCaption));
	 if (_tcsncmp(wndCaption,_T("$Geometry:"),10) != 0)
		 return false;

	 return true;
}

template <typename T> inline void CGeometryWnd<T>::GeometryInitialize()
{
	T*	pThis = static_cast<T*>(this);

	pThis->GetClientRect(m_minRect);
	pThis->GetClientRect(m_originalRect);

	BuildGeometryList();
	m_cList.SetRects(pThis->GetSafeHwnd());
}

template <typename T> inline void CGeometryWnd<T>::BuildGeometryList()
{
	T*			pThis = static_cast<T*>(this);
	TCHAR		wndCaption[256];
	TCHAR*		p;

	USES_CONVERSION;

	for (HWND hFrame = ::GetWindow(pThis->GetSafeHwnd(),GW_CHILD); hFrame; hFrame = ::GetWindow(hFrame,GW_HWNDNEXT))
		if (IsGeometryCtl(hFrame))
		{
			::GetWindowText(hFrame,wndCaption,sizeof(wndCaption));

			if ((p = _tcsstr(wndCaption,_T("$Geometry:"))) != NULL)
				p += 10;

			for (HWND hCtl = ::GetWindow(pThis->GetSafeHwnd(),GW_CHILD); hCtl; hCtl = ::GetWindow(hCtl,GW_HWNDNEXT))
				if (!IsGeometryCtl(hCtl) && IsInFrame(hFrame,hCtl))
					AddConstraint(::GetDlgCtrlID(hCtl),CConstraint(T2A(p)),this);
		}
}

template <typename T> inline bool CGeometryWnd<T>::IsInFrame(HWND hFrame,HWND hCtl)
{
	CRect	rFrame,rCtl,rDest;

	::GetWindowRect(hFrame,rFrame);									// the window area of the frame
	::GetWindowRect(hCtl,rCtl);										// the window area of the control

	rDest.IntersectRect(rFrame,rCtl);

	return (rCtl == rDest)? true : false;							// rCtl is contained into rFrame
}

inline void InvalidateRects(CWnd* pWnd,CRect irect,CRect frect)
{
	// irect is the initial rect of the control
	// frect is the final rect of the control (in parent's coordinates)
	CRect drect;
	static CRect prect;

	if (frect.TopLeft() != irect.TopLeft())
		return;	
	
	irect.IntersectRect(prect,irect);
	drect.UnionRect(irect,frect);
	pWnd->InvalidateRect(drect);
	drect.IntersectRect(irect,frect);
	drect.DeflateRect(0,0,3,3);
	pWnd->ValidateRect(drect);

	pWnd->GetClientRect(prect);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CGeometryWnd<typename T>

template <typename T> const AFX_MSGMAP* CGeometryWnd<T>::GetMessageMap() const
{
	return T::GetMessageMap();
};

template <typename T> inline BOOL CGeometryWnd<T>::OnWndMsg(UINT message,WPARAM wParam,LPARAM lParam,LRESULT* pResult)
{
	switch (message)
	{
	case WM_CREATE:													// Property sheets
		if (IsKindOf(RUNTIME_CLASS(CPropertySheet)))
			_OnPSCreate(reinterpret_cast<LPCREATESTRUCT>(lParam));
		break;

	case WM_INITDIALOG:
		T::OnWndMsg(message,wParam,lParam,pResult);

		if (IsKindOf(RUNTIME_CLASS(CPropertySheet)))				// init dialog for property sheets
			_OnPSInitDialog();
		else
			_OnInitDialog();										// and for dialogs and property pages

		return TRUE;
	
	case WM_SIZE:
		if (IsKindOf(RUNTIME_CLASS(CPropertySheet)) || IsKindOf(RUNTIME_CLASS(CFormView)))
			_OnPSorFVSize(wParam,LOWORD(lParam),HIWORD(lParam));
		else if (IsKindOf(RUNTIME_CLASS(CPropertyPage)))
			_OnPPSize(wParam,LOWORD(lParam),HIWORD(lParam));
		else if (IsKindOf(RUNTIME_CLASS(CDialog)))
			_OnSize(wParam,LOWORD(lParam),HIWORD(lParam));
		break;

	case WM_GETMINMAXINFO:
		if (IsKindOf(RUNTIME_CLASS(CPropertySheet)))
			_OnPSGetMinMaxInfo(reinterpret_cast<LPMINMAXINFO>(lParam));
		else if (IsKindOf(RUNTIME_CLASS(CPropertyPage)))
			_OnPPGetMinMaxInfo(reinterpret_cast<LPMINMAXINFO>(lParam));
		else if (IsKindOf(RUNTIME_CLASS(CDialog)))
			_OnGetMinMaxInfo(reinterpret_cast<LPMINMAXINFO>(lParam));
		break;
	}

	return T::OnWndMsg(message,wParam,lParam,pResult);
};

template <typename T> inline int CGeometryWnd<T>::_OnPSCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!(lpCreateStruct->style & WS_CHILD))
		ModifyStyle(WS_DLGFRAME,WS_THICKFRAME);

	return 0;
};

template <typename T> inline BOOL CGeometryWnd<T>::_OnInitDialog()
{
	GeometryInitialize();
	return TRUE;
};

template <typename T> inline BOOL CGeometryWnd<T>::_OnPSInitDialog()
{
	CRect			rClient;
	CPropertySheet*	pThis = reinterpret_cast<CPropertySheet*>(this);

	GetClientRect(rClient);

	::CreateWindowEx(0,_T("BUTTON"),_T("$Geometry:CX+CY"),WS_CHILD|BS_GROUPBOX,
					 0,0,rClient.Width(),rClient.Height(),
					 GetSafeHwnd(),(HMENU)IDC_STATIC,AfxGetInstanceHandle(),NULL);
	::CreateWindowEx(0,_T("BUTTON"),_T("$Geometry:X+Y"),WS_CHILD|BS_GROUPBOX,
					 0,rClient.Height()-60,rClient.Width(),60,
					 GetSafeHwnd(),(HMENU)IDC_STATIC,AfxGetInstanceHandle(),NULL);

	int ap = pThis->GetActiveIndex();
	for (int i = 0; i < pThis->GetPageCount(); i++)
		pThis->SetActivePage(i);
	pThis->SetActivePage(ap);

	GeometryInitialize();

	return TRUE;
};

template <typename T> inline void CGeometryWnd<T>::_OnPSorFVSize(UINT nType,int cx,int cy)
{
	bool	b;

	if (nType == SIZE_MINIMIZED)
		return;

	if (GetSafeHwnd() == NULL || m_minRect.Width() == 0)
		return;

	if (cx == 0 || cy == 0)
		return;
	
	b = (cx > m_minRect.Width()) || (m_originalRect.Width() > m_minRect.Width()) ||
		(cy > m_minRect.Height()) || (m_originalRect.Height() > m_minRect.Height());

	if (b)
		m_cList.ResizeControls(GetSafeHwnd(),CSize(cx,cy),CSize(m_minRect.Width(),m_minRect.Height()));

	GetClientRect(m_originalRect);
	if (m_originalRect.right < m_minRect.right)
		m_originalRect.right = m_minRect.right;
	if (m_originalRect.bottom < m_minRect.bottom)
		m_originalRect.bottom = m_minRect.bottom;
}

template <typename T> inline void CGeometryWnd<T>::_OnSize(UINT nType,int cx,int cy)
{
	if (nType == SIZE_MINIMIZED)
		return;

	if (GetSafeHwnd() == NULL)
		return;

	m_cList.ResizeControls(GetSafeHwnd(),CSize(cx,cy),CSize(m_minRect.Width(),m_minRect.Height()));

	GetClientRect(m_originalRect);
};

template <typename T> inline void CGeometryWnd<T>::_OnPPSize(UINT nType,int cx,int cy)
{
	bool			b;
	CPropertySheet*	pSheet = (CPropertySheet*)GetParent();

	if (nType == SIZE_MINIMIZED)
		return;

	if (GetSafeHwnd() == NULL)
		return;

	if (pSheet->GetPageIndex((CPropertyPage*)this) != pSheet->GetActiveIndex())
		ShowWindow(SW_HIDE);
		
	if (cx == 0 || cy == 0)
		return;
	
	b = (cx > m_minRect.Width()) || (m_originalRect.Width() > m_minRect.Width()) ||
		(cy > m_minRect.Height()) || (m_originalRect.Height() > m_minRect.Height());

	if (b)
		m_cList.ResizeControls(GetSafeHwnd(),CSize(cx,cy),CSize(m_minRect.Width(),m_minRect.Height()));

	GetClientRect(m_originalRect);
	if (m_originalRect.right < m_minRect.right)
		m_originalRect.right = m_minRect.right;
	if (m_originalRect.bottom < m_minRect.bottom)
		m_originalRect.bottom = m_minRect.bottom;
};

template <typename T> inline void CGeometryWnd<T>::_OnPSGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	int		cxframe = 0,cyframe = 0;
	long	style;

	if (GetSafeHwnd() == NULL)
		return;

	style = GetStyle();

	if (style & WS_CAPTION)
		cyframe += GetSystemMetrics(SM_CYCAPTION);

	if (style & WS_THICKFRAME)
	{
		cxframe += GetSystemMetrics(SM_CXSIZEFRAME) * 2;
		cyframe += GetSystemMetrics(SM_CYSIZEFRAME) * 2;
	}

	if (m_minRect.Width() != 0)
		lpMMI->ptMinTrackSize.x = m_minRect.Width() + cxframe;
	if (m_minRect.Height() != 0)
		lpMMI->ptMinTrackSize.y = m_minRect.Height() + cyframe;
};

template <typename T> inline void CGeometryWnd<T>::_OnPPGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	if (GetSafeHwnd() == NULL)
		return;

	if (m_minRect.Width() != 0)
		lpMMI->ptMinTrackSize.x = m_minRect.Width();
	if (m_minRect.Height() != 0)
		lpMMI->ptMinTrackSize.y = m_minRect.Height();
};

template <typename T> inline void CGeometryWnd<T>::_OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	int		cxframe = 0,cyframe = 0;
	long	style;

	if (GetSafeHwnd() == NULL)
		return;

	style = GetStyle();

	if (style & WS_CAPTION)
		cyframe += GetSystemMetrics(SM_CYCAPTION);

	if (style & WS_THICKFRAME)
	{
		cxframe += GetSystemMetrics(SM_CXSIZEFRAME) * 2;
		cyframe += GetSystemMetrics(SM_CYSIZEFRAME) * 2;
	}

	if (m_minRect.Width() != 0)
		lpMMI->ptMinTrackSize.x = m_minRect.Width() + cxframe;
	if (m_minRect.Height() != 0)
		lpMMI->ptMinTrackSize.y = m_minRect.Height() + cyframe;
};

template <typename T> BOOL CGeometryWnd<T>::OnNotify(WPARAM wParam,LPARAM lParam,LRESULT* pResult)
{
	NMHDR* pNMHDR = (NMHDR*)lParam;
	CRect	rWnd;

	T::OnNotify(wParam,lParam,pResult);

	// don't handle messages not from the page/sheet itself
	if (pNMHDR->hwndFrom != m_hWnd && pNMHDR->hwndFrom != ::GetParent(m_hWnd))
		return FALSE;

	if (pNMHDR->code == PSN_SETACTIVE)
	{
		GetWindowRect(rWnd);
		SetWindowPos(NULL,0,0,rWnd.Width(),rWnd.Height(),SWP_NOZORDER|SWP_NOMOVE);
	}

	*pResult = 0;
	return TRUE;
}

};																	// Geometry namespace

#endif																// _GEOMETRY_H_
