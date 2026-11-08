// STRegionsListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "math.h"
#include "resource.h"

#include "STImageList.h"
#include "Eressea.h"
#include "CR.h"
#include "StatsCR.h"
#include "wSTCVSortedListCtrl.h"
#include "STRegionsListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int predefinedInt(const char * regionname,const char *pEntry)
{
	CString sKeyName;
	sKeyName.Format("%s.%s",regionname,pEntry);
	return ::GetPrivateProfileInt("resources",(LPCSTR)sKeyName,0,".\\regionstats.ini");
}

BOOL predefinedBool(const char * regionname,const char *pEntry)
{
	return (predefinedInt(regionname,pEntry)==1);
}

CString predefinedStr(const char * regionname,const char *pEntry)
{
	CString sKeyName;
	CString sValue;
	sKeyName.Format("%s.%s",regionname,pEntry);
	::GetPrivateProfileString("resources",(LPCSTR)sKeyName,"",sValue.GetBuffer(50),50,".\\regionstats.ini");
	sValue.ReleaseBuffer();
	if (!sValue.IsEmpty()) return CCR::English2German(sValue);
	return "";
}

BOOL CRegionItemInfo::m_bCompareIntByDiffs = FALSE;
BOOL CRegionItemInfo::m_bWithDiffs = TRUE;
UINT CRegionItemInfo::m_nMaxDigits[COL_NBMAX][V_NB];
UINT CRegionItemInfo::m_nMaxValues[COL_NBMAX][V_NB];
BOOL CRegionItemInfo::m_bWithIndent = FALSE;
/*
	CString m_sColumnTitle;
	CString m_sNumericCREntry; // empty only for column with not numeric values
	UINT	m_nColumnDefaultWidth;		
	UINT	m_nColumnCurrentWidth;
	*/

TColumnInfos CSTRegionsListCtrl::columnInfos[COL_NBMAX]=
{
	TColumnInfos(EN_REGION,"",95,95),
	TColumnInfos(EN_TERRAIN,"",60,60),
	TColumnInfos(EN_HERB,"",90,90,SET_RESOURCE),
	TColumnInfos(EN_CASTLE,CR_SIZE,45,45),
	TColumnInfos(EN_STONES,CR_STONES,60,60,SET_RESOURCE_WITH_SKILL),
	TColumnInfos(EN_IRONS,CR_IRONS,55,55,SET_RESOURCE_WITH_SKILL),
	TColumnInfos(EN_LAEN,CR_LAEN,10,10,SET_RESOURCE_WITH_SKILL),
	TColumnInfos(EN_TREES,CR_TREES,55,55,SET_RESOURCE),
	TColumnInfos(EN_YOUNGTREES,CR_YOUNGTREES,55,55,SET_RESOURCE),
	TColumnInfos(EN_HORSES,CR_HORSES,60,60,SET_RESOURCE),
	TColumnInfos(EN_PEASANTS,CR_PEASANTS,65,65,SET_SILVER),
	TColumnInfos(EN_TAX,"",60,60,SET_SILVER),
	TColumnInfos(EN_ENTERTAIN,CR_ENTERTAIN,90,90,SET_SILVER),
	TColumnInfos(EN_TRADE,"",45,45,SET_SILVER),
	TColumnInfos(EN_SILVER,CR_SILVER,90,90,SET_SILVER)
};

///////

/*
const char * herbAmountInEN(const CString & strHerbAmount)
{
	if (!strHerbAmount.CompareNoCase("viele")) return "many";
	if (!strHerbAmount.CompareNoCase("sehr viele")) return "very many";
	if (strHerbAmount.IsEmpty()) return "";
	return strHerbAmount;
}
*/

void CRegionItemInfo::init()
{
	if (m_bWithIndent)
	{
		for (int i=0;i<COL_NBMAX;i++) 
			for (int k=0;k<V_NB;k++)
				m_nMaxDigits[i][k] = 7; 
		for (int i=0;i<COL_NBMAX;i++) 
			for (int k=0;k<V_NB;k++)
				m_nMaxValues[i][k] = 0;
	}
}

