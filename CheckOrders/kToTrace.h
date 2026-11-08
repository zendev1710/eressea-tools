// | File Name    : kToTrace.h
// | Created By   : cguillon
// | Creation Date: jeudi, février 14, 2002
// | 
// | Purpose      : force traces in release mode
// | 
//  ------------------------------------------------------------
#ifndef _KTOTRACE_H_
#define _KTOTRACE_H_

#include "kDbgTrace.h"

extern void output(const char *pzFormat,...);
extern BOOL errormsg(const char * pzFormat, ...);

#ifdef _AFX

inline void STOutputV(LPCTSTR lpszFormat, va_list argList, BOOL bAddCarriageReturn = FALSE)
{
	CString str;
	str.FormatV(lpszFormat, argList);
	if (bAddCarriageReturn)	str+="\n";
	output(str);
}

#else

inline void STOutputV(const char * pszFormat, va_list argList, BOOL bAddCarriageReturn = FALSE)
{
	char 	buffer[1024]; 
	vsprintf (buffer, pszFormat, argList);
	if (bAddCarriageReturn)	strcat(buffer,"\n");
	output(buffer);
}

#endif

inline void STOutput(LPCTSTR lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	STOutputV(lpszFormat,argList,TRUE);
	va_end(argList);
}

#endif //_KTOTRACE_H_
//  ------------------------------------------------------------
