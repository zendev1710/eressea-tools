// MyEcheck.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdarg.h>
#include <conio.h>

#include "EchkMgr.h"
#include "Factions.h"
#include "Units.h"
#include "Buildings.h"
#include "Regions.h"
#include "Roads.h"
#include "Ships.h"
#include "EcheckTypes.h"
#include "Spells.h"
#include "MyEresseaCR.h"
#include "CRFileScanOrders.h"
#include "CREresseaFile.h"
#include "OrdersFile.h"
#include "Message.h"
#include "Messages.h"
#include "wToFile.h"
#include "wToDir.h"
#include "CRError.h"
#include "Order.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CHKORDERUNIT if (!_strcmpi(itob(order_unit->id()),"mkwt"))
#define BKPTORDERUNIT {int zzzzzz=1;}

/* vi: set ts=2 ai sw=2:
 *
 *    Eressea PBeM host Copyright (C) 1997-2001
 *		Enno Rehling (rehling@usa.net)
 *		Christian Schlittchen (corwin@amber.kn-bremen.de)
 *		Katja Zedel (katze@felidae.kn-bremen.de)
 *		Henning Peters (faroul@gmx.de)
 *
 *  based on:
 *
 * Atlantis v1.0  13 September 1993 Copyright 1993 by Russell Wallace
 * Atlantis v1.7                    Copyright 1996 by Alex Schröder
 *
 * This program is freeware. It may not be sold or used commercially.
 * Please send any changes or bugfixes to the authors.
 *
 * Eressea PBeM Order Syntax Checker
 */

////////////////

static t_settings s_settings;

static CCRFileScanOrders sCRFile;

CMyEresseaCR & cr() { return sCRFile.cr();}

void initSettings()
{	
	s_settings.m_errs.m_bRegionName = FALSE;
	s_settings.m_errs.m_bRegionLocation = FALSE;
	s_settings.m_errs.m_bUnitName = FALSE;
	s_settings.m_errs.m_bUnitID = TRUE;
	
	s_settings.m_stats.m_bRegions = TRUE;
	s_settings.m_stats.m_bBuildings = TRUE;
	s_settings.m_stats.m_bShips = TRUE;
	s_settings.m_stats.m_bFactions = TRUE;

	s_settings.m_stats.m_bTeachings = FALSE;
	s_settings.m_stats.m_bMoves = FALSE;
} 

inline int my_isspace(int c) { 
	return iswspace(c);
	//return isspace(c);
}

inline int my_iscntrl(int c) {
	return iswcntrl(c);
	//return iscntrl(c);
}

CString validFilename(const char *ps)
{
	if (!ps) return "";
	CString sf(ps);
	extractName(sf);
	return sf;
}

#define MAXPARMS (7)

char
a_tolower(char c) {
	switch (c) {
		case 'Ä': return 'ä';
		case 'Ö': return 'ö';
		case 'Ü': return 'ü';
		default: return (char) tolower(c);
	}
}

int
a_strncasecmp(const char *a, const char *b, size_t s) {
	int i,j;

	if (!a || !b || !*a || !*b || strlen(b) < s)
		return -1;
	do {
		i=*a++ & 0xDF;	// 1101 1111 - Grandement/minuscule omettre
		j=*b++ & 0xDF;
	} while (i-j==0 && *a && *b && --s);
	return i-j;
}

# ifdef tolower
#  undef tolower
# endif
//# define tolower(x) a_tolower(x)
//#endif

void pause()
{
	fputs("\npress a key to continue",stdout);
	_getch();
}

void onExitProg(int nCode)
{
	if (nCode!=0)
		pause();
	exit(nCode);
}

// cmalloc & set the block to 0 (calloc)
void * cmalloc(int n) {
	void *p;

	if (n==0)
		n=1;
	p=calloc(n,1);
	if (p==NULL) {
		puts(" * no more available memory! *");
		onExitProg(1);
	}
	return p;
}

static char *Keys[UT_MAX]={
	"",
	"PARAM",
	"ITEM",
	"SKILL",
	"KEYWORD",
	"BUILDING",
	"HERB",
	"POTION",
	"RACE",
	"SPELL",
	"SHIP",
	"OPTION",
	"DIRECTION",
	"MSG",
	"HELP",
};

// Ces choses doivent être chargées, afin qu'il réussisse effectivement
#define HAS_PARAM				0x01
#define HAS_ITEM				0x02
#define HAS_SKILL				0x04
#define HAS_KEYWORD			0x08
#define HAS_DIRECTION		0x10
#define HAS_MESSAGES		0x20
#define HAS_ALL					0x3F

const t_ech_file ECheck_Files_EN[]={
	{ "help.txt", UT_HELP },
	{ "messages.txt", UT_ERRORS },
	{ "parameters.txt", UT_PARAM },
	{ "params.txt", UT_PARAM },
	{ "items.txt", UT_ITEM },
	{ "skills.txt", UT_SKILL },
	{ "commands.txt", UT_KEYWORD },
	{ "keywords.txt", UT_KEYWORD },
	{ "buildings.txt", UT_BUILDING },
	{ "builds.txt", UT_BUILDING },
	{ "herbs.txt", UT_HERB },
	{ "potions.txt", UT_POTION },
	{ "races.txt", UT_RACE },
	{ "factions.txt", UT_FACTION },
	{ "spells.txt", UT_SPELL },
	{ "ships.txt", UT_SHIP },
	{ "options.txt", UT_OPTION },
	{ "config.txt", UT_CONFIG },
	{ "directions.txt", UT_DIRECTION },
	{ "directs.txt", UT_DIRECTION },
	{ "translations.txt", UT_TRANSLATION },

	// le fichier générique, peut tout contenir, avec une reconnaissance des KEYWORD
	{ "tokens.txt", -1 }
};

/*
const t_ech_file ECheck_Files_DE[]={
	{ "help.txt", UT_HELP },
	{ "messages.txt", UT_ERRORS },
	{ "parameters.txt", UT_PARAM },
	{ "params.txt", UT_PARAM },
	{ "items.txt", UT_ITEM },
	{ "skills.txt", UT_SKILL },
	{ "commands.txt", UT_KEYWORD },
	{ "keywords.txt", UT_KEYWORD },
	{ "buildings.txt", UT_BUILDING },
	{ "builds.txt", UT_BUILDING },
	{ "herbs.txt", UT_HERB },
	{ "potions.txt", UT_POTION },
	{ "races.txt", UT_RACE },
	{ "factions.txt", UT_FACTION },
	{ "spells.txt", UT_SPELL },
	{ "ships.txt", UT_SHIP },
	{ "options.txt", UT_OPTION },
	{ "config.txt", UT_CONFIG },
	{ "directions.txt", UT_DIRECTION },
	{ "directs.txt", UT_DIRECTION },

	// le fichier générique, peut tout contenir, avec une reconnaissance des KEYWORD
	{ "tokens.txt", -1 }
};
*/

/*
const t_ech_file ECheck_Files[]={
	{ "hilfe.txt", UT_HELP },
	{ "meldungen.txt", UT_ERRORS },
	{ "meldung.txt", UT_ERRORS },
	{ "parameter.txt", UT_PARAM },
	{ "dinge.txt", UT_ITEM },
	{ "gegenstaende.txt", UT_ITEM },
	{ "talente.txt", UT_SKILL },
	{ "befehle.txt", UT_KEYWORD },
	{ "gebaeude.txt", UT_BUILDING },
	{ "kraeuter.txt", UT_HERB },
	{ "traenke.txt", UT_POTION },
	{ "rassen.txt", UT_RACE },
	{ "factionen.txt", UT_FACTION },
	{ "zauber.txt", UT_SPELL },
	{ "schiffe.txt", UT_SHIP },
	{ "optionen.txt", UT_OPTION },
	{ "richtungen.txt", UT_DIRECTION },
	{ "richtung.txt", UT_DIRECTION },

	{ "help.txt", UT_HELP },
	{ "messages.txt", UT_ERRORS },
	{ "parameters.txt", UT_PARAM },
	{ "params.txt", UT_PARAM },
	{ "items.txt", UT_ITEM },
	{ "skills.txt", UT_SKILL },
	{ "commands.txt", UT_KEYWORD },
	{ "keywords.txt", UT_KEYWORD },
	{ "buildings.txt", UT_BUILDING },
	{ "builds.txt", UT_BUILDING },
	{ "herbs.txt", UT_HERB },
	{ "potions.txt", UT_POTION },
	{ "races.txt", UT_RACE },
	{ "factions.txt", UT_FACTION },
	{ "spells.txt", UT_SPELL },
	{ "ships.txt", UT_SHIP },
	{ "options.txt", UT_OPTION },
	{ "config.txt", UT_CONFIG },
	{ "directions.txt", UT_DIRECTION },
	{ "directs.txt", UT_DIRECTION },
	{ "translations.txt", UT_TRANSLATION },

	// le fichier générique, peut tout contenir, avec une reconnaissance des KEYWORD
	{ "tokens.txt", -1 }
};
*/

//const int filecount=sizeof(ECheck_Files)/sizeof(ECheck_Files[0]);
const int filecount_EN=sizeof(ECheck_Files_EN)/sizeof(ECheck_Files_EN[0]);
//const int filecount_DE=sizeof(ECheck_Files_DE)/sizeof(ECheck_Files_DE[0]);

#if __STDC__
// strdup ist nicht ANSI
# if !(defined __USE_SVID) && !(defined __USE_BSD) && !(defined __USE_XOPEN_EXTENDED)
// ... ist aber unter Linux trotzdem definiert.
#  define strdup(s) (strcpy((char*)cmalloc(sizeof(char)*strlen(s)+1), s))
# endif
#endif

#ifdef AMIGA
	const char version[]="\0$VER: CheckOrders V"CHECKORDERS_VERSION", "__DATE__"\0";
#endif

#ifdef _MSC_VER
# include <crtdbg.h>
#endif

#ifndef BOOL_DEFINED
# define BOOL_DEFINED
# ifndef bool
#  define bool char
#  define true ((bool)1)
#  define false ((bool)0)
# endif
#endif

/*
#ifdef WIN32
# define strncasecmp _strnicmp
# define strcasecmp _stricmp
#endif
*/


#define MARGIN							78
#define RECRUIT_COST_DEFAULT			50

#define INDENT_FACTION			0
#define INDENT_UNIT					2
#define INDENT_ORDERS				4
#define INDENT_NEW_ORDERS		6

static BOOL s_bHerbTokensInited = FALSE;
static BOOL s_bItemTokensInited = FALSE;

void initHerbsToken();
void initItemsToken();

FILE *fERR/*=NULL*/;
FILE *fOUT/*=NULL*/;

int s_bArchiveCR=0;

/////////

#define getrandom( min, max ) ((rand() % (int)(((max) + 1) - (min))) + (min))

static BOOL first_random = 1;

int alea_value(int val1, int val2)
{
   if (val1 > val2)
      val2 = val1;
	
	if (first_random)
	{
		srand( (unsigned)time( NULL ) );
		first_random=FALSE;
	}

	return getrandom(val1,val2);
}

eID newID()
{
	char s[5];
	s[4]='\0';
	for (int i=0;i<4;i++)
		s[i]=(char)alea_value((int)'a',(int)'z');
	return btoi(s);
}

eID newTempID(const CRegion * pRegion, const char * pType, const char * pTempName)
{
	// try to keep a prevously created id
	CString sID;
	CString sTurn("");
	CString sKeyName("");
	CString sSuffix("");
	int nID;
	if (pTempName && _strcmpi(pTempName,"")) sSuffix.Format(".%s",pTempName);
	sTurn.Format("%d",cr().turn());
	sKeyName.Format("%s.%s%s",pRegion->outputName(),pType,(LPCSTR)sSuffix);
	::GetPrivateProfileString(sTurn,(LPCSTR)sKeyName,"",sID.GetBuffer(5),5,".\\eressea_tmp.ini");
	sID.ReleaseBuffer();
	if (!sID.IsEmpty())
	{
		nID = btoi(sID);
		CRegion *pR2=NULL;
		if (!sRegionsList.FindUnit(nID, pR2))
			return nID;
			int yy=1;
	}

	return newID();
}

BOOL s_bExtraOrders=FALSE;

CString sExtraOrdersFilename("");
CString sHierarchyFilename("");
CString sCRFilename("");
CString sCROutFilename("");

CString sMagellanFoldername("");
//CString sLangFoldername(".");

CCREresseaFile * sCurrentFile = NULL;
CRegion *pCurrentRegion = NULL;

int filesread=0;

char echo_it=0,						// option: echo input lines 
 no_comment=-3,						// No information in [ ] behind UNIT
 show_warnings=4,						// option: print warnings (levels)
 warnings_cl=0,						// -w donné sur la ligne de commande
 warn_off=0,							// ECHECK NOWARN
 use_stderr=0,						// option: use stderr for errors etc
 brief=0,									// option: don't list errors
 ignore_NameMe=0,					// option: ignore name ME comments ;;
 piping=0,								// option: wird output als pipe-input using? 
 lohn=10,									// Salaire pour travail - placer par région
 silberpool=0,					// option: Silberpool-Administration
 line_start=0,						// option: Commencement du compte de ligne
 noship=0,
 noroute=0,
 nolost=0,
 has_version=0,
 at_cmd=0,
 combine_factions=0, //C.G. 
 has_stats = 1,
 compile=0;								// option: compiler-/magellan-style warnings
int error_count=0,				// counter: errors
 warning_count=0;					// counter: warnings
//char checked_buf[BUFSIZE];		// checked order line
 char message_buf[BUFSIZE],		// messages are composed here
 warn_buf[BUFSIZE],				// warnings are composed here
 indent, next_indent,			// indent index
 does_default=0,					// Is DEFAULT active?
 *echeck_locale="en";

BOOL s_bUseCROnly = FALSE;
BOOL s_bOutputCR = FALSE;
BOOL s_bSaveAsMinimalAlso = FALSE;

int g_nRecruitCost=RECRUIT_COST_DEFAULT;

int this_command,
 Rx, Ry;								// Coordinates of the current region
char *path = NULL;


enum {
	OUT_NORMAL,
	OUT_COMPILE,
	OUT_MAGELLAN
};

t_keyword *keywords=NULL;
t_translation *translations=NULL;

#define igetkeyword(s)	(K_Order)findtoken(igetstr(s), UT_KEYWORD)

static char *magiegebiet[]={
	"Illaun",
	"Tybied",
	"Cerddor",
	"Gwyrrd",
	"Draig"
};

static const char *Params[MAXPARAMS]={
	"ALL",
	"PEASANTS",
	"CASTLE",
	"BUILDING",
	"UNIT",
	"FLEE",
	"REAR",
	"FRONT",
	"CONTROL",
	"HERBS",
	"SEED",
	"NOT",
	"NEXT",
	"FACTION",
	"PERSON",
	"REGION",
	"SHIP",
	"SILVER",
	"ROAD",
	"TEMP",
	"PRIVATE",
	"FIGHT",
	"COMBAT",
	"OBSERVE",
	"GIVE",
	"GUARD",
	"FACTIONSTEALTH",
	"WARN",
	"LEVEL",
	"HORSES",
	"FOREIGN",
	"AGGRESSIVE",
	"DEFENSIVE",
	"NUMBER",
	"LOCALE",
	"EACH"
};

t_params *parameters=NULL;


static char *reports[]={	// Erreur et communications dans le rapport
	"Kampf",
	"Ereignisse",
	"Bewegung",
	"Einkommen",
	"Handel",
	"Produktion",
	"Orkvermehrung",
	"Alles"
};

static const int MAXREPORTS=sizeof(reports)/sizeof(reports[0]);


static char *message_levels[]={
	"Wichtig",
	"Debug",
	"Fehler",
	"Warnungen",
	"Infos"
};

static const int ML_MAX=sizeof(message_levels)/sizeof(message_levels[0]);

t_liste *options=NULL;
t_config *config=NULL;

static const char *Directions[]={
	"EAST",
	"WEST",
	"PAUSE",
	"NORTHEAST",
	"NORTHWEST",
	"SOUTHEAST",
	"SOUTHWEST"
};

t_direction *directions=NULL;
t_faction *factions=NULL;


t_liste *Races=NULL;