void CRegionItemInfo::after()
{
	if (m_bWithIndent)
	{
		for (int i=0;i<COL_NBMAX;i++) {
			for (int k=0;k<V_NB;k++)
			{
				if (m_nMaxValues[i][k] < 100000) m_nMaxDigits[i][k] = 6;
				if (m_nMaxValues[i][k] < 10000) m_nMaxDigits[i][k] = 5;
				if (m_nMaxValues[i][k] < 1000) m_nMaxDigits[i][k] = 4;
				if (m_nMaxValues[i][k] < 100) m_nMaxDigits[i][k] = 3;
			}
		}
	}
}

CRegionItemInfo::CRegionItemInfo (int iItem, const CString & strName, const CString & strTerrain, const CString & strHerb/*, const CString & strHerbAmount*/, BOOL bHasUnits, BOOL bSelected) : CItemInfo(iItem), 
m_strName(strName), m_bHasUnits(bHasUnits), m_bMallorn(FALSE)
{
	m_bCRSelected = bSelected;
	m_strHerb = CCR::German2English(strHerb);
	/*
	if (!strHerbAmount.IsEmpty())
	{
		CString ssuff;
		ssuff.Format(" [%s]",herbAmountInEN(strHerbAmount));
		m_strHerb+= ssuff;
	}
	*/
	m_strType = CCR::German2English(strTerrain);

	m_strHerb.MakeLower();
	m_strType.MakeLower();
	for (int i=0;i<COL_NBMAX;i++) 
		for (int j=0;j<V_NB;j++) 
			m_nValues[i][j] = 0;
}

///////

BOOL CRegionItemInfo::HasSpecial(int nCol) const 
{ 
	if (m_bMallorn && nCol==COL_TREES) return TRUE;
	//if (m_bLaen && nCol==COL_IRONS) return TRUE;
	return FALSE;
} 

int CRegionItemInfo::resourceLevelFromValue(EColumnsIndex nCol, int nAmount) const
{
	int nLevel=0;
	if (CSTRegionsListCtrl::columnInfos[nCol].m_nSetType==SET_RESOURCE_WITH_SKILL && nAmount>1000)
	{
		while (nAmount > 1000) 
		{
			nAmount-=1000;
			nLevel++;
		}
	}
	return nLevel;
}

CString CRegionItemInfo::IntToStr(EColumnsIndex nCol) const
{
	CString s;
	CString sprf("");
	int nAmount = value(nCol);
	int nLevel = resourceLevelFromValue(nCol, nAmount);
	if (nLevel>0)
	{
		sprf.Format("T%d/",nLevel);
		nAmount -= 1000*nLevel;
	}
	if (m_bWithDiffs)
	{
		int nDiff = value(nCol,TRUE);
		if (m_bWithIndent)
		{
			//TODO: which maxDigits to use?
			int nDigitsAmount=m_nMaxDigits[nCol][V_AMOUNT];
			int nDigitsDelta=m_nMaxDigits[nCol][V_DELTA];
			if (!m_bCompareIntByDiffs)	
			{
				if (nDiff!=0) 
					s.Format("%s%*d [%0+*d]",(LPCSTR)sprf,nDigitsAmount,nAmount,nDigitsDelta,nDiff); 
				else 
					s.Format("%s%*d",(LPCSTR)sprf,nDigitsAmount,nAmount);
			}
			else 
			{
				if (nDiff!=0) 
					s.Format("[%0+*d] / %s%*d",nDigitsDelta,nDiff,(LPCSTR)sprf,nDigitsAmount,nAmount); 
				else 
					s.Format("%s%*d",(LPCSTR)sprf,nDigitsDelta+nDigitsAmount,nAmount);
			}
		}
		else
		{

			if (!m_bCompareIntByDiffs)	{
				if (nDiff!=0) 
					s.Format("%s%-d [%-+d]",(LPCSTR)sprf,nAmount,nDiff); 
				else 
					s.Format("%s%-d",(LPCSTR)sprf,nAmount);
			}
			else 
			{
				if (nDiff!=0) 
					s.Format("[%-+d] / %s%-d",nDiff,(LPCSTR)sprf,nAmount); 
				else 
					s.Format("%s%-d",(LPCSTR)sprf,nAmount);
			}
		}
	}
	else
		s.Format("%s%-d",(LPCSTR)sprf,nAmount); 
	return s;
}

