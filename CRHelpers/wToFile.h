// /------------------------------------------------------------
// | SharingIncludeSource
// | 
// | File Name    :wToFile.h
// | Created By   :Scharles
// | Creation Date: Tuesday, August 01, 2000
// | 
// | Contain      : ###
// | 
//  ------------------------------------------------------------
#ifndef _WTOFILE_H_
#define _WTOFILE_H_


BOOL STExistFile(const CString & strFName);
BOOL STCanReadFile(const CString & strFName, CFileException *pExcep = NULL);
BOOL STCanAccessFile(const CString & strFName, CFileException *pExcep = NULL);
BOOL STIsWritableFile(const CString & strFName, CFileException *pExcep = NULL);

CString STGetFullPathname(const CString & szFullPathname);

///////////

inline CString STGetFileExtension(const CString & rStrPathname, BOOL bWithDot = TRUE)
{
	CString strExtension("");
	int iDotChar = rStrPathname.ReverseFind('.');
	if ((iDotChar != -1) && (iDotChar != rStrPathname.GetLength()-1))
	{
		if (!bWithDot) iDotChar++;
		strExtension = rStrPathname.Right(rStrPathname.GetLength() - iDotChar);
	}
	return strExtension;
}

///////////

inline BOOL STIsFileExtension(const CString & rStrPathname, const CString & rStrExt)
{
	CString strExt = STGetFileExtension(rStrPathname,(rStrExt[0] == '.'));
	return (strExt.CompareNoCase(rStrExt) == 0);
}

///////////

inline CString STGetShortPathname(const CString & strFilePath, BOOL bWithExtension=FALSE)
{
	CString sTitle=strFilePath;
		
	if (!strFilePath.IsEmpty())
	{
		int lInd=strFilePath.ReverseFind('\\');
		if (lInd!=-1)
			sTitle=strFilePath.Right(strFilePath.GetLength()-lInd-1);
		lInd=sTitle.ReverseFind('.');
		if (lInd!=-1 && !bWithExtension)
			sTitle=sTitle.Left(lInd);
	}
	else
		sTitle="";

	return sTitle;
}

/*
inline CString STGetLongFilename(CString strShortFileName)
{
	WIN32_FIND_DATA lFindFileData; 
	HANDLE hdl=FindFirstFile(strShortFileName,&lFindFileData);
	
	CString strLongFileName = strShortFileName;

	if (hdl!=INVALID_HANDLE_VALUE)
	{
		CString strFileName = lFindFileData.cFileName;
		FindClose(hdl);

		CString strPath;
		int nIndex = strShortFileName.ReverseFind('\\');
		if (nIndex != -1)
		{
			CString strPath = strShortFileName.Left(nIndex + 1);
			strLongFileName = strPath + strFileName;
		}
	}

	return strLongFileName;
}

CString STGetFileFolder(LPCTSTR lpszFileName)
{
	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR];
	_tsplitpath(lpszFileName, szDrive, szDir, NULL, NULL);
	return CString(szDrive) + CString(szDir);
}
*/

///////////

inline CString STGetDisplayShortPathname(const CString & strFilePath, BOOL bWithExtension=FALSE)
{

//JO :25/01/2002
	//The above solution caused some problems on WinMe. We now rely on shell functions to retrieve the
	//short name hoping that their implementation isn't Windows version specific ;-)
	CString sTitle=strFilePath;
	SHFILEINFO sfi; 
	memset(&sfi, 0, sizeof(sfi));
	
	unsigned int flag= SHGFI_DISPLAYNAME | SHGFI_USEFILEATTRIBUTES ;

	::SHGetFileInfo (sTitle, 0, &sfi, sizeof(sfi), flag);
	sTitle = sfi.szDisplayName;
		
	if (!strFilePath.IsEmpty())
	{
		int lInd=strFilePath.ReverseFind('\\');
		if (lInd!=-1)
			sTitle=strFilePath.Right(strFilePath.GetLength()-lInd-1);
		lInd=sTitle.ReverseFind('.');
		if (lInd!=-1 && !bWithExtension)
			sTitle=sTitle.Left(lInd);
	}
	else
		sTitle="";

	return sTitle;
}

///////////

inline BOOL STGetTempFilename(const CString & szFullFilename, CString & strUserPath, const CString & strSubdir = "")
{
	strUserPath="";
	CString strTempPath("");
	LPTSTR lpBuffer = strTempPath.GetBuffer(512);
	::GetTempPath(512,lpBuffer);
	strTempPath.ReleaseBuffer();

	if (!strSubdir.IsEmpty())
	{
		strTempPath += strSubdir;
		if (strTempPath.Right(1) != _T("\\")) strTempPath += _T("\\");
	}

	CString strUserFile = ::STGetShortPathname(szFullFilename,TRUE);
	if (!strUserFile.IsEmpty())
		strUserPath=strTempPath+strUserFile;
	return TRUE;
}

///////////

#endif //_WTOFILE_H_
//  ------------------------------------------------------------