static char *Errors[MAX_ERRORS] = {
	"0PERSONSLEARN",
	"0PERSONSTEACH",
	"1ATTACKPERUNIT",
	"1CARRYPERUNIT",
	"1PERSONPERMAGEUNIT",
	"ACTIVATED",
	"ALREADYUSEDINLINE",
	"AND",
	"ASSUMING200STUDYCOSTS",
	"AWARNING",
	"BEFORE",
	"BEFOREINCOME",
	"BEHIND",
	"BREEDHORSESORHERBS",
	"BUILDINGNEEDSSILVER",
	"BUTDOESNTLEARN",
	"BUYALLNOTPOSSIBLE",
	"CANTATTACKTEMP",
	"CANTDESCRIBEOBJECT",
	"CANTENTEROBJECT",
	"CANTFINDUNIT",
	"CANTHANDLEPERSONCOMMENT",
	"CANTHANDLESILVERCOMMENT",
	"CANTMAINTAINBUILDING",
	"CANTMAKETHAT",
	"CANTREADFILE",
	"CANTRENAMEOBJECT",
	"CHECKYOURORDERS",
	"COMBATSPELLSET",
	"DELIVERYTO",
	"DIRECTION",
	"DISCOVERED",
	"DOESNTCARRY",
	"DOESNTRIDE",
	"ECHECK",
	"ENDWITHOUTTEMP",
	"ERRORCOORDINATES",
	"ERRORHELP",
	"ERRORLEVELPARAMETERS",
	"ERRORNAMEFOREIGN",
	"ERROROPINION",
	"ERRORREGION",
	"ERRORREGIONPARAMETER",
	"ERRORSPELLSYNTAX",
	"ERRORSURVEY",
	"FACTION0USED",
	"FACTIONINVALID",
	"FACTIONMISSING",
	"FACTIONS",
	"FOLLOW",
	"FOUNDERROR",
	"FOUNDERRORS",
	"FOUNDORDERS",
	"GIVEWHAT",
	"HINTNOVERSION",
	"INTERNALCHECK",
	"INVALIDEMAIL",
	"ISCOMBATSPELL",
	"ISUSEDIN2REGIONS",
	"ISNOTCOMBATSPELL",
	"ITEM",
	"LINETOOLONG",
	"LONGORDERMISSING",
	"MAGIC",
	"MISSINGFILES",
	"MISSFILEPARAM",
	"MISSFILECMD",
	"MISSFILEITEM",
	"MISSFILESKILL",
	"MISSFILEDIR",
	"MISSFILEMSG",
	"MISSINGQUOTES",
	"MISSINGDISGUISEPARAMETERS",
	"MISSINGEND",
	"MISSINGFACTIONNUMBER",
	"MISSINGNEXT",
	"MISSINGNUMRECRUITS",
	"MISSINGOFFER",
	"MISSINGPARAMETERS",
	"MISSINGPASSWORD",
	"MISSINGUNITNUMBER",
	"MOVENOTPOSSIBLEWITHPAUSE",
	"MSGTO",
	"NAMECONTAINSBRACKETS",
	"NEEDBOTHCOORDINATES",
	"NOCARRIER",
	"NOFIND",
	"NOLUXURY",
	"NORMALUNITSONLY",
	"NOSEND",
	"NOTEMPNUMBER",
	"NOTEXECUTED",
	"NOTEXT",
	"NOTFOUND",
	"NTOOBIG",
	"NUMBER0SENSELESS",
	"NUMBERNOTPOSSIBLE",
	"NUMCASTLEMISSING",
	"NUMLUXURIESMISSING",
	"NUMMISSING",
	"OBJECTNUMBERMISSING",
	"ONLYSABOTAGESHIP",
	"ORDERNUMBER",
	"ORDERSOK",
	"ORDERSREAD",
	"PASSWORDCLEARED",
	"PASSWORDMSG1",
	"PASSWORDMSG2",
	"PASSWORDMSG3",
	"POST",
	"PRE",
	"PROCESSINGFILE",
	"QUITMSG",
	"RECRUITCOSTSSET",
	"REGIONMISSSILVER",
	"RESEARCHHERBSONLY",
	"RESERVE0SENSELESS",
	"RESERVEDTOOMUCH",
	"RESERVEWHAT",
	"RESTARTMSG",
	"RIDESWRONGUNIT",
	"ROUTENOTCYCLIC",
	"ROUTESTARTSWITHPAUSE",
	"SCHOOLCHOSEN",
	"SEARCHPATHIS",
	"SILVERPOOL",
	"SORT",
	"SUPPLYISOBSOLETE",
	"TEACHED",
	"TEACHWHO",
	"TEMPHASNTPERSONS",
	"TEMPNOTTEMP",
	"TEMPUNITSCANTRESERVE",
	"TEXTTOOLONG",
	"THERE",
	"UNIT0NOTPOSSIBLE",
	"UNIT0USED",
	"UNITALREADYHAS",
	"UNITALREADYHASLONGORDERS",
	"UNITALREADYHASMOVED",
	"UNITALREADYHASORDERS",
	"UNITCANSTILLTEACH",
	"UNITHASNTPERSONS",
	"UNITHASPERSONS",
	"UNITHASSILVER",
	"UNITISTEACHED",
	"UNITBADTEACH",
	"UNITGIVETOOMUCH",
	"UNITALREADYGAVE",
	"UNITATTACKBADSTATUS",
	"UNITLOSESITEMS",
	"UNITMISSCONTROL",
	"UNITMISSING",
	"UNITMISSPERSON",
	"UNITMISSSILVER",
	"UNITMOVESSHIP",
	"UNITMOVESTOOFAR",
	"UNITMUSTBEONSHIP",
	"UNITNEEDSTEACHERS",
	"UNITTEACHTOOMUCH",
	"UNITNOTONSHIPBUTONSHIP",
	"UNITNOTPOSSIBLEHERE",
	"UNITONSHIPHASMOVED",
	"UNITS",
	"UNRECOGNIZEDDIRECTION",
	"UNRECOGNIZEDOBJECT",
	"UNRECOGNIZEDOPTION",
	"UNRECOGNIZEDORDER",
	"UNRECOGNIZEDPOTION",
	"UNRECOGNIZEDRACE",
	"UNRECOGNIZEDREPORTOPTION",
	"UNRECOGNIZEDSCHOOL",
	"UNRECOGNIZEDSKILL",
	"UNRECOGNIZEDSPELL",
	"USED1",
	"USEEMAIL",
	"USINGUNITINSTEAD",
	"WARNINGS",
	"WARNINGLEVEL",
	"WAS",
	"WERE",
	"WRONGFACTIONNUMBER",
	"WRONGFIGHTSTATE",
	"WRONGNUMBER",
	"WRONGOUTPUTLEVEL",
	"WRONGPARAMETER",
	"CANTCHANGELOCALE",
	"MAINTAINANCEMOVED",
	"NOTRADENOCASTLE",
	"REGIONBUYTOOMUCH",
	"REGIONSELLTOOMUCH",
	"REGIONENTERTAINTOOMUCH",
	"REGIONRECRUITSTOOMUCH",
	"REGIONPEASANTSEATEN",
	"REGIONTAXTOOMUCH",
	"REGIONSPENDTOOMUCH",
	"UNITALREADYTEACH",
	"UNITGUARDSTEALTH",
	"UNITCANNOTGUARD",
	"UNITCANNOTGUARDNOWEAPON",
	"UNITCANNOTGUARDNOWEAPONEDSKILL",
	"UNITCANNOTGUARDFLEE",
	"UNITNOTAXSKILL",
	"UNITUNKNOWNTAXSKILL",
	"UNITNOTAXGUARDED",
	"UNITNOWEAPONEDTAXSKILL",
	"UNITNOTAXNOWEAPON",
	"UNITNOTAXNOTENOUGHWEAPON",
	"UNITNOENTERTAINSKILL",
	"UNITUNKNOWNENTERTAINSKILL",
	"UNITNOENTERTAINONSHIP",
	"UNITITEMNOTBUYABLE",
	"UNITMAYNOTEARN",
	"UNITNOSKILLLEVEL",
	"UNITUNKNOWNSKILLLEVEL",
	"UNITBUYNOTENOUGHTRADE",
	"UNITITEMNOTSELLABLE",
	"UNITSELLNOTENOUGHTRADE",
	"UNITMAYNOTTRADE",
	"UNITWILLNOTTRADE",
	"UNITNOTRADEONSHIP",
	"UNITNOWORKONSHIP",
	"UNITMAYNOTRECRUIT",
	"UNITLEARNERNOTFOUND",
	"UNITLEARNERNOTONREGION",
	"UNKNOWNITEM",
	"UNKNOWNBUILDING",
	"BADBUILDINGTYPE",
	"UNITATTACKNOTENOUGHWEAPON",
	"UNITATTACKSTARVING",
	"UNITCANNOTSELLSOMUCH",
	"UNITALREADYENTERED",
	"UNITENTERSTEALTH",
	"CANBUILDMORE",
	"CANNOTBUILDSOMUCH",
	"BUILDLACK",
	"BUILDSKILL",
	"SHIPNOTCAPTAIN",
	"BUILDINGNOOWNER",
	"CANUSEBUILDERFOROTHER",
	"BUILDINGEXIST",
	"UNKNOWNSHIP",
	"UNITNOTIN",
	"UNITRECEIVEDCONTROLBUT",
	"SHIPSEVERALCAPTAINS",
	"SHIPFULLYBUILT",
	"UNITCAPTAINSAILING",
	"SHIPSAILSKILL",
	"SHIPCANNOTGO",
	"SHIPNOTBUILT",
	"CANNOTMAKEHERB",
	"CANNOTRESEARCHHERB",
	"SHIPDAMAGED",
	"SHIPMOVESTOOFAR",
	"SHIPMEETLAND",
	"ENTERWHAT",
	"RECRUITMUSTLEARN",
	"UNKNOWNUNIT",
	"CANNOTGIVECONTROL",
	"SHIPNOCAPTAIN",
	"UNITMAKEITEM",
	"UNITMAKENOITEM",
	"MAKEITEMNOSKILL",
	"CANNOTMAKEITEM",
	"SHIPENEMYCAPTAIN",
	"CONTROLMAYBELOST",
	"UNITMUSTLEAVE",
	"MUSTGIVECONTROL",
	"GAVEBADCONTROL",
	"UNITSOMEWHEREELSE",
	"TOOMUCHMEN",
	"NOLONGORDER",
	"CATNOPLATEMAIL",
	"ELVENBOWERR",
	"POTIONSKILL",
	"POTIONHERBS",
	"UNITLEAVEENTER",
	"UNITMOVESOCEAN",
	"UNITMOVESTARVING",
	"UNITBADCOMBATSTATUS",
	"UNITONSHIPWAR",
	"MAKERESOURCENOSKILL",
	"MAKESOONRESOURCENOSKILL",
	"LEARNINGNEEDSSILVER",
	"CARRIERDOESNTMOVE",
	"UNITTOOMANYHORSES",
	"UNITTOOMANYCARTS",
	"UNITCANNOTSWIM",
	"UNITNOHERBTOGIVE",
	"UNITNOTHINGTOGIVE",
	"UNITLOSESRECEIVED",
	"TOOHEAVILYLOADED",
	"MOVETOOHEAVILYLOADED",
	"SHIPTOOHEAVILYLOADED",
	"MONEYPROMOTION",
	"CANNOTMAKEROAD",
	"MAKEROADNEED",
	"ROADCOMPLETED",
	"UNITGIVEUNITTOOMUCH",
	"UNITGIVEUNITRACE",
	"UNITGIVEUNITTEMP",
	"NULLPERSONSSAIL",
	"UNITUNUSEDHERB",
	"INSECTCANNOTENTERGLACIER",
	"SHIPCANNOTENTERGLACIER",
	"RESOURCELEVELINC",
	"NOTENOUGHTOCAST",
	"NOTINMAGICSCHOOL",
	"PEASANTBLOODFORDEMON",
	"UNITCANBUYMORE",
	"UNITBUYNOTENOUGHMONEY",
	"UNITUSETOOMUCH",
	"USEWHAT",
	"CANOVERLOAD",
	"UNITCASTNEEDITEM",
	"CRMSG_DONATESSILVER",
	"CRMSG_GIVE",
	"CRMSG_EARN",
	"CRMSG_WALK",
	"CRMSG_RIDE",
	"CRMSG_TRANSPORT",
	"CRMSG_STUDY",
	"CRMSG_TEACH",
	"CRMSG_TAUGHT",
	"CRMSG_RESEARCH",
	"CRMSG_MAKEHERB",
	"CRMSG_BUILD",
	"CRMSG_SAIL",
	"CRMSG_PRODUCE",
	"CRMSG_PAY",
	"CRMSG_PAYBUY",
	"CRMSG_BUY",
	"CRMSG_SELL",
	"CRMSG_PROMOTE",
	"CRMSG_USES",
	"CRMSGOF_PRODUCE",
	"CRMSGOF_EARN"
};

char *s_errorMsgs[MAX_ERRORS];

enum {
	HELP_CAPTION,
	HELP_PATH,
	HELP_TEXT,
	MAX_HELP
};

static char *Help[MAX_HELP] = {
	"CAPTION",
	"PATH",
	"TEXT"
};

t_liste *help=NULL;
char *help_caption=NULL;
char *help_path=NULL;

t_ship *shipdata=NULL;
t_building *buildingdata=NULL;
t_item *itemdata=NULL;
t_skills *skilldata=NULL;
t_potion *potiondata=NULL;

t_liste *herbnames=NULL;

// --------------------------------------------------------------------- 

CSpells sSpellsList;
CUnits sUnitsList;
CFactions sFactionsList;
CFaction *	sCurrentFaction = NULL;

CUnit *order_unit = NULL;		// L'unité qui est droite
CUnit *mother_unit = NULL;		// L'unité qui fait MAKE TEMP

CRegions sRegionsList;
CMessages sMessagesList;

tnode tokens[UT_MAX];
static bool	s_bOrderInErrors = true;
static bool s_bOutputsWithRegion = true;
static CString sOutputRegionName("");

CUnit *newunit(CFaction *pFaction, int n, int t);
int findstr(char **v, const char *s, int max);

void outputSetRegionName(const char * pzRegionName)
{
	sOutputRegionName = pzRegionName;
}

const char * _asError(const char *pzMsg)
{
	if (!s_bOutputsWithRegion) return pzMsg;
	static CString sz("");
	//todo: check if coords in name for indent
	CString sName;
	sName.Format("%15.15s",(LPCSTR)sOutputRegionName);
	//sz=sErrorRegionName+" - "+pzMsg;
	sz=sName+" - "+pzMsg;
	const char *pp = (LPCSTR)sz;;
	return (LPCSTR)sz;
}

const char * _valistToString(const char * pzFormat, va_list argList)
{
	static CString sOutput("");
	sOutput.FormatV(pzFormat, argList);
	return _strdup(sOutput);
}


const char * _msg(int nErrorNum,...)
{
	const char * msgcontent = NULL;
	va_list argList;
	va_start(argList, nErrorNum);
	msgcontent = _valistToString(s_errorMsgs[nErrorNum],argList);
	va_end(argList);
	return msgcontent;
}


const char * _formatFromCompileOption()
{
	char sOrderDelim[5]="'%s'";
	if (!s_bOrderInErrors) strcpy_s(sOrderDelim,5,"");
	static char sf[64];
	strcpy_s(sf,64,"");
	if (OUT_NORMAL == compile) strcpy_s(sf,64,"%s%s: %s"); else
	if (OUT_COMPILE == compile) strcpy_s(sf,64,"%s|%s|%s"); else
	if (OUT_MAGELLAN == compile) strcpy_s(sf,64,"%s|%s|%s");
	if (_strcmpi(sf,""))
	{
		strcat_s(sf,sOrderDelim);
		strcat_s(sf,"\n");
	}
	return sf;
}

char * wrap(const char *s) {
	int i, j, k, m, o=0;

	m=MARGIN;
	strcpy_s(message_buf,UTF8_to_ansi(s));

	// i counts the length of the stringer s starting from j counts the length of the line. 
	// If the edge is reached, k is set on i, and searched backwards a blank character, 
	// which one could replace with one ' \n '.

	for (i=0, j=25; s[i]; i++, j++) {	// j=25: "Warning to the line xyz:"
		if (j==m) {
			for (k=i; !my_isspace(s[k]) && k>0; k--);
				//if one finds a blank character, she is replaced through ' \n ':
			if (k>0) {
				message_buf[k+o]='\n';
				o++;
				memmove(message_buf+k+o+1, message_buf+k+o, strlen(message_buf));
					// an indication shift further thereby TAB there clean-fit
				message_buf[k+o]='\t';
				j=i-k+8;
				if (m==MARGIN) m-=8;
			}
		} else
			//.If one found last times no blank character, then one reacts simply to the next blank character, 
			// without after forward searching (one has that already once made)

			if (j>m && my_isspace(s[i])) { {
				message_buf[i+o]='\n';
				o++;
				memmove(message_buf+i+o+1, message_buf+i+o, strlen(message_buf));
					// an indication shift further thereby TAB there clean-fit
				message_buf[i+o]='\t';
			}
			j=8;
			if (m==MARGIN) m-=8;
		}
	}
	return message_buf;
}

const char * iToString(int n) {
	static char is[10];
	sprintf_s(is, "%d", n);
	return is;
}

void outputError(const char *order, int nLevel, const char * txt)
{
	const char * pzTxt = txt;
	char pzFilename[64];
	const char * pzFormat = _formatFromCompileOption();
	char orderBuffer[255];
	strcpy_s(orderBuffer,55,"");
	//strcpy(orderBuffer,"<--MISSING!-->");
	if (order) 
		strncpy_s(orderBuffer,255,order,255);
	if (order && strlen(order)>=255)
		strcpy_s(orderBuffer+251,255,"...");		// to long instructions if necessary shorten
	if (sCurrentFile) strcpy_s(pzFilename,64,sCurrentFile->filename()); else strcpy_s(pzFilename,64,"");

	if (OUT_NORMAL==compile) strcpy_s(pzFilename,64,"");
	output(pzFormat,pzFilename,nLevel==-1?"":iToString(nLevel),pzTxt,orderBuffer);
}


void addError(const char *order, int nLevel, int nErrorID, const char * txt)
{

	const char * pzTxt = txt;
//	char pzFilename[64];
//	const char * pzFormat = _formatFromCompileOption();
	char orderBuffer[255];
	strcpy_s(orderBuffer,255,"");
	//strcpy(orderBuffer,"<--MISSING!-->");
	if (order) 
		strncpy_s(orderBuffer,255,order,255);
	if (order && strlen(order)>=255)
		strcpy_s(orderBuffer+251,255,"...");		// to long instructions if necessary shorten

//	if (sCurrentFile) strcpy(pzFilename,sCurrentFile->filename()); else strcpy(pzFilename,"");

//	if (OUT_NORMAL==compile) strcpy(pzFilename,"");
	sErrorsList.AddTail(new CCRError(nLevel,nErrorID,pzTxt,orderBuffer));
	//output(pzFormat,pzFilename,nLevel==-1?"":iToString(nLevel),pzTxt,orderBuffer);
}

int Pow(int p) {
	if (!p)
		return 1;
	else
		return 2<<(p-1);
}

// checked_buf is only for error message purpose
//#define scat(X) //strcat_s(checked_buf, 8192,X)
//#define Scat(X) scat(" ");scat(X)

/*
void qcat(const char *s) {
	const char *x;

	x=strchr(s, ' ');
	if (x != NULL && does_default==0)
		scat(" \"");
	else
		scat(" ");
	scat(s);
	if (x != NULL && does_default==0)
	{scat("\"");}
}
*/

/*
void icat(int n) {
	static char s[10];
	sprintf_s(s, 10," %d", n);
	scat(s);
}
*/

/*
void bcat(int n) {
	static char s[10];
	sprintf_s(s, 10, " %s", itob(n));
	scat(s);
}
*/

int ItemPrice(int i) {
	static int ino=0;
	static t_item *item=NULL, *it=NULL;
	int x;

	if (i<0)
		return 0;
	if (!it) item=it=itemdata;
	if (i!=ino) {
		x=i;
		if (i<ino) it=itemdata;
		else i-=ino;
		ino=x;
		while (0<i-- && it) it=it->m_pNext;
		if (!it)
			return 0; // unknown item		
		item=it;
	}
	return item->price;
}


