// /------------------------------------------------------------
// | StepOneImplementationSource
// | 
// | File Name    :wToDir.cpp
// | Created By   :Scharles
// | Creation Date: Monday, August 07, 2000
// | 
// | Contain      : Windows implementation for looged user dir management
// | 
//  ------------------------------------------------------------

#include "stdafx.h"
#include "wToDir.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////

//C.G. 10.08.2000
//to use with caution !
//delete wanted files in the specified directory (do not delete directories)
int STDeleteFiles(const char * szDirectory, const char * szFilter, DWORD nRequiredFlag, DWORD nForbiddenFlag)
{
	WIN32_FIND_DATA lFindFileData; 
	CString sFilename("");
	CString sDirectory(szDirectory);
	if (sDirectory.IsEmpty()) return 0;
	if (sDirectory.Right(1) != _T("\\")) sDirectory += _T("\\");
	CString sFilter(szFilter);
	CString sGlobalFilter = sDirectory + sFilter;
	
	HANDLE hdl=FindFirstFile(sGlobalFilter,&lFindFileData);
	BOOL lCorrect=(hdl!=INVALID_HANDLE_VALUE);
	
	int lNb=0;
	nForbiddenFlag |= FILE_ATTRIBUTE_DIRECTORY; //to be sure not to try to delete directories
	
	while (lCorrect)
	{
		if (!(lFindFileData.dwFileAttributes & nForbiddenFlag))
		{
			if ((lFindFileData.dwFileAttributes & nRequiredFlag)==nRequiredFlag)
			{
				sFilename=sDirectory+lFindFileData.cFileName;
				if (::DeleteFile(sFilename))
					lNb++;
			}
		}
		lCorrect=FindNextFile(hdl,&lFindFileData);
	}
	FindClose(hdl);
	
	return lNb;
}

//C.G. 10.08.2000
//to use with caution !
//by default (i.e. STRemoveDir(szConstDir)), this function removes empty directories only
BOOL STCleanDir(const char *szConstDir, BOOL bRemoveDirs, BOOL bDeleteFiles, const char *szFilesFilter, DWORD nRequiredFlagForFiles, DWORD nForbiddenFlagForFiles)
{
	WIN32_FIND_DATA lFindFileData; 
	
	CString sDirname;
	CString sDirectory(szConstDir);
	if (sDirectory.IsEmpty()) return 0;
	if (sDirectory.Right(1) != _T("\\")) sDirectory += _T("\\");

	
	//we scan all directories (with any attribute)
	DWORD nRequiredFlagDir=FILE_ATTRIBUTE_DIRECTORY;
	DWORD nForbiddenFlagDir=0;
	
	if (bDeleteFiles)
		STDeleteFiles(szConstDir,szFilesFilter,nRequiredFlagForFiles,nForbiddenFlagForFiles);
	
	CString sGlobalFilterDir = sDirectory + _T("*.*");
	
	HANDLE hdl=FindFirstFile(sGlobalFilterDir,&lFindFileData);
	BOOL lCorrect=(hdl!=INVALID_HANDLE_VALUE);
	while (lCorrect)
	{
		if ((!(lFindFileData.dwFileAttributes & nForbiddenFlagDir)) && ((lFindFileData.dwFileAttributes & nRequiredFlagDir)==nRequiredFlagDir))
		{
			//do not try to remove [.] and [..] directories
			if (_strcmpi(lFindFileData.cFileName,".") && _strcmpi(lFindFileData.cFileName,".."))
			{
				sDirname=sDirectory+lFindFileData.cFileName;
				STCleanDir(sDirname,bRemoveDirs,bDeleteFiles,szFilesFilter,nRequiredFlagForFiles,nForbiddenFlagForFiles);
			}
		}
		lCorrect=FindNextFile(hdl,&lFindFileData);
	}
	FindClose(hdl);

	if (bRemoveDirs)
		RemoveDirectory(szConstDir);

	return TRUE;
}

///////////////////////

