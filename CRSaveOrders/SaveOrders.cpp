// SaveOrders.cpp : Defines the entry point for the console application.
//

#include <stdafx.h>
#include <stdarg.h>
#include <conio.h>
#include "wToDir.h"
#include "wToFile.h"
#include "CR.h"
//#include "MapiAdmin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

constexpr auto MAINVERSION = "1";
constexpr auto MINVERSION = "0";
constexpr auto PATCHLEVEL = "0";
constexpr auto BUILDNUM = "2";

//#define SAVEORDERS_VERSION MAINVERSION"."MINVERSION"."PATCHLEVEL"b"BUILDNUM

extern void output(const char * pzFormat, ...);
extern void console(const char * pzFormat, ...);
extern void consoleln(const char * pzFormat, ...);
extern void outputln(const char * pzFormat, ...);

extern void consoleError(const char * pzFormat, ...);

extern int printSaveOrdersHelp(BOOL bExtented);

////////////////

FILE *fERR=stderr;
FILE *fOUT=stdout;

CString sHierarchyFilename("eressea.crh");
CString sCRFilename("");
CString sSuffixFilename("");
CString sPrefixFilename("");

CString sMagellanFoldername("");

CCR cr;

BOOL s_bSeveralOrderFiles = FALSE;
BOOL s_bInSelectedRegionsOnly = FALSE;
BOOL s_bEnglishLocale = TRUE;
BOOL s_bPause = FALSE;
BOOL s_bSendOrders = FALSE;

CStringList sFactionIDS;

int s_nLineMaxLength = DEFAULT_LINE_MAXLENGTH;

CString validFilename(const char *ps)
{
	if (!ps) return "";
	CString sf(ps);
	sf.Remove('"');
	return sf;
}

void pause()
{
	fputs("\npress a key to continue",stdout);
	_getch();
}

void pause2()
{
	fputs("press a key to continue",stdout);
	_getch();
}

void onExitProg(int nCode)
{
	if (nCode!=0)
		pause();

	exit(nCode);
}

int my_atoi(const char *p, int nDefValue)
{
	if (!p) return nDefValue;
	int n=atoi(p);
	if (n==-1) n = nDefValue;
	return n;
}

