// CheckResources.cpp : Defines the entry point for the console application.
//

#include <stdafx.h>
#include <stdarg.h>

#include "wTofile.h"
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

CString sHierarchyFilename("");
CString sCRFilename("");
CString sCRReferenceFilename("");
CString sCROutFilename("");
CString sResourceName("");
CString sOtherRefRegionName("");
CString sInShortName("");
CString sRefShortName("");
CString sHeaderLine("");

CCR cr_in;
CCR cr_ref;

BOOL s_bSave = FALSE;
BOOL s_bCheckRegionName = FALSE;
BOOL s_bEnglishResourceName = FALSE;
BOOL s_bAppendOutput = FALSE;

BOOL s_bOutputHeader = FALSE;
int s_bRemoveZeros = 0;

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

				case 'a':
						s_bAppendOutput = TRUE;	
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
						char sErrFilename[255];
						strcpy_s(sErrFilename,255,x);
						int nNumErrFile=fopen_s(&fERR,sErrFilename,s_bAppendOutput?"a":"w");
						if (!fERR && nNumErrFile) {
							char smsg[255];
							strerror_s(smsg,255,errno);
							fprintf(stderr,"Can't write to file `%s':\n	%s",sErrFilename,smsg);
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
						int nErrCode=fopen_s(&fOUT,x,"w");
						if (!fOUT && nErrCode) {
							char serrmsg[255];
							strerror_s(serrmsg,255,errno);
							fprintf(stderr,"Can't write to file `%s':\n	%s",x,serrmsg);
							exit(0);
						}
					}
					break;
				case 'B':
					if (dostop)	{ // bei Optionen via "; ECHECK" nicht mehr machen
						if (argv[i][2]==0) {	// "-D name"
							i++;
							x=argv[i];
						} else							// "-Dname"
							x=argv[i]+2;
						sOtherRefRegionName=x;
						if (sOtherRefRegionName.IsEmpty()) {
							fputs("Empty sOtherRefRegionName\n",stderr);
							exit(0);
						}
					}
					break;

				case 'D':
					if (dostop)	{ // bei Optionen via "; ECHECK" nicht mehr machen
						if (argv[i][2]==0) {	// "-D name"
							i++;
							x=argv[i];
						} else							// "-Dname"
							x=argv[i]+2;
						sResourceName=x;
						if (sResourceName.IsEmpty()) {
							fputs("Empty resource german name\n",stderr);
							exit(0);
						}
					}
					break;

				case 'E':
					if (dostop)	{ // bei Optionen via "; ECHECK" nicht mehr machen
						if (argv[i][2]==0) {	// "-D name"
							i++;
							x=argv[i];
						} else							// "-Dname"
							x=argv[i]+2;
						sResourceName=x;
						s_bEnglishResourceName=TRUE;
						if (sResourceName.IsEmpty()) {
							fputs("Empty resource english name\n",stderr);
							exit(0);
						}
					}
					break;

				case 's': s_bSave = TRUE; break;
				case 'n': s_bCheckRegionName = TRUE; break;

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

				case 'r':
					if (argv[i][2]==0) { 	// -r CRRefFilename // reference CR
						i++;
						sCRReferenceFilename=validFilename(argv[i]);
					} else {
						if (argv[i][2])
							sCRReferenceFilename=validFilename(argv[i]+2);
					}
					if (sCRReferenceFilename.IsEmpty())
						fputs("Missing input reference CR file\n", stderr);
					break;

				case 'S':
					if (argv[i][2]==0) { 	// -S CRFilename // output CR
						i++;
						sCROutFilename=validFilename(argv[i]);
					} else {
						if (argv[i][2])
							sCROutFilename=validFilename(argv[i]+2);
					}
					if (sCROutFilename.IsEmpty()) fputs("Missing output CR file\n", stderr); else s_bSave = TRUE;
					break;
				case 'z': s_bRemoveZeros=1; break;
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

	outputln("%s\n",(LPCSTR)sFullCommandLine);
}

//////////

#define NBFEATURES (9)

#define I_STRING_FEATURES (6) // CR_HERB

