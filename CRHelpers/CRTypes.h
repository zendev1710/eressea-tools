// CRTypes.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRTYPES_H__BE85D52A_BBC6_46D8_A384_7A5197A4DE9D__INCLUDED_)
#define AFX_CRTYPES_H__BE85D52A_BBC6_46D8_A384_7A5197A4DE9D__INCLUDED_

//added
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define RACE_RECRUIT_COST_AQUARIAN	(80)
#define RACE_RECRUIT_COST_CAT		(90)
#define RACE_RECRUIT_COST_DEMON		(150)
#define RACE_RECRUIT_COST_DWARF		(110)
#define RACE_RECRUIT_COST_ELF		(130)
#define RACE_RECRUIT_COST_HALFLING	(80)
#define RACE_RECRUIT_COST_HUMAN		(75)
#define RACE_RECRUIT_COST_GOBLIN	(50)
#define RACE_RECRUIT_COST_INSECT	(80)
#define RACE_RECRUIT_COST_ORC		(70)
#define RACE_RECRUIT_COST_TROLL		(90)

#define FACTION_ID_UNKNOWN	(0)

#define ID_UNKNOWN			(-1)
#define VALUE_UNKNOWN		(-1)

#define REGIONNAME_ECHECK_NULLVALUE		_T("null")
#define S_VALUE_UNKNOWN_HERB			_T("unknown herb")
#define S_VALUE_UNKNOWN					_T("unknown")

#define LOCATION_MIN_VALUE (-9999)
#define LOCATION_MAX_VALUE (-LOCATION_MIN_VALUE)

#define LOCATION_UNKNOWN_VALUE (LOCATION_MIN_VALUE)
#define LOCATION_BAD_VALUE     (LOCATION_MIN_VALUE-1)

//#define itob(i) (CCR::IDToString(i))
#define itob(i) (IDToString(i))

#define LBS_RATIOF (100.0F)
#define LBS_RATIO (100)

#define HORSE_WEIGHT			(50*LBS_RATIO)
#define CART_WEIGHT				(40*LBS_RATIO)
#define MAN_WEIGHT				(10*LBS_RATIO)
#define GOBLIN_WEIGHT			(6*LBS_RATIO)
#define TROLL_WEIGHT			(20*LBS_RATIO)

#define HORSE_CAPACITY			(20*LBS_RATIO)
#define CART_CAPACITY			(140*LBS_RATIO)
#define MAN_CAPACITY			((int)(5.40F*LBS_RATIOF))
#define GOBLIN_CAPACITY			((int)(4.40F*LBS_RATIOF))
#define TROLL_CAPACITY			((int)(10.80F*LBS_RATIOF))
#define MAGIC_BAG_CAPACITY		(200*LBS_RATIO)

inline float displayedWeight(int nw) { return (float)(nw)/LBS_RATIOF;}
inline int nDisplayedWeight(int nw) { return (int)(displayedWeight(nw)+0.5F);}

typedef int eID;

typedef enum eCoast
{
	COAST_NONE = -1, //just created
	COAST_NW = 0,
	COAST_NE,
	COAST_E,
	COAST_SE,
	COAST_SW,
	COAST_W,
	COAST_UNKNOWN=99
} eCoast;

typedef	enum eEffect
{
	EFT_BUSYBEER,
	EFT_BRAINWAX,
	EFT_PEASANT_BLOOD,
	EFT_MAX
} eEffect;

typedef	enum eSkill
{
	SK_NONE=-1,
	SK_ALCHEMY=0,
	SK_ARMOURSMITHING,
	SK_BOW,
	SK_CARTMAKING,
	SK_CATAPULT,
	SK_CROSSBOW,
	SK_ENDURANCE,
	SK_ENTERTAIN,
	SK_ESPIONAGE,
	SK_FORESTRY,
	SK_HERBALISM,
	SK_MAGIC,
	SK_MASONRY,
	SK_MELEE,
	SK_MINING,
	SK_PERCEPTION,
	SK_POLEARM,
	SK_QUARRYING,
	SK_RIDING,
	SK_ROADWORK,
	SK_SAILING,
	SK_SHIPCRAFT,
	SK_STEALTH,
	SK_TACTICS,
	SK_TAMING,
	SK_TAX,
	SK_TRADE,
	SK_WEAPONSMITHING,
	SK_UNARMED_COMBAT,
	SK_MAX
} eSkill; 

