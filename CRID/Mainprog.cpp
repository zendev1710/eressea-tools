// MainProg.cpp : Defines the entry point for the console application.
//

#include <stdafx.h>
#include <stdarg.h>

//#include "wTofile.h"
#include "CR.h"

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

FILE *fERR/*=NULL*/;
FILE *fOUT/*=NULL*/;


int check_options(int argc, char *argv[], char dostop, char command_line) {
	int i;
	char *x;
	for (i=1; i<argc; i++) {
		if (argv[i][0]=='-') {
			switch (argv[i][1]) {

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
						fERR=fopen(x,"w");
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
						fOUT=fopen(x,"w");
						if (!fOUT) {
							fprintf(stderr,"Can't write to file `%s':\n"
									"	%s",x,strerror(errno));
							exit(0);
						}
					}
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

int my_btoi(const char *s) {
	const char *x=s;
	int i=0;
	int n=0;
	if (!(*x)) return 0;
	while(isalnum(*x)) {x++;n++;}
	if (n>4) return 1; //pbm
	i=(int)(strtol(s, NULL, 36));
	return i;
}

int MainProg(int argc, char *argv[]) {
	int nextarg;

	fERR=stdout;
	fOUT=stdout;

	if (argc>1)
		nextarg=check_options(argc, argv, 1, 1);

	//output(ECHECK, CHECKORDERS_VERSION,__DATE__);
	// outputCommandLine(argc,argv);

	for (int i=nextarg; i<argc; i++)
		output("%5s ==> %d\n",argv[i],my_btoi(argv[i]));

	return 0;
}

/////////////////////////////////////////
// output management

void outputvalist(const char * pszFormat, va_list argList, BOOL bAddCarriageReturn = FALSE)
{
	char buffer[1024]; 
	vsprintf (buffer, pszFormat, argList);
	if (bAddCarriageReturn)	strcat(buffer,"\n");
	fputs(buffer,fERR);
	if (fOUT!=fERR) fputs(buffer,fOUT);

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