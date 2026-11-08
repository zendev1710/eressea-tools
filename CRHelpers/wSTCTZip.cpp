// /------------------------------------------------------------
// | PapirusCPPSource
// | 
// | File Name    : wSTCTZip.cpp
// | Created By   : HZarrabi
// | Creation Date: lundi, février 11, 2002
// | 
// | Purpose      : 
// | 
//  ------------------------------------------------------------


#include "stdafx.h"
#include "wSTCTZip.h"

#include "kSTCTTempFolders.h"
#include "wToFile.h"
#include "wToDir.h"

#include ST_INC_CTCTZIP(kSTCTZipError.h)

#include ST_INC_CTCTZIP(ZipFile.h)
#include ST_INC_CTCTZIP(unzipFile.h)
#include ST_INC_CTCTZIP(wSTStreamZipFile.h)
#include ST_INC_CTCTZIP(wSTStreamUnzipFile.h)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// static buffer size
#define	BUF_SIZE	4096

//////////////////////////////////////////////////////////////

BOOL CopyTempDocumentFile(const CString & szFullFilename, CString & strUserPath, DWORD & dwError, const CString & strSubdir, BOOL bCanOverwrite)
{
	BOOL bFailIfExists = !bCanOverwrite;
	dwError=0;

	if (STGetTempFilename(szFullFilename,strUserPath,strSubdir))
	{
		if (!strSubdir.IsEmpty())
		{
			CString strFullDir=::STGetDir(strUserPath, FALSE);
			if (!::STCreateDir(strFullDir))
			{
				dwError = ::GetLastError();
				return FALSE;
			}
		}

		if (!::CopyFile(szFullFilename,strUserPath,bFailIfExists))
		{
			dwError = ::GetLastError();
			long nTraceLevel=0;
			if (dwError==ERROR_FILE_EXISTS)
				nTraceLevel=1;
			return FALSE;
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CopyTemp(const CString & szFullFilename, CString & szDstFilename)
{
	DWORD errorCode = 0;
	BOOL bOverwrite = TRUE;
	return CopyTempDocumentFile(szFullFilename,szDstFilename,errorCode,STCTTOOLS_DEFAULT_ZIP_TEMPSUBDIR,bOverwrite);
}

// /------------------------------------------------------------
// | PapirusFunction
// | 
// | Funct  Name: STZip
// | 
// | Return Type: CSTError
// | 
// | Parameters:  const CString & strFullFileName
// | 
// | Purpose: 
// | 
//  ------------------------------------------------------------
CSTError STZip(const CString & rStrFullFileName)
{
	CSTError error;

	//////////////////////////

	CString strFullFileName(rStrFullFileName);
	CString szDstFilename("");
	CFile * pFile = NULL;
	BOOL bCopy = FALSE;

	int index = 0;
	int indexChar = -1;
	
	
	do{
		index = strFullFileName.Find('.', index);
		if( index != -1 )
		{
			indexChar = index++;
		}
	}while( index != -1);
	
	if( indexChar == -1 )
	{
		throw new CException(1);
	}
	
	// Extraction of the file name without extension
	CString strFullFileNameWithoutExt( strFullFileName.Left(indexChar));

	TRY
	{
		pFile = new CFile(strFullFileName, CFile::modeRead);
	}

	CATCH(CFileException, pEx)
	{
		if (pEx->m_cause == CFileException::sharingViolation && CopyTemp(strFullFileName, szDstFilename))
		{
			bCopy = TRUE;
			strFullFileName = szDstFilename;
			pFile = new CFile(strFullFileName, CFile::modeRead | CFile::shareDenyNone | CFile::modeNoInherit);
			if (!pFile)	error.Add(STCTZIP_ERROR(ST_CTZIP_COMPRESS_FAILED));
		}
		else
		{
			CString szMsg("");
			TCHAR   szCause[255];
			pEx->GetErrorMessage(szCause, 255);
			szMsg.Format("File exception on <%s> - cause : %s\n",pEx->m_strFileName,szCause);
			error.Add(STCTZIP_ERRORMSG(szMsg));
			error.Add(STCTZIP_ERROR(ST_CTZIP_COMPRESS_FAILED));
		}
	}
	END_CATCH

	if (!error)
	{
		// adding zip extension
		CString strFullZipFileName(strFullFileNameWithoutExt + ".zip");
		CZipFile zf(strFullZipFileName, 0);

		try
		{
			char buf[BUF_SIZE];
			
			zip_fileinfo zi;
			
			// getting information about the date and the attributes
			// (this is new in ZipFunc)
			zf.UpdateZipInfo(zi, *pFile);
			
			// getting the file Name
			int index = 0;
			int indexChar = -1;
			
			do{
				index = strFullFileName.Find('\\', index);
				if( index != -1 )
				{
					indexChar = ++index;
				}
			}while( index != -1);
			
			if( indexChar == -1 )
			{
				throw new CException(1);
			}
			
			CString strFileName( strFullFileName.Mid(indexChar));
			
			zf.OpenNewFileInZip(strFileName, zi, Z_BEST_COMPRESSION);
			
			int size_read;
			do
			{
				size_read = pFile->Read(buf, BUF_SIZE);
				if (size_read)
					zf.WriteInFileInZip(buf, size_read);
				
			}
			while (size_read == BUF_SIZE);
			
			// cannot be called by the destructor because it may throw an exception
			// (it is not good to throw an exception when another one may be progress)
			zf.Close();
		}
		catch (CException* e)
		{
			if (e->IsKindOf( RUNTIME_CLASS( CZipException )))
			{
				CZipException* p = (CZipException*) e;
				if (p)
				{
					CString szMsg("");
					szMsg.Format("zip exception:cause=%d,zip=%d,filename=<%s>\n",p->m_iCause,p->m_bDuringZip,p->m_szFileName);
					error.Add(STCTZIP_ERRORMSG(szMsg));
				}
				error.Add(STCTZIP_ERROR(ST_CTZIP_COMPRESS_FAILED));
			}
			else if (e->IsKindOf( RUNTIME_CLASS( CFileException )))
			{
				CFileException* p = (CFileException*) e;
				if (p)
				{
					CString szMsg("");
					TCHAR   szCause[255];
					p->GetErrorMessage(szCause, 255);
					szMsg.Format("File exception on <%s> - cause : %s\n",p->m_strFileName,szCause);
					error.Add(STCTZIP_ERRORMSG(szMsg));
				}
				error.Add(STCTZIP_ERROR(ST_CTZIP_COMPRESS_FAILED));
			}
			else // the only possibility is a memory exception I suppose
			{
				CString szMsg("");
				TCHAR   szCause[255];
				e->GetErrorMessage(szCause, 255);
				szMsg.Format("Exception - cause : %s\n",szCause);
				error.Add(STCTZIP_ERRORMSG(szMsg));
				error.Add(STCTZIP_ERROR(ST_CTZIP_COMPRESS_FAILED));
			}
			e->Delete();
		}
	}

	if (pFile != NULL)
	{
		pFile->Close();
		delete pFile;
	}

	if (bCopy && !szDstFilename.IsEmpty()) 
		::DeleteFile(szDstFilename);

	return error;
}
// End of CSTError STZip(const CString & strFullFileName )

///////////////////////////
// C.G. 04/03/2002: helper to return stream (instead of creating .zip file) for filename to zip
CSTError STZip(const CString &rszFullFilename, IStream *& pZippedStream)
{
	CSTError error;

	int index = 0;
	int indexChar = -1;

	CFile * pFile = NULL;
	CString szFullFilename(rszFullFilename);
	CString szDstFilename("");
	BOOL bCopy = FALSE;

	TRY
	{
		pFile = new CFile(szFullFilename, CFile::modeRead);
	}

	CATCH(CFileException, pEx)
	{
		if (pEx->m_cause == CFileException::sharingViolation && CopyTemp(szFullFilename, szDstFilename))
		{
			bCopy = TRUE;
			szFullFilename = szDstFilename;
			pFile = new CFile(szFullFilename, CFile::modeRead);
			if (!pFile)	error.Add(STCTZIP_ERROR(ST_CTZIP_COMPRESS_FAILED));
		}
		else
		{
			CString szMsg("");
			TCHAR   szCause[255];
			pEx->GetErrorMessage(szCause, 255);
			szMsg.Format("File exception on <%s> - cause : %s\n",pEx->m_strFileName,szCause);
			error.Add(STCTZIP_ERRORMSG(szMsg));
			error.Add(STCTZIP_ERROR(ST_CTZIP_COMPRESS_FAILED));
		}
	}
	END_CATCH
		
	if (!error)
	{
		CSTStreamZipFile zf;
		try
		{
			char buf[BUF_SIZE];
			zip_fileinfo zi;
			
			// getting information about the date and the attributes
			// (this is new in ZipFunc)
			zf.UpdateZipInfo(zi, *pFile);
			
			// getting the file Name
			int index = 0;
			int indexChar = -1;
			
			do{
				index = szFullFilename.Find('\\', index);
				if( index != -1 )
				{
					indexChar = ++index;
				}
			}while( index != -1);
			
			if( indexChar == -1 )
			{
				throw new CException(1);
			}
			
			CString strFileName( szFullFilename.Mid(indexChar));
			
			zf.OpenNewFileInZip(strFileName, zi, Z_BEST_COMPRESSION);
			
			int size_read;
			do
			{
				size_read = pFile->Read(buf, BUF_SIZE);
				if (size_read)
					zf.WriteInFileInZip(buf, size_read);
				
			}
			while (size_read == BUF_SIZE);
			
			// cannot be called by the destructor because it may throw an exception
			// (it is not good to throw an exception when another one may be progress)
			pZippedStream = zf.Close();
		}
		catch (CException* e)
		{
			if (e->IsKindOf( RUNTIME_CLASS( CZipException )))
			{
				CZipException* p = (CZipException*) e;
				CString szMsg("");
				szMsg.Format("zip exception:cause=%d,zip=%d,filename=<%s>\n",p->m_iCause,p->m_bDuringZip,p->m_szFileName);
				error.Add(STCTZIP_ERRORMSG(szMsg));
				error.Add(STCTZIP_ERROR(ST_CTZIP_COMPRESS_FAILED));
			}
			else if (e->IsKindOf( RUNTIME_CLASS( CFileException )))
			{
				CFileException* p = (CFileException*) e;
				CString szMsg("");
				TCHAR   szCause[255];
				p->GetErrorMessage(szCause, 255);
				szMsg.Format("File exception on <%s> - cause : %s\n",p->m_strFileName,szCause);
				error.Add(STCTZIP_ERRORMSG(szMsg));
				error.Add(STCTZIP_ERROR(ST_CTZIP_COMPRESS_FAILED));
			}
			else // the only possibility is a memory exception I suppose
			{
				CString szMsg("");
				TCHAR   szCause[255];
				e->GetErrorMessage(szCause, 255);
				szMsg.Format("Exception - cause : %s\n",szCause);
				error.Add(STCTZIP_ERRORMSG(szMsg));
				error.Add(STCTZIP_ERROR(ST_CTZIP_COMPRESS_FAILED));
			}
			
			e->Delete();
		}
	}
	
	TRACE("[CTTOOLSZIP] - STZip will return code error %d\n",(int)error);

	if (pFile != NULL)
	{
		pFile->Close();
		delete pFile;
	}

	if (bCopy && !szDstFilename.IsEmpty()) 
		::DeleteFile(szDstFilename);

	return error;
}

// End of CSTError STZip(const CString & strFullFileName, IStream *& pZippedStream)

///////////////////////


// /------------------------------------------------------------
// | PapirusFunction
// | 
// | Funct  Name: STZip
// | 
// | Return Type: CSTError
// | 
// | Parameters:  const CString & strFullFileName, const CString & strZipFullFileName
// | 
// | Purpose: 
// | 
//  ------------------------------------------------------------
CSTError STZip(const CString & rstrFullFilename, const CString & strZipFullFilename)
{
	CSTError error;
	CFile * pFile = NULL;
	CString strFullFilename(rstrFullFilename);
	CString szDstFilename("");
	BOOL bCopy = FALSE;

	TRY
	{
		pFile = new CFile(strFullFilename, CFile::modeRead);
	}

	CATCH(CFileException, pEx)
	{
		if (pEx->m_cause == CFileException::sharingViolation && CopyTemp(strFullFilename, szDstFilename))
		{
			bCopy = TRUE;
			strFullFilename = szDstFilename;
			pFile = new CFile(strFullFilename, CFile::modeRead);
			if (!pFile)	error.Add(STCTZIP_ERROR(ST_CTZIP_COMPRESS_FAILED));
		}
		else
		{
			CString szMsg("");
			TCHAR   szCause[255];
			pEx->GetErrorMessage(szCause, 255);
			szMsg.Format("File exception on <%s> - cause : %s\n",pEx->m_strFileName,szCause);
			error.Add(STCTZIP_ERRORMSG(szMsg));
			error.Add(STCTZIP_ERROR(ST_CTZIP_COMPRESS_FAILED));
		}
	}
	END_CATCH

	if (!error)
	{
		int index = 0;
		int indexChar = -1;
		
		CZipFile zf(strZipFullFilename, 0);
		
		try
		{
			char buf[BUF_SIZE];
			
			zip_fileinfo zi;
			
			// getting information about the date and the attributes
			// (this is new in ZipFunc)
			zf.UpdateZipInfo(zi, *pFile);
			
			// getting the file Name
			int index = 0;
			int indexChar = -1;
			
			do{
				index = rstrFullFilename.Find('\\', index);
				if( index != -1 )
				{
					indexChar = ++index;
				}
			}while( index != -1);
			
			if( indexChar == -1 )
			{
				throw new CException(1);
			}
			
			CString strFileName(rstrFullFilename.Mid(indexChar));
			
			zf.OpenNewFileInZip(strFileName, zi, Z_BEST_COMPRESSION);
			
			int size_read;
			do
			{
				size_read = pFile->Read(buf, BUF_SIZE);
				if (size_read)
					zf.WriteInFileInZip(buf, size_read);
				
			}
			while (size_read == BUF_SIZE);
			
			// cannot be called by the destructor because it may throw an exception
			// (it is not good to throw an exception when another one may be progress)
			zf.Close();
		}
		catch (CException* e)
		{
			if (e->IsKindOf( RUNTIME_CLASS( CZipException )))
			{
				CZipException* p = (CZipException*) e;
				CString szMsg("");
				szMsg.Format("zip exception:cause=%d,zip=%d,filename=<%s>\n",p->m_iCause,p->m_bDuringZip,p->m_szFileName);
				error.Add(STCTZIP_ERRORMSG(szMsg));
				error.Add(STCTZIP_ERROR(ST_CTZIP_COMPRESS_FAILED));
			}
			else if (e->IsKindOf( RUNTIME_CLASS( CFileException )))
			{
				CFileException* p = (CFileException*) e;
				CString szMsg("");
				TCHAR   szCause[255];
				p->GetErrorMessage(szCause, 255);
				szMsg.Format("File exception on <%s> - cause : %s\n",p->m_strFileName,szCause);
				error.Add(STCTZIP_ERRORMSG(szMsg));
				error.Add(STCTZIP_ERROR(ST_CTZIP_COMPRESS_FAILED));
			}
			else // the only possibility is a memory exception I suppose
			{
				CString szMsg("");
				TCHAR   szCause[255];
				e->GetErrorMessage(szCause, 255);
				szMsg.Format("Exception - cause : %s\n",szCause);
				error.Add(STCTZIP_ERRORMSG(szMsg));
				error.Add(STCTZIP_ERROR(ST_CTZIP_COMPRESS_FAILED));
			}
			
			e->Delete();
		}
	}

	if (pFile != NULL)
	{
		pFile->Close();
		delete pFile;
	}

	if (bCopy && !szDstFilename.IsEmpty()) 
		::DeleteFile(szDstFilename);

	return error;
}
// End of CSTError STZip( const CString & strFullFileName, const CString & strZipFullFileName)





// /------------------------------------------------------------
// | PapirusFunction
// | 
// | Funct  Name: STUnZip
// | 
// | Return Type: CSTError
// | 
// | Parameters:  const CString & strZippedFullFileName
// | 
// | Purpose: 
// | 
//  ------------------------------------------------------------
CSTError STUnZip(const CString & strZippedFullFileName)
{
	CSTError error;

	try
	{
		// getting the file to unzip 
		CUnzipFile uf(strZippedFullFileName);

		uf.GoToFirstFile();

		// in case of multiple zipped files
//			do
//			{
			unz_file_info ui;

			// 	getting the information about the file 
			// (date, attributes, the size of the filename)
			uf.GetCurrentFileInfo(&ui);

			int iNameSize = ui.size_filename + 1;
			char* pName = new char [iNameSize];

			// get the name of the file
			uf.GetCurrentFileInfo(NULL, pName, iNameSize);

			TCHAR szDir[_MAX_DIR];
			TCHAR szDrive[_MAX_DRIVE];
			_tsplitpath(strZippedFullFileName, szDrive, szDir, NULL, NULL);
			CString szPath = CString(szDrive) + szDir;
			CFile f( szPath + pName, CFile::modeWrite | CFile::modeCreate);

			delete[] pName;
			uf.OpenCurrentFile();
			char buf[BUF_SIZE];

			int size_read;
			do
			{
				size_read = uf.ReadCurrentFile(buf, BUF_SIZE);
				if (size_read > 0)
					f.Write(buf, size_read);
				
			}
			while (size_read == BUF_SIZE);

			// set the original date stamp and attributes to the unpacked file
			// (this function closes the file "f" which is needed to apply
			// the attributes)
			uf.UpdateFileStatus(f, ui);

//			}while( uf.GoToNextFile() );

		// cannot be called by the destructor 
		uf.Close();
	}
	catch (CException* e)
	{
		if (e->IsKindOf( RUNTIME_CLASS( CZipException )))
		{
			CZipException* p = (CZipException*) e;
			CString szMsg("");
			szMsg.Format("zip exception:cause=%d,zip=%d,filename=<%s>\n",p->m_iCause,p->m_bDuringZip,p->m_szFileName);
			error.Add(STCTZIP_ERRORMSG(szMsg));
			error.Add(STCTZIP_ERROR(ST_CTZIP_UNCOMPRESS_FAILED));
		}
		else if (e->IsKindOf( RUNTIME_CLASS( CFileException )))
		{
			CFileException* p = (CFileException*) e;
			CString szMsg("");
			TCHAR   szCause[255];
			p->GetErrorMessage(szCause, 255);
			szMsg.Format("File exception on <%s> - cause : %s\n",p->m_strFileName,szCause);
			error.Add(STCTZIP_ERRORMSG(szMsg));
			error.Add(STCTZIP_ERROR(ST_CTZIP_UNCOMPRESS_FAILED));
		}
		else // the only possibility is a memory exception I suppose
		{
			CString szMsg("");
			TCHAR   szCause[255];
			e->GetErrorMessage(szCause, 255);
			szMsg.Format("Exception - cause : %s\n",szCause);
			error.Add(STCTZIP_ERRORMSG(szMsg));
			error.Add(STCTZIP_ERROR(ST_CTZIP_UNCOMPRESS_FAILED));
		}
				
		e->Delete();
	}
	return error;
}
// End of CSTError STUnZip(const CString & strZippedFullFileName )



// /------------------------------------------------------------
// | PapirusFunction
// | 
// | Funct  Name: STUnZip
// | 
// | Return Type: CSTError
// | 
// | Parameters:  const CString & strZippedFullFileName, CString & strUnZippedFullFileName
// | 
// | Purpose: 
// | 
//  ------------------------------------------------------------
CSTError STUnZip(const CString & strZippedFullFileName, CString & strUnZippedFullFileName)
{
	CSTError error;

	try
	{
		// getting the file to unzip 
		CUnzipFile uf(strZippedFullFileName);

		uf.GoToFirstFile();

		// in case of multiple zipped files
//			do
//			{
			unz_file_info ui;

			// 	getting the information about the file 
			// (date, attributes, the size of the filename)
			uf.GetCurrentFileInfo(&ui);

			int iNameSize = ui.size_filename + 1;
			char* pName = new char [iNameSize];


			// get the name of the file
			uf.GetCurrentFileInfo(NULL, pName, iNameSize);

			TCHAR szDir[_MAX_DIR];
			TCHAR szDrive[_MAX_DRIVE];
			_tsplitpath(strZippedFullFileName, szDrive, szDir, NULL, NULL);

			CString szPath = CString(szDrive) + szDir;

			// return the unzipped full path file name
			strUnZippedFullFileName  = szDrive;
			strUnZippedFullFileName += szDir;
			strUnZippedFullFileName += pName;
			
			CFile f( szPath + pName, CFile::modeWrite | CFile::modeCreate);

			delete[] pName;
			uf.OpenCurrentFile();
			char buf[BUF_SIZE];

			int size_read;
			do
			{
				size_read = uf.ReadCurrentFile(buf, BUF_SIZE);
				if (size_read > 0)
					f.Write(buf, size_read);
				
			}
			while (size_read == BUF_SIZE);

			// set the original date stamp and attributes to the unpacked file
			// (this function closes the file "f" which is needed to apply
			// the attributes)
			uf.UpdateFileStatus(f, ui);

//			}while( uf.GoToNextFile() );

		// cannot be called by the destructor 
		uf.Close();
	}
	catch (CException* e)
	{
		if (e->IsKindOf( RUNTIME_CLASS( CZipException )))
		{
			CZipException* p = (CZipException*) e;
			CString szMsg("");
			szMsg.Format("zip exception:cause=%d,zip=%d,filename=<%s>\n",p->m_iCause,p->m_bDuringZip,p->m_szFileName);
			error.Add(STCTZIP_ERRORMSG(szMsg));
			error.Add(STCTZIP_ERROR(ST_CTZIP_UNCOMPRESS_FAILED));
		}
		else if (e->IsKindOf( RUNTIME_CLASS( CFileException )))
		{
			CFileException* p = (CFileException*) e;
			CString szMsg("");
			TCHAR   szCause[255];
			p->GetErrorMessage(szCause, 255);
			szMsg.Format("File exception on <%s> - cause : %s\n",p->m_strFileName,szCause);
			error.Add(STCTZIP_ERRORMSG(szMsg));
			error.Add(STCTZIP_ERROR(ST_CTZIP_UNCOMPRESS_FAILED));
		}
		else // the only possibility is a memory exception I suppose
		{
			CString szMsg("");
			TCHAR   szCause[255];
			e->GetErrorMessage(szCause, 255);
			szMsg.Format("Exception - cause : %s\n",szCause);
			error.Add(STCTZIP_ERRORMSG(szMsg));
			error.Add(STCTZIP_ERROR(ST_CTZIP_UNCOMPRESS_FAILED));
		}
				
		e->Delete();
	}
	return error;
}
// End of CSTError STUnZip(const CString & strZippedFullFileName, CString & strUnZippedFullFileName)

////////////////////////////////
// C.G. 04/03/2002: helper to return stream:
//  - no file created
//  - work only on streams
//  remarks: some infos like file attributes & filename will not be written in unzipped stream
CSTError STUnZip(IStream *pZippedStream, IStream *& pUnZippedStream)
{
	CSTError error;

	try
	{
		// getting the file to unzip 
		CSTStreamUnzipFile uf(pZippedStream);
		
		unz_file_info ui;
		
		// 	getting the information about the file 
		// (date, attributes, the size of the filename)
		uf.GetCurrentFileInfo(&ui);
		COleStreamFile f;
		f.CreateMemoryStream();
		uf.OpenCurrentFile();
		
		char buf[BUF_SIZE];
		
		int size_read;
		do
		{
			size_read = uf.ReadCurrentFile(buf, BUF_SIZE);
			if (size_read > 0)
				f.Write(buf, size_read);
			
		}
		while (size_read == BUF_SIZE);
		
		pUnZippedStream = f.Detach();
		uf.Close();
	}
	catch (CException* e)
	{
		if (e->IsKindOf( RUNTIME_CLASS( CZipException )))
		{
			CZipException* p = (CZipException*) e;
			CString szMsg("");
			szMsg.Format("zip exception:cause=%d,zip=%d,filename=<%s>\n",p->m_iCause,p->m_bDuringZip,p->m_szFileName);
			error.Add(STCTZIP_ERRORMSG(szMsg));
			error.Add(STCTZIP_ERROR(ST_CTZIP_UNCOMPRESS_FAILED));
		}
		else if (e->IsKindOf( RUNTIME_CLASS( CFileException )))
		{
			CFileException* p = (CFileException*) e;
			CString szMsg("");
			TCHAR   szCause[255];
			p->GetErrorMessage(szCause, 255);
			szMsg.Format("File exception on <%s> - cause : %s\n",p->m_strFileName,szCause);
			error.Add(STCTZIP_ERRORMSG(szMsg));
			error.Add(STCTZIP_ERROR(ST_CTZIP_UNCOMPRESS_FAILED));
		}
		else // the only possibility is a memory exception I suppose
		{
			CString szMsg("");
			TCHAR   szCause[255];
			e->GetErrorMessage(szCause, 255);
			szMsg.Format("Exception - cause : %s\n",szCause);
			error.Add(STCTZIP_ERRORMSG(szMsg));
			error.Add(STCTZIP_ERROR(ST_CTZIP_UNCOMPRESS_FAILED));
		}
				
		e->Delete();
	}
	return error;
}

/////////////////////////////

// C.G. 04/03/2002: helper using stream as input parameter:
//  - no temporary zip file created
//  - file into zip will be created
// if szDestDir is "", temp default dir is used; else szDestDir must end with "\\"
CSTError STUnZip(IStream *pZippedStream, const CString & szDestDir, CString & szCreatedFilename)
{
	CSTError error;

	try
	{
		// getting the file to unzip 
		CSTStreamUnzipFile uf(pZippedStream);
		
		unz_file_info ui;
		
		// 	getting the information about the file 
		// (date, attributes, the size of the filename)
		uf.GetCurrentFileInfo(&ui);
		int iNameSize = ui.size_filename + 1;
		char* pName = new char [iNameSize];

		// get the name of the file
		uf.GetCurrentFileInfo(NULL, pName, iNameSize);

		CString szPath(szDestDir);
		if (szPath.IsEmpty())
		{
			CString strTempDirectory;
			DWORD dwRes = ::GetTempPath (MAX_PATH, strTempDirectory.GetBuffer (MAX_PATH));
			strTempDirectory.ReleaseBuffer();
			szPath=strTempDirectory;
		}

		szCreatedFilename = szPath + pName;
		CFile f( szCreatedFilename, CFile::modeWrite | CFile::modeCreate);
		delete[] pName;
		uf.OpenCurrentFile();
		
		char buf[BUF_SIZE];
		
		int size_read;
		do
		{
			size_read = uf.ReadCurrentFile(buf, BUF_SIZE);
			if (size_read > 0)
				f.Write(buf, size_read);
			
		}
		while (size_read == BUF_SIZE);

		uf.UpdateFileStatus(f, ui);
		uf.Close();
	}
	catch (CException* e)
	{
		if (e->IsKindOf( RUNTIME_CLASS( CZipException )))
		{
			CZipException* p = (CZipException*) e;
			CString szMsg("");
			szMsg.Format("zip exception:cause=%d,zip=%d,filename=<%s>\n",p->m_iCause,p->m_bDuringZip,p->m_szFileName);
			error.Add(STCTZIP_ERRORMSG(szMsg));
			error.Add(STCTZIP_ERROR(ST_CTZIP_UNCOMPRESS_FAILED));
		}
		else if (e->IsKindOf( RUNTIME_CLASS( CFileException )))
		{
			CFileException* p = (CFileException*) e;
			CString szMsg("");
			TCHAR   szCause[255];
			p->GetErrorMessage(szCause, 255);
			szMsg.Format("File exception on <%s> - cause : %s\n",p->m_strFileName,szCause);
			error.Add(STCTZIP_ERRORMSG(szMsg));
			error.Add(STCTZIP_ERROR(ST_CTZIP_UNCOMPRESS_FAILED));
		}
		else // the only possibility is a memory exception I suppose
		{
			CString szMsg("");
			TCHAR   szCause[255];
			e->GetErrorMessage(szCause, 255);
			szMsg.Format("Exception - cause : %s\n",szCause);
			error.Add(STCTZIP_ERRORMSG(szMsg));
			error.Add(STCTZIP_ERROR(ST_CTZIP_UNCOMPRESS_FAILED));
		}
				
		e->Delete();
	}
	return error;
}

/////////////////////////////

BOOL STIsZip(IStream *pStream)
{
	CSTStreamUnzipFile uf;
	bool bres=uf.Check(pStream);
//			CString szMsg("");
//			TCHAR   szCause[255];
//			e->GetErrorMessage(szCause, 255);
//			
//			szMsg.Format("Exception - cause : %s\n",szCause);
//	
//			TRACE("[CTTOOLSZIP] - STIsZip failed : %s\n",szMsg);
//	
			//e->Delete();
//			uf.Close();



	TRACE("[CTTOOLSZIP] - STIsZip will return %x\n",bres);
	return bres;
}

/////////////////////////////