void CRegionItemInfo::addToList(int nIndex, CSTRegionsListCtrl & regions) const
{
	if (nIndex!=-1)
	{
		regions.AddItem(m_strType, nIndex, COL_TYPE);
		regions.AddItem(m_strHerb, nIndex, COL_HERB);
		for (int i=COL_FIRSTNUM;i<COL_NBMAX;i++)
			regions.AddItem(IntToStr(EColumnsIndex(i)), nIndex, i);
	}
}

void CRegionItemInfo::checkMax(EColumnsIndex nCol) const
{
	if (m_bWithIndent)
	{
		int nv = value(nCol);
		int nvPrev = value(nCol,TRUE);
		UINT nval1=abs(nv);
		UINT nval2=abs(nvPrev);
		if (nval1 > nv) 
			m_nMaxValues[nCol][V_AMOUNT]=nval1;
		if (nval2 > nvPrev) 
			m_nMaxValues[nCol][V_DELTA]=nval2;
	}
}

void CRegionItemInfo::set(block *r, int nIndex, CSTRegionsListCtrl & regions, CStatsCR & cr)
{
	block *castle = cr.castle(r);
	block *b = castle;
	for (int i=COL_FIRSTNUM;i<COL_NBMAX;i++)
	{
		CString sEntry("");
		//if (i!=COL_LAENVALUE)
			sEntry=CSTRegionsListCtrl::columnInfos[i].m_sNumericCREntry;
		if (!sEntry.IsEmpty())
		{
			if (b) 
			{
				cr.geti(b, sEntry, m_nValues[i][V_AMOUNT],&m_nValues[i][V_DELTA]);
				if (CSTRegionsListCtrl::columnInfos[i].m_nSetType==SET_RESOURCE_WITH_SKILL)
				{
					b = cr.resourceByName(sEntry,NULL,r);
					if (b)
					{	
						int nMult=0;
						cr.geti(b, "skill", nMult);
						m_nValues[i][V_AMOUNT] += 1000*nMult;
					}
				}
			}
			checkMax((EColumnsIndex)i);
		}
		b = r;
	}

	// built castle size
	if (b) 
		m_nValues[COL_CASTLESIZE][V_DELTA] = cr.builtCastleSize(b);

	m_nValues[COL_MAXTRADE][V_AMOUNT] = m_nValues[COL_PEASANTS][V_AMOUNT]/100;
	m_nValues[COL_MAXTRADE][V_DELTA] = m_nValues[COL_PEASANTS][V_DELTA]/100;// trade=peasants/100
	m_nValues[COL_MAXENTERTAIN][V_DELTA]=m_nValues[COL_SILVER][V_DELTA]/20; // entertain=silver/20

	// prev castle size is not known nor computed, so use current
	int nCoef0 = taxRatioFromCastle(); 
	m_nValues[COL_MAXTAX][V_AMOUNT] = nCoef0*m_nValues[COL_PEASANTS][V_AMOUNT];
	m_nValues[COL_MAXTAX][V_DELTA] = nCoef0*m_nValues[COL_PEASANTS][V_DELTA];

	checkMax(COL_MAXTAX);
	checkMax(COL_MAXTRADE);
	checkMax(COL_MAXENTERTAIN);

	int nMallorn = 0;
	int nLaen = 0;
	int nLaenPrev = 0;
	if (cr.geti(r, CR_MALLORN, nMallorn) && nMallorn>0) 
		m_bMallorn = TRUE; 
	else 
		m_bMallorn = predefinedBool(name(),"mallorn");
	/*
	if (cr.geti(r, CR_LAEN, nLaen, &nLaenPrev) && nLaen>0) {
		m_bLaen = TRUE; 
		m_nValues[COL_LAENVALUE][V_AMOUNT]=nLaen;
		m_nValues[COL_LAENVALUE][V_DELTA]=nLaenPrev;
		// TODO: laen skill retrieve
	}
	else 
		m_bLaen = predefinedBool(name(),"laen");
		*/

	addToList(nIndex,regions);
}

/////////////////////////////////////////////////////////////////////////////
// CSTRegionsListCtrl

int CSTRegionsListCtrl::compare_s(const CString & s1, const CString & s2, BOOL bAsc)
{
	if (bAsc) return strcmp(s2, s1);
	return strcmp(s1, s2);
}