static CString sFeatures[NBFEATURES]={
	CR_HORSES,
	CR_PEASANTS,
	CR_YOUNGTREES,
	CR_TREES,
	CR_SILVER,
	CR_ENTERTAIN,
	CR_HERB,
	CR_HERBAMOUNT

	/*CR_HORSES_PREV,
	CR_PEASANTS_PREV,
	CR_TREES_PREV,
	CR_YOUNGTREES_PREV,
	CR_SILVER_PREV,*/
	""};


BOOL isFeature(const char *pDEName, int & ind)
{
	ind = -1;
	for (int i=0;i<NBFEATURES;i++)
	{
		if (!_strcmpi(sFeatures[i],"")) 
			return FALSE;
		if (!_strcmpi(sFeatures[i],pDEName)) 
		{
			ind=i; 
			return TRUE;
		}
	}

	return FALSE;
}

BOOL compare_resources(const char *pRegionName, const char *pResourceName, block * resources_ref, block * resources_in)
{
	BOOL bSpecifiedResourceName = FALSE;
	BOOL bModif = FALSE;

	if (!resources_ref) 
		return FALSE;
	if (!resources_in) 
		return FALSE;

	CString sType(pResourceName);
	block *b_ref = resources_ref;
	if (!sType.IsEmpty())
	{
		b_ref=cr_ref.resourceByName(sType,b_ref);
		bSpecifiedResourceName=TRUE;
	}

	while (b_ref)
	{
		if (bSpecifiedResourceName || (cr_ref.gets(b_ref, CR_RESOURCE_TYPE, sType) && !sType.IsEmpty()))
		{
			block * b_in=cr_in.resourceByName(sType,resources_in);
			if (b_in)
			{
				CString sENType = CCR::German2English(sType);
				int nNumber_in = 0;
				int nNumber_ref = 0;
				int nSkill_in = 0;
				int nSkill_ref = 0;
				int nid_ref = *(b_ref->ids);
				int nid_in = *(b_in->ids);
				cr_in.geti(b_in,"number",nNumber_in);
				cr_ref.geti(b_ref,"number",nNumber_ref);
				cr_in.geti(b_in,"skill",nSkill_in);
				cr_ref.geti(b_ref,"skill",nSkill_ref);
				CString sDiffIDs(nid_ref!=nid_in?"[different ids]":"");
				CString sOldnum("");
				CString sNewnum("");
				CString sOldSkill("");
				CString sNewSkill("");
				CString sOld("");
				CString sNew("");
				if (nNumber_in) sOldnum.Format("%3d",nNumber_in);
				if (nNumber_ref) sNewnum.Format("%3d",nNumber_ref);
				if (nSkill_in) sOldSkill.Format("/T%d",nSkill_in);
				if (nSkill_ref) sNewSkill.Format("/T%d",nSkill_ref);
				if (nNumber_in) sOld.Format("%s%s",sOldnum,sOldSkill); else sOld=sOldSkill;
				if (nNumber_ref) sNew.Format("%s%s",sNewnum,sNewSkill); else sNew=sNewSkill;
				if (sOld.CompareNoCase(sNew))
				{
					bModif = TRUE;
					output_header();
					outputln("%15s - %10s: %s ==> %s %s%s",pRegionName,(LPCSTR)sENType,(LPCSTR)sOld,(LPCSTR)sNew,(LPCSTR)sDiffIDs,s_bSave?" (written)":"");
					if (nNumber_ref) cr_in.seti(b_in,"number",nNumber_ref); else cr_in.onSave_removeEntry(b_in,"number");
					if (nSkill_ref) cr_in.seti(b_in,"skill",nSkill_ref); else cr_in.onSave_removeEntry(b_in,"skill");
					*(b_in->ids)=*(b_ref->ids);
				}
			}
		}
		b_ref = b_ref->next;
		if (bSpecifiedResourceName) break;
	}
	return bModif;
}

BOOL compare_castle(const char *pRegionName, block * r_ref, block * r_in)
{
	block * br=cr_ref.castle(r_ref);
	block * bi=cr_in.castle(r_in);
	if (br && bi)
	{
		int nrefSize=0;
		int ninSize=0;
		cr_ref.reg_i(RFT_CASTLESIZE, br, nrefSize);
		cr_in.reg_i(RFT_CASTLESIZE, bi, ninSize);
		if (nrefSize!=ninSize) 
		{
			outputln("%15s - %10s: %4d ==> %4d%s",pRegionName,(LPCSTR)"castle",ninSize,nrefSize,s_bSave?" (written)":"");
			return cr_in.seti(bi, CR_SIZE, nrefSize);
		}
	}
	return FALSE;
}

