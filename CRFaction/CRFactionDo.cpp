// CRFactionDo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdarg.h>

#include "CR.h"
#include "FInfos.h"
#include "FInfosSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern void output(int nErrorNum, ...);
extern void outputln(int nErrorNum, ...);
extern void output(const char * pzFormat, ...);
extern void outputln(const char * pzFormat, ...);

BOOL errormsg(const char * pzFormat, ...);

////////////////

FILE *fERR/*=NULL*/;
FILE *fOUT/*=NULL*/;

CString sHierarchyFilename("");
CString sCRFilename("");
CString sCROutFilename("");

CCR cr_in;
int nFactionsID[20];
BOOL bFactions[20];

int count = 0;

BOOL s_bErrors = FALSE;
BOOL s_bAll = FALSE;
BOOL s_bAnonym = FALSE;
BOOL s_bRemoveFactionsWithNoUnit = FALSE;
BOOL s_bModif = FALSE;
BOOL s_bTestOnly = FALSE;

static CFInfosSet s_factionsWithUnits;

//////////////

BOOL IsInFactions(int nPlayerID, int & ind)
{
	for (int i=0;i<count;i++)
	{
		ind = i;
		if (nFactionsID[i]==nPlayerID) 
			return TRUE;
	}
	return FALSE;
}

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

				case 'x': 
					s_bAnonym=TRUE; 
					break;

				case 'd': 
					s_bRemoveFactionsWithNoUnit=TRUE; 
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
						int nCodeErr=fopen_s(&fERR,x,"w");
						if (!fERR && nCodeErr) 
						{
							char sErrmsg[255];	
							strerror_s(sErrmsg,255,errno);
							fprintf(stderr,"Can't write to file `%s':\n%s",x,sErrmsg);
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
						int nc=fopen_s(&fOUT,x,"w");
						if (!fOUT && nc) {
							char sm[255];
							strerror_s(sm,255,errno);
							fprintf(stderr,"Can't write to file `%s':\n	%s",x,sm);
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

				case 't': s_bTestOnly=TRUE; break;

				case 'S':
					if (argv[i][2]==0) { 	// -S CRFilename // output CR
						i++;
						sCROutFilename=validFilename(argv[i]);
					} else {
						if (argv[i][2])
							sCROutFilename=validFilename(argv[i]+2);
					}
					if (sCROutFilename.IsEmpty()) fputs("Missing output CR file\n", stderr);
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
	char *toknext;
	vl=strtok_s(p, " \t,",&toknext);
	do {
		*ap++=vl;
		argc++;
	} while ((vl=strtok_s(NULL, " \t,",&toknext))!=NULL);
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

BOOL removeEmptyFaction(block *f, BOOL bErrMessage) 
{
	if (!f) 
		return FALSE;

	CFInfos * pf=NULL;
	if (!cr_in.b_isFaction(f))
		return FALSE;
	int nfid = f->ids[0];
	CString sfid(itob(nfid));
	if (!s_factionsWithUnits.find(pf,nfid))
	{
		if (cr_in.removeFaction(f))
		{
			outputln("faction %s removed [empty]",(LPCSTR)sfid);
			s_bModif = TRUE;
			return TRUE;
		}
		outputln("faction %s failed to be removed",(LPCSTR)sfid);
		return FALSE;
	}

	if (bErrMessage)
		outputln("faction %s cannot be removed because not empty",(LPCSTR)sfid);

	return FALSE;
}

BOOL removeEmptyFaction(int nPlayerID, BOOL bErrMessage) 
{
	int nID=nPlayerID;
	block * f = cr_in.factions(&nID);
	return removeEmptyFaction(f,bErrMessage);
}

BOOL removeEmptyFactions(int &nb) 
{
	nb = 0;
	block * f = cr_in.factions();
	if (!f) 
		return FALSE;

	CFInfos * pf=NULL;
	while (f)
	{
		block * fnext=f->next;
		if (removeEmptyFaction(f,FALSE))
		{
			f=fnext;
			nb++;
			continue;
		}
		f = f->next;
	}
	return TRUE;
}

BOOL correctDemonFactions()
{
	BOOL bDone=FALSE;
	CFInfos * pf_xeno=NULL;
	CFInfos * pf_crawlingChaos=NULL;
	CFInfos * pf_sexyCatAngel=NULL;
	int nfid_xeno=1558649;
	int nfid_crawlingChaos=637726;
	int nfid_sexyCatAngel=375621;
	block * f_xeno=NULL;
	block * f_crawlingChaos=NULL;
	block * f_sexyCatAngel=NULL;
	if (s_factionsWithUnits.find(pf_xeno,nfid_xeno) && pf_xeno)
		f_xeno=pf_xeno->cr_faction();
	if (s_factionsWithUnits.find(pf_crawlingChaos,nfid_crawlingChaos) && pf_crawlingChaos)
		f_crawlingChaos=pf_crawlingChaos->cr_faction();
	if (s_factionsWithUnits.find(pf_sexyCatAngel,nfid_sexyCatAngel) && pf_sexyCatAngel)	
		f_sexyCatAngel=pf_sexyCatAngel->cr_faction();

	if (!f_crawlingChaos)
		f_crawlingChaos=cr_in.factions(&nfid_crawlingChaos);
	if (!f_xeno)
		f_xeno=cr_in.factions(&nfid_xeno);
	if (!f_sexyCatAngel)
		f_sexyCatAngel=cr_in.factions(&nfid_sexyCatAngel);


	if (cr_in.setFactionAsDemon(f_xeno)) 
	{
		outputln("xeno set as a demon");
		bDone=TRUE;
	}

	if (cr_in.setFactionAsDemon(f_crawlingChaos)) 
	{
		outputln("crawling chaos set as a demon");
		bDone=TRUE;
	}

	if (cr_in.setFactionAsDemon(f_sexyCatAngel)) 
	{
		outputln("sexy cat angel set as a demon");
		bDone=TRUE;
	}
	return bDone;
}

int checkFactions(int argc, char *argv[]) 
{
	int nextarg=0;

	fERR=stdout;

	if (argc>1)
		nextarg=check_options(argc, argv, 1, 1);

	for (int j=0; j < 20; j++)
	{
		bFactions[j] = FALSE;
		nFactionsID[j] = 0;
	}

	for (int i=nextarg; i<argc && count < 20; i++) {
		if (!_strcmpi(argv[i],"*"))
		{
			s_bAll=TRUE;
			break;
		}
		nFactionsID[count] = btoi(argv[i]);
		count++;
	}

	if (count == 0 && !s_bAll) 
		return errormsg("no faction specified");
	if (!s_bAnonym && !s_bRemoveFactionsWithNoUnit) 
		return errormsg("no command specified (-x for set anonym, -d for delete empty faction)");

	//output(ECHECK, CHECKORDERS_VERSION,__DATE__);
	outputCommandLine(argc,argv);

	BOOL bres = cr_in.load(sCRFilename,sHierarchyFilename,&crdata_ireport_std);

	block * rs = cr_in.regions();
	block * r = rs;

	//browse regions
	while (r)
	{
		CString sTerrain("");
		CString sName("");
		CString sRegionName;
		cr_in.gets(r,CR_TERRAIN,sTerrain);
		if (sTerrain.IsEmpty() || !sTerrain.CompareNoCase(DE_FIREPROOF_WALL)) 
		{
			r=r->next;
			continue;
		}

		cr_in.gets(r,CR_NAME,sRegionName);

		// remove name attribute for badly named oceans
		if (!sTerrain.CompareNoCase(CR_REGIONTYPE_OCEAN)) 
		{
			if (!sRegionName.IsEmpty())
			{
				if (cr_in.onSave_removeEntry(r,CR_NAME))
					s_bModif = TRUE;
			}

		}

		block * us = cr_in.units(r);
		block * u = us;
		//browse units in current region
		while (u)
		{
			if (!cr_in.b_isUnit(u))
				break;
			block *	n = u->next;
			int nPlayerID = 0;
			// zombies & other monsters have something like: 1;Parteitarnung, no PARTEI
			cr_in.geti(u,CR_PLAYER,nPlayerID);
			int ind=0;
			if (nPlayerID!=0 && (s_bAll || IsInFactions(nPlayerID,ind)))
			{
				if (s_bRemoveFactionsWithNoUnit)
					s_factionsWithUnits.Add(nPlayerID);

				if (!s_bAll)
					bFactions[ind]=TRUE;

				if (s_bAnonym)
				{
					CString sUnitName("");
					cr_in.gets(u,CR_NAME,sName);
					BOOL bDone = cr_in.setUnitAsAnonym(u);
					if (bDone)
						s_bModif = TRUE;
					if (!s_bAll) 
						outputln("%15s - %20s (%s): %d",(LPCSTR)sRegionName,(LPCSTR)sName,itob(nPlayerID),bDone);
				}

			}
			u = n;
		}
		r=r->next;
	}

	if (s_bAnonym)
	{
		if (s_bAll)
		{
			int nb=0;
			cr_in.setAllFactionsAnonym(nb);
			if (nb>0)
				s_bModif = TRUE;
			outputln("all factions (%d) set as anonym",nb);
		}
		else
		{	
			for (int k=0;k<count;k++)
				if (bFactions[k])
				{
					if (cr_in.setFactionAnonym(nFactionsID[k]))
						s_bModif = TRUE;
				}
		}
	}

	if (s_bRemoveFactionsWithNoUnit)
	{
		if (correctDemonFactions())
			s_bModif = TRUE;

		if (s_bAll)
		{
			int nb=0;
			removeEmptyFactions(nb);
			outputln("all empty factions removed (%d removals)",nb);
		}
		else
		{	
			for (int k=0;k<count;k++)
		//		if (!bFactions[k])
					removeEmptyFaction(nFactionsID[k],TRUE);
		}
	}
	if (sCROutFilename.IsEmpty()) 
		sCROutFilename = sCRFilename;
	if (s_bModif && !s_bTestOnly)
		cr_in.save(sCROutFilename,FALSE);
	if (s_bTestOnly)
		outputln("no file saved - test option (-t)");

	return 0;
}

/////////////////////////////////////////
// output management

void outputvalist(const char * pszFormat, va_list argList, BOOL bAddCarriageReturn=FALSE)
{
	static CString sOutput("");
	sOutput.FormatV(pszFormat, argList);
	if (bAddCarriageReturn)	
		sOutput+="\n";

	fputs(sOutput,fERR);
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