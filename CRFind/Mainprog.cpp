// CRFind.cpp : Defines the entry point for the console application.
//

#include <stdafx.h>
#include <stdarg.h>

#include "wTofile.h"
#include "CR.h"
#include "CRReport.h"
#include "CRRegions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern void output(int nErrorNum, ...);
extern void outputln(int nErrorNum, ...);
extern void output(const char * pzFormat, ...);
extern void outputln(const char * pzFormat, ...);
extern void output_header();

BOOL errormsg(const char * pzFormat, ...);

////////////////

FILE *fERR=stderr;
FILE *fOUT=stdout;

CString sHierarchyFilename("");
CString sCRFilename("");
//CString sCRReferenceFilename("");
//CString sRefShortName("");
CString sHeaderLine("");

CCR cr_ref;

BOOL s_bAppendOutput = FALSE;
BOOL s_bOutputHeader = FALSE;

CString validFilename(const char *ps)
{
	if (!ps) return "";
	CString sf(ps);
	sf.Remove('"');
	return sf;
}

int check_options(int argc, char *argv[], char dostop, char command_line) {
	int i;
	char *x;
	for (i=1; i<argc; i++) {
		if (argv[i][0]=='-') {
			switch (argv[i][1]) {

				case 'a':s_bAppendOutput = TRUE; break;	
				case 'O':
					if (dostop)	{ // bei Optionen via "; ECHECK" nicht mehr machen
						if (argv[i][2]==0) {	// "-O file"
							i++;
							x=argv[i];
						} else							// "-Ofile"
							x=argv[i]+2;
						if (!x) {
							fputs("Using stderr for error output\n",stderr);
							fERR=stderr;
							break;
						}
						fERR=fopen(x,s_bAppendOutput?"a":"w");
						if (!fERR) {
							fprintf(stderr,"Can't write to file `%s':\n"
									"	%s",x,strerror(errno));
							exit(0);
						}
					}
					break;

				case 'o':
					if (dostop)	{ // bei Optionen via "; ECHECK" nicht mehr machen
						if (argv[i][2]==0) {	// "-o file"
							i++;
							x=argv[i];
						} else							// "-ofile"
							x=argv[i]+2;
						if (!x) {
							fputs("Empty file for checked file, using stdout\n",stderr);
							fOUT=stdout;
							break;
						}
						fOUT=fopen(x,s_bAppendOutput?"a":"w");
						if (!fOUT) {
							fprintf(stderr,"Can't write to file `%s':\n"
									"	%s",x,strerror(errno));
							exit(0);
						}
					}
					break;

				case 'H':
					if (argv[i][2]==0) { 	// -H hierarchy
						i++;
						sHierarchyFilename=validFilename(argv[i]);
					} else {
						if (argv[i][2])
							sHierarchyFilename=validFilename(argv[i]+2);
					}
					if (sHierarchyFilename.IsEmpty())
						fputs("Missing hierarchy file\n", stderr);
					break;

				case 'f':
					if (argv[i][2]==0) { 	// -f CRFilename // input CR
						i++;
						sCRFilename=validFilename(argv[i]);
					} else {
						if (argv[i][2])
							sCRFilename=validFilename(argv[i]+2);
					}
					if (sCRFilename.IsEmpty())
						fputs("Missing input CR file\n", stderr);
					break;

				default:
					if (argv[i][1]) {		// sonst ist das nur '-' => stdin lesen
						output("Unknown option `%s'\n", argv[i]);
						if (dostop)		// Nicht stoppen, wenn dies die Parameter aus der Datei selbst sind!
							exit(10);
				}
			}
		}
		else
			break;
	}
	return i;
}

void parse_options(char *p, char dostop) {
	char *argv[30], **ap=argv, *vl, argc=0;

	vl=strtok(p, " \t,");
	do {
		*ap++=vl;
		argc++;
	} while ((vl=strtok(NULL, " \t,"))!=NULL);
	*ap=0;
	check_options(argc, argv, dostop, 0);
}

void outputCommandLine(int argc, char *argv[])
{
	CString sFullCommandLine("command line: ");
	for (int i=1; i<argc; i++)
	{
		sFullCommandLine+=argv[i];
		sFullCommandLine+=" ";
	}

	outputln("%s\n",(LPCSTR)sFullCommandLine);
}

//////////

void output_header()
{
	if (!s_bOutputHeader)
			outputln("[%s]",(LPCSTR)sHeaderLine);
	s_bOutputHeader = TRUE;
}