int ItemWeight(int i) {
	static int ino=0;
	static t_item *item=NULL, *it=NULL;
	int x;

	if (i<0)
		return 0;
	if (!it) item=it=itemdata;
	if (i!=ino) {
		x=i;
		if (i<ino) it=itemdata;
		else i-=ino;
		ino=x;
		while (0<i-- && it) it=it->m_pNext;
		if (!it) {
			output("Internal error: ItemWeight %d not found!\n",i);
			onExitProg(123);
		}
		item=it;
	}
	return item->weight();
}

int ItemMinSkill(int i) {
	static int ino=0;
	static t_item *item=NULL, *it=NULL;
	int x;

	if (i<0)
		return 0;
	if (!it) item=it=itemdata;
	if (i!=ino) {
		x=i;
		if (i<ino) it=itemdata;
		else i-=ino;
		ino=x;
		while (0<i-- && it) it=it->m_pNext;
		if (!it) {
			output("Internal error: ItemMinSkill %d not found!\n",i);
			onExitProg(123);
		}
		item=it;
	}
	return item->minskill;
}

t_item * itemFromUTID(eUTID i) {
	static int ino=0;
	static t_item *item=NULL, *it=NULL;
	int x;

	if (i<0)
		return 0;
	if (!it) item=it=itemdata;
	if (i!=ino) {
		x=i;
		if (i<ino) it=itemdata;
		else i-=ino;
		ino=x;
		while (0<i-- && it) it=it->m_pNext;
		if (!it) {
			output("Internal error: Item id[%d] not found!\n",i);
			return NULL;
		}
		item=it;
	}
	return item;
}

t_building * buildingFromUTID(eUTID i) {
	static int ino=0;
	static t_building *item=NULL, *it=NULL;
	int x;

	if (i<0)
		return 0;
	if (!it) item=it=buildingdata;
	if (i!=ino) {
		x=i;
		if (i<ino) it=buildingdata;
		else i-=ino;
		ino=x;
		while (0<i-- && it) it=it->m_pNext;
		if (!it) {
			output("Internal error: Building id[%d] not found!\n",i);
			return NULL;
		}
		item=it;
	}
	return item;
}

t_ship * shipFromUTID(eUTID i) {
	static int ino=0;
	static t_ship *item=NULL, *it=NULL;
	int x;

	if (i<0)
		return 0;
	if (!it) item=it=shipdata;
	if (i!=ino) {
		x=i;
		if (i<ino) it=shipdata;
		else i-=ino;
		ino=x;
		while (0<i-- && it) it=it->m_pNext;
		if (!it) {
			output("Internal error: ship id[%d] not found!\n",i);
			return NULL;
		}
		item=it;
	}
	return item;
}

void addTranslations() {
	t_translation *tt = translations;
	t_translation *tn=NULL;
	while (tt) { 
		tn = tt->m_pNext; 
		cr().addTranslation(tt->name_en,tt->name_de);
		tt = tn;
	}
}

t_potion * potionFromUTID(eUTID i) {
	static int ino=0;
	static t_potion *item=NULL, *it=NULL;
	int x;

	if (i<0)
		return 0;
	if (!it) item=it=potiondata;
	if (i!=ino) {
		x=i;
		if (i<ino) it=potiondata;
		else i-=ino;
		ino=max(x,0);
		while (0<i-- && it) it=it->m_pNext;
		if (!it) {
			output("Internal error: potion id[%d] not found!\n",i);
			return NULL;
		}
		item=it;
	}
	return item;
}

char * ItemName(int i) {
	static int ino=0;
	static t_item *item=NULL, *it=NULL;
	int x;

	if (!it) item=it=itemdata;
	if (i!=ino) {
		x=i;
		if (i<ino) it=itemdata;
		else i-=ino;
		// C.G. 12/12/08 - if an item is not found ino becomes -1 and after there is a pbm to retrieve names
		ino=max(x,0); // ino=x; //this is bad
		while (0<i-- && it) it=it->m_pNext;
		if (!it) {
			output("Internal error: ItemName id[%d] not found!\n",i);
			return "";
		}
		item=it;
	}
	return item->name;
}


char * PotionName(int i) {
	static int ino=0;
	static t_potion *item=NULL, *it=NULL;
	int x;

	if (!it) item=it=potiondata;
	if (i!=ino) {
		x=i;
		if (i<ino) it=potiondata;
		else i-=ino;
		ino=max(x,0);
		while (0<i-- && it) it=it->m_pNext;
		if (!it) {
			output("Internal error: PotionName id[%d] not found!\n",i);
			return "";
		}
		item=it;
	}
	return item->name;
}

char * findName(int i, int nType)
{
	if (nType==UT_ITEM) return ItemName(i);
	if (nType==UT_HERB) return herbname(i);
	if (nType==UT_POTION) return PotionName(i);
	return "";
}

FILE * path_fopen(const char *path_par, const char *file, const char *mode) {
	FILE *fl;
	char *pathw=_strdup(path_par);
	char *token;
	char *nexttoken;

	char buf[1024];

	token=strtok_s(pathw, PATH_DELIM,&nexttoken);
	while(token) {
		sprintf_s(buf, 1024, "%s\\%s\\%s", token, echeck_locale, file);
		//sprintf_s(buf, 1024, "%s/%s/%s", token, echeck_locale, file);
		 int errnum=fopen_s(&fl, buf, mode);
		if (errnum==0) {
			free(pathw);
			return fl;
		}
		token=strtok_s(NULL, PATH_DELIM,&nexttoken);
	}
	free(pathw);
	return NULL;
}

void parse_line(char *s, const char *seps, char **pValues)
{
	int i = 0;
	if (s==NULL) return;
	char *nexttoken;
	pValues[0] = strtok_s(s,seps,&nexttoken);
	while (pValues[i] && i < MAXPARMS-1)
		pValues[++i] = strtok_s(NULL,seps,&nexttoken);
}

int myatoi(const char *p, int nDefValue)
{
	if (!p) return nDefValue;
	int n=atoi(p);
	if (n==-1) n = nDefValue;
	return n;
}

double myatof(const char *p, double fDefValue)
{
	if (!p) return fDefValue;
	double f=atof(p);
	if (f==-1.0) f = fDefValue;
	return f;
}

CString maxChars(const CString &s, int maxLen)
{
	CString sr(s);
	if (sr.GetLength()>maxLen)
		sr.Truncate(maxLen);
	return sr;
}

/*CString leftExtent(const CString &s, int len)
{
	CString sr(s);
	if (sr.GetLength()>maxLen)
		sr.Truncate(maxLen);
	return sr;
}*/

void readconfig(char *s)
{
	char *x;
	t_config *cf;

	cf=(t_config *)cmalloc(sizeof(t_config));
	x=strchr(s,';');
	if (!x) x=strchr(s,',');
	if (x) *x=0;
	else {
		x=strchr(s,'\n');
		if (x) *x=0;
		x=NULL;
	}
	cf->name=_strdup(s);
	if (x) {
		s=(char *)(x+1);
		while (my_isspace(*s)) s++;
		if (*s)
			cf->value=atoi(s); //C.G. todo
	}
	cf->m_pNext=config;
	config=cf;
}

void readskill(char *s) {		// parsed einen String après Skill
	char *x;
	t_skills *sk;

	sk=(t_skills *)cmalloc(sizeof(t_skills));
	x=strchr(s,';');
	if (!x) x=strchr(s,',');
	if (x) *x=0;
	else {
		x=strchr(s,'\n');
		if (x) *x=0;
		x=NULL;
	}
	sk->name=_strdup(s);
	if (x) {
		s=(char *)(x+1);
		while (my_isspace(*s)) s++;
		if (*s)
			sk->kosten=atoi(s);
	}
	sk->m_pNext=skilldata;
	skilldata=sk;
}

int readitem(char *s) {
	t_item *it;

	char *params[MAXPARMS];
	char *parmsCost[MAXPARMS];

	for (int i=0;i<MAXPARMS;i++)
	{
		params[i] = NULL;
		parmsCost[i] = NULL;
	}

	it=(t_item *)cmalloc(sizeof(t_item));
	parse_line(s,";",params);
	it->name = _strdup(params[0]);
	it->price = myatoi(params[1]);
	it->parseWeight(params[2]);
	it->minskill = myatoi(params[3],1);

	if (params[4]) 
		parse_line(params[4],",",parmsCost);
	it->cost.silver  = 0;
	it->cost.woods = myatoi(parmsCost[0]);
	it->cost.stones = myatoi(parmsCost[1]);
	it->cost.iron = myatoi(parmsCost[2]);
	it->cost.mallorn = myatoi(parmsCost[3]);
	it->cost.laen = myatoi(parmsCost[4]);
	it->cost.adamantium = myatoi(parmsCost[5]);

	it->m_pNext=itemdata;
	itemdata = it;
	return 1;
}


void readliste(char *s, t_liste **L) {		// parsed einen String nach einem Token
	char *x;
	t_liste *ls;

	ls=(t_liste*)cmalloc(sizeof(t_liste));
	x=strchr(s,'\n');
	if (x) *x=0;
	ls->name=_strdup(s);
	addlist(L,ls);
}

int readtranslation(char *s) {
	char *x;
	char *e;
	t_translation *k;

	k=(t_translation*)cmalloc(sizeof(t_translation));
	x=strchr(s,';');
	if (x) *x=0; else return 0;
	CString s1(s);
	s1.Remove('"');

	x=(char*)(x+1);
	while (my_isspace(*x)) x++;
	if (!x) return 0;
	e=strchr(x,'\n');
	if (e) *e=0;
	CString s2(x);
	k->name_en=_strdup((LPCSTR)s1);
	k->name_de=_strdup((LPCSTR)s2);
	k->m_pNext=translations;
	translations=k;
	return 1;
}



int readparams(char *s) {		// parsed einen String nach Parametern
	char *x;
	t_params *p;
	int i;

	p=(t_params*)cmalloc(sizeof(t_params));
	x=strchr(s,';');
	if (!x) x=strchr(s,',');
	if (x) *x=0;
	else return 0;
	for (i=0; i<MAXPARAMS; i++)
		if (strcasecmp(s, Params[i])==0)
			break;
	if (i==MAXPARAMS)
		return 0;
	s=(char*)(x+1);
	while (my_isspace(*s)) s++;
	if (!s)
		return 0;
	x=strchr(s,'\n');
	if (x) *x=0;
	p->name=_strdup(s);
	p->param=i;
	p->m_pNext=parameters;
	parameters=p;
	return 1;
}


int readship(char *s) {
	t_ship *sh;
	char *params[MAXPARMS];
	
	for (int i=0;i<MAXPARMS;i++)
		params[i] = NULL;

	sh=(t_ship *)cmalloc(sizeof(t_ship));
	parse_line(s,",; ",params);
	sh->name = _strdup(params[0]);
	sh->moverange = myatoi(params[1]);
	sh->capacity = myatoi(params[2]);
	sh->captainSkill = myatoi(params[3]);
	sh->totalSkill = myatoi(params[4]);
	sh->size = myatoi(params[5]);
	sh->shipcraftSkill = myatoi(params[6]);
	sh->m_pNext = shipdata;
	shipdata = sh;
	return 1;
}

// check and set <command>,<translation> line (for instance: WORK, Work)
int readkeyword(const char *s) 
{
	CString sLine(s);
	int nSep = sLine.Find(',');
	if (nSep==-1) return 0; // no ',' in line
	if (nSep==sLine.GetLength()-1) return 0; // nothing after ,
	CString sk = sLine.Left(nSep).Trim();
	int i=COrder::findKeyword(sk);
	if (i==-1) return 0; // unknown keyword
	CString st = sLine.Mid(nSep+1).Trim();
	if (st.IsEmpty()) return 0; // only ignored characters (' '...) after ','
	t_keyword *k=(t_keyword*)cmalloc(sizeof(t_keyword));
	k->name=_strdup(st);
	k->keyword=i;
	k->m_pNext=keywords;
	keywords=k;
	return 1;
}

int readbuilding(char *s) {
	t_building *bu;
	char *params[MAXPARMS];
	char *parmsUpkeep[MAXPARMS];
	char *parmsCost[MAXPARMS];
	
	for (int i=0;i<MAXPARMS;i++)
	{
		params[i] = NULL;
		parmsUpkeep[i] = NULL;
		parmsCost[i] = NULL;
	}

	bu=(t_building *)cmalloc(sizeof(t_building));
	parse_line(s,";",params);
	bu->name = _strdup(params[0]);

	bu->masonryMinSkill = myatoi(params[3]);
	bu->maxsize = myatoi(params[4]);
	bu->capacity = myatoi(params[5]);

	parse_line(params[1],",",parmsUpkeep);
	bu->upkeep.silver = myatoi(parmsUpkeep[0]);
	bu->upkeep.woods = myatoi(parmsUpkeep[1]);
	bu->upkeep.stones = myatoi(parmsUpkeep[2]);
	bu->upkeep.horses = myatoi(parmsUpkeep[3]);
	
	parse_line(params[2],",",parmsCost);
	bu->cost.silver  = myatoi(parmsCost[0]);
	bu->cost.woods = myatoi(parmsCost[1]);
	bu->cost.stones = myatoi(parmsCost[2]);
	bu->cost.iron = myatoi(parmsCost[3]);
	bu->cost.mallorn = myatoi(parmsCost[4]);
	bu->cost.laen = myatoi(parmsCost[5]);
	bu->cost.adamantium = myatoi(parmsCost[6]);


	bu->m_pNext = buildingdata;
	buildingdata = bu;
	return 1;
}

int readpotion(char *s)
{
	t_potion *p;
	char *params[MAXPARMS];
	char *parmsHerbs[MAXPARMS];
	int nIndexesHerbs[MAXPARMS];
	int nNbHerbs=0;

	initHerbsToken(); // because use findherb
	initItemsToken(); // because use finditem

	for (int j=0;j<MAXPARMS;j++)
	{
		params[j] = NULL;
		parmsHerbs[j] = NULL;
		nIndexesHerbs[j] = ID_UNKNOWN;
	}

	p=(t_potion *)cmalloc(sizeof(t_potion));
	parse_line(s,";",params);
	p->name = _strdup(params[0]);
	p->nskill = myatoi(params[1]);

	parse_line(params[2],",",parmsHerbs);
	for (int i=0;i<MAXPARMS;i++)
	{
		if (parmsHerbs[i]!= NULL)
		{
			CString sh(parmsHerbs[i]);
			sh.TrimLeft();
			sh.TrimRight();
			int ni = findherb(sh);
			if (ni==ID_UNKNOWN) {
				ni = finditem(sh);
				if (ni==ID_UNKNOWN)
					errormsg("readpotion::%s: unknown '%s' item",p->name,parmsHerbs[i]);
				continue;
				ni = -ni; //negative value means 'item' (not herb)

			}
			nIndexesHerbs[nNbHerbs] = ni;
			nNbHerbs++;
		}
	}

	p->nbherbs = nNbHerbs;

	if (nNbHerbs > 0)
	{
		p->herb_utids=(int*)cmalloc(sizeof(int)*nNbHerbs);
		for (int k=0;k<nNbHerbs;k++)
			p->herb_utids[k]=nIndexesHerbs[k]; 
	}

	p->m_pNext = potiondata;
	potiondata = p;

	return 1;
}

int readfaction(char *s) {		// parsed einen String nach Richtungen
	char *params[MAXPARMS];
	t_faction *d=(t_faction*)cmalloc(sizeof(t_faction));

	parse_line(s,";, ",params);
	d->id = _strdup(params[0]);
	d->pswd = _strdup(params[1]);
	d->race = _strdup(params[2]);
	d->kerubinStatus = atoi(params[3]);
	d->nOutConfirm = (params[4]?atoi(params[4]):0);
	d->m_pNext = factions;
	factions = d;
	return 1;
}

int readdirection(char *s) {		// parsed einen String nach Richtungen
	char *x;
	t_direction *d;
	int i;

	d=(t_direction*)cmalloc(sizeof(t_direction));
	x=strchr(s,';');
	if (!x) x=strchr(s,',');
	if (x) *x=0;
	else return 0;
	for (i=0; i<MAXDIRECTIONS; i++)
		if (strcasecmp(s, Directions[i])==0)
			break;
	if (i==MAXDIRECTIONS)
		return 0;
	s=(char*)(x+1);
	while (my_isspace(*s)) s++;
	if (!s)
		return 0;
	x=strchr(s,'\n');
	if (x) *x=0;
	d->name=_strdup(s);
	d->dir=i;
	d->m_pNext=directions;
	directions=d;
	return 1;
}


int readerror(char *s) {
	int i;
	char *x;

	x=strchr(s,',');
	if (!x)
		return 0;
	*x=0;
	i=findstr(Errors, s, MAX_ERRORS);
	if (i<0)
		return 0;
	x++;
	while (my_isspace(*x)) x++;
	if (!(*x) || *x=='\n')
		return 0;
	s=x;
	while (*x=='~') {
		*x=' ';
		x++;
	}
	do {
		x=strchr(x,'\\');
		if (!x) break;
		*x='\n';	// nur  \n  erlaubt...
		x++;
		memmove(x, x+1, strlen(x));		// Rest ein Zeichen "ranziehen"
	} while (*x);
	if (s_errorMsgs[i])
		free (s_errorMsgs[i]);
	s_errorMsgs[i]=_strdup(s);
	filesread|=HAS_MESSAGES;
	return 1;
}


int readhelp(char *s) {
	int i;
	char *x;
	t_liste *h;

	x=strchr(s,',');
	if (!x)
		return 0;
	*x=0;
	x++;
	if (!(*x))
		return 0;
	x++;	// first blank behind that, overlook
	if (!(*x))
		return 0;
	i=findstr(Help, s, MAX_HELP);
	if (i<0)
		return 0;
	s=x;
	do {
		s=strchr(s,'\\');
		if (!s) break;
		*s='\n';	// nur  \n  erlaubt...
		s++;
		memmove(s, s+1, strlen(s));		// Remainder an indication "ranziehen"
	} while (*s);
	
	switch (i) {
		case HELP_CAPTION:
			help_caption=_strdup(x);
			break;
		case HELP_PATH:
			help_path=_strdup(x);
			break;
		default:
			h=(t_liste*)cmalloc(sizeof(t_liste));
			h->name=_strdup(x);
			addlist(&help,h);
			break;
	}
	return 1;
}