int CSTRegionsListCtrl::compare_i(CRegionItemInfo *p1, CRegionItemInfo *p2, EColumnsIndex nCol, ESortType nSortType)
{
	if (SORT_SPECIAL==nSortType) 
	{
		if (p1->HasSpecial(nCol) != p2->HasSpecial(nCol)) 
			return p1->HasSpecial(nCol) ? 1:0;
		return 0;
	}

	int n1 = p1->value(nCol,CRegionItemInfo::m_bCompareIntByDiffs);
	int n2 = p2->value(nCol,CRegionItemInfo::m_bCompareIntByDiffs);
	if (n2==n1)
	{
		n1 = p1->value(nCol,!CRegionItemInfo::m_bCompareIntByDiffs);
		n2 = p2->value(nCol,!CRegionItemInfo::m_bCompareIntByDiffs);
	}
	if (SORT_ASC==nSortType) return n2 >= n1;
	if (SORT_DESC==nSortType) return n2 < n1;
	return FALSE;
}

int CSTRegionsListCtrl::CompareItems(CItemInfo *pItemInfo1, CItemInfo *pItemInfo2)
{
	CRegionItemInfo *p1=(CRegionItemInfo *)pItemInfo1;
	CRegionItemInfo *p2=(CRegionItemInfo *)pItemInfo2;
	if (!p1 || !p2) return 0;

	int ncol = GetSortedColumn();
	ESortType nSort = GetSortType();
	if (IsNumericColumn()) return compare_i(p1,p2,(EColumnsIndex)ncol,nSort);
	if (COL_NAME==ncol) return compare_s(p1->name(),p2->name(),nSort);
	if (COL_TYPE==ncol) return compare_s(p1->type(),p2->type(),nSort);
	if (COL_HERB==ncol) return compare_s(p1->herb(),p2->herb(),nSort);
	return 0;
}

int CSTRegionsListCtrl::Build()
{
	int lFirstColumnNumber=0;
	m_Sorting=TRUE;
	m_bDeleteItemData=TRUE;
	SetFullRowSel(TRUE);
	int i = 0;
	for (i = 0; i < COL_NBMAX; i++)
		InsertColumn(i,columnInfos[i].m_sColumnTitle,LVCFMT_LEFT,columnInfos[i].m_nColumnCurrentWidth);

	m_HeaderCtrl.SetIconsBkColor(STCOLOR_MAGENTA);
	/*
	// icons management
	if (m_HeaderCtrl.SetWithIcons())
	{
		m_HeaderCtrl.SetTextWithIcons(TRUE);
		//m_HeaderCtrl.AddIcon(COL_NAME, IDB_HORSE,TRUE);
		//m_HeaderCtrl.AddIcon(COL_TYPE, IDB_HORSE, TRUE);
		//m_HeaderCtrl.AddIcon(COL_HERB, IDB_HORSE, TRUE);
		m_HeaderCtrl.AddIcon(COL_CASTLESIZE, IDB_BURG, TRUE);
		m_HeaderCtrl.AddIcon(COL_STONES, IDB_STONE, TRUE);
		m_HeaderCtrl.AddIcon(COL_IRONS, IDB_IRON, TRUE);
		m_HeaderCtrl.AddIcon(COL_TREES, IDB_TREES, TRUE);
		m_HeaderCtrl.AddIcon(COL_YOUNGTREES, IDB_YOUNGTREES, TRUE);
		m_HeaderCtrl.AddIcon(COL_HORSES, IDB_HORSE, TRUE);
		m_HeaderCtrl.AddIcon(COL_PEASANTS, IDB_PEASANT, TRUE);
		m_HeaderCtrl.AddIcon(COL_MAXTAX, IDB_TAX, TRUE);
		m_HeaderCtrl.AddIcon(COL_MAXENTERTAIN, IDB_ENTERTAIN, TRUE);
		m_HeaderCtrl.AddIcon(COL_MAXTRADE, IDB_TRADE, TRUE);
		m_HeaderCtrl.AddIcon(COL_SILVER, IDB_SILVER, TRUE);
		//m_HeaderCtrl.AddIcon(COL_, IDB_, TRUE);
		//m_HeaderCtrl.AddIcon(COL_, IDB_, TRUE);
		//m_HeaderCtrl.AddIcon(COL_, IDB_, TRUE);
	}
	*/
	return i;
}