CString findIDIntoString(const CString & s)
{
	int n1=s.ReverseFind('(');
	int n2=s.ReverseFind(')');
	if (n1<1) return "";
	if (n2==-1) return "";
	if (n2 < n1) return "";
	if (n2 > n1+5) return "";
	if (n2!=s.GetLength()-1) return "";
	if (s.GetAt(n1-1)!=' ') return "";
	return s.Mid(n1+1,n2-n1-1);
}


void output_invisible_movers(CCR & cr, int & nbInvisibles)
{
	CCRRegion * pr = NULL;
	CCRUnit * pu = NULL;
	CCRShip *ps = NULL;

	nbInvisibles = 0;

	CCRReport & crreport = cr.report();
	CCRRegions  & rs = crreport.allRegions();
	CCRUnits  & us = crreport.allUnits();
	CCRShips  & ss = crreport.allShips();

	POSITION pos = rs.GetHeadPosition();
	while  (pos) 
	{
		BOOL bRegionHeader=FALSE;
		pr = rs.GetNext(pos);
		block * bJourneys = pr->throughJourneyUnits();
		block * bj = bJourneys;
		while (bj)
		{
			entry * e = NULL;
			// COMMENTS can be inside these tags list
			// type should be only: CR_THROUGH_SAILING, CR_THROUGH_JOURNEY				
			if (strcmpi(bj->type->name,CR_COMMENTS))
				e = bj->entries;
			while (e) {
				if (!e->tag && e->data.cp)
				{
					CString s = e->data.cp;
					CString sUnitID = findIDIntoString(s); //(xxx) searched
					int nID = btoi(sUnitID);
					if (nID!=0)
					{
						if (!us.Find(nID,pu) && !ss.Find(nID,ps))
						{
							CString sprefix(" ");
							if (!bRegionHeader)
								outputln("\n%s",pr->outputName());
							bRegionHeader=TRUE;
							nbInvisibles++;
							outputln("     %s",(LPCSTR)s);
						}
					}
				}
				e = e->next;
			}
			bj=bj->next;
		}
	}

	outputln("\n%d invisible travel through\n",nbInvisibles);
}

void output_monsters(CCRReport & crreport, int & nbInvisibles)
{
	CCRRegion * pr = NULL;
	CCRUnit * pu = NULL;
	CCRShip *ps = NULL;

	nbInvisibles = 0;

	CCRUnits  & us = crreport.allUnits();

//	POSITION pos = us.GetHeadPosition();
//	while  (pos) 
//	{
//		BOOL bRegionHeader=FALSE;
////		pr = rs.GetNext(pos);
//	}

//	outputln("\n%d invisible travel through\n",nbInvisibles);
}

int MainProg(int argc, char *argv[]) {
	int nextarg;
	CCRRegion * pr = NULL;
	CCRUnit * pu = NULL;
	CCRShip *ps = NULL;

	if (argc>1)
		nextarg=check_options(argc, argv, 1, 1);

	//output(ECHECK, CHECKORDERS_VERSION,__DATE__);
	// outputCommandLine(argc,argv);

	CString sRefShortName = STGetShortPathname(sCRFilename);
	sHeaderLine.Format("%s",(LPCSTR)sRefShortName);

	BOOL bLoaded = cr_ref.load(sCRFilename,sHierarchyFilename,&crdata_ireport_std);

	int nOptions = OPT_REGIONS|OPT_FACTIONS|OPT_UNITS|OPT_SHIPS;

	cr_ref.fillReport(nOptions);

	//CCRReport crreport(cr_ref);
	//crreport.fill(nOptions);

	int nbVisibles = 0;
	int nbInvisibles = 0;
	output_header();
	output_invisible_movers(cr_ref,nbInvisibles);

	return 0;
}

/////////////////////////////////////////
// output management

void outputvalist(const char * pszFormat, va_list argList, BOOL bAddCarriageReturn = FALSE)
{
	char buffer[1024]; 
	vsprintf (buffer, pszFormat, argList);
	if (bAddCarriageReturn)	strcat(buffer,"\n");
	fputs(buffer,stdout);
	if (fOUT!=stdout) fputs(buffer,fOUT);
}

void output(const char * pzFormat, ...)
{
	va_list argList;
	va_start(argList, pzFormat);
	outputvalist(pzFormat,argList);
	va_end(argList);
}

void outputln(const char * pzFormat, ...)
{
	va_list argList;
	va_start(argList, pzFormat);
	outputvalist(pzFormat,argList,TRUE);
	va_end(argList);
}

BOOL errormsg(const char * pzFormat, ...)
{
	va_list argList;
	va_start(argList, pzFormat);
	outputvalist(pzFormat,argList,TRUE);
	va_end(argList);
	return FALSE;
}

////////////////////