int parseLineFromFile(char *s, int typ) {		// suitable routine calls on not all Zeilenparser returns value
	int ok;							
	char *x, *y;
	int i;

	ok=1;
	switch (typ) {
		case UT_PARAM:
			ok=readparams(s);
			if (ok)
				filesread|=HAS_PARAM;
			break;

		case UT_ITEM:
			ok=readitem(s);
			if (ok)
				filesread|=HAS_ITEM;
			break;

		case UT_SKILL:
			readskill(s);
			filesread|=HAS_SKILL;
			break;

		case UT_KEYWORD:
			ok=readkeyword(s);
			if (ok)
				filesread|=HAS_KEYWORD;
			break;

		case UT_BUILDING:
			readbuilding(s);
			break;

		case UT_HERB:
			readliste(s, &herbnames);
			break;

		case UT_POTION:
			//readliste(s, &potionnames);
			readpotion(s);
			break;

		case UT_RACE:
			readliste(s, &Races);
			break;

		case UT_SPELL:
			sSpellsList.Add(s);
			break;

		case UT_SHIP:
			readship(s);
			break;

		case UT_OPTION:
			readliste(s, &options);
			break;

		case UT_CONFIG:
			//readconfig(s);
			break;

		case UT_DIRECTION:
			ok=readdirection(s);
			if (ok)
				filesread|=HAS_DIRECTION;
			break;

		case UT_ERRORS:
			ok=readerror(s);
			break;

		case UT_HELP:
			ok=readhelp(s);
			break;

		case UT_FACTION:
			ok=readfaction(s);
			break;

		case UT_TRANSLATION:
			ok=readtranslation(s);
			break;

		default:	// tokens.txt
			x=strchr(s, ':');		// TOKEN: rate
			if (!x)
				return 0;
			y=x+1;
			while (my_isspace(*(x-1))) x--;
			*x=0;
			for (i=1; i<UT_MAX; i++)
				if (strcmp(s, Keys[i])==0)
					break;
			if (i==UT_MAX)
				return 0;
			while (my_isspace(*y)) y++;
			if (*y && (*y=='#' || *y=='\n'))
				return 1;
			return parseLineFromFile(y, i);
	}
	return ok;
}

/*
void parseSettings()
{
	int i;
	t_liste *l;
	for (i=0,l=settings; l; l=l->m_pNext,++i)
	{
		if (findsetting(l->name,"error.region.name")=="1"
		addtoken(&tokens[UT_OPTION], l->name, i);
	}

}
*/

BOOL readafile(const char *fn, int typ) {
	int ok, line;
	FILE *F;
	char *s, *x;
	char lorder_buf[BUFSIZE];	// current order line

	F=path_fopen(path, fn, "r");
	if (!F)
	{
		ERTRACE("%s\\%s file not found",echeck_locale,fn);
		return FALSE;
	}

	for (line=1;;line++) {
		do {
			s=fgets(lorder_buf, MAXLINE, F);
		} while (!feof(F) && s && (*s=='#' || *s=='\n'));	// Leer- und Kommentarzeilen überlesen
		if (feof(F) || !s) {
			fclose(F);
			return FALSE;
		}
		x=strchr(s,'\n');
		if (x) *x=0;	// \n am Zeilenende löschen
		ok=parseLineFromFile(s, typ);
		if (!ok)
		{
			output("Error in file %s(%d): `%s'\n", fn, line, s);
			return FALSE;
		}
		// maybe todo: defaults english values when file not here (commands.txt...)
	}
	return TRUE;
}


void readfiles(int doall) {		// liest externen Files
	int i;

	CString sLangFolder("");
	sLangFolder.Format("%s\\%s",path,echeck_locale);
	if (!STExistLocalDir(sLangFolder))
		consoleError("Error: %s folder does not exist",(LPCSTR)STGetFullPathname(sLangFolder));

	if (doall)
		// alle Files aus der Liste der Reihe nach zu lesen versuchen
		for (i=0; i<filecount_EN; i++)
			readafile(ECheck_Files_EN[i].name, ECheck_Files_EN[i].type);
	else
		// nur die Help-Files und tokens.txt
		for (i=0; i<filecount_EN; i++)
			if (ECheck_Files_EN[i].type==UT_HELP || ECheck_Files_EN[i].type<0)
				readafile(ECheck_Files_EN[i].name, ECheck_Files_EN[i].type);
	
}


void porder() {
	int i;

	if (echo_it) {
		if (does_default != 2)
			for (i=0; i != indent; i++)
				putc(' ',fOUT);

		if (at_cmd)
			putc('@',fOUT);
		at_cmd=0;
		// for the moment no echo
		/*
		if (does_default>0)
			fprintf(fOUT,"%s%s",checked_buf, does_default==2 ? "\"\n" : "");
		else
		{
			fputs(checked_buf,fOUT);
			putc('\n',fOUT);
		}
		*/
	}
	//checked_buf[0]=0;
	if (next_indent != indent)
		indent=next_indent;
}

BOOL errvalist(int nLevel, int nErrorNum, const char *pzPrefixFormat, va_list argList) 
{
	if (nLevel < 0)
	{
		nLevel=-nLevel;
		if (CCRErrors::s_bBasicErrors==1)
			return FALSE;
	}
	const char *order=NULL;
	if (nErrorNum!=LONGORDERMISSING) 
		order = sCurrentFile->buffer();
	char bDo=!brief;
	BOOL bWarning=FALSE;
	if (nLevel > 0) { 
		if (warn_off) 
			return FALSE;	
		if (nLevel>show_warnings) 
			return FALSE;
		warning_count++; 
	}
	else 
		error_count++;
	if (bDo)
	{
		CString pzFormat(pzPrefixFormat);
		pzFormat+=(LPCSTR)s_errorMsgs[nErrorNum];
		addError(order, nLevel, nErrorNum, _asError(_valistToString(pzFormat,argList)));
	}
	return FALSE;
}

BOOL errorvalist(int nLevel, int nErrorNum, const char *order, va_list argList) 
{
	char bDo=!brief;
	BOOL bWarning=FALSE;
	if (nLevel < 0)
	{
		nLevel=-nLevel;
		if (CCRErrors::s_bBasicErrors==1)
			return FALSE;
	}
	if (nLevel > 0) 
	{ 
		if (warn_off) 
			return FALSE;	
		if (nLevel>show_warnings) 
			return FALSE;
		warning_count++; 
	}
	else 
		error_count++;
	if (bDo)
		addError(order, nLevel, nErrorNum, _asError(_valistToString(s_errorMsgs[nErrorNum],argList)));
	return FALSE;
}

BOOL region_error(int nLevel, int nErrorNum, va_list argList) 
{
	return errorvalist(nLevel, nErrorNum, NULL, argList);
}

const char * UnitName(int i) {
	CUnit *u=sUnitsList.Find(i,0);
	if (!u) u=sUnitsList.Find(i,1);
	if (!u) {
		Error(CANTFINDUNIT,itob(i));
		u=newunit(NULL,-1,0);
	}
	return u->outputName();
}

void checkstring(const char *s, size_t l, int type) {
	if (l>0 && strlen(s)>l)
		Warning(TEXTTOOLONG, 2, (int)(l));

	if (s[0]==0 && type==NECESSARY)
		Warning(NOTEXT,1);

	strncpy_s(warn_buf, 8192, s, l);

	// no echo for the moment
	/*
	if (echo_it && s[0]) {
		if (strlen(s) + strlen(checked_buf) > MARGIN) {
			qcat(wrap(s));
		} else
			qcat(s);
	}
	*/
}

int from_temp_unit_no;
	// The temp NO contains, if instructions are read by a temp unit.

CUnit * newunit(CFaction * pFaction, int n, int t) {

	CUnit *u=sUnitsList.Add(pFaction, n, t, Rx, Ry, sCurrentFile->buffer());
	if (u->TempStatus() < UNIT_STATUS_NOTTEMP)
		Error(ISUSEDIN2REGIONS, u->outputName(), Rx, Ry, u->x(), u->y());

	if (u->TempStatus() < UNIT_STATUS_TEMP)
		u->SetTempStatus(t);

	return u;
}


int atoip(const char *s) {
	int n;

	n=atoi(s);
	if (n<0)
		n=0;
	return n;
}

/*
const char * cgetstr(const char *s1) {
	int i;
	static const char *s;
	static char buf[BUFSIZE];

	if (s1)
		s=s1;
	while (*s==SPACE)
		s++;

	for (i=0; *s && *s!=SPACE && i<(int)sizeof(buf); i++, s++) {
		buf[i]=*s;

		if (*s==SPACE_REPLACEMENT) {
			if (i>0 && buf[i-1]==ESCAPE_CHAR)
				buf[--i]=SPACE_REPLACEMENT;
			else
				buf[i]=SPACE;
		}
	}
	buf[i]=0;
	if (i>0 && buf[i-1]==';')
			// A semicolon stands directly in the back to, this deletes
		buf[i-1]=0;

	return buf;
}
*/

const char * igetstr(const char *s1) {
	int i;
	static const char *s;
	static char buf[BUFSIZE];

	if (s1)
		s=s1;
	while (*s==SPACE)
		s++;

	for (i=0; *s && *s!=SPACE && i<(int)sizeof(buf); i++, s++) {
		buf[i]=*s;
		if (*s==SPACE_REPLACEMENT) {
			if (i>0 && buf[i-1]==ESCAPE_CHAR)
				buf[--i]=SPACE_REPLACEMENT;
			else
				buf[i]=SPACE;
		}
	}
	buf[i]=0;
	if (i>0 && buf[i-1]==';')
			// A semicolon stands directly in the back to, this deletes
		buf[i-1]=0;

	return buf;
}

const char * igetstr2(const char *s1) {
	static CString s;
	s = igetstr(s1);
	return extractName(s);
}

char * printkeyword(int key) {
	t_keyword *k;
	for (k=keywords; k; k=k->m_pNext)
	{
		if (key==k->keyword)
			return k->name;
	}
	return NULL;
	// this does not work if more entries in Keywords than in commands.txt file
	//key = MAXKEYWORDS-key-1;
	//for (k=keywords; key && k; k=k->m_pNext, key--);
	//return k->name;
}

void traceKeywords()
{
	int k;
	for (k=0; k < MAXKEYWORDS; k++)
		output(printkeyword(k));
}

char * printdirection(int dir) {
	t_direction *d=directions;
	
	while (d && d->dir!=dir) d=d->m_pNext;
	if (!d)
		return "<Error in Directions>";
	return d->name;
}


char *
printparam(int par) {
	t_params *p=parameters;
	
	while (p && p->param!=par) p=p->m_pNext;
	if (!p)
		return "<Error in Params>";
	return p->name;
}


char * printliste(int key, t_liste *L) {
	t_liste *l;
	for (l=L; key; l=l->m_pNext, key--);
	return l->name;
}

int findstr(char **v, const char *s, int max) {
	int i;
	size_t ss=strlen(s);

	if (!s[0])
		return -1;
	for (i=0; i < max; i++)
		if (v[i] && strncasecmp(s, v[i], ss)==0)
			return i;
	return -1;
}


int findNode(const char *str, tnode *first) {
	tnode *tk = first;

	while (*str) {
		char c=(char)tolower((unsigned char)*str);
		tk=tk->m_pNext[((unsigned char)c)%32];
		while (tk && tk->c!=c) tk=tk->m_pNexthash;
		if (!tk) return K_NOTHING;
		++str;
	}
	if (tk->id>=0) return tk->id;
	return K_NOTHING;
}

int findtoken(const char *str, int type) {
	tnode *tk = &tokens[type];

	if (*str=='@') {
		str++;
		at_cmd=1;
	} else
		at_cmd=0;

	return findNode(str, tk);
}


int findparam(const char *s) {
	int p;
	if (!*s)
		return -1;
	p=findtoken(s, UT_PARAM);
	if (p != -1)
		return p;
	p=findtoken(s, UT_BUILDING);
	if (p != -1)
		return P_BUILDING;
	return -1;
}


int isOrderParam(const char *s, int i, char print) {
	if (i != findparam(s))
		return 0;
	return 1;
}

int findMagicSchool(const char *s)
{
	return findstr(magiegebiet,s,5);
}

t_skills * findskill(const char *s) {
	int i;
	t_skills *sk=skilldata;

	i=findtoken(s, UT_SKILL);
	if (i<0)
		return NULL;
	while (i--) sk=sk->m_pNext;
	return sk;
}


#define findreport(s) 	findstr(reports,s,MAXREPORTS)
#define findoption(s) 	findtoken(s, UT_OPTION)
#define findsetting(s) 	findtoken(s, UT_CONFIG)
#define getskill()	findskill(getstr2())
#define getparam()	findparam(getstr2())


int finddirection(const char *s) {
	if (!*s)
		return -2;
	if (strcmp(s,"//")==0)		// "is permitted AFTER size NO NO // after Xontormia"
		return -2;
	return findtoken(s, UT_DIRECTION);
}

#define getdirection()	finddirection(getstr2())

#define getoption() findoption(getstr())


BOOL long_order() {
	if (order_unit->HasLongOrder()) {
		CString s(order_unit->longOrder());
		int nInd = s.Find(' ');
		if (nInd!=-1) s = s.Left(nInd);
		int i=findkeyword(s);
		switch (i) {
			case K_CAST: if (this_command==i) return TRUE;
				// ZAUBERE is no long instruction, but one may have also no other long - therefore is with those long_order()
			case K_SELL:
			case K_BUY:
				if (this_command==K_SELL || this_command==K_BUY) return TRUE;
				// There is possible several for SALES and PURCHASE per unit
		}
		if ((i==K_FOLLOW && this_command!=K_FOLLOW) || (i!=K_FOLLOW && this_command==K_FOLLOW))
			return TRUE;		//CONSEQUENCE is only trigger

		return order_unit->_warning(UNITALREADYHASLONGORDERS, 1, 0, order_unit->longOrder());
	}
	return order_unit->setLongOrder(sCurrentFile->buffer());
}


void getafaction(const char *s) {
	int i;

	i=btoi(s);
	if (!s[0])
		Error(FACTIONMISSING);
	else {
		if (!i)
			Warning(FACTION0USED,1);
		//icat(i);
	}
}


int getmoreunits(bool partei, int nIndex) {
	const char *s;
	int i, count, temp;

	count=0;
	for (;;) {
		s=getstr();
		if (!(*s))
			break;

		if (partei) {
			i=btoi(s);
			if (i<1)
				Error(FACTIONINVALID,s);
			//else	bcat(i);
		} else {
			if (findparam(s)==P_TEMP) {
				//scat(" TEMP");
				temp=1;
				s=getstr();
			} else
				temp=0;
			i=btoi(s);

			if (!i)
				order_unit->_error(UNITNOTPOSSIBLEHERE,s);
			else {
				//bcat(i);
				order_unit->onTeachOrder(i,temp);
			}
		}
		count++;
	}
	return count;
}


int getaunit(int type, CUnit **cmdUnit) {
	const char *s;
	char is_temp=0;
	int i;

	if (cmdUnit)
		*cmdUnit=NULL;

	s=getstr();
	if (s[0]==0) {
		if (type==NECESSARY) {
			if (order_unit) order_unit->_error(UNITMISSING); else Error(UNITMISSING);
			return GETAUNIT_FAILURE;
		}
		return 1;
	}
	switch (findparam(s)) {
		case P_PEASANT:
			//Scat(printparam(P_PEASANT));
			return GETAUNIT_THROWAWAY;

		case P_TEMP:
			//scat(" TEMP");
			s=getstr();
			i=btoi(s);
			is_temp=1;
			break;

		default:
			i=btoi(s);
			break;
	}

	if (type==42) {	//Seulement essai, si une unité vient, parce que c'est une erreur
		if (i)
			return 42;
		return GETAUNIT_FAILURE;
	}

	if (i==0) 
		return GETAUNIT_THROWAWAY; // ex GIVE 0

	if (cmdUnit)
		*cmdUnit=newunit(NULL,i,is_temp); // The unit already make, because of temp checks
	//bcat(i);
	if (is_temp)
		return GETAUNIT_TEMP;
	return GETAUNIT_ID;
}


void checkemail() {
	const char *s;
	//char bf[200];
	const char *addr;
	//scat(printkeyword(K_EMAIL));
	addr=getstr();
	checkstring(addr, DESCRIBESIZE, NECESSARY);

	if (!addr) {
		Warning(USEEMAIL,3);
		//sprintf_s(bf,200,"; %s!",_msg(USEEMAIL));
		//scat(bf);
		return;
	}
	s=strchr(addr, '@');
	if (!s) {
		Error(INVALIDEMAIL);
		return;
	}
	//scat(_msg(DELIVERYTO));
	//scat(addr);
}


// Contrôle sur un ordre long, etc.. - aus ACheck.c 4.1

void end_unit_orders() {
	if (!order_unit) // Pour le premier ordre de la première unité.
		return;

	order_unit->OnOrdersEnded();
}

