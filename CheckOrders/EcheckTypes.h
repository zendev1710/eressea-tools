#ifndef _ECHECKTYPES_H_
#define _ECHECKTYPES_H_

//added
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//added

#include "CRTypes.h"
#include "EchkMgr.h"

#define VALUE_ALL			(-1)

#define SPACE								' '
#define ESCAPE_CHAR					'\\'

#define FEEDSILVER_PER_MAN (10)

#define finditem(s) findtoken(s, UT_ITEM)
#define findherb(s) findtoken(s, UT_HERB)
#define findpotion(s) findtoken(s, UT_POTION)

#define potionname(i) PotionName(i)
#define herbname(i) (printliste(i, herbnames))

///////////////////////////////

extern BOOL itemKind(const char *pName, int & i, int &nType);
extern BOOL itemIsHerb(const char *pName, int & nHerbID);

extern int myatoi(const char *p, int nDefValue=0);
extern double myatof(const char *p, double fDefValue=0.0);
extern CString maxChars(const CString &s, int maxLen);

typedef int eUTID;

#define CAMOUFLAGED_PLAYER_ID (-1)

#define IDUT_UNKNOWN		(-1)
#define UTTYPE_UNKNOWN		(-1)

#define VALUE_NOTSPECIFIED	(-1)

class CRegion;

eID newTempID(const CRegion * pRegion, const char * pType, const char * pTempName);

inline eCoast directionToCoast(eDirection nDir)
{
	if (D_EAST == nDir) return COAST_E;
	if (D_WEST == nDir) return COAST_W;
	if (D_NORTHEAST == nDir) return COAST_NE;
	if (D_NORTHWEST == nDir) return COAST_NW;
	if (D_SOUTHEAST == nDir) return COAST_SE;
	if (D_SOUTHWEST == nDir) return COAST_SW;
	return COAST_NONE;
}

// n2 is considered as positive
#define positiveMin(n1,n2) min(n1<0?n2:(n2<0?n1:n2),n2<0?n1:(n1<0?n2:n1))

#define findbuildingtype(s) findtoken(s, UT_BUILDING)
#define findshiptype(s) 	findtoken(s, UT_SHIP);


#define SORT_NONE			(0)
#define SORT_ON_POOLSILVER	(1)
#define SORT_ON_MEN			(2)
#define SORT_ON_REGION		(4)
#define SORT_ON_TYPE		(8)

class CFaction;

typedef struct TDemonInfos
{
private:
	int m_nNbDemonFeed;
	int m_nNbFinalPeasantBlood;
	CFaction *m_pFaction;

public:
	TDemonInfos(CFaction *pFaction){m_pFaction=pFaction;m_nNbDemonFeed=0;m_nNbFinalPeasantBlood=0;}
	int addFeed(int nb) { m_nNbDemonFeed+=nb; return m_nNbDemonFeed;}
	int addNbFinalPeasantBlood(int nb) { m_nNbFinalPeasantBlood+=nb; return m_nNbFinalPeasantBlood;}

	BOOL is(CFaction *pFaction) const{ return pFaction==m_pFaction;}
	int feed() const { return m_nNbDemonFeed;}
	int finalPeasantBlood() const { return m_nNbFinalPeasantBlood;}
	CFaction * faction() { return m_pFaction;}

	TDemonInfos *m_pNext;

} TDemonInfos;

typedef struct TMoney
{
	int m_nMoney;
	int m_nInitialMoney;
	int m_nByTax;
	int m_nByEntertain;
	int m_nBySell;
	int m_nByWork;
	int m_nByBuy;
	int m_nReserved;
	int m_nMaintainCost;
	int m_nLearningCost;
	int m_nUpkeep;

	BOOL empty() const { 
		if (m_nMoney != 0) return FALSE;
		if (m_nByTax != 0) return FALSE;
		if (m_nByEntertain != 0) return FALSE;
		if (m_nByWork != 0) return FALSE;
		if (m_nBySell != 0) return FALSE;
		if (m_nByBuy != 0) return FALSE;
		if (m_nReserved != 0) return FALSE;
		return TRUE;
	}

	void reset(BOOL bAll=TRUE) 
	{ 	
		m_nUpkeep = m_nMoney = m_nByTax = m_nByEntertain = m_nByWork = m_nBySell = m_nByBuy = m_nReserved = m_nInitialMoney = 0;
		if (bAll) 
		{
			m_nMaintainCost = 0;
			m_nLearningCost = 0;
		}
	}
	
} TMoney;