int check_options(int argc, char *argv[], char dostop, char command_line) {
	int i;
	char *x;
	CString sCurrentID;
	for (i=1; i<argc; i++) {
		if (argv[i][0]=='-') {
			switch (argv[i][1]) {
	
				case 'k': 
					s_bPause = TRUE;
					break; 

				case 'm': 
					s_bSeveralOrderFiles = TRUE; 
					break; 
	
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
						int numerr=fopen_s(&fERR,x,"w");
						if (!fERR && numerr!=0) {
							char sErr[255];
							int numErr=strerror_s(sErr,255,errno);
							fprintf(stderr,"Can't write to file `%s':\n	%s",x,sErr);
							onExitProg(1);
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
						int numErrOut=fopen_s(&fOUT,x,"w");
						if (!fOUT && numErrOut!=0) {
							char sErrOut[255];
							int nErr=strerror_s(sErrOut,255,errno);
							fprintf(stderr,"Can't write to file `%s':\n	%s",x,sErrOut);
							onExitProg(1);
						}
					}
					break;

				case 's':
					if (dostop)	{ 
						if (argv[i][2]==0) {	// "-s suffix_filename"
							i++;
							x=argv[i];
						} else							// "-ssuffix_filename"
							x=argv[i]+2;
						if (!x)
							fputs("Empty suffix for filename\n",stderr);
						else
							sSuffixFilename.Format("_%s",x);
					}
					break;

				case 'n':
					if (dostop)	{ 
						if (argv[i][2]==0) {	// "-n prefix_filename"
							i++;
							x=argv[i];
						} else							// "-nprefix_filename"
							x=argv[i]+2;
						if (!x)
							fputs("Empty prefix for filename\n",stderr);
						else
							sPrefixFilename.Format("%s_",x);
					}
					break;

				case 'r': s_bInSelectedRegionsOnly = TRUE; break;

				case 'l':
					if (argv[i][2]==0) { 	// -l <number>
						i++;
						s_nLineMaxLength=my_atoi(argv[i],0);
					} else {
						if (argv[i][2])
							s_nLineMaxLength=my_atoi(argv[i]+2,0);
					}
					if (s_nLineMaxLength==0) 
					{
						fputs("Missing line max length (-l option)\n", stderr);
						s_nLineMaxLength = DEFAULT_LINE_MAXLENGTH;
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

				case 'p':
					if (argv[i][2]==0) { 	// -p FactionID // faction id
						i++;
						sCurrentID=argv[i];
					} else {
						if (argv[i][2])
							sCurrentID=argv[i]+2;
					}
					if (sCurrentID.IsEmpty())
						fputs("Missing faction ID\n", stderr);
					else
						sFactionIDS.AddTail(sCurrentID);
					break;

				case 'i': 
						if (argv[i][2]==0) {	
							i++;
							x=argv[i]; // "-i folder"
						} else							
							x=argv[i]+2; // "-ifolder"
						sMagellanFoldername=validFilename(x); 
						break;

				case 'h':
				case '?':
					printSaveOrdersHelp(TRUE);
					break;
				default: if (argv[i][1]) console("Unknown option `%s'\n", argv[i]);
			}
		}
		else
			break;
	}
	return i;
}

void parse_options(char *p, char dostop) {
	char *argv[30], **ap=argv, *vl, argc=0;
	char *nexttoken;
	vl=strtok_s(p, " \t,",&nexttoken);
	do {
		*ap++=vl;
		argc++;
	} while ((vl=strtok_s(NULL, " \t,",&nexttoken))!=NULL);
	*ap=0;
	check_options(argc, argv, dostop, 0);
}

// Replace the console header output in outputCommandLine (around line 271-274) with:
void outputCommandLine(int argc, char* argv[])
{
	CString header;
	header.Format("\n  **  SaveOrders V%s.%s.%sb%s, %s  **\n\n",
		MAINVERSION, MINVERSION, PATCHLEVEL, BUILDNUM, __DATE__);
	consoleln("%s", (LPCSTR)header);

	CString sFullCommandLine("command line: ");
	for (int i = 1; i < argc; i++)
	{
		sFullCommandLine += argv[i];
		sFullCommandLine += " ";
	}

	consoleln("%s\n", (LPCSTR)sFullCommandLine);
}

block * regionWithName(CCR & cr, block * first, const char * pName)
{
	CString sn;
	block * r = first;
	while (r)
	{
		if (cr.gets(r,CR_NAME,sn) && !_strcmpi(sn,pName))
			return r;
		r = r->next;
	}
	return NULL;
}

/*
BOOL sendOrders(const CString & sTxtFilename)
{
	CMapiAdmin mapi;
	if ( mapi.Logon() )
	{
		LPCTSTR ppRecipients[1] = {"asm.eressea@gmail.com"};
		mapi.CreateMessage(1, ppRecipients, "Hello Geezah!", "Here is a test message", TRUE);
		return (mapi.SendMessage());
	}
	return FALSE;
}
*/

int printSaveOrdersHelp(BOOL bExtended)
{
	CString header;
	header.Format("  **  SaveOrders V%s.%s.%sb%s, %s  **\n",
		MAINVERSION, MINVERSION, PATCHLEVEL, BUILDNUM, __DATE__);
	consoleln("%s", (LPCSTR)header);

	outputln("-f <.cr filename>\t input .cr file where to save orders from");
	outputln("-H <.crh filename>\t .crh filename (eresea.crh by default)");
	outputln("-i <magellan.ini path>\t magellan.ini location (current folder by default)");
	outputln("-p <faction id>\t\t orders saved for specified faction only");
	outputln("-m\t\t\t one file per faction");
	outputln("-r\t\t\t orders saved for selection regions only");
	outputln("-l <maxlinelen>\t\t order line maxlength (80 is default)");
	outputln("-n <prefix>\t\t .txt filenames with specified prefix");
	outputln("-s <suffix> \t\t .txt filenames with specified suffix");
	outputln("-k\t\t\t pause");
	outputln("-O <filename> \t\terrors in specified file");
	outputln("-o <filename> \t\tmessage output in specified file");
	if (bExtended)
	{
		outputln("\nSave the confirmed orders from a .cr file to .txt orders file(s).");
		outputln("Orders written only for factions with password specified in magellan.ini\n");
		outputln("  CRSaveOrders -f map.cr -i c:\\myMagellan : orders saved in <xxx>_orders.txt");
		outputln("    <xxx> is the turn number; eressea.crh must be in the current folder");
		outputln("    magellan.ini must be in the c:\\myMagellan folder");
		outputln("  CRSaveOrders -f map.cr -m ==> <fID>_orders.txt for each faction");
		outputln("  CRSaveOrders -f map.cr -n txt\\me_ ==> txt\\me_<xxx>_orders.txt");
	}
	else
		outputln("\nCRSaveOrders -h or -? for more help.");

	pause();
	onExitProg(0);
	return 0;
}

int saveOrders(int argc, char *argv[]) {

	int nextarg;
	int nbUnitsWithOrderAllFactions=0;
	int nbUnitsAllFactions=0;

	fERR=stdout;

	if (argc<=1)
		return printSaveOrdersHelp(FALSE);

	if (argc>1)
		nextarg=check_options(argc, argv, 1, 1);

	//output(ECHECK, CHECKORDERS_VERSION,__DATE__);
	outputCommandLine(argc,argv);

	if (!STExistFile(sCRFilename))
		consoleError("error: %s file not found\n",(LPCSTR)STGetFullPathname(sCRFilename));

	if (sHierarchyFilename.IsEmpty())
		consoleError("error: eressea.crh hierarchy filename not specified (-H option)\n");

	if (!STExistFile(sHierarchyFilename))
		consoleError("error: %s file not found\n",(LPCSTR)STGetFullPathname(sHierarchyFilename));

	if (!cr.load(sCRFilename,sHierarchyFilename,&crdata_ireport_std))
		consoleError("error: %s file could not be loaded\n",(LPCSTR)STGetFullPathname(sCRFilename));
		
	int nTurn = cr.turn();
	CString sTxtFilename;
	CString sSuffix("orders");
	int nbFactions = sFactionIDS.GetCount();

	if (nbFactions == 0) 
	{
		if (!sMagellanFoldername.IsEmpty())
		cr.setMagellanFoldername(sMagellanFoldername);

		// retrieve all factions to save - those having a password
		block *bfs = cr.factions();
		block *bf=bfs;
		while (bf)
		{
			if (_strcmpi(bf->type->name,CR_PLAYER))
				break;
			int nIndex = -1;
			if (cr.isUserFaction(bf->ids[0]))
				sFactionIDS.AddTail(itob(bf->ids[0]));
			bf = bf->next;
		}
		nbFactions=sFactionIDS.GetCount();
	}
	else
		s_bSeveralOrderFiles=TRUE;
	if (s_bInSelectedRegionsOnly) 
		output("\nfor selected regions only...\n\n");
	sTxtFilename.Format("%s%d_%s%s.txt",(LPCSTR)sPrefixFilename,nTurn,(LPCSTR)sSuffix,(LPCSTR)sSuffixFilename);
	POSITION pos = sFactionIDS.GetHeadPosition();
	int nbf1 = nbFactions;
	int nbf2 = sFactionIDS.GetCount();
	for (int i=1;i<=nbFactions;i++)
	{
		int nIndex=-1;
		CString sFactionID = sFactionIDS.GetNext(pos);
		int nFactionId =  btoi(sFactionID);
		CString sPswd = cr.factionPassword(nFactionId);
		TSaveOrderResults results;
		results.reset();
		if (s_bSeveralOrderFiles) 
			sTxtFilename.Format("%s%d_%s%s.txt",(LPCSTR)sPrefixFilename,nTurn,(LPCSTR)sFactionID,(LPCSTR)sSuffixFilename);

		if (cr.saveOrders(s_bInSelectedRegionsOnly,sTxtFilename,nFactionId,sPswd,s_nLineMaxLength,results,(i!=1 && !s_bSeveralOrderFiles)))
		{
			//if (s_bSendOrders)
			//	sendOrders(sTxtFilename);

			nbUnitsWithOrderAllFactions+=results.m_nNbUnitsWithOrders;
			nbUnitsAllFactions+=results.m_nNbUnitsTotal;
			console("%4s: orders saved for %3d units of %3d total units\n",sFactionID,results.m_nNbUnitsWithOrders,results.m_nNbUnitsTotal);
		}
	}
	console("\ntotal: %d factions [%d units of %d total units]\n",nbFactions,nbUnitsWithOrderAllFactions,nbUnitsAllFactions);
	if (s_bPause) 
		pause();
	return 0;
}

/////////////////////////////////////////
// output management

void outputvalist(const char * pszFormat, va_list argList, BOOL bAddCarriageReturn=FALSE, BOOL bConsoleOutput=FALSE)
{
	static CString sOutput("");
	sOutput.FormatV(pszFormat, argList);
	if (bAddCarriageReturn)	
		sOutput+="\n";

	fputs(sOutput,fERR);

	if (bConsoleOutput && fERR!=stdout) 
		fputs(sOutput,stdout);
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

// msg on stdout also
void consoleError(const char * pzFormat, ...)
{
	va_list argList;
	va_start(argList, pzFormat);
	outputvalist(pzFormat,argList,FALSE,TRUE);
	va_end(argList);
	onExitProg(1);
}

// msg on stdout also
void console(const char * pzFormat, ...)
{
	va_list argList;
	va_start(argList, pzFormat);
	outputvalist(pzFormat,argList,FALSE,TRUE);
	va_end(argList);
}

void consoleln(const char * pzFormat, ...)
{
	va_list argList;
	va_start(argList, pzFormat);
	outputvalist(pzFormat,argList,TRUE,TRUE);
	va_end(argList);
}

////////////////////