BOOL CSTRegionsListCtrl::IsNumericColumn() const
{
	int ncol = GetSortedColumn();
	if (COL_FIRSTNUM>ncol) return FALSE;
	return TRUE;
}

BOOL CSTRegionsListCtrl::GetSortedColumnHasSpecial()
{
/*	int nCol=GetSortedColumn();
	if (nCol == COL_IRONS) return TRUE;
	if (nCol == COL_TREES) return TRUE;*/
	return FALSE;
}

void CSTRegionsListCtrl::Rebuild(const CRegionItemInfoList & regionsWithNoUnit, int nNbRegionsWithUnits, BOOL bRegionsWithUnitsOnly, BOOL bRegionsSelectedOnly, BOOL bResort)
{
	CRegionItemInfo *t = NULL;
	int nIndex;
	int n = nNbRegionsWithUnits;
	if (n > 0)
	{
		m_Sorting = FALSE;
		m_bDeleteItemData = FALSE;

		// no painting during change
		LockWindowUpdate();

		POSITION pos = regionsWithNoUnit.GetHeadPosition();
		while (pos)	{
			t = regionsWithNoUnit.GetNext(pos); 
			if (t)
			{
				if (bRegionsWithUnitsOnly)
				{
					LV_FINDINFO finfos;
					finfos.flags = LVFI_PARAM;
					finfos.lParam = (long)t;
					nIndex = FindItem(&finfos);
					DeleteItem(nIndex);
				}
				else
				{
					nIndex = AddItem(t->name(), n, 0, -1, -1,(long)t);
					t->addToList(nIndex, *this);
					n++;
				}
			}
		}
		// repaint changes
		UnlockWindowUpdate();

		m_bDeleteItemData = TRUE;
		m_Sorting = TRUE;
		if (bResort) ReSort();
	}
}


void CSTRegionsListCtrl::RebuildByDiffs()
{
	// no painting during change
	LockWindowUpdate();
	int nbItems=GetItemCount();
	for (int i=0;i<nbItems;i++)
	{
		CRegionItemInfo *p=(CRegionItemInfo *)GetItemData(i);
		for (int k=COL_FIRSTNUM;k<COL_NBMAX;k++)
			SetItemText(i, k, p->IntToStr((EColumnsIndex)k));
	}
	// repaint changes
	UnlockWindowUpdate();
}

int CSTRegionsListCtrl::onDrawText(CDC * pDC, LPCTSTR lpszString, int nCount, LPRECT lpRect, UINT nFormat, int nItem, int nColumn)
{
	COLORREF nBkColorMallorn=RGB(255,255,0); //yellow
	COLORREF nBkColorLaen=RGB(128,128,128); //gray
	
	if (nColumn==COL_TREES || nColumn==COL_IRONS) 
	{
		CRegionItemInfo *p=(CRegionItemInfo *)GetItemData(nItem);
		COLORREF cl = 0;
		if (p && p->HasSpecial(nColumn) && nColumn==COL_TREES) cl = nBkColorMallorn;
		if (p && p->HasSpecial(nColumn) && nColumn==COL_IRONS) cl = nBkColorLaen;
		if (cl!=0)
		{
			CRect rcLabel(*lpRect);
			int OFFSET_FIRST = pDC->GetTextExtent(_T(" "), 1 ).cx*2; // =2
			rcLabel.left-=OFFSET_FIRST;
			rcLabel.right+=OFFSET_FIRST;
			pDC->FillRect(&rcLabel,&CBrush(cl));
		}
	}
	return CSTCVSortedListCtrl::onDrawText(pDC,lpszString,nCount,lpRect,nFormat,nItem,nColumn);
}

CSTRegionsListCtrl::CSTRegionsListCtrl():CSTCVSortedListCtrl(TRUE,TRUE)
{
	m_bClientWidthSel=TRUE;
}

CSTRegionsListCtrl::~CSTRegionsListCtrl()
{
}

BEGIN_MESSAGE_MAP(CSTRegionsListCtrl, CSTCVSortedListCtrl)
	//{{AFX_MSG_MAP(CSTRegionsListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSTRegionsListCtrl message handlers