typedef int TSkillsTab[SK_MAX];

typedef enum eItem
{
	IT_SILVER=0,
	IT_WOOD,
	IT_STONE,
	IT_IRON,
	IT_HORSE,
	IT_MALLORN,
	IT_LAEN,
	IT_CART,
	IT_CATAPULT,
	IT_CATAPULT_AMMUNITION,
	IT_SEED,
	IT_ADAMANTIUM,
	IT_NBMAX
} eItem;

typedef enum ePotion
{
	PO_GOLIATH_WATER=0,				// weight capacity		
	PO_SEVEN_MILE_TEA,
	PO_OF_TRUTH,
	PO_WATER_OF_LIFE,				// saplings grow
	PO_PEASANT_BLOOD,				// demon recruitment		
	PO_BUSYBEER,
	PO_OINTMENT,					// healing
	PO_BERSERKERS_BLOOD,
	PO_DUNCEBUN,
	PO_BRAIN_WAX,					// learn facilities	
	PO_OF_NEST_WARMTH,		// insect recruitment
	PO_HORSEPOWER,
	PO_NBMAX
} ePotion;

typedef enum eItemType
{
	ITT_WEAPON=1,
	ITT_ARMOUR=2,
	ITT_HERB=4,
	ITT_POTION=8,
	ITT_LUXURY=16,
	ITT_RESOURCE=32,
	ITT_ITEM=64,
	ITT_ALL=255
} eItemType;

typedef enum eDirection {
	D_EAST,
	D_WEST,
	D_PAUSE,
	D_NORTHEAST,
	D_NORTHWEST,
	D_SOUTHEAST,
	D_SOUTHWEST,
	MAXDIRECTIONS
} eDirection;

#define IT_LASTCOST			(IT_LAEN+1)

inline eCoast coastNeighbour(eCoast n)
{
	if (n==COAST_E) return COAST_W;
	if (n==COAST_W) return COAST_E;
	if (n==COAST_NE) return COAST_SW;
	if (n==COAST_SW) return COAST_NE;
	if (n==COAST_NW) return COAST_SE;
	if (n==COAST_SE) return COAST_NW;
	return COAST_UNKNOWN;
}

inline eCoast coastFrom(const CPoint & s, const CPoint & t)
{
	if (t.x < s.x && t.y == s.y) return COAST_W;
	if (t.x > s.x && t.y == s.y) return COAST_E;
	if (t.x == s.x && t.y < s.y) return COAST_SW;
	if (t.x == s.x && t.y > s.y) return COAST_NE;
	if (t.x > s.x && t.y < s.y) return COAST_SE;
	if (t.x < s.x && t.y > s.y) return COAST_NW;
	return COAST_UNKNOWN;
}

extern eCoast directionToCoast(eDirection nDir);

inline eCoast coastTo(const CPoint & s, const CPoint & t)
{
	return coastNeighbour(coastFrom(s,t));
}

inline BOOL coastNear(eCoast s, eCoast t)
{
	if (s==t) return TRUE;
	unsigned int n=abs(s-t);
	if (n==1) return TRUE; 
	if (n==5) return TRUE; 
	return FALSE;
}

inline BOOL invalidID(eID n) { return n <= 0;}
inline BOOL unknownID(eID n) { return n == ID_UNKNOWN;}

extern const char * IDToString(int i);
extern eID newID();

#endif // !defined(AFX_CRTYPES_H__BE85D52A_BBC6_46D8_A384_7A5197A4DE9D__INCLUDED_)