/////////

#define NBMAXMOVE (255)

typedef struct TMoveInfo
{
	int m_nCount;
	BOOL m_bRouting;
	BOOL m_bPauseInRoute;
	BOOL m_bUseRoads;
	int m_nRoutingIndex;
	CPoint moves[NBMAXMOVE]; //moves[0]=source/from location

	void copy(const TMoveInfo & ts) 
	{
		m_nCount=ts.m_nCount; m_bRouting=ts.m_bRouting;m_bPauseInRoute=ts.m_bPauseInRoute;m_nRoutingIndex=ts.m_nRoutingIndex; m_bUseRoads=ts.m_bUseRoads;
		for(int i=0;i<NBMAXMOVE;i++) moves[i]=ts.moves[i];
	}

} TMoveInfo;

/////////

#define SPELL_CATEGORY_SHIP		(1)
#define SPELL_CATEGORY_DISTANCE (2)

typedef struct _faction {
	struct _faction *m_pNext;
	char * id;
	char * pswd;
	char * race;
	int kerubinStatus;
	int nOutConfirm;
} t_faction;

typedef struct t_spell_cost {
	int silver;
	int a;
	int pa;
	int al;
	char *other_items;
} t_spell_cost;

typedef struct _spell {
	struct _spell *m_pNext;
	char * name;
	char * secondname;
	char * magic_schools;
	char type; //  0 = "normal/ordinary" 1 = combat 2 = post-combat	3 = pre-combat spell
	int rank;
	int lvl;
	char category; //0=none,1=ship,2=distance
	t_spell_cost cost;
} t_spell;

#pragma once

#define UMLAUTE

enum {
	UT_NONE=0,
	UT_ITEM,
	UT_HERB,
	UT_POTION,
	UT_PARAM,
	UT_SKILL,
	UT_KEYWORD,
	UT_BUILDING,
	UT_RACE,
	UT_FACTION,
	UT_SPELL,
	UT_SHIP,
	UT_OPTION,
	UT_DIRECTION,
	UT_ERRORS,
	UT_HELP,
	UT_CONFIG,
	UT_TRANSLATION,
	UT_MAX
};

//////////

#define VERBOSE 0
#define MAINVERSION "1"
#define MINVERSION  "0"
#define PATCHLEVEL  "2"
#define BUILDNUM	"39"

#define DEFAULT_PATH "."
#define PATH_DELIM ";"
#define UMLAUTE

#define CHECKORDERS_VERSION MAINVERSION"."MINVERSION"."PATCHLEVEL"b"BUILDNUM

/////////////

#define DESCRIBESIZE 4095
#define NAMESIZE 127

#define addlist(L,p)	(p->m_pNext=*L, *L=p)
#define addlist2Reversed(p)	(p->m_pNext=m_pFirst, m_pFirst=p)

#define val(x)	(x!=0)

typedef struct _errorsSettings {
	BOOL m_bRegionName;
	BOOL m_bRegionLocation;
	BOOL m_bUnitName;
	BOOL m_bUnitID;
} t_errorsSettings;

typedef struct _statsSettings {
	BOOL m_bRegions;
	BOOL m_bFactions;
	BOOL m_bTeachings;
	BOOL m_bMoves;
	BOOL m_bBuildings;
	BOOL m_bShips;
} t_statsSettings;


typedef struct _settings {
	t_errorsSettings m_errs;
	t_statsSettings m_stats;
} t_settings;

typedef struct _ech_file {
	char * name;
	int type;
} t_ech_file;


typedef struct _keyword {
	struct _keyword *m_pNext;
	char * name;
	int keyword;
} t_keyword;

typedef struct _translation {
	struct _translation *m_pNext;
	char * name_en;
	char * name_de;
} t_translation;


typedef struct _params {
	struct _params *m_pNext;
	char * name;
	int param;
} t_params;

typedef struct _liste {
	struct _liste *m_pNext;
	char * name;
} t_liste;


typedef struct tnode {
	char c;
	bool leaf;
	int id;
	struct tnode *m_pNexthash;
	struct tnode *m_pNext[32];
} tnode;

typedef struct _direction {
	struct _direction *m_pNext;
	char * name;
	int dir;
} t_direction;

