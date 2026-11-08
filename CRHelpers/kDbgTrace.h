// | File Name    : kDbgTrace.h
// | Created By   : cguillon
// | Creation Date: jeudi, février 14, 2002
// | 
// | Purpose      : force traces in release mode
// | 
//  ------------------------------------------------------------
#ifndef _KDBGTRACE_H_
#define _KDBGTRACE_H_

#include <stdarg.h>

#define STTRACE_RELEASEMODE ::STDebugOutput
#define STTRACE_NONE		1 ? (void)0 : ::STDebugOutput

#ifdef _AFX

inline void STDebugOutputV(LPCTSTR lpszFormat, va_list argList, BOOL bAddCarriageReturn = FALSE)
{
	CString str;
	str.FormatV(lpszFormat, argList);
	if (bAddCarriageReturn)	str+="\n";
	::OutputDebugString(str);
}

#else

inline void STDebugOutputV(const char * pszFormat, va_list argList, BOOL bAddCarriageReturn = FALSE)
{
	char 	buffer[1024]; 
	vsprintf (buffer, pszFormat, argList);
	if (bAddCarriageReturn)	strcat(buffer,"\n");
	::OutputDebugString(buffer);
}

#endif

inline void STDebugOutput(LPCTSTR lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	STDebugOutputV(lpszFormat,argList);
	va_end(argList);
}

#endif //_KDBGTRACE_H_
//  ------------------------------------------------------------
