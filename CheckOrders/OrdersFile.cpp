// OrdersFile.cpp: implementation of the COrdersFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CheckOrders.h"
#include "EchkMgr.h"
#include "OrdersFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COrdersFile::COrdersFile() {}

COrdersFile::COrdersFile(const char *pFilename):CCREresseaFile(pFilename)
{
}

COrdersFile::~COrdersFile()
{

}


BOOL COrdersFile::nextOrder()
{
	const char *buf;
	BOOL ok=FALSE;

	while (!eof() && !ok) {
		buf=getbuf();

		if (buf) {
			if (buf[0]==COMMENT_CHAR && buf[1]==COMMENT_CHAR) {
 				//if (ignore_NameMe) gLine_no--;
				continue;
			}
			setBuffer(buf);
			ok=TRUE;
		} 
		else
			setEOF();
	}
	return !eof();
}

const char * COrdersFile::getbuf() 
{
	char lbuf[MAXLINE];
	BOOL cont = FALSE;
	BOOL quote = FALSE;
	BOOL report = FALSE;
	CString sBuffer;
	char * cp = sBuffer.GetBuffer(BUFSIZE);
	char * lwarnbuf = cp;

	lbuf[MAXLINE-1] = '@';

	do {
		BOOL eatwhite = TRUE;
		BOOL start = TRUE;
		char *end;
		char *bp = fgets(lbuf, MAXLINE, m_file);

		if (!bp) return NULL;

		end=bp+strlen(bp);
		if (*(end-1)=='\n')
			*(--end)=0;
		else {
			// if the line were longer than permitted, the remainder is thrown away:
			while (bp && !lbuf[MAXLINE-1] && lbuf[MAXLINE-2]!='\n')
				bp=fgets(lwarnbuf, 1024, m_file);
			Error(LINETOOLONG);
			//sprintf(warn_buf, "%.30s", lbuf);
			//Error(errtxt[LINETOOLONG], gLine_no, warn_buf);
			bp=lbuf;
		}
		cont=FALSE;
		while (cp!=lwarnbuf+MAXLINE && bp!=lbuf+MAXLINE && *bp) {
			if (isspace(*bp)) {
				if (eatwhite) {
					do { ++bp; } while (bp!=lbuf+MAXLINE && isspace(*bp));
					if (!quote && !start) *(cp++)=' ';
				} else {
					do {
						*(cp++)=SPACE_REPLACEMENT;
						++bp;
					} while (cp!=lwarnbuf+MAXLINE && bp!=lbuf+MAXLINE && isspace(*bp));
				}
			} else {
				cont=FALSE;
				if (*bp=='"') {
					quote=(BOOL)!quote;
					eatwhite=TRUE;
				} else {
					if (*bp=='\\') cont=TRUE;
					else
					if (!iscntrl(*bp)) {
						*(cp++) = *bp;
						eatwhite = (BOOL)!quote;
					}
				}
				++bp;
			}
			start=FALSE;
		}
		if (cp==lwarnbuf+MAXLINE) {
			--cp;
			if (!report) {
				report=TRUE;
				Error(LINETOOLONG);
				//sprintf(lbuf, "%.30s", warn_buf);
				//Error(errtxt[LINETOOLONG], gLine_no, lbuf);
			}
		}
		*cp=0;
	} while (cont || cp==lwarnbuf);

	if (quote)
		Error(MISSINGQUOTES);

	return setBuffer(lwarnbuf);
}

BOOL COrdersFile::checkOPTIONS() {

	if (!nextOrder())
		return FALSE;

	const char * pbuf = buffer();

	/*
	if (strncmp((LPCSTR), "From ", 5)==0) {	// it is a Mail
		char orderbuf[255];
		do {	// Up to the blank line search - Mailheader to end
			fgets(orderbuf, BUFSIZE, m_file);
		} while (orderbuf[0]!='\n' && !feof(m_file));
		m_sCurrentOrderLine = orderbuf;
		if (feof(m_file)) {
			setEOF();
			return FALSE;
		}
		if (!nextOrder())
			return FALSE;
	}
	*/

	if (pbuf[0]==COMMENT_CHAR)
	{
		do {
			if (strlen(pbuf)>9) {
				if (a_strncasecmp(pbuf, "; OPTION", 8)==0 ||
					 a_strncasecmp(pbuf, "; ECHECK", 8)==0) {
					parse_options((char *)(pbuf+2),0);
						// "; " jump over; points then to "OPTION"
				} 
				else 
					if (a_strncasecmp(pbuf, "; VERSION", 9)==0)
						output("%s\n",pbuf);
			}
			if (!nextOrder())
				return FALSE;
		} while (pbuf[0]==COMMENT_CHAR);
	}
	return TRUE;
}

BOOL COrdersFile::open()
{
	CString sFilename = filename();
	if (sFilename.IsEmpty())
		m_file = stdin;
	else
	{

		int errnum=fopen_s(&m_file,sFilename,"r");
		if (errnum!=0) {
			output(CANTREADFILE,(const char *)sFilename);
			return FALSE;
		} 

//		if (!compile)
//			outputln(PROCESSINGFILE,(const char *)sFilename);
//		else
//			output("%s|version|"VERSION"|"__DATE__"\n",(const char *)sFilename);
	}
	return TRUE;
}

BOOL COrdersFile::process()
{
	int next=0;
	int nFactionID=0;
	CString sRegionName("");
	char lorder_buf[BUFSIZE];	// current order line

	outputln(PROCESSINGFILE,filename());

	//TODO
	//gLine_no=0;

	//m_bOrderInErrors = true;

	if (!checkHeader())
		return FALSE;

	//Rx=Ry=-10000;

	// Find the first player, and afterwards process up to the last player
	while (!eof()) {
		strcpy_s(lorder_buf,buffer());
		switch (igetparam(lorder_buf)) {
			case P_LOCALE:
				if (!check_locale()) return FALSE;
				nextOrder();
				break;
				
			case P_REGION: 
			check_region(); 
			nextOrder();
			break;

			case P_FACTION: 
				if (nFactionID && !next)
					Warning(MISSINGNEXT,0);
				check_faction(nFactionID);
				next=0;
				break;

			case P_UNIT:
				if (nFactionID) {
					//scat(buffer());
					if (!readaunit()) 
						nextOrder();
					incNbUnits();
				} 
				else
					nextOrder();
				break;

				// If in READ unit one breaks off, stand there either a new player, a new unit or the file end. 
				// The SWITCH is again gone through, and called the appropriate function. 
				// One may not overwrite order_buf in any case! 
				// With all other entries here must again be filled order_buf, since 
				// the information concerned is located in only one line, and now the next must be read

			case P_NEXT:
				// next faction
				nFactionID=0;
				setNextFaction();
				next=1;

			default: 
				check_otherParam(buffer(),nFactionID);
				nextOrder();
				break;
		}
	} // end while !sCurrentFile->eof()

	if (igetparam(buffer())==P_NEXT)	// this line was read if necessary and then came
		next=1;							// EOF -> no more check, next=0...
	if (nFactionID && !next)
		return Error(MISSINGNEXT);
	return TRUE;
}