typedef struct _names {
	struct _names *m_pNext;
	char *txt;
} t_names;

typedef struct _upkeep {
	int silver;
	int woods;
	int stones;
	int horses;
} t_upkeep;

typedef struct _cost t_cost;

typedef struct _cost {
	int silver;
	int woods;
	int stones;
	int iron;
	int mallorn;
	int laen;
	int adamantium;
	int sum() const { return silver+woods+stones+iron+mallorn+laen+adamantium;}
	void copyTo(t_cost & t, int c=1) const	{t.silver=silver*c;t.woods=woods*c;	t.stones=stones*c;t.iron=iron*c;t.mallorn=mallorn*c;t.laen=laen*c;t.adamantium=adamantium*c;}
	void set(int v){silver=woods=stones=iron=mallorn=laen=adamantium=v;}
} t_cost;

/*
typedef struct TWEIGHT {
	int nWeight;
	BOOL b100th;
	TWEIGHT():nWeight(0),b100th(FALSE){}
} TWEIGHT;
*/

typedef struct _item {
	struct _item *m_pNext;
	char *name;
	int price;	// Biens de luxe - prix
	int minskill; // for items being made
	t_cost cost; // for producing item

	void parseWeight(const char *p){
		if (strchr(p,'.'))
			weight_k=(int)(myatof(p)*LBS_RATIOF+0.5);
		else
			weight_k=(int)(myatoi(p)*LBS_RATIO);
		weight_k=max(weight_k,0);
	}

	int weight() const { return weight_k;}
		
	private:
		int weight_k; // poids: 100th de lbs

} t_item;

typedef struct _potion {
	struct _potion *m_pNext;
	char *name;
	int nskill;
	int * herb_utids;
	int nbherbs;	
} t_potion;

typedef struct _building {
	struct _building *m_pNext;
	char * name;
	t_upkeep upkeep;
	t_cost cost;
	int masonryMinSkill;
	int maxsize;
	int capacity;
} t_building;

typedef struct _ship {
	struct _ship *m_pNext;
	char * name;
	int moverange;
	int capacity;
	int captainSkill;
	int totalSkill;
	int size;
	int shipcraftSkill;
} t_ship;

typedef struct _skills {
	struct _skills *m_pNext;
	char * name;
	int kosten;
} t_skills;


typedef struct _config {
	struct _config *m_pNext;
	char * name;
	BOOL value;
} t_config;

typedef struct list {
	struct list *m_pNext;
} list;

/////////////

extern void addtoken(tnode *root, const char *str, int id);
extern void * cmalloc(int n);

class CMyEresseaCR;

extern CMyEresseaCR & cr();

/////////////

extern tnode tokens[UT_MAX];

///////////////////////

extern char compile;

extern int g_nRecruitCost;

extern int Pow(int p);
extern int a_isspace(char c);
extern int ItemPrice(int i);
extern int findtoken(const char *str, int type);
extern char * printliste(int key, t_liste *L);
extern char * ItemName(int i);
extern int ItemWeight(int i);
extern int ItemMinSkill(int i);

extern char * PotionName(int i);

extern t_item * itemFromUTID(eUTID i);
extern t_ship * shipFromUTID(eUTID i);
extern t_building * buildingFromUTID(eUTID i);
extern t_potion * potionFromUTID(eUTID i);

extern t_spell * findSpell(const char *s, const char *sMagie, BOOL & bInMagicSchool);

extern char ignore_NameMe;

//extern char order_buf[BUFSIZE];
//extern char message_buf[BUFSIZE];
//extern char warn_buf[BUFSIZE];

extern t_liste *herbnames;

/*
extern void output(int nErrorNum, ...);
extern void outputln(int nErrorNum, ...);
extern void output(const char * pzFormat, ...);
extern void outputln(const char * pzFormat, ...);
*/

extern void outputSetRegionName(const char * pzRegionName);

//extern const char * _msg(int nErrorNum,...);

//extern BOOL errormsg(const char * pzFormat, ...);

//extern BOOL Error(int nErrorNum, ...);
//extern BOOL Warning(int nErrorNum, int nLevel, ...);
//extern BOOL errorvalist(int line, int nLevel, const char *order, const char *pzFormat, va_list argList);

extern BOOL region_error(int nLevel, int nErrorNum, va_list argList);

#endif