void orders_for_unit(int i, CUnit *u, BOOL bIgnoreOrders) {
	char *j, *e;
	int s;

	end_unit_orders();
	mother_unit=order_unit=u;
	
	if (!bIgnoreOrders) 
	{
	/*	if (u->hasOrder()) {
			u->_warning(UNITALREADYHASORDERS, 1);
			do {
				i++; if (i<1) i=1;
				u=newunit(sCurrentFaction,i, 0);
			} while (u->hasOrder());
			Warning(USINGUNITINSTEAD,1,itob(i));
			order_unit=u;
		}*/

		u->setStartOfOrder(sCurrentFile->buffer());

		if (no_comment>0)
			return;
	}

	char *k=strchr((char *)sCurrentFile->buffer(),'[');
	if (!k) {
		Warning(CANTHANDLEPERSONCOMMENT,4);
		no_comment++;
		return;
	}
	k++;
	while (!atoi(k)) {	// Ici, une est [ dans le nom
						// 0 n'est pas des personnes dans la présentation de train
		k=strchr(k,'[');
		if (!k) {
			Warning(CANTHANDLEPERSONCOMMENT,4);
			no_comment++;
			return;
		}
		k++;
	}
	e=strchr(k,']');

	if (!bIgnoreOrders)
		u->setInitialNbPeople(atoi(k));

	j=strchr(k,',');
	if (!j) j=strchr(k,';');
	if (!j || j>e) {
		Warning(CANTHANDLESILVERCOMMENT,4);
		no_comment++;
		return;
	}
	while (!isdigit(*j)) j++;
	u->setInitialMoney(atoi(j));

	if (bIgnoreOrders)
		return ;

	while (isdigit(*j)) j++;	// derrière le nombre

	k=j;
	if (k) j=strchr(k,',');
	if (!j && k) j=strchr(k,';');

	if (j) {
		j++;
		if (j<e && *j=='U') {	// Un bâtiment doit entretenir
			j++;
			if (isdigit(*j)) {
				u->setIsInBuilding(ID_UNKNOWN,atoi(j));
				while (isdigit(*j)) j++; j++;
			}
		}

		if (j<e && *j=='I') {	// I comme une illusion, aussi Untote - justement tout ce qui mange nix et ne nécessite pas de long ordre
			u->SetAliveStatus(ALIVE_ILLUSION);
			j+=2; // derrière le I et le signe
		}
		if (j<e && *j=='s') {	// est sur un navire
			j++;
			u->setIsOnShip(btoi(j));
		} else if (j<e && *j=='S') {	// Le capitaine sur un navire est
			j++;
			s=btoi(j);
			//s=-btoi(j);
			//sUnitsList.SwitchCaptainStatus(s);
			u->setIsOnShip(s,TRUE);
		}
	}
	u->setRegionFrom(Rx, Ry);
}


void orders_for_temp_unit(CUnit *u) {
	if (u->hasStartOrder() && u->isOnRegion(pCurrentRegion))
		Error(ALREADYUSEDINLINE, u->outputName());

	u->setCurrentOrder(sCurrentFile->buffer());
	u->setStartOfOrder(sCurrentFile->buffer());

	mother_unit=order_unit;
	order_unit=u;
	order_unit->setMother(mother_unit);
}

void checknaming() {
	int i;
	const char *s;

	//scat(printkeyword(K_NAME));
	i=findparam(getstr());
	s=getstr();
	if (i==P_FOREIGN) {
		if (i==P_REGION)
			Error(ERRORNAMEFOREIGN,printparam(i));
		//else Scat(printparam(P_FOREIGN));
		i=findparam(s);
		s=getstr();
	}
	if (strchr(s,'('))
		Warning(NAMECONTAINSBRACKETS,1);

	switch (i) {
		case -1:
			order_unit->_error(UNRECOGNIZEDOBJECT,s);
			break;

		case P_UNIT:
		case P_FACTION:
		case P_BUILDING:
		case P_CASTLE:
		case P_SHIP:
		case P_REGION:
			//Scat(printparam(i));
			checkstring(s, NAMESIZE, NECESSARY);
			order_unit->OnRename(i,s);
			break;

		default:
			Error(CANTRENAMEOBJECT);
	}
}


void checkdisplay() {
	int i;

	//scat(printkeyword(K_DESCRIBE));
	const char *s = getstr();
	i=findparam(s);

	switch (i) {
		case -1:
			order_unit->_error(UNRECOGNIZEDOBJECT,s);
			break;

		case P_REGION:
		case P_UNIT:
		case P_BUILDING:
		case P_CASTLE:
		case P_SHIP:
		case P_PRIVAT:
			//Scat(printparam(i));
			checkstring(getstr(), 0, POSSIBLE);
			break;

		default:
			Error(CANTDESCRIBEOBJECT);
	}
}


BOOL checkenter() {
	int i;
	int nID;

	//scat(printkeyword(K_ENTER));
	i=findparam(getstr());

	switch (i) {
		case -1:return order_unit->_error(ENTERWHAT);
		case P_BUILDING:
		case P_CASTLE:
		case P_SHIP:
			//Scat(printparam(i));
			nID=get_b();
			if (!nID) 
				return order_unit->_error(OBJECTNUMBERMISSING);
			//bcat(nID);
			break;
		default:
			return order_unit->_error(CANTENTEROBJECT);
	}
	order_unit->setCurrentOrder(sCurrentFile->buffer());
	return order_unit->OnEnter(i,nID);
}

t_spell * findSpell(const char *s, const char *sMagie, BOOL & bInMagicSchool)
{
	return sSpellsList.Find(s,sMagie,bInMagicSchool);
}

int getaspell(const char *s, char spell_typ, CUnit *u, int reallycast) {
	t_spell *sp;
	int x_region=0;
	int y_region=0;
	int level=1;
	BOOL bRegion=FALSE;
//SP_KAMPF
	if (u && u!=order_unit)
		int hhhppp=1;

	if (*s=='[' || *s=='<') {
		Error(ERRORSPELLSYNTAX);
		return 0;
	}
	if (findparam(s) == P_REGION) {
		//scat(printparam(P_REGION));
		s=getstr();
		if (*s) {
			x_region=atoi(s);
			//icat(x_region);
			s=getstr();
			if (*s) {
				y_region=atoi(s);
				//icat(y_region);
				bRegion=TRUE;
			} else {
				Error(ERRORCOORDINATES);
				return 0;
			}
		} else {
			Error(ERRORREGIONPARAMETER);
			return 0;
		}
		s=getstr();
	}
	if (findparam(s) == P_LEVEL) {
		//scat(printparam(P_LEVEL));
		s=getstr();
		if (!*s || atoi(s)<1) {
			Error(ERRORLEVELPARAMETERS);
			return 0;
		}
		level=atoi(s);
		//icat(level);
		s=getstr();
	}
	CString sMagie("");
	if (u)
		u->magicSchool(sMagie);
	BOOL bInMagicSchool=FALSE;
	sp=sSpellsList.Find(s,sMagie,bInMagicSchool);
	if (!sp) {
		if (u) {	// otherwise that is the test of GIVES
			if (show_warnings>0)	// nicht bei -w0
				order_unit->_error(UNRECOGNIZEDSPELL,s);
			if (*s>='0' && *s<='9')
				order_unit->_error(MISSINGPARAMETERS);
			//qcat(s);
		}
		return 0;
	}
	if (!bInMagicSchool && u && !u->isPet())
		return u->_error(NOTINMAGICSCHOOL,sp->name,(LPCSTR)sMagie);

	//qcat(sp->name);
	if (!(sp->type & spell_typ)) 
		Warning(ISCOMBATSPELL,1,sp->name,(sp->type & SP_ZAUBER) ? _msg(ISNOTCOMBATSPELL) : "");
	else {
		if (u && (sp->type & SP_BATTLE) && (u->spell & sp->type)) {
			int nOrder=0;
			switch (sp->type) {
				case SP_POST:nOrder = POST; break;
				case SP_PRAE: nOrder = PRE; break;
			}
			//if (show_warnings>0)
			//	u->_warning(UNITALREADYHAS,1,(nOrder==0?"":_msg(nOrder)),_msg(COMBATSPELLSET));
		}

		if (u && spell_typ==SP_ZAUBER) {
			// TODO: combatspell management
			// TODO: transfer other params
			u->OnCastOrder(sp, reallycast, level, x_region, y_region, bRegion);
		}
	}
	do {
		s=getstr();
		//if (*s) {Scat(s);}
	} while (*s);		// remaining parameters without checks spend
	return 1;
}

void checkmake() {
	int i;
	int nAmount = -1;
	int nTempID = -1;
	int nDirectionID = -1;
	int nBuildingID = -1;
	int nShipID = -1;
	char bf[200];
	BOOL bOK=TRUE;

	//scat(printkeyword(K_MAKE));
	const char *ssrc=getstr();

	if (isdigit(*ssrc)) { // MAKE amount of "article"
		nAmount = atoi(ssrc);
		if (nAmount == 0) Warning(NUMBER0SENSELESS,2);
		ssrc=getstr();
	}

	CString s(ssrc);
	s.Replace("\\\"","");
	i=findparam(s);

	switch (i) {
		case P_HERBS:
			//if (nAmount > 0) icat(nAmount);
			//Scat(printparam(P_HERBS));
			if (long_order()) 
				order_unit->onMakeHerbOrder(nAmount);
			return;

		case P_TEMP:
			if (nAmount > 0) 
				order_unit->_error(NUMBERNOTPOSSIBLE);
			next_indent=INDENT_NEW_ORDERS;
			//scat(" TEMP");
			nTempID = get_b();
			if (!nTempID)
				order_unit->_error(NOTEMPNUMBER);
			else 
			{
				CUnit *u;
				//bcat(nTempID);
				s=getstr();
				//if (*s) qcat(s);
				from_temp_unit_no=nTempID;
				u=newunit(sCurrentFaction,nTempID,UNIT_STATUS_TEMP);
				//if (!u->IsOnShip())
				//	u->setOnShip(abs(order_unit->ShipNumber()));
				orders_for_temp_unit(u);
			}
			return;

		case P_SHIP:
			//if (nAmount > 0) icat(nAmount);
			nShipID=get_b();
			//Scat(printparam(i));
			//if (nShipID) bcat(nShipID);
			if (long_order()) 
				order_unit->onMakeShipOrder(NULL,nShipID,nAmount);
			return;

		case P_ROAD:
			//if (nAmount > 0) icat(nAmount);
			//Scat(printparam(i));
			nDirectionID=getdirection();
			if (nDirectionID<0) {
				sprintf_s(bf,200,"%s %s %s",printkeyword(K_MAKE), printparam(P_ROAD),_msg(DIRECTION));
				output(bf);
			} 
			//else Scat(printdirection(nDirectionID));
			if (long_order()) 
				order_unit->onMakeRoadOrder(directionToCoast(eDirection(nDirectionID)),nAmount);
			return;
	}

	i=findshiptype(s);
	if (i!=-1) {
		//qcat(s);
		nShipID = get_b();
		if (long_order())
			order_unit->onMakeShipOrder(shipFromUTID(i),nShipID,nAmount);
		return;
	}

	if (!s.CompareNoCase(EN_HORSES))
		s=EN_HORSE;

	i=finditem(s);
	if (i>=0 && ItemPrice(i)==0) {
		//if (nAmount > 0) icat(nAmount);
		//qcat(ItemName(i));
		if (long_order()) 
			order_unit->onMakeItemOrder(i,nAmount);
		return;
	}

	i=findpotion(s);
	if (i!=-1) {
		//qcat(potionname(i));
		if (long_order()) 
			order_unit->onMakePotionOrder(i,nAmount);
		return;
	}

	i=findbuildingtype(s);
	if (i!=-1) {
		//if (nAmount > 0) icat(nAmount);
		//qcat(s);
		
		nBuildingID=get_b();
		//if (nBuildingID) bcat(nBuildingID);
		if (long_order())
			order_unit->onMakeBuildingOrder(buildingFromUTID(i),nBuildingID,nAmount);
		return;
	}

	//TODO: MAKE <id> : possible? 
	// On peut utiliser MAKE sans paramètres, si on est dans un château ou un navire.  
	if (!s.IsEmpty())
		bOK = order_unit->_error(CANTMAKETHAT);
	if (long_order() && bOK)
		/*order_unit->onMakeContainer(nAmount)*/{;} //TODO


}


BOOL checkdirections(int key,TMoveInfo & moveinfo) {
	int i, x, y;
	int count = 0;
	int nRoutingIndex = 0;
	
	x=Rx; y=Ry;
	moveinfo.m_bRouting = (key==K_ROUTE);
	moveinfo.m_nRoutingIndex = 0;
	moveinfo.m_bPauseInRoute=FALSE;

	moveinfo.moves[0] = CPoint(x,y);
	//scat(printkeyword(key));

	do {
		i=getdirection();
		if (i<=-1) {
			if (i==-2) break;		// La ligne prend fin trop
			return order_unit->_error(UNRECOGNIZEDDIRECTION);
		} 
		if (key==K_ROUTE && i==D_PAUSE && count==0)
			order_unit->_warning(ROUTESTARTSWITHPAUSE,2);
		if (key==K_MOVE && i==D_PAUSE)
			return order_unit->_error(MOVENOTPOSSIBLEWITHPAUSE);
		//Scat(printdirection(i));
		count++;
		switch (i) {
			case D_NORTHEAST: y++; break;
			case D_NORTHWEST: y++; x--;	break;
			case D_SOUTHEAST: y--; x++;	break;
			case D_SOUTHWEST: y--; break;
			case D_EAST: x++; break;
			case D_WEST: x--; break;
			case D_PAUSE: 
			count--; 
			moveinfo.m_bPauseInRoute=TRUE;
			if (nRoutingIndex == 0) nRoutingIndex = count; 
			break;
			default:
			int zzzzzzz=1;
			break;
		}
		moveinfo.moves[count] = CPoint(x,y);
	} while (i>=0 && count < NBMAXMOVE);

	if (!count)	
		return order_unit->_error(UNRECOGNIZEDDIRECTION);

	if (key==K_ROUTE && !noroute && (moveinfo.moves[0]!=moveinfo.moves[count]))
		order_unit->_warning(ROUTENOTCYCLIC,13,moveinfo.moves[0].x,moveinfo.moves[0].y,moveinfo.moves[count].x,moveinfo.moves[count].y);

	// CG 21/02/08: ROUTE without PAUSE: means MOVE max, but without error message
	if (nRoutingIndex == 0 && moveinfo.m_bRouting && !moveinfo.m_bPauseInRoute)
		nRoutingIndex = count; 

	moveinfo.m_nCount = count;
	moveinfo.m_nRoutingIndex = nRoutingIndex;

	return TRUE;
}


void check_sabotage() {
	if (getparam() != P_SHIP)
		order_unit->_error(ONLYSABOTAGESHIP);
}


void checkmail() {
	const char *s;

	//scat(printkeyword(K_MESSAGE));
	s=getstr();
	if (strcasecmp(s,"an")==0 || strcasecmp(s,"to")==0)
		s=getstr();

	switch (findparam(s)) {
		case P_FACTION:
			//Scat(printparam(P_FACTION));
			break;

		case P_REGION:
			//Scat(printparam(P_REGION));
			break;

		case P_UNIT:
			//Scat(printparam(P_UNIT));
			//bcat(get_b());
			break;

		default:
			Error(MSGTO);
			break;
	}
	s=getstr();
	checkstring(s, 0, NECESSARY);
}


void check_ally() {
	int i;
	const char *s;

	//scat(printkeyword(K_HELP));
	getafaction(getstr());
	s=getstr();

	i=findparam(s);
	switch (i) {
		case P_GIVE:
		case P_FIGHT:
		case P_COMBAT:
		case P_SILVER:
		case P_OBSERVE:
		case P_GUARD:
		case P_FACTIONSTEALTH:
		case P_ALLES:
		case P_NOT:
			//Scat(printparam(i));
			break;
		default:
			Error(ERRORHELP);
			return;
	}

	s=getstr();
	//if (findparam(s)==P_NOT)
		//Scat(printparam(P_NOT));
	
}


void check_meinung() {
	int i;

	//scat(printkeyword(K_OPINION));
	i=get_i();
	if (i <= 0)
		Error(ERRORSURVEY);
	//icat(i);
	i=get_i();
	if (i <= 0)
		Error(ERROROPINION);
	//icat(i);
	checkstring(getstr(),DESCRIBESIZE,POSSIBLE);
}


void check_comment() {
	const char *s;
	int m;

	s=getstr();
	if (strncasecmp(s,"ECHECK",6))
		return;
	s=getstr();

	if (strncasecmp(s,"VERSION",7)==0) {
		output("%s\n", sCurrentFile->buffer());
		return;
	}
	if (strncasecmp(s,"NOWARN", 6)==0) {	// Warnings for next line out
		warn_off=2;
		return;
	}
	if (strncasecmp(s,"LOHN",4)==0 || strncasecmp(s,"WAGE",4)==0) {	// WAGES for work
		m=get_i();
		lohn=(char)max(10,m);
		//TODO: set to current region: r->setDefaultWage(lohn);
		
		return;
	}
	if (strncasecmp(s,"ROUT",4)==0) {	// ROUTE
		noroute=(char)(1-noroute);
		return;
	}
	if (strncasecmp(s,"KOMM",4)==0 || strncasecmp(s,"COMM",4)==0) {	// KOMMando
		m=get_b();
		if (!m) {
			//m=order_unit->shipID();
			if (invalidID(m))
				m=abs(rand());
		}
		order_unit->setOnShip(m,TRUE); // captain
		return;
	}
	if (strncasecmp(s,"EMPTY",5)==0) {
		order_unit->emptyMoney();
		return;
	}
	if (strncasecmp(s,"NACH",4)==0 || strncasecmp(s,"MOVE",4)==0) {
		order_unit->SetMoveStatus(MOVESTATUS_SOMEWHERE);
		order_unit->SetXY(get_i(),get_i());
		return;
	}
	m=atoi(s);
	if (m) {
		if (order_unit)
			order_unit->addMoney(m,TRUE);
		return;
	}
}

BOOL missingQuotes(const CString & ss)
{
	CString s(ss);
	int ind=0;
	BOOL bInsideQuotes=FALSE;
	int len=s.GetLength();
	while (ind<len)
	{
		ind = s.Find('\"',ind);
		if (ind==-1)
			break;
		bInsideQuotes=!bInsideQuotes;
		ind++;
	}
	return bInsideQuotes;
}