BOOL compare_feature(const char *pRegionName, int iFeatureIndex, block * r_ref, block * r_in)
{
	BOOL bModif = FALSE;
	BOOL bSpecifiedFeatureName = FALSE;
	CString sType(sFeatures[iFeatureIndex]);
	block *b_ref = r_ref;
	if (!sType.IsEmpty())
		bSpecifiedFeatureName = TRUE;

	if (bSpecifiedFeatureName)
	{
		int nNumber_in = 0;
		int nNumber_ref = 0;
		if ((iFeatureIndex < I_STRING_FEATURES))
		{
			BOOL bRefOK=cr_ref.geti(r_ref,sType,nNumber_ref);
			BOOL bInOK=cr_in.geti(r_in,sType,nNumber_in);
			CString sENType = CCR::German2English(sType);
			CString sOldnum("");
			CString sNewnum("");
			sOldnum.Format("%3d",nNumber_in);
			sNewnum.Format("%3d",nNumber_ref);
			// outputln("%15s - %3d %10s",pRegionName,nNumber_in,(LPCSTR)sENType);

			if (sOldnum.CompareNoCase(sNewnum))
			{
				bModif = TRUE;
				output_header();
				CString sdiag(" ");
				if (!bRefOK) 
					sdiag=" (not in ref) ";
				outputln("%15s - %10s: %s ==> %s%s%s",pRegionName,(LPCSTR)sENType,(LPCSTR)sOldnum,(LPCSTR)sNewnum,(LPCSTR)sdiag,s_bSave?" (written)":"");
				if (nNumber_ref==0 && !bRefOK && s_bRemoveZeros==1)
					bRefOK=TRUE;

				if (bRefOK) 
				{
					if (nNumber_ref) 
						cr_in.seti(r_in,sType,nNumber_ref); 
					else 
						cr_in.onSave_removeEntry(r_in,sType);
				}
			}
		}
		CString svalue_ref("");
		CString svalue_in("");
		if (iFeatureIndex >= I_STRING_FEATURES && cr_ref.gets(r_ref,sType,svalue_ref))
		{
			cr_in.gets(r_in,sType,svalue_in);
			CString sENType = CCR::German2English(sType);

			if (svalue_in.CompareNoCase(svalue_ref))
			{
				bModif = TRUE;
				output_header();
				outputln("%15s - %10s: %s ==> %s%s",pRegionName,(LPCSTR)sENType,(LPCSTR)svalue_in,(LPCSTR)svalue_ref,s_bSave?" (written)":"");
				cr_in.sets(r_in,sType,svalue_ref);
			}
		}
	}
	return bModif;
}

BOOL compare_all(const char *pRegionName, block * r_ref, block * r_in)
{
	CString sFeatureName("");
	BOOL bModif=FALSE;
	for (int i=0;i<NBFEATURES;i++)
	{
		if (!_strcmpi(sFeatures[i],"")) 
			break;
		// correct resources: horses, peasants, youngtrees, trees, silver, entertain, herb, herbamount
		if (compare_feature(pRegionName, i, r_ref, r_in))
			bModif=TRUE;
	}

	// correct castle size
	if (compare_castle(pRegionName,r_ref,r_in))
		bModif=TRUE;	
	if (compare_resources(pRegionName,"",cr_ref.resources(r_ref),(r_in?cr_in.resources(r_in):NULL)))
		bModif=TRUE;
	return bModif;
}

