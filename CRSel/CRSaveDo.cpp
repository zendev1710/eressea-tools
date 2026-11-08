// CRSelDo.cpp : Defines the entry point for the console application.
//

#include <stdafx.h>
#include <stdarg.h>

#include "wToFile.h"
#include "cr.h"

// memory leaks detecting
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern void output(int nErrorNum, ...);
extern void outputln(int nErrorNum, ...);
extern void output(const char * pzFormat, ...);
extern void outputln(const char * pzFormat, ...);

BOOL errormsg(const char * pzFormat, ...);

////////////////

#define PROG_NAME	"CRSave"

#define MAINVERSION "1"
#define MINVERSION  "0"
#define PATCHLEVEL  "2"
#define BUILDNUM	"01"

#define PROG_VERSION MAINVERSION"."MINVERSION"."PATCHLEVEL"b"BUILDNUM

#define MAXREGIONS (20000)

////////////////

FILE *fERR/*=NULL*/;
FILE *fOUT/*=NULL*/;

CString sHierarchyFilename("");
CString sCRFilename("");
CString sCROutFilename("");
CString sOriginRegionName("");

//CString sIslands[512];
//CString sIslandNames[512];

CString sSelFilename("");

int nXMin =LOCATION_MIN_VALUE;
int nYMin = LOCATION_MIN_VALUE;
int nXMax = LOCATION_MAX_VALUE;
int nYMax = LOCATION_MAX_VALUE;

CPoint selectedRegionPositions[MAXREGIONS];
BOOL selectedRegionAstral[MAXREGIONS];

int nNbSelectedRegionPositions = 0;

CCR cr_in;

BOOL s_bErrors = FALSE;
BOOL s_bChangeOrigin = FALSE;
BOOL s_bNotSel = FALSE;
BOOL s_bSelFile = FALSE;
BOOL s_bRemoveBattles = FALSE;
BOOL s_bRemoveAllBattles = FALSE;

BOOL isSelectedRegion(int nx, int ny, BOOL bAstral)
{
//	BOOL bDeb=FALSE;
	if (s_bSelFile)
	{
		for (int i=0;i<nNbSelectedRegionPositions;i++) 
		{
			if (selectedRegionPositions[i].x == nx && selectedRegionPositions[i].y == ny && bAstral==selectedRegionAstral[i])
				return TRUE;
		}
		return FALSE;
	}
	if (nx < nXMin) 
		return FALSE;
	if (nx > nXMax) 
		return FALSE;
	if (ny < nYMin) 
		return FALSE;
	if (ny > nYMax) 
		return FALSE;
	return TRUE;
}

CString validFilename(const char *ps)
{
	if (!ps) return "";
	CString sf(ps);
	sf.Remove('"');
	return sf;
}

int myatoi(const char *p, int nDefValue=0)
{
	if (!p) return nDefValue;
	int n=atoi(p);
	if (n==-1) n = nDefValue;
	return n;
}

