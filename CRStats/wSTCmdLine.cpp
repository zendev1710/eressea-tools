#include "stdafx.h"
#include "wSTCmdLine.h"

#define ST_TOKEN_HIERARCHY		_T("H=")
#define ST_TOKEN_CR				_T("C=")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CSTCmdLine::CSTCmdLine():
	CCommandLineInfo(),
	m_sCRFilename(_T("")),
	m_sHierarchyFilename(_T(""))
{
}

CSTCmdLine::~CSTCmdLine()
{
}

void CSTCmdLine::ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast )
{
	CString strParam = lpszParam;
	int iLen = strParam.GetLength();
	if (iLen == 1)
	{
		/*if (strParam.CompareNoCase(ST_TOKEN_AUTOCONNECT)==0)
			m_bAutoConnect = TRUE;
		else
		if (strParam.CompareNoCase(ST_TOKEN_TESTOPEN)==0)
			m_bTestOpen = TRUE;
			*/
	} else
		if (iLen>1)
		{
			CString strToken = strParam.Left(2);
			CString strValue = strParam.Right(iLen-2);
			//remove quotes
			int vLen = strValue.GetLength();
			if (vLen>1)
			{
				if (strValue.Right(1)=="\"")
				{
					vLen--;
					if (strValue.Left(1)=="\"")
					{
						vLen--;
						strValue = strValue.Mid(2,vLen);
					}
				}
			}
			strToken.MakeUpper();
			if (strToken.Compare(ST_TOKEN_HIERARCHY)==0)
				m_sHierarchyFilename = strValue;
			else
			if (strToken.Compare(ST_TOKEN_CR)==0)
				m_sCRFilename = strValue;

		}
	CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast );
}