void checkanorder(const char *Orders) {
	int x;
	const char *s;
	t_skills *sk;
	CUnit *u;
	CUnit *attacked_unit;
	CUnit *carried_unit;
	CUnit *numbered_unit;
	CUnit *pipo_unit;
	CUnit *stolen_unit;
	CUnit *contacted_unit;
	CUnit *followed_unit;
	CUnit *carrying_unit;
	TMoveInfo moveinfo;
	int nAmount = -1;

	CString ss(Orders);
	int nind= ss.Find(";");
	if (nind!=-1) ss = ss.Left(nind);

	//ERTRACE("check order %s",Orders);

	if (ss.IsEmpty())
		return; 	// This was a comment line

	if (missingQuotes(ss))
	{
		if (order_unit) order_unit->_error(MISSINGQUOTES,(LPCSTR)ss); else Error(MISSINGQUOTES,(LPCSTR)ss);
		return ;
	}

	K_Order cmd=igetkeyword(ss);
	if (order_unit && order_unit->AddOrderBySequence(ss,cmd))
		return;

	BOOL bAutomatic = (ss.GetAt(0)=='@'); 
	s = (LPCSTR)ss;
	int i=cmd;
	this_command=i;
	switch (cmd) {
		case K_NUMBER:
			//scat(printkeyword(K_NUMBER));
			i=getparam();
			if (!(i==P_UNIT || i==P_SHIP || i==P_BUILDING || i==P_CASTLE || i==P_FACTION)) {
				Error(ORDERNUMBER);
				break;
			}
			//Scat(printparam(i));
			if (getaunit(POSSIBLE,&numbered_unit)!=1)	/// "TEMP xx"or to "does not go 0"
				Error(WRONGNUMBER);
			break;

		case K_OPINION:
			check_meinung();

		case K_BANNER:
			//scat(printkeyword(K_BANNER));
			checkstring(getstr(),DESCRIBESIZE,NECESSARY);
			break;

		case K_EMAIL:
			checkemail();

		case K_ORIGIN:
			//scat(printkeyword(K_ORIGIN));
			s=getstr();
			if (*s) {
				x=atoi(s);
				//icat(x);
				s=getstr();
				if (*s) {
					//x=atoi(s);
					//icat(x);
				} else
					Error(NEEDBOTHCOORDINATES);
			}
			break;

		case K_MESSAGE:
			checkmail();
			break;



		case K_ATTACK:
			//scat(printkeyword(K_ATTACK));
			if (getaunit(NECESSARY,&attacked_unit)==3)
				order_unit->_error(CANTATTACKTEMP);
			if (getaunit(42,&pipo_unit)==42)
				order_unit->_error(ONEATTACKPERUNIT);
			order_unit->onAttackOrder(attacked_unit);
			break;

		case K_SIEGE:
			//scat(printkeyword(K_SIEGE));
			i=get_b();
			if (!i)
				Error(NUMCASTLEMISSING);
			//else
			//	bcat(i);
			long_order();
			break;

		case K_NAME:
			checknaming();
			break;

		case K_BREED:
		case K_GROW:
			//scat(printkeyword(K_BREED));
			i=getparam();
			if (i!=P_HERBS && i!=P_HORSE && i!=P_SEED)
				Error(BREEDHORSESORHERBS);
			//else scat(printparam(i));
			long_order();
			break;

		case K_PLANT:
			//scat(printkeyword(K_PLANT));
			i=getparam();
			if (i!=P_SEED)
				Error(BREEDHORSESORHERBS);
			//else scat(printparam(i));
			
			long_order();
			break;

		case K_STEAL:
			//scat(printkeyword(K_STEAL));
			getaunit(NECESSARY,&stolen_unit);
			long_order();
			break;

		case K_DESCRIBE:
			checkdisplay();
			break;

		case K_END: //FROM MAKE TEMP
			if (from_temp_unit_no==0)
				Warning(ENDWITHOUTTEMP,2);
			//scat(printkeyword(K_END));
			indent=next_indent=INDENT_ORDERS;
			end_unit_orders();
			from_temp_unit_no=0;
			if (mother_unit) {
				order_unit=mother_unit;
				mother_unit=NULL;
			}
			break;

		case K_RESEARCH:
			//scat(printkeyword(K_RESEARCH));
			i=getparam();
			if (i!=P_HERBS)
				Error(RESEARCHHERBSONLY);
			// else	scat(printparam(P_HERBS));	//momentarily only RESEARCHES HERBS
			if (long_order()) 
				order_unit->onMakeHerbOrder(nAmount);
			break;

		case K_HIDE:
			//scat(printkeyword(K_HIDE));
			s=getstr();
			i=findtoken(s, UT_RACE);
			if (i >= 0) {
				//Scat(printliste(i, Races));
				// nothing done
				break;
			}
			i=findparam(s);
			if (i==P_FACTION) {
				//Scat(printparam(i));
				s=getstr();
				if (*s) {
					if (findparam(s)==P_NOT) {
						//Scat(printparam(P_NOT));
					} else if (findparam(s)==P_NUMBER) {
						//Scat(printparam(P_NUMBER));
						i=get_b();
						if (!i)
							Error(WRONGFACTIONNUMBER);
						else
						{
						
							//icat(i);
							order_unit->OnDisguise(i);
						}
					} else
						Error(WRONGPARAMETER);
				}
				break;
			}
			if (isdigit(s[0])) {
				i=atoip(s);
				//icat(i);
				break;
			}
			Warning(MISSINGDISGUISEPARAMETERS,5);
			break;

		case K_HELP:
			check_ally();
			break;
		case K_FIGHT:
		case K_COMBAT:
			//scat(printkeyword(i));
			s=getstr();
			i=findparam(s);
			switch (i) {
				case P_AGGRESSIVE:
				case P_DEFENSIVE:
				case P_NOT:
				case P_BEHIND:
				case P_FLEE:
				case P_FRONT:
					//Scat(printparam(i));
					break;
				default:
					if (*s) {
						if (findkeyword(s)==K_HELP) {
							//Scat(printkeyword(K_HELP));
							s=getstr();
							if (*s) {
								if (findparam(s)==P_NOT) {
									//Scat(printparam(P_NOT));
								} 
								else

									Error(WRONGFIGHTSTATE);
							}
						} else

							Error(WRONGFIGHTSTATE);
						//Scat(s);
						i = K_NOTHING; // COMBAT HELP, or bad COMBAT COMMAND
					} 
					else 
					{
						//Scat(printparam(P_FRONT));
						i = P_FRONT;
						//COMBAT = COMBAT FRONT 
					}
			}
			if (i!=K_NOTHING) 
				order_unit->onCombatStatus(i);
			break;

		case K_COMBATSPELL:
			//scat(printkeyword(K_COMBATSPELL));
			s=getstr();
			getaspell(s, SP_KAMPF | SP_POST | SP_PRAE, order_unit, 1);
			break;

		case K_CONTACT:
			//scat(printkeyword(K_CONTACT));
			getaunit(NECESSARY,&contacted_unit);
			break;

		case K_SPY:
			//scat(printkeyword(K_SPY));
			i=get_b();
			if (!i)
				order_unit->_error(UNITMISSING);
			//else
			//	bcat(i);
			if (long_order())
			{
				// order_unit->onSpyOrder();
			}
			break;

		case K_TEACH:
			//scat(printkeyword(K_TEACH));
			if (long_order() && !getmoreunits(false,K_TEACH))
				order_unit->_error(TEACHWHO);
			break;

		case K_FORGET:
			//scat(printkeyword(K_FORGET));
			sk=getskill();
			if (!sk)
				order_unit->_error(UNRECOGNIZEDSKILL);
			break;

		case K_LEARN:
			//scat(printkeyword(K_LEARN));
			sk=getskill();
			if (!sk)
				order_unit->_error(UNRECOGNIZEDSKILL);
			else {
				//Scat(sk->name);
				if (strcasecmp(sk->name,_msg(MAGIC))==0)
					if (order_unit->people()>1)
						order_unit->_error(ONEPERSONPERMAGEUNIT);
				if (long_order())
					order_unit->onLearnOrder(sk);
			}
			break;

		case K_MAKE:
			checkmake();
			break;

		case K_SABOTAGE:
			//scat(printkeyword(K_SABOTAGE));
			check_sabotage();
			long_order();
			break;

		case K_PASSWORD:
			//scat(printkeyword(K_PASSWORD));
			s=getstr();
			if (!s[0])
				Warning(PASSWORDCLEARED,0);
			else
				checkstring(s, NAMESIZE, POSSIBLE);
			break;

		case K_RECRUIT:
			//scat(printkeyword(K_RECRUIT));
			i=get_i();
			if (i) {
				//icat(i);
				if (from_temp_unit_no)
					u=newunit(sCurrentFaction,from_temp_unit_no, 1);
				else
					u=order_unit;
				u->onRecruit(i,mother_unit);
			} else
				order_unit->_error(MISSINGNUMRECRUITS);
			break;

		case K_QUIT:
			//scat(printkeyword(K_QUIT));
			s=getstr();
			if (!s[0])
				order_unit->_error(MISSINGPASSWORD);
			else
				checkstring(s, NAMESIZE, POSSIBLE);
			Warning(QUITMSG,0);
			break;

		case K_ENTER:
			checkenter();
			break;

		case K_LEAVE:
			//scat(printkeyword(K_LEAVE));
			order_unit->OnLeave(TRUE);
			break;

		case K_ROUTE:
		case K_MOVE:
			if (checkdirections(i,moveinfo) && long_order())
				order_unit->OnMoveOrder(moveinfo);
			break;

		case K_FOLLOW:
			//scat(printkeyword(K_FOLLOW));
			s=getstr();
			if (s) {
				i=findparam(s);
				if (i==P_UNIT) {
					//Scat(printparam(i));
					getaunit(NECESSARY,&followed_unit);
				} else if (i==P_SHIP) {
					//Scat(printparam(i));
					s=getstr();
					x=btoi(s);
					//Scat(s);
				} else
					Error(FOLLOW);
			}
			break;

		case K_REPORT:
			//scat(printkeyword(K_REPORT));
			s=getstr();
			i=findreport(s);
			if (i==-1) {
				if (strncasecmp(s, printkeyword(K_SHOW), strlen(s)))
					Error(UNRECOGNIZEDREPORTOPTION);
				else {
					//Scat(printkeyword(K_SHOW));
				}
				break;
			}
			//Scat(reports[i]);
			s=getstr();
			i=findstr(message_levels, s, ML_MAX);
			if (i==-1) {
				Error(WRONGOUTPUTLEVEL);
				break;
			}
			//Scat(message_levels[i]);
			break;

		case K_OPTION:
			//scat(printkeyword(K_OPTION));
			i=getoption();
			if (i<0) {
				Error(UNRECOGNIZEDOPTION);
				break;
			}
			//Scat(printliste(i, options));
			i=getparam();
			if (i==P_WARN) {
				//Scat(printparam(i));
				i=getparam();
			}
			if (i==P_NOT) {
				//Scat(printparam(i));
			}
			break;

		case K_SHOW:
			//scat(printkeyword(K_SHOW));
			s=getstr();
			//Scat(s);
			break;

		case K_CLAIM:
			//scat(printkeyword(K_CLAIM));
			s=getstr();
			//Scat(s);
			break;


		case K_DESTROY:
			//scat(printkeyword(K_DESTROY));
			break;

		case K_RIDE:
			//scat(printkeyword(K_RIDE));
			if (getaunit(NECESSARY,&carrying_unit)==2)
				order_unit->_error(UNIT0NOTPOSSIBLE);
			else 
			if (long_order())
				order_unit->OnRideOrder(carrying_unit);
			break;

		case K_CARRY:
			//scat(printkeyword(K_CARRY));
			getaunit(NECESSARY,&carried_unit);
			if (!does_default) {
				if (carried_unit) 
					carried_unit->OnCarriedBy(order_unit); else Warning(NOCARRIER,3);
			}
			if (getaunit(42,&carried_unit)==42)
				order_unit->_error(ONECARRYPERUNIT);
			break;

		case K_PIRACY:
			getmoreunits(true,K_PIRACY);
			break;

		case K_SCHOOL:		// Magic area
			s=getstr();
			i=findstr(magiegebiet,s,5);
			if (i<0)
				Error(UNRECOGNIZEDSCHOOL,s);
			else {
				//scat(printkeyword(K_SCHOOL));
				//Scat(magiegebiet[i]);
			}
			break;

		case K_DEFAULT:
			//scat(printkeyword(K_DEFAULT));
			//scat(" \"");
			u=newunit(sCurrentFaction,-1,0);
			order_unit->CopyTo(u);
			order_unit->setCurrentOrder("");
			order_unit->SetTempStatus(0);
			s=getstr();
			does_default=1;
			porder();
			checkanorder(s);
			does_default=2;
			while (getstr()[0]);
			u->CopyTo(order_unit);
			u->reset();
			break;

		case K_COMMENT:
			check_comment();
			//scat(Orders);
			break;

		case K_RESTART:
			i=findtoken(getstr(), UT_RACE);
			if (i<0) {
				Error(UNRECOGNIZEDRACE);
				break;
			} 
			//else
				//Scat(printliste(i, Races));
			s=getstr();
			if (!*s) {
				Error(MISSINGPASSWORD);
				break;
			} 
			//else
			//	qcat(s);
			Warning(RESTARTMSG,0);
			break;

		case K_GROUP:
			s=getstr2(); // name without ""
			//if (*s) { Scat(s);}
			order_unit->onOrder_group(s);
			break;

		case K_SORT:
			//scat(printkeyword(K_SORT));
			s=getstr();
			if (*s) {
				if (strncasecmp(s, _msg(BEFORE), strlen(s))==0 ||
						strncasecmp(s, _msg(BEHIND), strlen(s))==0) {
					//Scat(s);
					i=getaunit(NECESSARY,&pipo_unit);
					if (i==1 || i==3)	//normal or temp unit: ok
						break;
				}
			}
			Error(SORT);
			break;

		case K_PREFIX:
			//scat(printkeyword(K_PREFIX));
			s=getstr();
			//if (*s)	{Scat(s);}
			break;

		default: if (order_unit) order_unit->_error(UNRECOGNIZEDORDER); else Error(UNRECOGNIZEDORDER);
	}
	if (does_default != 1) {
		porder();
		does_default=0;
	}
}


BOOL readaunit(const TCRUnit * unitInfos) {
	int i;
	CUnit *u;
	BOOL bIgnoreOrders = FALSE;
	BOOL bCRConfirmedOrders = FALSE;
	BOOL bInitialGuarding = FALSE;

	if (unitInfos)
	{
		bCRConfirmedOrders = unitInfos->m_bConfirmedOrders;
		bInitialGuarding = 	unitInfos->m_bInitialGuarding;
	}

	i=get_b();
	if (i==0) 
		return Error(MISSINGUNITNUMBER);
		//return sCurrentFile->nextOrder();

	u=newunit(sCurrentFaction,i, 0);

	if (bCRConfirmedOrders)	u->setCRConfirmedOrders();
	if (bInitialGuarding)	u->setInitialGuarding();

	if (s_bExtraOrders)
	{ 
		//ignore .TXT orders if orders confirmed in CR for same unit
		if (u->CRConfirmedOrders())
			bIgnoreOrders=TRUE;
	}
	
	// Otherwise the line number is the line, where the unit was addressed first by another unit
	orders_for_unit(i,u,bIgnoreOrders);

	indent=INDENT_UNIT;
	next_indent=INDENT_ORDERS;
	porder();
	from_temp_unit_no=0;

	for (;;) {
		if (!sCurrentFile->nextEntry())
			return FALSE;


		// Only if we are safe that no instruction was entered, 
		// we checken whether now a new unit or a new player to-comes
		const char * pBuffer = sCurrentFile->buffer();

		//ERTRACE("%s...",pBuffer);

		if (igetkeyword(pBuffer)==-1) {
			if (pBuffer[0]==';') {
				check_comment();
				continue;
			}
			else switch (igetparam(pBuffer)) {
				case P_UNIT:
				case P_FACTION:
				case P_NEXT:
				case P_REGION:
					if (from_temp_unit_no != 0) {
						Warning(MISSINGEND,2,itob(from_temp_unit_no));
						from_temp_unit_no=0;
					}
					return TRUE;
			}
		}
		if (pBuffer[0] && !bIgnoreOrders)
			checkanorder(pBuffer);
	}
	return TRUE;
}

t_faction * findFactionFromDescribed(int nID, const char *pswd)
{
	t_faction *f=NULL;
	t_faction *ff=NULL;
	const char * sID = itob(nID);
	for (ff=factions;ff; ff=ff->m_pNext)
	{
		if ((strcasecmp(pswd, ff->pswd)==0) || (strcasecmp(sID, ff->id)==0))
		{
			f=ff;
			break;
		}
	}
	return f;
}

CFaction * findFaction(int nID, const char *pswd)
{
	t_faction * f = findFactionFromDescribed(nID, pswd);
	return sFactionsList.Add(nID,pswd,f);
}

BOOL setCurrentFaction(int nID, const char *pswd)
{
	sCurrentFaction = findFaction(nID, pswd);
	return sCurrentFaction!=NULL;
}

BOOL readafaction() {
	int i;
	const char *spswd;
	BOOL bOK = FALSE;

	//TODO
	//if (line_start==1)
		//gLine_no=1;
	i=get_b();
	if (i>0) {
		//bcat(i);
		spswd=getstr();
		//qcat(spswd);
		bOK = setCurrentFaction(i,spswd);
	} 
	else
		Error(MISSINGFACTIONNUMBER);

	indent=next_indent=INDENT_FACTION;
	porder();
	return bOK;
}