//C.G. 18.06.2002: no recursion for this function
long STGetFilesList(const char * szDirectory, CStringList & rStrFilenamesList, const char * szFilter, DWORD nRequiredFlag, DWORD nForbiddenFlag)
{
	WIN32_FIND_DATA lFindFileData; 
	CString sFilename("");
	CString sDirectory(szDirectory);
	if (sDirectory.IsEmpty()) return 0;
	if (sDirectory.Right(1) != _T("\\")) sDirectory += _T("\\");
	CString sFilter(szFilter);
	CString sGlobalFilter = sDirectory + sFilter;
	
	HANDLE hdl=FindFirstFile(sGlobalFilter,&lFindFileData);
	BOOL lCorrect=(hdl!=INVALID_HANDLE_VALUE);
	
	int lNb = 0;
	nForbiddenFlag |= FILE_ATTRIBUTE_DIRECTORY; //search to be sure not to try to delete directories

	CString strSuffixExt(sFilter);
	strSuffixExt.MakeLower();
	long lLenSuffix = 0;
	long lInd = strSuffixExt.ReverseFind('.');
	if ((lInd != -1) && (strSuffixExt.ReverseFind('?') < lInd) &&  (strSuffixExt.ReverseFind('*') < lInd))
	{
		lLenSuffix = strSuffixExt.GetLength()-lInd;
		strSuffixExt = strSuffixExt.Right(lLenSuffix); //'.' is included
	}
	
	while (lCorrect)
	{
		if (!(lFindFileData.dwFileAttributes & nForbiddenFlag))
		{
			if ((lFindFileData.dwFileAttributes & nRequiredFlag)==nRequiredFlag)
			{
				BOOL bAdd = TRUE;

				// verify exact matching on extension
				// else toto.txt_old should be found for "*.txt" filter
				if (lLenSuffix > 0)
				{
					CString strShortFilename(lFindFileData.cFileName);
					strShortFilename.MakeLower();
					if (strShortFilename.Find(strSuffixExt) != (strShortFilename.GetLength()-lLenSuffix))
						bAdd = FALSE;
				}

				if (bAdd)
				{
					sFilename = sDirectory + lFindFileData.cFileName;
					rStrFilenamesList.AddTail(sFilename);
					lNb++;
				}
			}
		}
		lCorrect=FindNextFile(hdl,&lFindFileData);
	}
	FindClose(hdl);
	
	return lNb;
}

///////////////////////

// C.G. 18.06.2002: returns number of found files
// rStrFilenamesList should have been reseted before this function call
long STGetFilesListRecursively(const char *szConstDir, CStringList & rStrFilenamesList, BOOL bRecursively, const char *szFilesFilter, DWORD nRequiredFlagForFiles, DWORD nForbiddenFlagForFiles)
{
	WIN32_FIND_DATA lFindFileData; 
	
	CString sDirname;
	CString sDirectory(szConstDir);
	if (sDirectory.IsEmpty()) return 0;
	if (sDirectory.Right(1) != _T("\\")) sDirectory += _T("\\");
	
	//we scan all directories (with any attribute)
	DWORD nRequiredFlagDir = FILE_ATTRIBUTE_DIRECTORY;
	DWORD nForbiddenFlagDir = 0;

	long lNbFiles = STGetFilesList(szConstDir,rStrFilenamesList,szFilesFilter,nRequiredFlagForFiles,nForbiddenFlagForFiles);

	if (bRecursively)
	{
		CString sGlobalFilterDir = sDirectory + "*.*";
		
		HANDLE hdl=::FindFirstFile(sGlobalFilterDir,&lFindFileData);
		BOOL lCorrect=(hdl!=INVALID_HANDLE_VALUE);
		while (lCorrect)
		{
			if ((!(lFindFileData.dwFileAttributes & nForbiddenFlagDir)) && ((lFindFileData.dwFileAttributes & nRequiredFlagDir)==nRequiredFlagDir))
			{
				//do not try to remove [.] and [..] directories
				if (_strcmpi(lFindFileData.cFileName,".") && _strcmpi(lFindFileData.cFileName,".."))
				{
					sDirname = sDirectory + lFindFileData.cFileName;
					lNbFiles += STGetFilesListRecursively(sDirname,rStrFilenamesList,TRUE,szFilesFilter,nRequiredFlagForFiles,nForbiddenFlagForFiles);
				}
			}
			lCorrect=FindNextFile(hdl,&lFindFileData);
		}
		FindClose(hdl);
	}
	return lNbFiles;
}

///////////

//C.G. 22/10/2001:retrieve directoryname from fullpathname
CString STGetDir(const CString & szFullPathname, BOOL bWithEndingSlash)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR]; 
	_splitpath_s((const char *)szFullPathname, drive, _MAX_DRIVE, dir,_MAX_DIR,  NULL, 0, NULL, 0);
	CString strDirectory = drive; 
	strDirectory += dir;
	if (strDirectory.IsEmpty()) return "";
	BOOL bHasSlash = (strDirectory.Right(1) == _T("\\"));
	if (bHasSlash!=bWithEndingSlash)
	{
		if (bWithEndingSlash)
			strDirectory += _T("\\");
		else
			strDirectory = strDirectory.Left(strDirectory.GetLength()-1); //remove ending slash
	}
	return strDirectory;
}

//////////////////////////////////////////
