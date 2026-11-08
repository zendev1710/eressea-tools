// /------------------------------------------------------------
// | SharingCPPSource
// | 
// | File Name    :wToFile.h
// | Created By   :Scharles
// | Creation Date: Tuesday, August 01, 2000
// | 
// | Contain      : file existence checking
// | 
//  ------------------------------------------------------------


#include "stdafx.h"
#include "wToDir.h"
#include "wToFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL STExistFile(const CString & strFName)
{
	//C.G. 17/01/01: use GetStatic to know if file exist 
	// (CFile.Open function could return FALSE if File was opened by another processus)
	//+++
	CFileStatus rStatus;
	BOOL bRes = CFile::GetStatus( (LPCTSTR)strFName, rStatus );
//	return CFile::GetStatus( (LPCTSTR)strFName, rStatus );
	return bRes;
	//+++
}

//C.G. 17/01/01
//+++
BOOL STCanAccessFile(const CString & strFName, CFileException *pExcep)
{
	CFile sourceFile;
	// we'll use a CFileException object to get error information
	CFileException ex;
	CFileException *pEx=&ex;
	if (pExcep)	pEx=pExcep;
	
	// open the source file for reading
	if (!sourceFile.Open(strFName, CFile::modeRead | CFile::shareDenyWrite, pEx))
		return FALSE;

	sourceFile.Close();
	return TRUE;
}
//+++

///////////////////

BOOL STCanReadFile(const CString & strFName, CFileException *pExcep)
{
	CFile sourceFile;
	// we'll use a CFileException object to get error information
	CFileException ex;
	CFileException *pEx=&ex;
	if (pExcep)	pEx=pExcep;
	
	// open the source file for reading
	if (!sourceFile.Open(strFName, CFile::modeRead, pEx))
		return FALSE;

	sourceFile.Close();
	return TRUE;
}

///////////////////

BOOL STIsWritableFile(const CString & strFName, CFileException *pExcep)
{
	CFile sourceFile;
	// we'll use a CFileException object to get error information
	CFileException ex;
	CFileException *pEx=&ex;
	if (pExcep)	pEx=pExcep;
	
	// open the source file for reading
	if (!sourceFile.Open(strFName, CFile::modeReadWrite, pEx))
		return FALSE;

	sourceFile.Close();
	return TRUE;
}

CString STGetFullPathname(const CString & szFilename)
{
	TCHAR szFullPathname [_MAX_PATH];
	char * szPartFile[_MAX_PATH]={NULL};
	DWORD dres = ::GetFullPathName(szFilename,_MAX_PATH,szFullPathname,szPartFile);
	return szFullPathname;
}

///////////////////

