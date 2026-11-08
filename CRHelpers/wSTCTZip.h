// /------------------------------------------------------------
// | PapirusIncludeSource
// | 
// | File Name    : wSTCTZip.h
// | Created By   : HZarrabi
// | Creation Date: lundi, février 11, 2002
// | 
// | Purpose      : 
// | 
//  ------------------------------------------------------------
#ifndef _ZIPUNZIP_H_
#define _ZIPUNZIP_H_

#include "STInclude.h"
#include ST_INC_ERR(kToErr.h)


CSTError STZip( const CString & strFullFileName );
CSTError STZip( const CString & strFullFileName, const CString & strZipFullFileName );

// C.G. 04/03/2002: helper to return stream (instead of creating .zip file) for filename to zip
CSTError STZip(const CString &szFullFilename, IStream *& pZippedStream);

CSTError STUnZip( const CString & strFileName );
CSTError STUnZip( const CString & strZippedFullFileName, CString & strUnZippedFullFileName );

// C.G. 04/03/2002: helpers working on streams
//+++
CSTError STUnZip(IStream *pZippedStream, IStream *& pUnZippedStream);
CSTError STUnZip(IStream *pZippedStream, const CString & szDestDir, CString & szCreatedFilename);
//+++

BOOL STIsZip(IStream *pStream);

#endif //_ZIPUNZIP_H_
//  ------------------------------------------------------------
