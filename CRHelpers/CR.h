// CR.h: interface for the CCR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CR_H__73DC631E_E2AF_4356_B466_40E522495569__INCLUDED_)
#define AFX_CR_H__73DC631E_E2AF_4356_B466_40E522495569__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "crdata.h"
#include "crparse.h"
#include "hierarchy.h"

#include "CRReport.h"

#define ERTRACE ERDebugOutput

#define DEFAULT_LINE_MAXLENGTH (80)

#define DIFF_NEW_UNIT (-99)

#define B_REGION_XY(x,y,br) {x = br->ids[0]; y = br->ids[1];}

////////////

extern int btoi(const char *s);
extern const char * extractName(CString & sName);
extern void ERDebugOutput(const char * lpszFormat, ...);

extern CString UTF8_to_ansi(const char *psvalue);
extern CString ansi_to_UTF8(const char *psvalue);


////////////

typedef struct TSaveOrderResults
{
	int m_nNbUnitsWithOrders;
	int m_nNbUnitsTotal;
	int m_nNbRegionsWithOrders;
	int m_nNbRegionsScanned;
	int m_nNbRegionsTotal;
	
	void reset() { m_nNbUnitsWithOrders=m_nNbUnitsTotal=m_nNbRegionsWithOrders=m_nNbRegionsScanned=m_nNbRegionsTotal=0;}

} TSaveOrderResults;

///////////

typedef	enum eRegionFeature
{
	RFT_BALM=0,
	RFT_OIL,
	RFT_SPICE,
	RFT_GEM,
	RFT_MYRRH,
	RFT_SILK,
	RFT_INCENSE,
	RFT_WAGE,
	RFT_CASTLESIZE,
	RFT_PEASANTS,
	RFT_SILVER,
	RFT_MAXRECRUITS,
	RFT_MAXTAX,	
	RFT_MAXENTERTAIN,
	RFT_HERB,
	RFT_HORSES,
	RFT_TREES,
	RFT_YOUNGTREES,
	RFT_STONES,
	RFT_IRON,
	RFT_STONE_LEVEL,
	RFT_IRON_LEVEL,
	RFT_LAEN_LEVEL,
	RFT_MAX

} eRegionFeature;


#define NBLUXURIES (RFT_INCENSE+1)

///////////

class CCR  
{
public:
	CCR();
	virtual ~CCR();
	
	virtual BOOL load(const char * CRFilename, const char *hierarchyFilename, const report_interface * rr);
	virtual BOOL save(const char * outFilename=NULL, BOOL bAsNextTurn=TRUE, BOOL bAsMinimal=FALSE);
	virtual BOOL preSave();

	BOOL saveOrders(BOOL bSelectedRegionsOnly, const char * pFilename, int nPlayerID, const char *pswd, int nLineMaxLength, TSaveOrderResults & results, BOOL bAppend=FALSE);

	CCRReport & report() { return m_report;	}
	int turn();

	int itemsNumber(block *u, const char *itemName); //EN name

	BOOL removeTalentsDiff(block *u, block *tl=NULL);

	block * throughJourneyUnits(block *r) const { return (m_bLoaded?(block*)m_parser->ireport->find(m_data, r, CR_THROUGH_JOURNEY, NULL, 0):NULL);}
	block * throughJourneyShips(block *r) const { return (m_bLoaded?(block*)m_parser->ireport->find(m_data, r, CR_THROUGH_SAILING, NULL, 0):NULL);}
	block * groups(block *f, int *p=NULL) const { return (m_bLoaded?(block*)m_parser->ireport->find(m_data, (f?f:m_data->blocks), CR_GROUP, p, (p?1:0)):NULL);}
	block * alliances(block *f, int *p=NULL) const { return (m_bLoaded?(block*)m_parser->ireport->find(m_data, (f?f:m_data->blocks), CR_ALLIANCE, p, (p?1:0)):NULL);}
	block * messages(block *f, int *p=NULL) const { return (m_bLoaded?(block*)m_parser->ireport->find(m_data, (f?f:m_data->blocks), CR_MESSAGE, p, (p?1:0)):NULL);}
	block * battles(block *f, int *p=NULL) const { return (m_bLoaded?(block*)m_parser->ireport->find(m_data, (f?f:m_data->blocks), CR_BATTLE, p, (p?1:0)):NULL);}
	block * factions(int *p=NULL) const;
	block * luxury(block *r, const char *pName, block *pLuxuries = NULL) const;
	block * luxuries(block *r) const { return (m_bLoaded?(block*)m_parser->ireport->find(m_data, r, CR_PRICES, NULL, 0):NULL);}
	block * resourceByName(const CString & sWantedType, block * rs = NULL, block * reg = NULL);
	block * resources(block *r) const { return (m_bLoaded?(block*)m_parser->ireport->find(m_data, (r?r:m_data->blocks), CR_RESOURCE, NULL,0):NULL);}
	block * units(block *r, int *p=NULL) const { return (m_bLoaded?(block*)m_parser->ireport->find(m_data, (r?r:m_data->blocks), CR_UNIT, p, (p?1:0)):NULL);}
	block * regions(int *ids=NULL) const { return (m_bLoaded?(block*)m_parser->ireport->find(m_data, m_data->blocks, CR_REGION, ids, (ids?2:0)):NULL);}
	block * roads(block *r, int *p=NULL) const { return (m_bLoaded?(block*)m_parser->ireport->find(m_data, r, CR_BORDER, p, (p?1:0)):NULL);}
	block * buildings(block *r, int *p=NULL) const { return (m_bLoaded?(block*)m_parser->ireport->find(m_data, r, CR_CASTLE, p, (p?1:0)):NULL);}
	block * ships(block *r, int *p=NULL) const { return (m_bLoaded?(block*)m_parser->ireport->find(m_data, r, CR_SHIP, p, (p?1:0)):NULL);}
	block * castle(block *r) const;
	block * talents(block *u) const;
	block * cmds(block *u) const;
	block * effects(block *u) const;
	block * items(block * u) const { return (m_bLoaded?(block*)m_parser->ireport->find(m_data, u, CR_ITEMS, NULL, 0):NULL);}
	block * guards(block *r) const { return (m_bLoaded?(block*)m_parser->ireport->find(m_data, r, CR_GUARDED, NULL, 0):NULL);}