BOOL check_faction(int & nFactionID)
{
	//scat(printparam(P_FACTION));
	// befehle_ende=0;
	if (readafaction())
	{
		nFactionID = sCurrentFaction->id(); 
		if (compile)
			outputln("%s:faction:%s",(sCurrentFile?sCurrentFile->filename():"unknown"),itob(nFactionID));
		else
			outputln(FOUNDORDERS, itob(nFactionID));
		
		//after a PLAYER on "; OPTION "and/or"; ECHECK "test
		if (!sCurrentFile->checkHeader())	
			return FALSE;

		if (!compile) {
			if (VERBOSE) {
				output(RECRUITCOSTSSET, g_nRecruitCost);
				output(WARNINGLEVEL, show_warnings);
				if (silberpool)
					output(SILVERPOOL);
				fputs("\n\n", fERR);
				if (!has_version)
					output("%s %s.%s", HINTNOVERSION, MAINVERSION, MINVERSION);
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL check_otherParam(const char *buffer, int nFactionID)
{
	if (buffer[0]==';')
		check_comment();
	else 
	{
		if (nFactionID && buffer[0])
			return Warning(NOTEXECUTED,1);
	}
	return TRUE;
}

BOOL check_locale()
{
	const char *x=getstr();
	if (!*x)
		output("Missing locale\n");
	else
		if (strcasecmp(x,echeck_locale)!=0)
	return Error(CANTCHANGELOCALE);

	if (echo_it) {
		fputs(sCurrentFile->buffer(),fOUT);
		putc('\n',fOUT);
	}
	return TRUE;
}

void help_keys(char key) {
	int i,j;

	switch (key) {
		case 's':	 	
		case 'k':		// Keywords
			output("keywords:\n\n");
			for (i=1; i<UT_MAX; i++)
				output("%s\n",Keys[i]);
			break;

		case 'p':		// Parameter
			output("parameters:\n\n");
			for (i=0; i<MAXPARAMS; i++)
				output("%s\n",Params[i]);
			break;

		case 'r':		
		case 'd':		// Directions
			output("directions:\n\n");
			for (i=0; i<MAXDIRECTIONS; i++)
				fprintf(fERR, "%s\n",Directions[i]);
			break;

		case 'm':
			output("messages:\n\n");
			for (i=0; i<MAX_ERRORS; i++)
				output("%s\n",Errors[i]);
			break;

		case 'f':
			output("files:\n\n");
			for (j=UT_NONE+1; j<UT_MAX; j++) {
				output("%s:",Keys[j]);
				for (i=0; i<filecount_EN; i++)
					if (ECheck_Files_EN[i].type==j)
						output("  %s",ECheck_Files_EN[i]);
				putc('\n',fERR);
			}
			putc('\n',fERR);
			break;

		default:
			return;
	}
	onExitProg(0);
}


void recurseprinthelp(t_liste *h) {
	if (h->m_pNext) 
		recurseprinthelp(h->m_pNext);
	output("%s\n", h->name);
}

void printhelp(int argc, char *argv[], int index) {

	if (!help_caption)
		readfiles(0);	// possibly other echeck_locale is set; therefore _ _ read now

	if (!help_caption) {
		CString header;
		header.Format("\n * *ECheck V%s**\n\n can't read any file!\n\n Path: '%s/%s'\n\n", __DATE__, path, echeck_locale);
		output("%s", (LPCSTR)header);
		help_keys('f');
	}

	output("%s %s.%s.%sb%s %s %s", help_caption, MAINVERSION, MINVERSION, PATCHLEVEL, BUILDNUM, __DATE__, argv[0]);
	if (index>0 && argv[index][1]=='h') {
		if (argv[index][2]!=0)
			help_keys(argv[index][2]);
		if (argc>index+1)
			help_keys(*argv[index+1]);
	}

	output(help_path, echeck_locale);
	putc('\n',fERR);
	recurseprinthelp(help);
	onExitProg(1);
}

int check_options(int argc, char *argv[], char dostop, char command_line) {
	int i;
	char *x;
	CString sFilename;
	for (i=1; i<argc; i++) {
		if (argv[i][0]=='-') {
			switch (argv[i][1]) {
				case 'M': has_stats = 0; break;
				case 'P':
					//if (dostop) {		// with options via "; ECHECK " no more do not make
						if (argv[i][2]==0) {	// -P path
							i++;
							if (argv[i])
								path=_strdup(argv[i]);
							else {
								fputs("Empty path invalid\n",stderr);
								onExitProg(1);
							}
						} else	// -Ppath
							if (*(argv[i]+2))
								path=_strdup((char *)(argv[i]+2));
					//}
					break;

				case 'v':
					if (argv[i][2]==0) {	// -v version
						i++;
						if (!argv[i])
							break;
					}
					has_version=1;
					x=strchr(argv[i],'.');
					if (x) {
						*x=0;
						if (strcmp(MAINVERSION,(char *)(argv[i]+2))==0) {
							*x='.';
							x++;
							if (show_warnings>1 && strcmp(MINVERSION,x)!=0)
								fprintf(stderr,"Wrong ECheck-Version: %s\n", argv[i]+2);
						}
					}
					break;
				case 'z': CCRErrors::s_bBasicErrors = 1 ; break;
				case 'B': s_bArchiveCR = 1; break;
				case 'b':
					brief=1;
					break;

				case 'l':
					silberpool=1;
					break;

				case 'q':
					no_comment=1;
					noship=1; nolost=1; noroute=1;
					break;

				case 'r':
					if (argv[i][2]==0) { 	// -r nnn
						i++;
						if (argv[i])
							g_nRecruitCost=atoi(argv[i]);
						else if (VERBOSE)
							fprintf(stderr,"Missing recruiting costs, set to %d",g_nRecruitCost);
					} else
						g_nRecruitCost=atoi(argv[i]+2);
					break;

				case 'c': compile=OUT_COMPILE; break;
				case 'C': combine_factions=1; break;
				case 'm': compile=OUT_MAGELLAN;	break;
				case 'u': s_bSaveAsMinimalAlso=TRUE; break;

				case 'E':
					if (dostop)	{ // avec des options par l'intermédiaire "; ECHECK" ne faire plus
						echo_it=1;
						fOUT=stdout;
						fERR=stdout;
					}
					break;

				case 'e':
					if (dostop)	{ // bei Optionen via "; ECHECK" ne faire plus
						echo_it=1;
						fOUT=stdout;
						fERR=stderr;
					}
					break;

				case 'i': 
						if (argv[i][2]==0) {	
							i++;
							x=argv[i]; // "-i folder"
						} else							
							x=argv[i]+2; // "-ifolder"
						sMagellanFoldername=validFilename(x); 
						break;

				case 'O':
					if (dostop)	{ // bei Optionen via "; ECHECK" nicht mehr machen
						if (argv[i][2]==0) {	
							i++;
							x=argv[i]; // "-O file"
						} else							
							x=argv[i]+2; // "-Ofile"
						sFilename=validFilename(x); 
						if (sFilename.IsEmpty()) {
							fputs("Using stderr for error output\n",stderr);
							fERR=stderr;
							break;
						}

						int errnum=fopen_s(&fERR,sFilename,"w");
						if (errnum!=0) {
							char buferr[255];
							strerror_s(buferr,255,errno);
							fprintf(stderr,"Can't write to file `%s':\n	%s",(LPCSTR)sFilename,buferr);
							onExitProg(0);
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
						echo_it=1;
						sFilename=validFilename(x); 
						if (sFilename.IsEmpty()) {
							fputs("Empty file for checked file, using stdout\n",stderr);
							fOUT=stdout;
							break;
						}
						int errnum=fopen_s(&fOUT,sFilename,"w");
						if (errnum!=0) {
							char buferr[255];
							strerror_s(buferr,255,errno);
							fprintf(stderr,"Can't write to file `%s':\n	%s",(LPCSTR)sFilename,buferr);
							onExitProg(0);
						}
					}
					break;

				case 'p':
					piping=1;
					break;

				case 'x':
					line_start=1;
					break;

				case 'w':
					if (command_line==1 || warnings_cl==0) {
						if (argv[i][2])
							show_warnings=(char)atoi(argv[i]+2);
						else {
							if (argv[i+1] && isdigit(*argv[i+1])) {
								i++;
								show_warnings=(char)atoi(argv[i]);
							} else
								show_warnings=0;
						}
					}
					if (command_line==1)
						warnings_cl=1;
					break;

				case 's':
					if (dostop)	// bei Optionen via "; ECHECK" nicht mehr machen
						fERR=stderr;
					break;

				case 'n':
					if (strlen(argv[i])>2) {
						if (argv[i][3]==0) {	// -no xxx
							i++;
							x=argv[i];
						} else
							x=argv[i]+3;
						if (!x) {
							fputs("-no ???\n",stderr);
							break;
						}
						switch (*x) {
							case 's':
								noship=1;
								break;
							case 'r':
								noroute=1;
								break;
							case 'l':
								nolost=1;
								break;
						}
					} else
						ignore_NameMe=1;
					break;

				case '-':
					if (strcmp(argv[i]+2,"help")==0) {		// '--help'
						if (dostop)		// bei Optionen via "; ECHECK" nicht mehr machen
							printhelp(argc,argv,i);
						else
							output("Unknow option `%s'\n", argv[i]);
							if (dostop)		// Nicht stoppen, wenn dies die Parameter aus der Datei selbst sind! */
								onExitProg(10);
					}
					break;

				case '?':
				case 'h':
					if (dostop)		// bei Optionen via "; ECHECK" nicht mehr machen
						printhelp(argc,argv,i);
					break;

				case 'L':
					if (argv[i][2]==0) { 	// -L loc
						i++;
						if (argv[i])
							echeck_locale=argv[i];
						else
							fputs("Missing locale, using 'ne'\n", stderr);
					} else {
						if (argv[i][2])
							echeck_locale=argv[i]+2;
						else
							fputs("Missing locale, using 'en'\n", stderr);
					}
					break;
				
				/*case 'F': 
						if (argv[i][2]==0) {	
							i++;
							x=argv[i]; // "-F langfolder"
						} else							
							x=argv[i]+2; // "-Flangfolder"
						sLangFoldername=validFilename(x); 
				*/
				case 'H':
					if (argv[i][2]==0){ // -H hierarchy
						i++;
						sHierarchyFilename=validFilename(argv[i]);
					} else {
						if (argv[i][2])
							sHierarchyFilename=validFilename(argv[i]+2);
					}
					if (sHierarchyFilename.IsEmpty())
						fputs("Missing hierarchy file\n", stderr);
					break;

				case 'T':
					if (argv[i][2]==0) { 	// -T extras.txt
						i++;
						sExtraOrdersFilename=validFilename(argv[i]);
					} else {
						if (argv[i][2])
							sExtraOrdersFilename=validFilename(argv[i]+2);
					}
					if (sExtraOrdersFilename.IsEmpty())
						fputs("Missing extra orders file\n", stderr);
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

				case 'R':
					if (argv[i][2]==0) { 	// -R CRFilename // output CR
						i++;
						sCROutFilename=validFilename(argv[i]);
					} else {
						if (argv[i][2])
							sCROutFilename=validFilename(argv[i]+2);
					}
					if (sCROutFilename.IsEmpty())
						fputs("Missing output CR file\n", stderr);
					break;

				case 'X':
					s_bUseCROnly = TRUE;
					break;

				case 'S':
					s_bOutputCR = TRUE;
					break;

				case 'W':
					CShip::s_bShow_lbs_details = TRUE;
					break;

				case 'U':
					CUnit::s_bShow_lbs_details = TRUE;
					break;

				default:
					if (argv[i][1]) {		// sonst ist das nur '-' => stdin lesen
						output("Unknown option `%s'\n", argv[i]);
						if (dostop)		// Nicht stoppen, wenn dies die Parameter aus der Datei selbst sind!
							onExitProg(10);
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

CString wordAfter(char sep, const char *buff)
{
	CString sRes("");	
	char *x=strchr((char *)buff, sep);
	if (x) {
		x++;
		while (my_isspace(*x)) x++;
		char *z=strchr(x,'\n');
		if (z) *z=0;
		sRes=(LPCSTR)(x);
	}
	return sRes;
}


void ChecksAfterFaction(BOOL bAllFActionsAtOneTime)
{
	ERTRACE("ChecksAfterFaction");

	// outputln("Processing last checkings...");
	s_bOrderInErrors = false;

	CUnit::checkings();

	ERTRACE("ChecksAfterFaction:end");
}

void Statistics()
{
	//TODO:customize sort options
	if (has_stats)
	{
		ERTRACE("statistics...");
		if (s_settings.m_stats.m_bRegions) sRegionsList.Stats(SORT_ON_POOLSILVER); 
		if (s_settings.m_stats.m_bBuildings) sBuildingsList.Stats(SORT_ON_TYPE);
		if (s_settings.m_stats.m_bShips) sShipsList.Stats(SORT_ON_TYPE);
		if (s_settings.m_stats.m_bFactions) sFactionsList.Stats(SORT_NONE);
	}
	//if (s_settings.m_stats.m_bTeachings) sTeachsList.Stats();
	//if (s_settings.m_stats.m_bMoves) sUnitsList.StatsMoves();
	ERTRACE("end statistics");
}

BOOL setNextFaction()
{
	//scat(printparam(P_NEXT));
	indent=next_indent=INDENT_FACTION;
	porder();
	if (combine_factions == 0)
		ChecksAfterFaction(FALSE);
	return TRUE;
}

BOOL check_region()
{
	CString sRegionName("");
	const char *x;
	// Marks the temp flag of units of the last region.
	// wrong: temp numbers with GIVES or is noticeable in such a way
	if (combine_factions==0 && sRegionsList.GetCount()>0)
		sUnitsList.RemoveTempStatus(NULL/*sCurrentFaction*/);
	if (echo_it) {
		fputs(sCurrentFile->buffer(),fOUT);
		putc('\n',fOUT);
	}
	x = getstr();
	if (*x) {
		if (!isdigit(*x) && (*x!='-'))
			Rx=Ry=LOCATION_UNKNOWN_VALUE; // REGION without coordinates - e.g. branch ral level
		else {
			Rx=atoi(x);
			x=strchr(sCurrentFile->buffer(), ',');
			if (!x) {
				x=strchr(sCurrentFile->buffer(), ' ');
				if (x) 
					x=strchr(++x, ' ');		// 2. Space is disconnecting switch?
				else 
					x=getstr();
			}
			if (x && *(++x))
				Ry=atoi(x);
			else
				Ry=LOCATION_BAD_VALUE;
		}
	} 
	else
		Rx=Ry=LOCATION_BAD_VALUE;
;
	if (Rx<LOCATION_UNKNOWN_VALUE || Ry<LOCATION_UNKNOWN_VALUE)
		Warning(ERRORREGION,0);
	sRegionName = wordAfter(';',sCurrentFile->buffer());
	pCurrentRegion=sRegionsList.Add(Rx, Ry, 0, sRegionName);
	outputSetRegionName(pCurrentRegion->outputName());
	return TRUE;
}

void addtoken(tnode *root, const char *str, int id) {
	static char buf[1024];
	static struct replace {
		char c;
		char *str;
	} replace[] = {
		{'ä', "ae"},
		{'Ä', "ae"},
		{'ö', "oe"},
		{'Ö', "oe"},
		{'ü', "ue"},
		{'Ü', "ue"},
		{'ß', "ss"},
		{ 0, 0}
	};
	if (root->id>=0 && root->id!=id && !root->leaf) root->id=-1;
	if (!*str) {
		root->id=id;
		root->leaf=1;
	} else {
		char c=(char)tolower(*str);
		int lindex=((unsigned char)c) % 32;
		int i=0;
		tnode *tk=root->m_pNext[lindex];
		if (root->id<0) root->id=id;
		while (tk && tk->c != c) tk=tk->m_pNexthash;
		if (!tk) {
			tk=(tnode*)calloc(1, sizeof(tnode));
			tk->id=-1;
			tk->c=c;
			tk->m_pNexthash=root->m_pNext[lindex];
			root->m_pNext[lindex]=tk;
		}
		addtoken(tk, str+1, id);
		while (replace[i].str) {
			if (*str==replace[i].c) {
				strcpy_s(buf, 1024, replace[i].str);
				strcat_s(buf, 1024, str+1);
				addtoken(root, buf, id);
				break;
			}
			i++;
		}
	}
}

void initHerbsToken()
{
	t_liste *l;
	int i;
	if (s_bHerbTokensInited) return;
	s_bHerbTokensInited = TRUE;
	for (i=0,l=herbnames; l; l=l->m_pNext,++i)
		addtoken(&tokens[UT_HERB], l->name, i);
}

void initItemsToken()
{
	int i;
	t_item *l;
	if (s_bItemTokensInited) return;
	s_bItemTokensInited = TRUE;
	for (i=0,l=itemdata; l; l=l->m_pNext,++i)
		addtoken(&tokens[UT_ITEM], l->name, i);
}

void inittokens() {
	int i;
	t_building *b;
	t_ship *s;
	//t_item *it;
	t_skills *sk;
	t_potion *po;
	t_liste *l;
	t_direction *d;
	t_params *p;
	t_keyword *kw;
	
	ERTRACE("inittokens");

	initItemsToken();

	for (p=parameters; p; p=p->m_pNext)	
		addtoken(&tokens[UT_PARAM], p->name, p->param);
	for (i=0,sk=skilldata; sk; sk=sk->m_pNext,++i) addtoken(&tokens[UT_SKILL], sk->name, i);

	sSpellsList.toTokens();

	for (i=0,d=directions; d; d=d->m_pNext,++i)	addtoken(&tokens[UT_DIRECTION], d->name, d->dir);
	for (kw=keywords; kw; kw=kw->m_pNext) addtoken(&tokens[UT_KEYWORD], kw->name, kw->keyword);
	for (i=0,b=buildingdata; b; b=b->m_pNext,++i) addtoken(&tokens[UT_BUILDING], b->name, i);
	for (i=0,s=shipdata; s; s=s->m_pNext,++i) addtoken(&tokens[UT_SHIP], s->name, i);

	initHerbsToken();

	for (i=0,po=potiondata; po; po=po->m_pNext,++i)	addtoken(&tokens[UT_POTION], po->name, i);
	for (i=0,l=Races; l; l=l->m_pNext,++i) addtoken(&tokens[UT_RACE], l->name, i);
	for (i=0,l=options; l; l=l->m_pNext,++i) addtoken(&tokens[UT_OPTION], l->name, i);
}

void freeArrays() 
{
	ERTRACE("freeArrays");

	t_liste *l;
	while (itemdata) { t_item *it = itemdata->m_pNext; free(itemdata->name); free(itemdata); itemdata = it;}
	while (parameters) { t_params *p = parameters->m_pNext; free(parameters->name); free(parameters); parameters = p;}
	while (skilldata) { t_skills *sk = skilldata->m_pNext; free(skilldata->name); free(skilldata); skilldata = sk;}
	while (directions) { t_direction *d = directions->m_pNext; free(directions->name); free(directions); directions = d;}
	while (keywords) { t_keyword *kw = keywords->m_pNext; free(keywords->name); free(keywords); keywords = kw;}
	while (buildingdata) { t_building *b = buildingdata->m_pNext; free(buildingdata->name); free(buildingdata); buildingdata = b;}
	while (shipdata) { t_ship *s = shipdata->m_pNext; free(shipdata->name); free(shipdata); shipdata = s;}
	while (potiondata) { t_potion *po = potiondata->m_pNext; free(potiondata->name); free(potiondata); potiondata = po;}
	while (Races) { l = Races->m_pNext; free(Races->name); free(Races); Races = l;}
	while (options) { l = options->m_pNext; free(options->name); free(options); options = l;}
	while (herbnames) { l = herbnames->m_pNext; free(herbnames->name); free(herbnames); herbnames = l;}

	while (config) { t_config *c = config->m_pNext; free(config->name); free(config); config = c;}

	while (help) { l = help->m_pNext; free(help->name); free(help); help = l;}

	while (factions) { 
		t_faction *f = factions->m_pNext; 
		free(factions->id); 
		free(factions->pswd);
		free(factions->race); 
 		free(factions); 
		factions = f;
	}
	tokens;

	for (int i=0;i<UT_MAX;i++)
	{
		tnode *t = &tokens[i]; 
		for (int j=0;j<32;j++)
			if (t->m_pNext[j])
				free (t->m_pNext[j]);
		// free (t); // error here !
	}
	
	int yyy=1;
	for (int i=0; i < MAX_ERRORS; i++) 
	{
		if (s_errorMsgs[i]) 
			free (s_errorMsgs[i]);
		s_errorMsgs[i] = NULL;
	}

	if (help_caption) free(help_caption);
	if (help_path) free(help_path);
	//if (path) free(path);
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


BOOL loadFactionsFromCR()
{
	ERTRACE("loadFactionsFromCR");

	if (!sMagellanFoldername.IsEmpty())
		cr().setMagellanFoldername(sMagellanFoldername);

	block * bf = cr().factions();
	while (bf)
	{

		CString sPswd("");
		if (!cr().b_isFaction(bf))
			break;
		int nFactionID = bf->ids[0];
		CFaction * f = sFactionsList.Add(bf,cr().factionPassword(nFactionID));
		if (f)
		{
			t_faction * ff = findFactionFromDescribed(nFactionID, "");
			if (ff) 
			{
				f->setKerubinStatus(ff->kerubinStatus);
				f->setOutConfirm(ff->nOutConfirm);
			}
		}
		bf = bf->next;
	}
	ERTRACE("loadFactionsFromCR:end");
	return TRUE;
}

int checkOrders(int argc, char *argv[]) {
	int i, faction_count=0, unit_count=0, nextarg;

	setlocale(LC_CTYPE,"French"); //CG - to accept 'é' in names

	initSettings();
	CFaction::loadRaces();

	fERR=stderr;

	for (i=0; i<MAX_ERRORS; i++)
		s_errorMsgs[i]=_strdup(Errors[i]);	// with defaults occupied, because ZERO - crash

	// Path-Handling
	size_t lenPath;
	errno_t errPath = _dupenv_s( &path, &lenPath, "ECHECKPATH");
	if (errPath || !path)
		path=DEFAULT_PATH;

	fERR=stdout;
	char *filename;
	size_t lenFile;
	errno_t errFile = _dupenv_s( &filename, &lenFile, "ECHECKOPTS");
	if (!errFile && filename)
		parse_options(filename,1);

	if (argc<=1)
		printhelp(argc, argv, 0);

	if (argc>1)
		nextarg=check_options(argc, argv, 1, 1);

	readfiles(1);

	if (!(filesread & HAS_MESSAGES)) {
		ERTRACE("*** error *** cannot read any file");
		fprintf(fERR, "\n  **  ECheck V%s.%s.%sb%s %s**\n\n can't read any file!\n\n Pfad / Path: '%s/%s'\n\n", MAINVERSION, MINVERSION, PATCHLEVEL, BUILDNUM, __DATE__, path, echeck_locale);
		help_keys('f');		// help_keys() macht exit()
	}

	if (!compile)
		output(ECHECK, "%s %s.%s.%sb%s %s", MAINVERSION, MINVERSION, PATCHLEVEL, BUILDNUM, __DATE__);

	outputCommandLine(argc,argv);

/*
for(i=0;i<MAX_ERRORS;i++)printf("%3d: %s -> %s\n",i,Errors[i],errtxt[i]?errtxt[i]:"--------------");
return 42;
*/
	if (filesread!=HAS_ALL) {
		ERTRACE("*** error *** missing some files");
		fprintf(fERR,"%s\n",_msg(MISSINGFILES));
		if (!(filesread & HAS_PARAM)) fprintf(fERR," %s\n",_msg(MISSFILEPARAM));
		if (!(filesread & HAS_ITEM)) fprintf(fERR," %s\n",_msg(MISSFILEITEM));
		if (!(filesread & HAS_SKILL)) fprintf(fERR," %s\n",_msg(MISSFILESKILL));
		if (!(filesread & HAS_KEYWORD)) fprintf(fERR," %s\n",_msg(MISSFILECMD));
		if (!(filesread & HAS_DIRECTION)) fprintf(fERR," %s\n",_msg(MISSFILEDIR));
		if (!(filesread & HAS_MESSAGES)) fprintf(fERR," %s\n",_msg(MISSFILEMSG));
		return 5;
	}

	inittokens();

	ERTRACE("CRFilename %s",(LPCSTR)sCRFilename);

	if (!sCRFilename.IsEmpty())
	{
		if (!STExistFile(sCRFilename))
			consoleError("error: %s file not found\n",(LPCSTR)STGetFullPathname(sCRFilename));

		if (STGetFileExtension(sCRFilename).CompareNoCase(".cr"))
			consoleError("error: %s file should have .cr extension\n",(LPCSTR)STGetFullPathname(sCRFilename));

		if (STGetFileExtension(sHierarchyFilename).CompareNoCase(".crh"))
			consoleError("error: %s file should have .crh extension\n",(LPCSTR)STGetFullPathname(sHierarchyFilename));

		if (!STExistFile(sHierarchyFilename))
			consoleError("error: %s file not found\n",(LPCSTR)STGetFullPathname(sHierarchyFilename));

		if (!sCRFile.Open(sCRFilename,sHierarchyFilename))
			consoleError("error: %s file could not be opened\n",(LPCSTR)STGetFullPathname(sCRFilename));

		CRegion::loadLuxuriesIDs();
		addTranslations();

	}

	if (s_bArchiveCR)
	{
		// TODO
	}

	if (s_bOutputCR && sCROutFilename.IsEmpty())
		sCROutFilename = sCRFile.ResultFilename();

	ERTRACE("CR Result Filename %s",(LPCSTR)sCROutFilename);

	if (!sCROutFilename.IsEmpty())
		cr().setResultFilename(sCROutFilename);

	sCurrentFile = NULL;

	CUnit::loadUTIDs();

	if (!s_bUseCROnly)
	{
		for (i=nextarg; i<argc; i++) {
			sCurrentFile = new COrdersFile(argv[i]);
			if (!sCurrentFile->open())
				return 2;
			Rx=Ry=-10000;
			sCurrentFile->process();
			unit_count += sCurrentFile->nbUnits();
		}

		// If there were no inputs files, F is still set on stdin
		if (sCurrentFile == NULL)
		{
			sCurrentFile = new COrdersFile(NULL);
			sCurrentFile->open();
			Rx=Ry=-10000;
			sCurrentFile->process();
			unit_count += sCurrentFile->nbUnits();
		}
	}
	else
	{
		// TODO
		Rx=Ry=-10000;
		sCurrentFile = &sCRFile;
		loadFactionsFromCR();
		sCRFile.process();
		unit_count = sCRFile.nbUnits();
		ERTRACE("nbunits=%u...",unit_count);
	}

	if (!sExtraOrdersFilename.IsEmpty())
	{
		s_bExtraOrders = TRUE;
		CCREresseaFile * pPrevFile = sCurrentFile;
		sCurrentFile = new COrdersFile(sExtraOrdersFilename);
		if (!sCurrentFile->open())
			return 2;
		Rx=Ry=-10000;
		sCurrentFile->process();
		sCurrentFile=pPrevFile;
		s_bExtraOrders = FALSE;
		//unit_count += sCurrentFile->nbUnits();
	}

	if (combine_factions == 1)
		ChecksAfterFaction(TRUE);

	sErrorsList.Show();

	int nNbTotalUnitsWithOrders = 0;
	faction_count = sFactionsList.nbWithOrders(nNbTotalUnitsWithOrders);

	if (compile) {
		output("%s|warnings|%d\n%s|errors|%d\n",sCurrentFile?sCurrentFile->filename():"unknown",warning_count,sCurrentFile?sCurrentFile->filename():"unknown",error_count);
		return 0;
	}

	BOOL bSaved = FALSE;
	if (cr().toSave())
	{
		bSaved = TRUE;
		cr().preSave();

		sBuildingsList.save();
		sRoadsList.save();
		sRegionsList.save();
		sMessagesList.save();
		sShipsList.save();
		sUnitsList.save();

		cr_write = cr_writeblock_echeck;
		cr().save();
		if (s_bSaveAsMinimalAlso) 
		{
			cr_write = cr_writeblock_echeck_min;
			cr().save(NULL,TRUE,TRUE);
		}
	}

	Statistics();

	if (bSaved) 
		output("\n%s saved.\n",cr().resultFilename());

	fprintf(fERR, _msg(ORDERSREAD),
						faction_count, faction_count != 1 ? _msg(FACTIONS) : printparam(P_FACTION),
						unit_count, unit_count != 1 ? _msg(UNITS) : printparam(P_UNIT));

	if (unit_count==0) {
		output(CHECKYOURORDERS);
		return -42;
	}

	if (!error_count && !warning_count && faction_count && unit_count)
		output(ORDERSOK);

	if (error_count>1)
		fprintf(fERR, _msg(FOUNDERRORS), error_count);
	else if (error_count==1)
		output(FOUNDERROR);

	if (warning_count) {
		if (error_count)
			output(AND);
		else
			output(THERE);
	}

	if (warning_count>1) {
		if (!error_count)
			output(WERE);
		output(WARNINGS, warning_count);
	} else if (warning_count==1) {
		if (!error_count)
			output(WAS);
		output(AWARNING);
	}

	if (warning_count || error_count)
		output(DISCOVERED);

	//sRegionsList.RemoveAll();
	//sUnitsList.RemoveAll();
	//sFactionsList.RemoveAll();
	//sMessagesList.RemoveAll();
	//sBuildingsList.RemoveAll();
	//sShipsList.RemoveAll();

	freeArrays();

#ifdef AMIGA
	if (error_count>0)
		return 10;	// FAIL beim AMIGA
	if (warning_count>0)
		return 5;	// WARN beim AMIGA
#endif
	return 0;
}

BOOL checkBuffer(CString & sBuffer) 
{
	char lbuf[MAXLINE];
	BOOL cont = FALSE;
	BOOL quote = FALSE;
	BOOL bModif = FALSE;
	char * cp = lbuf;
	char * bp = lbuf;
	char * lwarnbuf = cp;

	lbuf[MAXLINE-1] = '@';

	BOOL eatwhite = TRUE;
	BOOL start = TRUE;
	strcpy_s(lbuf,4096,UTF8_to_ansi(sBuffer));

	cont=FALSE;
	while (cp!=lwarnbuf+MAXLINE && bp!=lbuf+MAXLINE && *bp) {
		if (my_isspace(*bp)) {
			if (eatwhite) {
				do { ++bp; } while (bp!=lbuf+MAXLINE && my_isspace(*bp));
				if (!quote && !start) 
					*(cp++)=' ';
				if (bp!=cp)
					bModif = TRUE;
			} else {
				do {
					*(cp++)=SPACE_REPLACEMENT;
					++bp;
				} while (cp!=lwarnbuf+MAXLINE && bp!=lbuf+MAXLINE && my_isspace(*bp));
				bModif = TRUE;
			}
		} 
		else 
		{
			cont=FALSE;
			if (*bp=='"') {
				quote = !quote;
				eatwhite=TRUE;
			} 
			else 
			{
				if (*bp=='\\') 
					cont=TRUE;
				else
				if (!my_iscntrl(*bp)) { //ASSERT debug for UTF8 chars
					*(cp++) = *bp;
					eatwhite = !quote;
				}
			}
			++bp;
		}
		start=FALSE;
	}
	*cp=0;

	if (quote)
	{
		// keep original string in case of a missing quote (bModif not taken into account)
		// error message called later (when processing orders)
		//Error(MISSINGQUOTES,(LPCSTR)sUnitInfo,(LPCSTR)sBuffer);
		return TRUE;
	}
	if (bModif)
		sBuffer = lwarnbuf;
	return TRUE;
}

BOOL SetNextRoute(CString & sOrder)
{
	// change & keep ROUTE commands
	// ROUTE xxx yyy PAUSE zzz PAUSE ==> ROUTE zzz PAUSE xxx yyy PAUSE

	CString sOrderUpper(sOrder);
	sOrderUpper.MakeUpper();
	int nInd=sOrderUpper.Find("ROUTE");

	if (nInd!=-1)
	{
		sOrderUpper = sOrderUpper.Mid(nInd+6);
		int n2 = sOrderUpper.Find("PAUSE");
		if (n2!=-1)
		{
			CString sprf = sOrderUpper.Left(n2+6);
			if (n2+6 >= sOrderUpper.GetLength())
				return FALSE;
			CString ssuf = sOrderUpper.Mid(n2+6);
			sOrder = "ROUTE "+ssuf+" "+sprf;
			sOrder.TrimLeft();
			sOrder.TrimRight();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL isKeepingOrder(CString & sOrder, BOOL & bModified, BOOL & bMadeTemp)
{
	// comments
	bModified = FALSE;
	if (sOrder.IsEmpty()) 
		return FALSE;
	if (sOrder.GetAt(0)==';') 
		return FALSE;
	if (sOrder.GetAt(0)=='/' && sOrder.Find("// ")==0) 
		return TRUE; // keep // comments
	if (!checkBuffer(sOrder)) 
		return FALSE;

	sOrder.TrimLeft();
	sOrder.TrimRight();
	if (sOrder.IsEmpty()) 
		return FALSE;

	// keep @commands
	if (sOrder.GetAt(0)=='@') 
		return TRUE;
	CString sOrderUpper(sOrder);
	sOrderUpper.MakeUpper();

	int i=igetkeyword(sOrderUpper);

	switch (i) {
		case K_WORK:
		case K_RESEARCH:
		case K_BUY:
		case K_SELL:
		case K_TEACH:
		case K_LEARN:
		case K_TAX:
		case K_ENTERTAIN:
		case K_CAST:
		case K_RIDE:
		case K_CARRY: return TRUE;
		case K_ATTACK:
		case K_STEAL:
		sOrder = "// DONE: "+sOrder;
		bModified=TRUE;
		return TRUE;
		case K_MAKE:  if (findparam(getstr())!=P_TEMP) return TRUE; bMadeTemp=TRUE;
		case K_ROUTE: if (SetNextRoute(sOrder)) {bModified=TRUE; return TRUE;} //TODO: ROUTE without PAUSE; nbmoves must be known
		default: return FALSE;
	}

	return FALSE;
}

BOOL isValidOrder(CString & sOrder)
{
	// comments
	if (sOrder.IsEmpty()) 
		return FALSE;
	if (sOrder.GetAt(0)==';') 
		return FALSE;
	if (sOrder.GetAt(0)=='/' && sOrder.Find("// ")==0) 
		return FALSE;
	if (!checkBuffer(sOrder)) 
		return FALSE;
	sOrder.Replace("\t"," ");
	sOrder.TrimLeft();
	sOrder.TrimRight();
	return (!sOrder.IsEmpty());
}


/////////////////////////////////////////
// output & error management

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

void output(int nErrorNum, ...)
{
	if (nErrorNum < MAX_ERRORS && nErrorNum >=0)
	{
		va_list argList;
		va_start(argList, nErrorNum);
		outputvalist(s_errorMsgs[nErrorNum],argList);
		va_end(argList);
	}
}

void output(const char * pzFormat, ...)
{
	va_list argList;
	va_start(argList, pzFormat);
	outputvalist(pzFormat,argList);
	va_end(argList);
}

void outputln(int nErrorNum, ...)
{
	if (nErrorNum < MAX_ERRORS && nErrorNum >=0)
	{
		va_list argList;
		va_start(argList, nErrorNum);
		outputvalist(s_errorMsgs[nErrorNum],argList,TRUE);
		va_end(argList);
	}
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

void consoleError(const char * pzFormat, ...)
{
	va_list argList;
	va_start(argList, pzFormat);
	outputvalist(pzFormat,argList,FALSE,TRUE);
	va_end(argList);
	onExitProg(1);
}

////////////////////
////////////////////
// error management

BOOL Error(int nErrorNum, ...)
{
	ERTRACE("Error %d",nErrorNum);

	va_list argList;
	va_start(argList, nErrorNum);
	errorvalist(0,nErrorNum,s_bOrderInErrors?sCurrentFile->buffer():NULL,argList);
	va_end(argList);
	return FALSE;
}

BOOL Warning(int nErrorNum, int nLevel, ...)
{
	//int nLineNumber = gLine_no;
	if (nErrorNum < 0) {
		nErrorNum=-nErrorNum;
		if (CCRErrors::s_bBasicErrors==1)
			return FALSE;
	}
	va_list argList;
	va_start(argList, nLevel);
	errorvalist(nLevel,nErrorNum,s_bOrderInErrors?sCurrentFile->buffer():NULL,argList);
	va_end(argList);
	return FALSE;
}

BOOL itemIsHerb(const char *pName, int & nHerbID)
{
	int nType = UT_HERB;
	return itemKind(pName, nHerbID, nType);
}

// check whether it is an item, herb, or potion
BOOL itemKind(const char *pName, int & i, int &nType)
{
	int nTypeFirst = UT_ITEM;
	int nTypeLast = UT_POTION;
	int nInitialType = nType;
	if (nType!=-1)	nTypeFirst = nTypeLast = nType;
	for (int n=nTypeFirst;n<=nTypeLast;n++)
	{
		char *s = NULL;
		int ii = findtoken(pName,n);
		if (ii >= 0) s = findName(ii,n);
		if (!s || _strcmpi(pName,s)) 
		{
			if (s)
				int hhjjkkll=1;
			continue;
		}
		i = ii;
		nType = n;
		return TRUE;
	}
	i=-1;
	nType=nInitialType;
	return FALSE;
}

////////////////////