int check_options(int argc, char *argv[], char dostop, char command_line) {
	int i;
	char *pr;
	CString sXMin("");
	CString sXMax("");
	CString sYMin("");
	CString sYMax("");

	for (i=1; i<argc; i++) {
		if (argv[i][0]=='-') {
			switch (argv[i][1]) {
				case 'b':
					s_bRemoveBattles = TRUE;
					if (argv[i][2]=='x')
						s_bRemoveAllBattles = TRUE;
					break;
				case 'n': 
				s_bNotSel = TRUE; 
				break;
				case 'g': 
				s_bChangeOrigin = TRUE; 
				break;
				case 'l':
						if (argv[i][2]==0) {	// "-l file"
							i++;
							sSelFilename=argv[i];
						} else							// "-lfile"
							sSelFilename=argv[i]+2;
				break;

				case 'x':
						if (argv[i][2]==0) {	// "-x file"
							i++;
							sOriginRegionName=argv[i];
						} 
						else
						{
							//xmin, xmax
							pr = argv[i]+2;
							if (strstr(pr,"min")!=NULL) 
							{
								if (pr[3]==0)
								{
									i++;
									sXMin=argv[i];
								}
								else
									sXMin=pr[3];
								nXMin = myatoi(sXMin, nXMin);
							}
							else				
							if (strstr(pr,"max")!=NULL) 
							{
								if (pr[3]==0)
								{
									i++;
									sXMax=argv[i];
								}
								else
									sXMax=pr[3];
								nXMax = myatoi(sXMax, nXMax);
							}
							else				
							sOriginRegionName=argv[i]+2;// "-xfile"
						}
				break;

				case 'y':
						if (argv[i][2]==0) {	// "-y file"
						//error
						} 
						else
						{
							//ymin, ymax
							pr = argv[i]+2;
							if (strstr(pr,"min")!=NULL) 
							{
								if (pr[3]==0)
								{
									i++;
									sYMin=argv[i];
								}
								else
									sYMin=pr[3];
								nYMin = myatoi(sYMin, nYMin);
							}
							else				
							if (strstr(pr,"max")!=NULL) 
							{
								if (pr[3]==0)
								{
									i++;
									sYMax=argv[i];
								}
								else
									sYMax=pr[3];
								nYMax = myatoi(sYMax, nYMax);
							}
							else
							{
							//error
							}				
						}
				break;

				case 'O':
					if (dostop)	{ // bei Optionen via "; ECHECK" nicht mehr machen
						if (argv[i][2]==0) {	// "-O file"
							i++;
							pr=argv[i];
						} else							// "-Ofile"
							pr=argv[i]+2;
						if (!pr) {
							fputs("Using stderr for error output\n",stderr);
							fERR=stderr;
							break;
						}
						int nErr=fopen_s(&fERR,pr,"w");
						if (!fERR) {
							char sErrmsg[255];
							int nErr2=strerror_s(sErrmsg,255,errno);
							fprintf(stderr,"Can't write to file `%s':\n	%s",pr,sErrmsg);
							exit(0);
						}
					}
					break;

				case 'o':
					if (dostop)	{ // bei Optionen via "; ECHECK" nicht mehr machen
						if (argv[i][2]==0) {	// "-o file"
							i++;
							pr=argv[i];
						} else							// "-ofile"
							pr=argv[i]+2;
						if (!pr) {
							fputs("Empty file for checked file, using stdout\n",stderr);
							fOUT=stdout;
							break;
						}
						int nErr=fopen_s(&fOUT,pr,"w");
						if (!fOUT && nErr!=0) {
							char sErrmsg[255];
							int nErr2=strerror_s(sErrmsg,255,errno);
							fprintf(stderr,"Can't write to file `%s':\n	%s",pr,sErrmsg);
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
	char *nexttoken;

	vl=strtok_s(p, " \t,",&nexttoken);
	do {
		*ap++=vl;
		argc++;
	} while ((vl=strtok_s(NULL, " \t,",&nexttoken))!=NULL);
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

	output("%s\n",(LPCSTR)sFullCommandLine);
}

BOOL readSelFile(const char *pFilename, CPoint * sels, BOOL *bAstral, int & nb, int & nbAstral)
{
	TRY
	{
		CString s("");
		nb = 0;
		nbAstral = 0;
		CStdioFile f(pFilename,CFile::modeRead);
		while (f.ReadString(s))
		{
			int nAstral=0;
			int nbscanned = sscanf_s(s,"%d %d %d",&sels[nb].x,&sels[nb].y,&nAstral);
			bAstral[nb]=((nbscanned==3 && nAstral==1)?TRUE:FALSE);
			if (bAstral[nb])
				nbAstral++;
			nb++;
			if (nb==MAXREGIONS)
			{
				outputln("error !! nbmaxregions (%d) reacht",MAXREGIONS);
				break;
			}

		}
		outputln("%s SEL file loaded - %d regions (%d astral)",pFilename,nb,nbAstral);
		return TRUE;

	}
	CATCH (CFileException, e)
	{
		int yyy=1;
	}
	END_CATCH
	return FALSE;
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

BOOL removeBattles(BOOL bAllBattles)
{
	block *parent = NULL;
	block *msgs = NULL;
	block *battles = NULL;
	block * factions = cr_in.factions();
	block * f = factions;
	int nbf=0;
	while (f)
	{
		//msgs=cr_in.messages(f);
		if (!cr_in.b_isFaction(f))
			break;
		battles=cr_in.battles(f);
		if (battles) 
			cr_in.onSave_remove(battles);
		/*
		block * bt = battles;
		block * btn = NULL;
		while (bt)
		{
			if (!cr_in.b_isBattle(bt))
				break;
			int nxb=0;
			int nyb=0;
			BOOL bChildren=FALSE;
			BOOL bBrothers=FALSE;
			btn = bt->next; 
			if (bAllBattles)
				cr_in.onSave_remove(bt,bBrothers,bChildren);
			bt = btn;
		}
		*/
		f = f->next;
	}
	return TRUE;
}

int checkSel(int argc, char *argv[]) {
	int nextarg;
	fERR=stdout;

	if (argc>1)
		nextarg=check_options(argc, argv, 1, 1);


	/*
	for (i=nextarg; i<argc && count < 512; i++) {
		sFileNames[count] = argv[i];
		::GetPrivateProfileString("islands",sIslandNames[count],"",sIslands[count].GetBuffer(512),512,".\\eressea.ini");
		count++;
	}
	*/

	output("%s version %s - %s\n",PROG_NAME, PROG_VERSION,__DATE__);
	outputCommandLine(argc,argv);
	int nNbAstral=0;

	if (!sSelFilename.IsEmpty()) 
	{
		if (sOriginRegionName.IsEmpty()) 
			return errormsg("no origin region name specified (-x option)");
	
		sSelFilename = STGetFullPathname(sSelFilename);
		CString sExt = STGetFileExtension(sSelFilename);
		sExt.MakeLower();
		if (sExt.IsEmpty()) 
			sSelFilename += ".sel";	
		else 
			if (sExt.CompareNoCase(".sel")) 
				return errormsg("specified file %s is not a .sel file",(LPCSTR)sSelFilename);
		s_bSelFile = readSelFile(sSelFilename, selectedRegionPositions, selectedRegionAstral, nNbSelectedRegionPositions,nNbAstral);
	}
	else
	{
//		if (nXMin==LOCATION_MIN_VALUE && nYMin==LOCATION_MIN_VALUE && nXMax==LOCATION_MAX_VALUE && nYMin==LOCATION_MAX_VALUE)
//			return errormsg("no .sel file specified nor no x/y min/max");
	}

	if (cr_in.load(sCRFilename,sHierarchyFilename,&crdata_ireport_std)) {
		outputln("%s CR file loaded",sCRFilename);
		cr_in.close();
	}

	block * rs = cr_in.regions();
	block *r = rs;
	int nbr = 0;

	int nDeltaX = 0;
	int nDeltaY = 0;
	if (s_bSelFile)
	{
		block *ro =  regionWithName(cr_in, rs, sOriginRegionName);
		if (!ro) 
			return errormsg("specified origin region name %s not found (-x option)",(LPCSTR)sOriginRegionName);
		nDeltaX = -ro->ids[0];
		nDeltaY = -ro->ids[1];
	}
	while (r)
	{
		BOOL bInAstral=FALSE;
		if (!cr_in.b_isRegion(r,&bInAstral))
			break;
		int ndx=(bInAstral?0:nDeltaX);
		int ndy=(bInAstral?0:nDeltaY);
		/*
		CString sTerrain("");
		block *br=(block *)r;
		if (cr_in.gets(br,CR_TERRAIN,sTerrain) && !sTerrain.CompareNoCase("Nebel"))
		{
			bInAstral=TRUE;
		}
		*/
		if (isSelectedRegion(r->ids[0]+ndx,r->ids[1]+ndy,bInAstral)==s_bNotSel)
		{
			block * t = r->next;
			cr_in.onSave_remove(r, FALSE, FALSE);
			r = t;
			continue;
		}
		nbr++;
		if (s_bChangeOrigin)
		{
			r->ids[0]+=nDeltaX;
			r->ids[1]+=nDeltaY;
		}
		r=r->next;
	}

	//if (nbr!=nNbSelectedRegionPositions) 
	//	return errormsg("found regions number (%d) is not the number of the selections of the .sel file (%d in %s)",nbr,nNbSelectedRegionPositions,(LPCSTR)sSelFilename);

	if (s_bRemoveBattles)
		removeBattles(s_bRemoveAllBattles);

	if (nbr > 0)
	{
		int nbCorrectedNames=cr_in.correctBadRegionNames();
		if (nbCorrectedNames>0)
			outputln("%d bad region names resolved",nbr);

		if (sCROutFilename.IsEmpty()) 
			sCROutFilename = sCRFilename;
		sCROutFilename = STGetFullPathname(sCROutFilename);
		if (cr_in.save(sCROutFilename,FALSE))
			outputln("%d regions in generated %s CR file.",nbr,sCROutFilename);
		else
			errormsg("failed to save %s CR file",sCROutFilename);
	}
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