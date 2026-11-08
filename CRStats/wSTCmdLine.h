// /------------------------------------------------------------
// | PapirusIncludeSource
// | 
// | File Name    : wSTCmdLine.h
// | Created By   : Scharles
// | Creation Date: Friday, February 02, 2001
// | 
// | Purpose      : 
// | 
//  ------------------------------------------------------------
#ifndef _WSTCMDLINE_H_
#define _WSTCMDLINE_H_

class CSTCmdLine: public CCommandLineInfo
{

public:
	CSTCmdLine();
	virtual ~CSTCmdLine();

public:
	CString CRFilename() {return m_sCRFilename;}
	CString HierarchyFilename() {return m_sHierarchyFilename;}

protected:
//implementation
	virtual void ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);

private:
//data
	CString		m_sCRFilename;
	CString		m_sHierarchyFilename;
};

#endif //_WSTCMDLINE_H_
//  ------------------------------------------------------------
