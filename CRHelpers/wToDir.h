// /------------------------------------------------------------
// | StepOneIncludeSource
// | 
// | File Name    :wToDir.h
// | Created By   :Scharles
// | Creation Date: Monday, August 07, 2000
// | 
// | Contain      : Declarations for looged user dir management
// | 
//  ------------------------------------------------------------
#ifndef _WTODIR_H_
#define _WTODIR_H_

///////////////////////////
// inlines functions header

BOOL STGetFullTempDirName(CString & strFinalDir, const CString & strSubdir = _T(""), DWORD * pErrorCode = NULL);
BOOL STCreateTempDir(CString & szFinalDir, const CString & szSubDir, DWORD * pErrorCode = NULL); // C.G. 09/07/2002
BOOL STCreateDir(const char *szConstDir, DWORD * pErrorCode = NULL);
BOOL STExistLocalDir(const CString & strDirName);

//////////////////////////////////////

///////////////////////////
// inlines functions body

// C.G. 09/07/2002 : returns a temporary dirname with trailing backslash, and using strSubdir
inline BOOL STGetFullTempDirName(CString & strFinalDir, const CString & strSubdir, DWORD * pErrorCode)
{
	strFinalDir = "";
	DWORD dwResult = ::GetTempPath(MAX_PATH,strFinalDir.GetBuffer(MAX_PATH));
	if (dwResult)
	{
		strFinalDir.ReleaseBuffer();
		if (strFinalDir.Right(1) != _T("\\")) strFinalDir += _T("\\");
		if (!strSubdir.IsEmpty())
		{
			strFinalDir += strSubdir;
			if (strFinalDir.Right(1)!=_T("\\"))	strFinalDir += _T("\\");
		}
		return TRUE;
	}

	if (pErrorCode)
		*pErrorCode = ::GetLastError();

	return FALSE;
}


///////////////

inline BOOL STCreateDir(const char * szConstDir, DWORD * pErrorCode)
{
	if (!szConstDir)
		return FALSE;

	DWORD dwErrorCode = ERROR_ALREADY_EXISTS;

	BOOL bDir = TRUE;
	//make a local copy to avoid problems
	int sLen = strlen(szConstDir);
	char *szDir = new char[sLen+1];
	strcpy_s(szDir, sLen, szConstDir);
	//now let's make the parse stuff
	char *szSubDir = szDir;
	// pass the drive
	for (;((*szSubDir) && (*szSubDir!='\\'));szSubDir++)
	{
	}
	//skip the root dir
	szSubDir++;
	// create sub dirs
	for (;(*szSubDir);szSubDir++)
	{
		switch(*szSubDir)
		{
		case '\\': // take the left part and create a subdir
			*szSubDir = '\0';
			bDir = ::CreateDirectory( szDir, NULL);
			// CG TATOOINE CR3 11/12/01:bug correction:if dir already exist, should continue
			if (!bDir) bDir=((dwErrorCode = ::GetLastError()) == ERROR_ALREADY_EXISTS);
			*szSubDir = '\\';
			break;
		}
		if (!bDir)
			break;
	}
	// if there is no trailing \, create leaf subdir
	szSubDir--;
	if (*szSubDir!='\\')
	{
		bDir = ::CreateDirectory( szDir, NULL);
		// CG TATOOINE CR3 11/12/01:bug correction:if dir already exist, should continue
		if (!bDir) bDir=((dwErrorCode = ::GetLastError()) == ERROR_ALREADY_EXISTS);
	}

	//clean up
	delete [] szDir;
	if (pErrorCode && (dwErrorCode != ERROR_ALREADY_EXISTS))
		*pErrorCode = dwErrorCode;

	return bDir;
}

///////////////

// C.G. 09/07/2002
inline BOOL STCreateTempDir(CString & strFinalDir, const CString & strSubdir, DWORD * pErrorCode)
{
	if (::STGetFullTempDirName(strFinalDir, strSubdir,pErrorCode))
		return ::STCreateDir(strFinalDir,pErrorCode);
	return FALSE;
}

///////////////

// caution: does not work for a network name !
inline BOOL STExistLocalDir(const CString & strDirName)
{
	CFileStatus rStatus;
	return (CFile::GetStatus( (LPCTSTR)strDirName, rStatus) && (rStatus.m_attribute & CFile::directory));
}

/////////////////////////////////////////////////////////////////////////////////
// other functions header (not inline)

// C.G. 10.08.2000: to use with caution !
// delete wanted files in the specified directory (do not delete directories)
// return number of deleted files
int STDeleteFiles(const char * szDirectory, 
				  const char * szFilter, 
				  DWORD nRequiredFlag=0, 
				  DWORD nForbiddenFlag=FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_READONLY|
				                       FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_TEMPORARY);

//C.G. 10.08.2000 : to use with caution !
//by default (i.e. STRemoveDir(szConstDir)), this function removes empty directories only
BOOL STCleanDir(const char *szConstDir, 
				BOOL bRemoveDirs=TRUE, 
				BOOL bDeleteFiles=FALSE, 
				const char *szFilesFilter="*.*", 
				DWORD nRequiredFlagForFiles=0, 
				DWORD nForbiddenFlagForFiles=FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_READONLY|
											 FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_TEMPORARY);


// C.G. 22/10/2001:retrieve directoryname from fullpathname
// if a directory is given as parameter, must absolutely have a trailing backslash
CString STGetDir(const CString & szFullPathname, BOOL bWithEndingSlash=TRUE);

///////////////////////

// C.G. 18.06.2002: store in rStrFilenamesList all filenames with specified filter (extension) & flags 
// NO recursion for this function - returns number of found files
long STGetFilesList(const char * szDirectory, CStringList & rStrFilenamesList, const char * szFilter = "*.*", DWORD nRequiredFlag = 0, DWORD nForbiddenFlag = 0);


// C.G. 18.06.2002: store in rStrFilenamesList all filenames with specified filter (extension) & flags 
// By default, recursion for this function - returns number of found files
// rStrFilenamesList should have been reseted before this function call
long STGetFilesListRecursively(const char *szConstDir, CStringList & rStrFilenamesList, BOOL bRecursively = TRUE, const char *szFilesFilter = "*.*", DWORD nRequiredFlagForFiles = 0, DWORD nForbiddenFlagForFiles = 0);

///////////////////////

#endif //_WTODIR_H_
//  ------------------------------------------------------------