	block * firstBrother(block *b, const char * stypename) const;
	block * lastBrother(block *b, const char * stypename=NULL) const;

	BOOL gets(block *b, const char *pblockname, CString & sValue) const;
	BOOL geti(block *b, const char *pblockname, int & nValue, int *nDiff = NULL) const;
	BOOL getints(block *b, const char *pblockname, const int ** pValues, size_t *pSize) const;

	BOOL sets(block *b, const char *pblockname, const CString & sValue);
	BOOL seti(block *b, const char *pblockname, int nValue);
	BOOL setints(block *b, const char *pblockname, const int * i, int nb);

	BOOL setTalent(block *b, const char *pblockname, int nValue, int bLevelRaised, int nDiff);

	BOOL isCastle(block *b) const;
	UINT builtCastleSize(block *b) const;
	block * buildingWithType(block *bs, const char *sTypeName) const;
	block * roadWithDirection(block *bs, int nDirection) const;

	BOOL Loaded() const { return m_bLoaded;}

	BOOL skillLevel(block *u, const char *pblockname, int & nValue, block * talents) const;

	BOOL onSave_move(block *u, block *brsrc, block *brdst);
	BOOL onSave_remove(block *u, BOOL bBrothers=FALSE, BOOL bChildren=FALSE);
	BOOL onSave_addRegionBlock(block *&r, int x, int y);

	block * onSave_addBlock(block *bparent, block *brother, const char *name, int id);

	BOOL onSave_removeEntry(block *b, const char *pTagname);

	BOOL toSave() const { return !m_sResultFilename.IsEmpty();}

	void setMagellanFoldername(const char *pName) { m_sMagellanFolderName=pName;}
	void setResultFilename(const char *pName) { m_sResultFilename=pName;}
	const char * resultFilename() const { return m_sResultFilename;}
	
	BOOL Silver2Pocket(block * u, block * items=NULL);
	BOOL Herbs2Herbbag(block * u, block * items=NULL);
	BOOL Potions2Vial(block * u, block * items=NULL);

	BOOL setUnitAsAnonym(block * u, block * talents = NULL, block * items = NULL, block *cmd = NULL);
	BOOL setFactionAnonym(int nPlayerID);
	BOOL setFactionAnonym(block * f);
	BOOL setFactionAsDemon(block *f);


	BOOL setAllFactionsAnonym(int & nb);
	BOOL removeFaction(block * f);
	BOOL removeFaction(int nPlayerID);

	BOOL reg_i(eRegionFeature n, block *b, int & nb);
	BOOL reg_s(eRegionFeature n, block *b, CString & s);

	int correctBadRegionNames();

//	block *	region(CCRRegion *pRegion);
	block *	region(int x, int y/*, BOOL * bSetAsCurrent*/);

	/////////////////
	// static methods

	static CString reg_feature(eRegionFeature n);
	static CString reg_featureInEnglish(eRegionFeature n);
	
	BOOL addTranslation(const char *name_en, const char *name_de);

	static const char * English2German(const char *name, BOOL bIsSkill=FALSE);
	static const char * German2English(const char *name);
	static BOOL isCastleType(const CString &sTyp);

	static BOOL setHierarchyFilename(const char *pFilename);

	BOOL isUserFaction(int nFactionID);
	CString factionPassword(int nFactionID);
	int nbUsersFactions(); 

	void fillReport(int nOptions);

	BOOL close();
	BOOL gets_ansi(block *b, const char *pblockname, CString & sValue) const;

	static BOOL b_isBattle(const block * bBattle);
	static BOOL b_isFaction(const block * bfaction);
	static BOOL b_isRegion(const block * bregion, BOOL *pbAstral=NULL);
	static BOOL b_isUnit(const block * bunit);
	static BOOL b_isShip(const block * bunit);

//	static const char * IDToString(int i);

protected:
	
	CCRReport			m_report;
	BOOL				m_bLoaded;

	FILE *				m_file;

	int					m_nLineMaxLength;

	crdata *			m_data;
	CString				m_sResultFilename;
	CString				m_filename;
	CString				m_sMagellanFolderName;
	parse_info *		m_parser;

	static CString		s_hierarchyFilename;
	static FILE *		s_hierarchy;
	static block *		s_translations;

	virtual BOOL read(parse_info * parser, const char * filename);

	int fprintf_valist_max(FILE *f, const char * pszFormat, va_list argList);
	int fprintf_max(FILE *f, const char * pzFormat, ...);

private:

	BOOL saveRegionOrders(BOOL bSelectedRegionsOnly, FILE *f, block *r, int nPlayerID, int & nbUnitsWithOrders_in_region, int & nbUnits_in_region);

	BOOL m_bUserFactionsRetrieved;
	CList <int, int> m_nUserFactionsID;
	CStringList m_sUserFactionsPswd;

	BOOL retrieveUserFactions();

	static CString s_sRegionFeatureEntries[RFT_MAX];

};

#endif // !defined(AFX_CR_H__73DC631E_E2AF_4356_B466_40E522495569__INCLUDED_)