BOOL compare_object(const char *pRegionName, const char *pObjectName, block * r_ref, block * r_in, int nFeatureIndex, BOOL bCheckAll)
{
	if (bCheckAll) 
		return compare_all(pRegionName,r_ref,r_in);
	if (nFeatureIndex!=-1) 
		return compare_feature(pRegionName,nFeatureIndex, r_ref,r_in);
	if (!_strcmpi(pObjectName,"castle")) 
		return compare_castle(pRegionName,r_ref,r_in);
	return compare_resources(pRegionName,pObjectName,cr_ref.resources(r_ref),(r_in?cr_in.resources(r_in):NULL));
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

void output_header()
{
	if (!s_bOutputHeader)
			outputln("%s:",(LPCSTR)sHeaderLine);
	s_bOutputHeader = TRUE;
}

BOOL addUnknownFactions(CCR & cr_in, CCR & cr_ref)
{
	BOOL bModif=FALSE;
	block * factions_in=cr_in.factions();
	block * factions_ref=cr_ref.factions();
	while (factions_ref){ if (cr_ref.b_isFaction(factions_ref)) break; factions_ref=factions_ref->next;}
	while (factions_in){ if (cr_in.b_isFaction(factions_in)) break; factions_in=factions_in->next;}

	block * f=factions_in;
	while (f)
	{
		if (!cr_in.b_isFaction(f))
		{
			f=f->next;
			continue;
		}
		int nfid=f->ids[0];
		if (nfid==500019)
		{
			int yyy=1;
		}
		CString sName("");
		CString sNewName("");
		CString sUnknownName("");
		sUnknownName.Format("Partei %s",(LPCSTR)itob(nfid));
		if (cr_in.gets(f,"Parteiname",sName) && !sName.Compare(sUnknownName))
		{
			block * f2 =cr_ref.factions(&nfid);
			if (f2 && cr_ref.gets(f2,"Parteiname",sNewName) && cr_in.sets(f,"Parteiname",sNewName)) 
			{
				outputln("%s converted into %s",(LPCSTR)sUnknownName,(LPCSTR)sNewName);
				bModif=TRUE;
			}
		}
		f=f->next;
	}
	return bModif;
}

BOOL treat_crref(CCR & cr_in, CCR & cr_ref, const CString & sResourceName, int nDeltaX, int nDeltaY) 
{
	BOOL bCheckAll = FALSE;
	BOOL bModif = FALSE;
	block * rs_ref = cr_ref.regions();
	int ids[2] = {0,0};
	int nFeatureIndex=-1;

	if (!sResourceName.CompareNoCase("*"))
		bCheckAll = TRUE;

	CCRRegion * pr = NULL;
	CCRReport & crreport = cr_ref.report();
	CCRRegions  & rs = crreport.allRegions();

	POSITION pos = rs.GetHeadPosition();
	while  (pos) 
	{
		pr = rs.GetNext(pos);

		// ignore firewalls & oceans
		if (!pr->isLand())
			continue;

		ids[0]=pr->x()-nDeltaX;
		ids[1]=pr->y()-nDeltaY;

		block * r_in = cr_in.regions(ids);

		CString sInRegionName("");
		CString sRefRegionName(pr->name());

		//pbm: sRefRegionName kan be empty!

		block * brin = cr_in.region(ids[0],ids[1]);
		if (brin)
		{
			cr_in.gets(r_in,CR_NAME,sInRegionName);

			if (!sInRegionName.CompareNoCase("kofan"))
				int hhhhh=1;
			// correct region resources
			if (compare_object(sInRegionName,sResourceName,pr->cr_region(),r_in,nFeatureIndex,bCheckAll))
				bModif = TRUE;

			if (sInRegionName.CompareNoCase(sRefRegionName) && !sRefRegionName.IsEmpty() && s_bCheckRegionName)
			{
				if (pr->hasKnownMen())
				{
					// correct regions name
					outputln("region name: %s ==> %s",(LPCSTR)sInRegionName,(LPCSTR)sRefRegionName);
					bModif=TRUE;
					cr_in.sets(r_in,CR_NAME,sRefRegionName);
				}
			}
		}
	}

	if (addUnknownFactions(cr_in,cr_ref))
		bModif = TRUE;

	return bModif;
}

int checkResources(int argc, char *argv[]) {
	int nextarg;
	int nDeltaX = 0;
	int nDeltaY = 0;
	int ids[2] = {0,0};

	CString sInOriginRegionName("");
	CString sRefOriginRegionName("");
	BOOL bModif = FALSE;

	fERR=stdout;

	if (argc>1)
		nextarg=check_options(argc, argv, 1, 1);

	//output(ECHECK, CHECKORDERS_VERSION,__DATE__);
	// outputCommandLine(argc,argv);

	sInShortName = STGetShortPathname(sCRFilename);

	if (!cr_in.load(sCRFilename,sHierarchyFilename,&crdata_ireport_std))
		return errormsg("failed to load %s in CR file",sCRFilename);
	//cr_in.closeHierarchyFile();
	if (s_bEnglishResourceName) 
		sResourceName = CCR::English2German(sResourceName);
	block * r_orig_in = cr_in.regions(ids);
	if (!cr_in.gets(r_orig_in,CR_NAME,sInOriginRegionName))
		return errormsg("%s: no region name specified for (0,0) !",sCRFilename);
	if (s_bSave && sCROutFilename.IsEmpty()) 
		sCROutFilename = sCRFilename;

	sRefShortName = STGetShortPathname(sCRReferenceFilename);
	sHeaderLine.Format("%s==>%s",(LPCSTR)sRefShortName,(LPCSTR)sInShortName);
	if (!cr_ref.load(sCRReferenceFilename,sHierarchyFilename,&crdata_ireport_std))
		return errormsg("failed to load %s ref CR file",sCRReferenceFilename);

	if (cr_ref.turn()!= cr_in.turn())
		return errormsg("%s: CR turns must be equals [in=%d, ref=%d] !",sHeaderLine,cr_in.turn(),cr_ref.turn());

	cr_ref.fillReport(OPT_REGIONS|OPT_FACTIONS|OPT_UNITS);

	block * rs_ref = cr_ref.regions();
	block * r_ref = rs_ref;

	block * r2 = regionWithName(cr_ref, rs_ref, sInOriginRegionName);
	if (r2)
	{
		nDeltaX = r2->ids[0];
		nDeltaY = r2->ids[1];
	}
	else
	{
		block * r_orig_ref = cr_ref.regions(ids);
		if (!r_orig_ref) {
		// r_orig_ref is NULL sometimes !
			if (sOtherRefRegionName.IsEmpty())
				return errormsg("%s: CR are not connected - (0,0)=%s",sHeaderLine,sInOriginRegionName);
			r_orig_ref= regionWithName(cr_ref, cr_ref.regions(), sOtherRefRegionName);
			if (!r_orig_ref)
				return errormsg("%s: %s not found in map",sHeaderLine,(LPCSTR)sOtherRefRegionName);
			nDeltaX = r_orig_ref->ids[0];
			nDeltaY = r_orig_ref->ids[1];
			sRefOriginRegionName=sOtherRefRegionName;
			block * r3 = regionWithName(cr_in, cr_in.regions(), sRefOriginRegionName);
			if (!r3) 
				return errormsg("%s: %s not found in map",sHeaderLine,(LPCSTR)sRefOriginRegionName);
			nDeltaX -= r3->ids[0];
			nDeltaY -= r3->ids[1];
	
		}
		else
		{
			cr_ref.gets(r_orig_ref,CR_NAME,sRefOriginRegionName);
			block * r3 = regionWithName(cr_in, cr_in.regions(), sRefOriginRegionName);
			if (!r3) 
				return errormsg("%s:%s not found in map",sHeaderLine,(LPCSTR)sRefOriginRegionName);
		}
	}

	if ((nDeltaX!=0) ||  (nDeltaY!=0))
	{
		CString s1("");
		CString s2("");
		CString s("");
		if (nDeltaX!=0) s1.Format("deltax[%d]",nDeltaX);
		if (nDeltaY!=0) s2.Format("deltay[%d]",nDeltaY);
		s.Format(" (%s,%s)",(LPCSTR)s1,(LPCSTR)s2);
		sHeaderLine += s;
	}

	if (treat_crref(cr_in,cr_ref, sResourceName, nDeltaX, nDeltaY))
		bModif = TRUE;

	output("\n");

	if (bModif) 
	{
		if (s_bSave)
		{
			if (!sCROutFilename.CompareNoCase(sCRReferenceFilename))
				return errormsg("%s:result filename cannot be the reference CR file",(LPCSTR)sHeaderLine);
			cr_in.save(sCROutFilename,FALSE);
			outputln("%s saved",(LPCSTR)sCROutFilename);
		}
	}
	else
		outputln("%s: no difference",(LPCSTR)sHeaderLine);


	return 0;
}

/////////////////////////////////////////
// output management

void outputvalist(const char * pszFormat, va_list argList, BOOL bAddCarriageReturn = FALSE)
{
	char buffer[1024]; 
	vsprintf_s(buffer, 1024, pszFormat, argList);
	if (bAddCarriageReturn)	
		strcat_s(buffer,1024,"\n");
	fputs(buffer,fERR);
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