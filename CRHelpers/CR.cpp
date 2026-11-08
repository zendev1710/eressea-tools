// CR.cpp: implementation of the CCR class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Eressea.h"
#include "CR.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static char nulls[]="\0\0\0\0\0\0\0\0";

static CString s_sID[10];
static int s_nIDNumber = 0;

const char * IDToString(int i)
{
	char *dst;
	int b=i, x;

	if (i==0) return "0";

	dst=nulls+6;
	do {
		x=b%36;
		b/=36; dst--;
		if (x<10)
			*dst=(char)('0'+x);
		else
		{
			*dst=(char)('a'+(x-10));
			if (*dst=='l') *dst='L';
		}
	} while (b>0);
	CString s((const char *)dst);
	if (s_nIDNumber==10) s_nIDNumber=0;
	s_sID[s_nIDNumber] = s;
	s_nIDNumber++;
	return (LPCSTR) s_sID[s_nIDNumber-1];
}

CString CCR::s_sRegionFeatureEntries[RFT_MAX] = 
{
	CR_BALM, // should be the first - do not change
	CR_OIL,
	CR_SPICE,
	CR_GEM,
	CR_MYRRH,
	CR_SILK,
	CR_INCENSE,
	CR_WAGE,
	CR_SIZE,
	CR_PEASANTS,
	CR_SILVER,
	CR_MAXRECRUITS,
	"",
	"",
	CR_HERB,
	CR_HORSES,
	CR_TREES,
	CR_YOUNGTREES,
	CR_STONES,
	CR_IRONS,
	CR_SKILL,
	CR_SKILL,
	CR_SKILL
};
/*
 *	
 infos about Eressea date:
   It is the last week of the month of snowbane in the 11. year of the second age. It is winter.

 2;Zeitalter ==> second age
 */
void ERDebugOutputV(const char * pszFormat, va_list argList, long bAddCarriageReturn = 1)
{
	char 	buffer[4096];
	vsprintf_s(buffer, 4096, pszFormat, argList);
	if (bAddCarriageReturn)	
		strcat_s(buffer,4096,"\n");
	::OutputDebugString(buffer);
}

void ERDebugOutput(const char * lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	ERDebugOutputV(lpszFormat,argList);
	va_end(argList);
}

block * CCR::s_translations = NULL;

CString		CCR::s_hierarchyFilename;
FILE *		CCR::s_hierarchy;


//int CCR::s_nIDNumber = 0;
//CString CCR::s_sID[10];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


int btoi(const char *s) {
	const char *x=s;
	int i=0;
	int n=0;
	if (!(*x)) return 0;
	while(isalnum(*x)) {x++;n++;}
	if (n>4) return 1; //pbm
	i=(int)(strtol(s, NULL, 36));
	return i;
}

const char * extractName(CString & sName)
{
	int n1=-1;
	int n2=-1;
	n1 = sName.Find("\\\"");
	if (n1!=-1) n2 = sName.Find("\\\"",n1+2);
	if (n1!=-1 && n2!=-1 && n1 < n2)
	{
		CString sn = sName.Left(n1)+sName.Mid(n1+2,n2-n1-2)+sName.Mid(n2+2);
		sName = sn;
	}
	n1 = sName.Find('"');
	if (n1!=-1) 
		n2 = sName.Find('"',n1+1); 
	else 
		n2=-1;
	if (n1!=-1 && n2!=-1 && n1 < n2)
	{
		CString sn = sName.Left(n1)+sName.Mid(n1+1,n2-n1-1)+sName.Mid(n2+1);
		sName = sn;
	}
	return sName;
}
/*
block *	CCR::region(CCRRegion * pRegion)
{
	if (!pRegion) return NULL;
	return region(pRegion->x(),pRegion->y());
}
*/
block *	CCR::region(int x, int y/*, BOOL bSetAsCurrent*/)
{
	if (!m_bLoaded) return NULL;
	int nIDs[2];
	nIDs[0]=x;
	nIDs[1]=y;

//	if (m_pCurrentRegion && m_pCurrentRegion->ids[0]==nIDs[0] && m_pCurrentRegion->ids[1]==nIDs[1]) return m_pCurrentRegion;
	block * r = (block*)m_parser->ireport->find(m_data, m_data->blocks, DE_REGION, nIDs, 2);
//	if (bSetAsCurrent) m_pCurrentRegion=r;
	return r;
}

//////////////

CCR::CCR():
m_data(NULL),
m_parser(NULL),
m_file (NULL),
m_filename(""),
m_sResultFilename(""),
m_sMagellanFolderName(""),
m_bLoaded(FALSE),
m_bUserFactionsRetrieved(FALSE),
m_nLineMaxLength(DEFAULT_LINE_MAXLENGTH),
m_report(*this)
{
 
}

CCR::~CCR()
{
//	if (m_parser && m_data->blocks)
//		m_parser->ireport->destroy(m_data,m_data->blocks);
	crdata_destroy(m_data);
	if (m_parser) 
		free(m_parser);
}

BOOL CCR::setHierarchyFilename(const char *pFilename)
{
	int errnum=0;
	if (s_hierarchy)
			return TRUE;
	if (pFilename!=NULL)
	{
		s_hierarchyFilename = pFilename;
		errnum=fopen_s(&s_hierarchy,s_hierarchyFilename, "rt+");
	}
	return s_hierarchy!=NULL ;
}

int CCR::turn()
{
	int nTurn=0;
	if (!m_bLoaded) return 0;
	return m_data->blocks->turn;
}

BOOL CCR::load(const char * CRFilename, const char *hierarchyFilename, const report_interface * rr)
{
	if (!setHierarchyFilename(hierarchyFilename))
		return FALSE;

	if (CRFilename)
	{
		if (!m_bLoaded)
		{
			m_data = crdata_init(s_hierarchy);
			m_parser=(parse_info *)calloc(1, sizeof(parse_info));
			m_parser->iblock = &crdata_iblock;
			m_parser->ireport = rr;
			m_data->parser = m_parser;
			m_parser->bcontext = (context_t)m_data;
			if (read(m_parser, CRFilename))
			{
				if (!s_translations)
					s_translations = (block*)m_parser->ireport->find(m_data, m_data->blocks, DE_TRANSLATION, NULL, 0);	//translation	

				m_bLoaded = TRUE;
			}
		}
	}
	return m_bLoaded;
}

BOOL CCR::close() {
	if (!m_file)
		return FALSE;
	fclose(m_file);
	m_file=NULL;
	return TRUE;
}

/*
BOOL CCR::closeHierarchyFile()
{
}
*/

BOOL CCR::read(parse_info * parser, const char * filename)
{
	int errnum=1;
	m_filename = filename;
	errnum = fopen_s(&m_file, filename, "rt+");
	if (errnum) {
		perror(filename);
		return FALSE;
	}
  
	//if (verbose) fprintf(stderr, "reading %s\n", filename);

	cr_parse(parser, m_file);
	return TRUE;
}

BOOL CCR::preSave()
{
	if (m_parser)
		m_parser->ireport = &crdata_ireport_save;
	return TRUE;
}

BOOL CCR::save(const char * outFilename, BOOL bAsNextTurn, BOOL bAsMinimal)
{
	block * b;
	
	CString sFilename (m_sResultFilename);
	if (outFilename) sFilename = outFilename;
	if (bAsMinimal) sFilename+="_min";

	ERTRACE("writing file %s\n",sFilename);
	if (m_file)
		close();
	int errnum= fopen_s(&m_file, sFilename, "wt");
	if (!errnum && m_file)
	{
		if (bAsNextTurn && m_data->blocks) 
			m_data->blocks->turn++; //CG
		for (b=m_data->blocks;b;b=b->next)
			cr_write(m_data, m_file, b);
		ERTRACE("file %s saved\n",sFilename);
		return TRUE;
	}
	char sErrmsg[255];
	int nErr2=strerror_s(sErrmsg,255,errno /*errnum*/);

	return FALSE;
}

CString UTF8_to_ansi(const char *psvalue) {

	CString utf8text(psvalue);
	CString ansitext = "";

	char m[500];
	WCHAR w[500];

	strcpy_s(m, 500, utf8text);
	MultiByteToWideChar(CP_UTF8, 0, m, -1, w, sizeof(w) / sizeof(WCHAR));
	WideCharToMultiByte(CP_ACP, 0, w, -1, m, sizeof(m), 0, 0);
  
	ansitext = m;
	return ansitext;
}

CString ansi_to_UTF8(const char *psvalue) {

	CString ansitext(psvalue);
	CString utf8text = "";
	CString utf8text2 = "";

	char m[500];
	WCHAR w[500];

	strcpy_s(m, 500, ansitext);
	MultiByteToWideChar(CP_ACP, 0, m, -1, w, sizeof(w) / sizeof(WCHAR));
	WideCharToMultiByte(CP_UTF8, 0, w, -1, m, sizeof(m), 0, 0);
  	utf8text = m;
	return utf8text;
}

BOOL CCR::gets(block *b, const char *pblockname, CString & sValue) const
{
	if (b==NULL)
		return FALSE;
	const char *svalue;
	if (m_parser->iblock->get_string(m_data, b, pblockname, &svalue)) {
		sValue=""; 
		return FALSE;
	}
	sValue = svalue;
	//sValue = UTF8_to_ansi(svalue);
	return TRUE;
}

BOOL CCR::gets_ansi(block *b, const char *pblockname, CString & sValue) const
{
	if (b==NULL)
		return FALSE;
	const char *svalue;
	if (m_parser->iblock->get_string(m_data, b, pblockname, &svalue)) {
		sValue=""; 
		return FALSE;
	}
	sValue = UTF8_to_ansi(svalue);
	return TRUE;
}

BOOL CCR::sets(block *b, const char *pblockname, const CString & sValue)
{
	if (b==NULL) 
		return FALSE;
	if (pblockname==NULL) 
		m_parser->iblock->set_entry(m_data, b, (const char *)sValue); 
	else 
		m_parser->iblock->set_string(m_data, b, pblockname, (const char *)sValue);
	return TRUE;
}

BOOL CCR::removeTalentsDiff(block *u, block *tl)
{
	if (!tl) tl=talents(u);
	if (!tl) return FALSE;
	entry *e  = tl->entries;
	while (e) 
	{
		if (e->data.ip[0] > 2) 
			e->data.ip[0]=2;
		e = e->next;
	}
	return TRUE;
}

BOOL CCR::geti(block *b, const char *pblockname, int & nValue, int *nDiff) const
{
	int nvalue;
	int nprevvalue;
	//if (b==NULL)
	//	return FALSE;
	// CG 02/11/2008
	if (b==NULL || m_parser->iblock->get_int(m_data, b, pblockname, &nvalue)) {
		nValue=0; 
		if (nDiff) *nDiff = 0;
		return FALSE;
	}
	nValue = nvalue;
	if (!nDiff) return TRUE;
	CString sName(CR_PREV);
	if (!_strcmpi(pblockname,DE_SILVER) || !_strcmpi(pblockname,DE_IRONS))
		sName=CR_PREVS;
	sName+=pblockname;
	if (m_parser->iblock->get_int(m_data, b, sName, &nprevvalue)) {*nDiff = 0; return TRUE;}
	*nDiff = nvalue-nprevvalue;
	return TRUE;
}

BOOL CCR::getints(block *b, const char *pblockname, const int ** pValues, size_t *pSize) const
{
	if (b==NULL) return FALSE;
	m_parser->iblock->get_ints(m_data, b, pblockname, pValues, pSize);
	return (*pValues != NULL);
}

BOOL CCR::setTalent(block *b, const char *pblockname, int nValue, int bLevelChanged, int nDiff)
{
	if (nDiff==DIFF_NEW_UNIT) nDiff=nValue;
	int nnn[3]={0,nValue,nDiff};
	if (!b)	return FALSE;
	if (!bLevelChanged) return seti(b,pblockname,nValue);
	return setints(b, pblockname, nnn, 3);
}

BOOL CCR::seti(block *b, const char *pblockname, int nValue)
{
	if (!b) 
		return FALSE;
	m_parser->iblock->set_int(m_data, b, pblockname, nValue);
	return TRUE;
}

BOOL CCR::setints(block *b, const char *pblockname, const int * i, int nb)
{
	if (!b)	return FALSE;
	m_parser->iblock->set_ints(m_data, b, pblockname, i, nb);
	return TRUE;
}

BOOL CCR::onSave_removeEntry(block *b, const char *pTagname)
{
	block_entry_remove(m_data, b, pTagname);
	return TRUE;
}

BOOL CCR::skillLevel(block *u, const char *pblockname, int & nValue, block * talents = NULL) const
{
	nValue = 0;
	if (!m_bLoaded) return FALSE;
	if (!u) return FALSE;
	if (!talents) talents = (block *)m_parser->ireport->find(m_data, u, CR_TALENTS, NULL, 0);
	if (!talents) return FALSE;
	entry * e = talents->entries;
	while (e) {
//							CString s(UTF8_to_ansi(e->tag->name));

		if (!_strcmpi(e->tag->name,pblockname)) {nValue = e->data.ip[2]; return TRUE;}
		e = e->next;
	}
	return FALSE;
}

block * CCR::talents(block *u) const
{
	if (!m_bLoaded) return NULL;
	if (!u) return NULL;
	//does not work every time !
	return (block *)m_parser->ireport->find(m_data, u, CR_TALENTS, NULL, 0);
}

block * CCR::cmds(block *u) const
{
	if (!m_bLoaded) return NULL;
	if (!u) return NULL;
	//does not work every time !
	return (block *)m_parser->ireport->find(m_data, u, CR_CMDS, NULL, 0);
}

block * CCR::effects(block *u) const
{
	if (!m_bLoaded) return NULL;
	if (!u) return NULL;
	return (block *)m_parser->ireport->find(m_data, u, CR_EFFECTS, NULL, 0);
}

block * CCR::buildingWithType(block *bs, const char *sTypeName) const
{
	block * b = bs;
	while (b)
	{
		CString sType;
		if (gets(b,CR_TYPE, sType) && !sType.CompareNoCase(sTypeName)) return b;
		b = b->next;
	}
	return NULL;
}

block * CCR::roadWithDirection(block *bs, int nDirection) const
{
	block * b = bs;
	while (b)
	{
		int nDir = 0;
		if (geti(b,CR_DIRECTION, nDir) && nDir==nDirection) return b;
		b = b->next;
	}
	return NULL;
}

block * CCR::castle(block *r) const
{
	block * bs = buildings(r);
	block * b = bs;
	while (b)
	{
		if (!_strcmpi(b->type->name,DE_CASTLE) && isCastle(b)) return b;
		b = b->next;
	}
	return NULL;
}

BOOL CCR::isCastle(block *b) const
{
	if (!b) return FALSE;
	CString sCastleType("");
	if (!gets(b,DE_TYPE, sCastleType)) return FALSE;
	return isCastleType(sCastleType);
}

UINT CCR::builtCastleSize(block *b) const
{
	//TODO
	// built castle size could be computed by selecting all messages with:
	// - type=1750373495
	// - building=<castle id>
	// - size added for each message

	return 0;
}

block * CCR::luxury(block *r, const char *pName, block *pLuxuries) const
{
	block *l = pLuxuries;
	if (!l) l = luxuries(r);
	if (!l) return NULL;
	return NULL;
	//if (!geti(l,English2German(pName), sCastleType)) return FALSE;
}

/////////////////
// static methods

BOOL CCR::isCastleType(const CString &sTyp)
{
	CString s=German2English(sTyp);
	if (!s.CompareNoCase(EN_FOUNDATION)) return TRUE;
	if (!s.CompareNoCase(EN_TRADEPOST)) return TRUE;
	if (!s.CompareNoCase(EN_FORTIFICATION)) return TRUE;
	if (!s.CompareNoCase(EN_TOWER)) return TRUE;
	if (!s.CompareNoCase(EN_STRONGHOLD)) return TRUE;
	if (!s.CompareNoCase(EN_FORTRESS)) return TRUE;
	if (!s.CompareNoCase(EN_CITADEL)) return TRUE;
	if (!s.CompareNoCase(EN_CASTLE)) return TRUE;
	return FALSE;
}

const char * CCR::German2English(const char *name)
{
	const char *res = name;
	const char *name2 = name;
	entry * e = NULL;
	CString nameutf8("");
	if (s_translations) 
		e = s_translations->entries;
	if (e) {
		nameutf8 = ansi_to_UTF8(name);
		name2=nameutf8;
	}
	while (e) {
		if (!_strcmpi(e->tag->name, name))
			return e->data.cp;
		if (name2 && name2!=name && !_strcmpi(e->tag->name, name2))
			return e->data.cp;

		e = e->next;
	}
		
	return res;
}

BOOL CCR::addTranslation(const char *name_en, const char *name_de)
{
	BOOL bres=FALSE;
	if (!s_translations) return FALSE;
	const char * res_de = German2English(name_en);
	if (_strcmpi(res_de,name_en)) return FALSE;
	entry * e = s_translations->entries;
	if (!e) return FALSE;
	CString nameutf8 = ansi_to_UTF8(name_de);
	return sets(s_translations,nameutf8,name_en);
}

const char * CCR::English2German(const char *name, BOOL bIsSkill)
{
	const char *res = name;
	entry * e = NULL;
	if (s_translations) e = s_translations->entries;
	while (e) {
		if (!_strcmpi(e->data.cp,name))
		{
			// bow & crossbow:two german translations depending on it's a skill or weapon
			// first encoutered translation is weapon, second is skill
			if (!bIsSkill)
				return e->tag->name;
			res = e->tag->name;
		}
		e = e->next;
	}
		
	return res;
}

// unit or ship
BOOL CCR::onSave_move(block *u, block *brsrc, block *brdst)
{
	if (!u)	return FALSE;
	if (!brsrc)	return FALSE;
	if (!brdst)	return FALSE;
	block_move(u, brdst);
	return TRUE;
}

BOOL CCR::onSave_remove(block *u, BOOL bBrothers, BOOL bChildren)
{
//	block *p=NULL;
//	block *b=NULL;

	if (!u)	return FALSE;
//	long un=(long)u;
	//if (u->parent) p = u->parent;
	if (bBrothers) block_removeBrothers(u); //else b=u->next;
	if (bChildren) block_removeChildren(u);
	block_remove(u);
/*	if (p)
	{
		long up=(long)(p->children);
		if (up==un)
			p->children=b;
	}*/
	return TRUE;
}


BOOL CCR::onSave_addRegionBlock(block *&r, int x, int y)
{
	if (!m_bLoaded) return NULL;
	block * bparent = m_data->blocks;
	block * brother = r;
	if (!brother)
	{
		if (bparent) brother = bparent->children;
		if (brother) brother = firstBrother(brother,CR_REGION);
	}
	if (brother) m_data->current=brother; else m_data->current=bparent;
	if (!bparent) bparent=brother->parent;

	int ids[2];
	ids[0]=x; ids[1]=y;
	r = (block *)m_parser->ireport->create(m_data, CR_REGION, ids, 2);
	if (!r) return FALSE;
	r->parent=bparent;
	r->turn = bparent->turn;
	if (brother) {
		r->next = brother->next;
		brother->next = r;
	} 
	else
	{
		if (!r->parent->children)
			r->parent->children = r;
	}
	sets(r,CR_TERRAIN,DE_OCEAN);
	sets(r,"visibility","neighbour");
	return TRUE;
}

block * CCR::onSave_addBlock(block *bparent, block *brother, const char *name, int id)
{
	if (!m_bLoaded) return NULL;
	if (!brother && !bparent) return NULL;
	if (!brother && bparent)
		brother = bparent->children;
	if (brother) m_data->current=brother; else m_data->current=bparent;
	if (!bparent) bparent=brother->parent;
	int nsize=1;
	int ids[1];
	ids[0]=id;
	if (id==0) nsize=0;
	block * b = (block *)m_parser->ireport->create(m_data, name, ids, nsize);
	b->parent = bparent;
	b->turn = bparent->turn;
	if (brother) {
		b->next = brother->next;
		brother->next = b;
	} 
	else
	{
		if (!b->parent->children)
			b->parent->children = b;
	}
	return b;
}

block * CCR::firstBrother(block *b, const char * stypename) const
{
	while (b)
	{
		if (!stypename && b->type && !_strcmpi(b->type->name,stypename))
			return b;
		b = b->next;
	}
	return NULL;
}

block * CCR::lastBrother(block *b, const char * stypename) const
{
	block * bb = b;
	while (b)
	{
		if (!stypename) break;
		if (!b->type) break; 
		if (_strcmpi(b->type->name,stypename)) break;
		bb = b;
		b = b->next;
	}
	if (bb==b) return firstBrother(b,stypename);
	return bb;
}

/////////////////

int CCR::itemsNumber(block *u, const char *itemName)
{
	int nSrcItems = 0;
	block *	t = (block*)m_parser->ireport->find(m_data, u, CR_ITEMS, NULL, 0);
	if (t) m_parser->iblock->get_int(m_data,t,English2German(itemName),&nSrcItems);
	return nSrcItems;
}

int CCR::fprintf_valist_max(FILE *f, const char * pszFormat, va_list argList)
{
	static CString sOutput("");
	int nNb=0;
	sOutput.FormatV(pszFormat, argList);
	while (sOutput.GetLength()>m_nLineMaxLength)
	{
		CString sOutputTmp=sOutput.Mid(m_nLineMaxLength-1);
		sOutput=sOutput.Mid(0,m_nLineMaxLength-1);
		sOutput+="\\\n";
		nNb+=fprintf(f,"%s",sOutput);
		sOutput=sOutputTmp;
	}
	nNb+=fprintf(f,"%s",sOutput);
	return nNb;
}

int CCR::fprintf_max(FILE *f, const char * pzFormat, ...)
{
	va_list argList;
	int nNb=0;
	va_start(argList, pzFormat);
	nNb=fprintf_valist_max(f,pzFormat,argList);
	va_end(argList);
	return nNb;
}

BOOL CCR::saveRegionOrders(BOOL bSelectedRegionsOnly, FILE *f, block *r, int nPlayerID, int & nbUnitsWithOrders_in_region, int & nbUnits_in_region)
{

	BOOL bSaveOrders = TRUE;
	BOOL bRegionTag = FALSE;
	block * u = units(r);
	nbUnits_in_region=0;
	nbUnitsWithOrders_in_region=0;
	if (bSelectedRegionsOnly)
	{
		int nSel=0;
		geti(r,"ejcIsSelected",nSel);
		if (nSel!=1) 
			bSaveOrders = FALSE;
	}

	while (u)
	{
		if (_strcmpi(u->type->name,CR_UNIT))
			break;
		int nID = 0;
		if (geti(u,CR_PLAYER, nID) && nID==nPlayerID)
		{
			nbUnits_in_region++;
			int noc = 0;
			if (bSaveOrders && geti(u,CR_ORDERSCONFIRMED,noc) && noc==1)
			{
				// orders confirmed
				if (!bRegionTag)
				{
					CString sRegionName("");
					gets_ansi(r,CR_NAME,sRegionName);
					if (sRegionName.IsEmpty()) 
						sRegionName="null";
					fprintf_max(f,"REGION %d,%d ; %s\n",r->ids[0],r->ids[1],(LPCSTR)sRegionName);
					bRegionTag = TRUE;
				}
				block * bcmd = cmds(u);
				if (bcmd)
				{
					if (_strcmpi(bcmd->type->name,CR_CMDS))
						break;

					block * bc=NULL; 
					int nContainerID=0;
					int nOwnerID = 0;
					int nbMen=0;
					CString sContainerSuffix("");
					CString sUnitName("");
					CString sUnitID = IDToString(u->ids[0]);
					int nbSilver = itemsNumber(u, EN_SILVER);

					geti(u,CR_NBMEN,nbMen);
					gets_ansi(u,CR_NAME,sUnitName);

					if (geti(u,CR_SHIP,nContainerID))
					{
						// ON SHIP
						// captain ?
						bc = ships(r,&nContainerID);
						if (bc)
						{
							char c='s';
							geti(bc, CR_CAPTAIN, nOwnerID);
							if (nOwnerID==u->ids[0]) 
								c='S';			
							sContainerSuffix.Format(",%c%s",c,IDToString(nContainerID));
						}
					}
					else
					{
						if (geti(u,CR_CASTLE,nContainerID))
						{
							// IN BUILDING
							bc = buildings(r,&nContainerID);
							if (bc && !isCastle(bc))
							{
								// owner ?
								geti(bc, CR_OWNER, nOwnerID);
								if (nOwnerID==u->ids[0]) 
								{
									CString sType("");
									if (gets(bc,CR_TYPE,sType))
									{
										int nUpkeep=e_silverUpkeepBuilding(German2English(sType));
										if (nUpkeep > 0)
											sContainerSuffix.Format(",U%d",nUpkeep);
									}
								}
							}
						}
					}
					nbUnitsWithOrders_in_region++;
					fprintf_max(f,"UNIT %s;\t%s [%d,%d$%s]\n",(LPCSTR)sUnitID,(LPCSTR)sUnitName,nbMen,nbSilver,(LPCSTR)sContainerSuffix);
					fprintf_max(f,";bestaetigt\n"); //confirmed
					entry * e = bcmd->entries;
					while (e) 
					{
						CString s(UTF8_to_ansi(e->data.cp));
						s.Remove('\\');
						fprintf_max(f,"%s\n",(LPCSTR)s);
						e = e->next;
					}
				}
			}
		}
		u=u->next;
	}
	return bSaveOrders;
}

/*
 *	
 typedef struct TSaveOrderResults
{
	int m_nNbUnitsWithOrders;
	int m_nNbUnitsTotal;
	int m_nNbRegionsWithOrders;
	int m_nNbRegionsScanned;
	int m_nNbRegionsTotal;
	
	void reset() { m_nNbUnitsWithOrders=m_nNbTotalUnits=m_nNbRegionsWithOrders=0;}

} TSaveOrderResults;

 */

BOOL CCR::saveOrders(BOOL bSelectedRegionsOnly, const char * pFilename, int nPlayerID, const char *pswd, int nLineMaxLength, TSaveOrderResults & results, BOOL bAppend)
{
	FILE *f;
	
	results.reset();
	int nNb=0;

	m_nLineMaxLength=nLineMaxLength;

	CString sOpenMode("wt");
	if (bAppend) 
		sOpenMode="at";
	int errnum = fopen_s(&f,pFilename,sOpenMode);
	if (!errnum && f)
	{
		fprintf_max(f,"ERESSEA %s \"%s\"\n",IDToString(nPlayerID),pswd);
		nNb=fprintf(f,"LOCALE en\n");

		block * rs = regions();
		block * r = rs;

		while (r)
		{
			if (_strcmpi(r->type->name,CR_REGION))
				break;
			int nbUnitsWithOrders_in_region = 0;
			int nbUnits_in_region = 0;
			if (saveRegionOrders(bSelectedRegionsOnly,f,r,nPlayerID,nbUnitsWithOrders_in_region,nbUnits_in_region))
			{
				results.m_nNbRegionsScanned++;
				if (nbUnitsWithOrders_in_region > 0)
					results.m_nNbRegionsWithOrders++;	
			}	
			results.m_nNbRegionsTotal++;
			results.m_nNbUnitsWithOrders+=nbUnitsWithOrders_in_region;
			results.m_nNbUnitsTotal+=nbUnits_in_region;
			r=r->next;
		}
		fprintf_max(f,"NEXT\n"); //next faction
		fclose(f);
	}
	return (results.m_nNbUnitsWithOrders > 0);
}

BOOL CCR::retrieveUserFactions()
{
	if (m_bUserFactionsRetrieved) 
		return TRUE;
	CString sFolder = m_sMagellanFolderName;
	if (sFolder.IsEmpty()) sFolder = STGetDir(m_filename,TRUE);
	if (sFolder.Right(1) != _T("\\")) sFolder+="\\";
	CString sIniFilename = sFolder+"magellan.ini";

	ERTRACE("retrieveUserFactions: %s file",sIniFilename);

	FILE *f;
	int errnum = fopen_s(&f,sIniFilename,"rt");
	if (!f || errnum) 
	{
		ERTRACE("*** error *** retrieveUserFactions: failed to open %s file",sIniFilename);
		return FALSE;
	}

	m_bUserFactionsRetrieved = TRUE;

	char sbf[256];
	while (fgets(sbf,255,f)!=NULL)
	{
		if (sbf[0]!=0 && sbf[0]!='#') 
		{
			CString sf(sbf);
			CString sv;
			CString sp;
			if (sf.Find("Faction.password.")==0)
			{
				if (sf.Find("Faction.password.1144560")!=-1)
				{
					int uuhh=1;
				}
				sv=sf.Mid(17);
				int nEqIndex = sv.Find("=");
				sp=sv.Mid(nEqIndex+1);
				sp.Remove('\n');
				sv=sv.Left(nEqIndex);
				if (!sp.IsEmpty())
				{
					m_nUserFactionsID.AddTail(atoi(sv));
					m_sUserFactionsPswd.AddTail(sp);
				}
			}
		}
	}

	ERTRACE("retrieveUserFactions: end");

	fclose(f);
	return TRUE;
}

BOOL CCR::isUserFaction(int nFactionID)
{
	if (!retrieveUserFactions())
		return FALSE;
	POSITION p = m_nUserFactionsID.Find(nFactionID);
	return p!=NULL;
}

int CCR::nbUsersFactions() 
{
	if (!retrieveUserFactions())
		return 0;
	return m_nUserFactionsID.GetCount();
}

CString CCR::factionPassword(int nFactionID)
{
	if (!retrieveUserFactions())
		return "";

	POSITION pos = m_nUserFactionsID.GetHeadPosition();
	int i = 0;
	while (pos)
	{
		if (m_nUserFactionsID.GetNext(pos)==nFactionID)
			return m_sUserFactionsPswd.GetAt(m_sUserFactionsPswd.FindIndex(i));
		i++;
	}
	return "";
}

BOOL CCR::Herbs2Herbbag(block * u, block * items)
{

	CString sHerbNames[]={
	"shallow roots",
	"spicy dragongrass",
	"owl's eyes",
	"green spiderons",
	"blue treevine",
	"elvenlove",
	"gargleherbs",
	"tangled suckroots",
	"pustule funguses",
	"water finders",
	"cactus syrups",
	"sand reekers",
	"wind pouches",
	"fjord growers",
	"mandrakes",
	"stone biters",
	"crevice growers",
	"cave's glints",
	"ice flowers",
	"white rage",
	"snow crystals",
	"tangy temerity",
	"owlsgaze",
	"spider ivy",
	"cobalt fungus",
	"elvendear",
	"bugleweed",
	"knotroot",
	"bubblemorel",
	"waterfinder",
	"peyote",
	"sand reeker",
	"windbag",
	"fjord fungus",
	"mandrake",
	"rock weed",
	"gapgrowth",
	"cave lichen",
	"ice begonia",
	"flatroot",
	"white hemlock",
	"snowcrystal petal",
	""
	};
	
	BOOL bHasHerb = FALSE;
	if (!u) return FALSE;
	if (!items) items = (block *)m_parser->ireport->find(m_data, u, CR_ITEMS, NULL, 0);
	if (!items) return FALSE;
	int herbbag=0;
	geti(items,CR_HERBBAG,herbbag);
	if (herbbag==1) return FALSE;

	for (int i=0;;i++)
	{
		if (sHerbNames[i].IsEmpty()) break;
		CString sHerbDE = English2German(sHerbNames[i]);
		int nb=0;
		geti(items,sHerbDE,nb);
		if (nb>0)
		{
			bHasHerb=TRUE;
			onSave_removeEntry(items,sHerbDE);
		}
	}
	if (bHasHerb) seti(items,CR_HERBBAG,1);
	return bHasHerb;
}

BOOL CCR::Potions2Vial(block * u, block * items)
{

	CString sPotionNames[]={
	"goliath water",
	"seven mile tea",
	"potion of truth",
	"water of life",
	"peasant blood",
	"busybeer",
	"ointment",
	"berserkers blood potion",
	"duncebun",
	"brain wax",
	"potion of nest warmth",
	"horsepower potion",
	"peasant love potion",
	"elixir of power",
	"healing potion",
	""
	};
	
	if (!u) return FALSE;
	if (!items) items = (block *)m_parser->ireport->find(m_data, u, CR_ITEMS, NULL, 0);
	if (!items) return FALSE;
	int potions=0;
	int nbp=0;
	geti(items,CR_VIAL,potions);
	if (potions>=1) return FALSE;

	for (int i=0;;i++)
	{
		if (sPotionNames[i].IsEmpty()) break;
		CString sPotionDE = English2German(sPotionNames[i]);
		int nb=0;
		geti(items,sPotionDE,nb);
		if (nb>0)
		{
			nbp+=nb;
			onSave_removeEntry(items,sPotionDE);
		}
	}
	if (nbp>0) seti(items,CR_VIAL,nbp);
	return (nbp>0);
}

BOOL CCR::Silver2Pocket(block * u, block * items)
{
	if (!u) return FALSE;
	if (!items) items = (block *)m_parser->ireport->find(m_data, u, CR_ITEMS, NULL, 0);
	if (!items) return FALSE;

	// silver ==> >5000==>silverchest, >500 silverbag

	int nSilver = 0;
	int nbMen = 0;
	geti(items,CR_SILVER,nSilver);
	geti(u,CR_NBMEN,nbMen);
	onSave_removeEntry(items,CR_SILVER);
	if (nSilver > nbMen*5000) seti(items,CR_SILVERCHEST,1); else 
	if (nSilver > nbMen*500) seti(items,CR_SILVERBAG,1); 
	return TRUE; 
}

BOOL CCR::setFactionAnonym(int nPlayerID)
{
	int np=nPlayerID;
	block * f = factions(&np);
	return setFactionAnonym(f);
}

BOOL CCR::setFactionAnonym(block * f)
{
	if (!f) 
		return FALSE;
	if (!b_isFaction(f)) 
		return FALSE;
	onSave_removeEntry(f,"locale");
	onSave_removeEntry(f,"Optionen");
	onSave_removeEntry(f,"Punkte");
	onSave_removeEntry(f,"Punktedurchschnitt");
	onSave_removeEntry(f,CR_TYPE);
	onSave_removeEntry(f,CR_PLAYER_RECRUIT_COST);
	onSave_removeEntry(f,"Anzahl Personen");
	onSave_removeEntry(f,"Magiegebiet");
	onSave_removeEntry(f,"typprefix");
	onSave_remove((block *)m_parser->ireport->find(m_data, f, "OPTIONEN", 0, 0));
	onSave_remove(alliances(f),TRUE);
	return TRUE;
}

BOOL CCR::setFactionAsDemon(block *f)
{
	if (!f) 
		return FALSE;
	sets(f,CR_TYPE,DE_DEMON);
	seti(f,CR_PLAYER_RECRUIT_COST,RACE_RECRUIT_COST_DEMON);
	return TRUE;
}

BOOL CCR::removeFaction(block *f)
{
	if (!f) 
		return FALSE;
	int nid=f->ids[0];
	setFactionAnonym(f);

	//anonym only if allianzes referencing the faction
	block * a=alliances(NULL,&nid);
	if (!a) 
		return onSave_remove(f); // else onSave_remove(a); //remove allianzes with the faction to kill - DO NOT WORK
	//onSave_remove(f);
//	if (f)
	return FALSE;
}

BOOL CCR::removeFaction(int nPlayerID)
{
	int np=nPlayerID;
	block * f = factions(&np);
	return removeFaction(f);
}

BOOL CCR::setAllFactionsAnonym(int & nb)
{
	nb = 0;
	block * f = factions();
	if (!f) return FALSE;
	while (f)
	{
		if (!b_isFaction(f))
		{
			f = f->next;
			continue;
		}
		nb++;
		onSave_removeEntry(f,"locale");
		onSave_removeEntry(f,"Optionen");
		onSave_removeEntry(f,"Punkte");
		onSave_removeEntry(f,"Punktedurchschnitt");
		onSave_removeEntry(f,"Typ");
		onSave_removeEntry(f,"Rekrutierungskosten");
		onSave_removeEntry(f,"Anzahl Personen");
		onSave_removeEntry(f,"Magiegebiet");
		onSave_removeEntry(f,"typprefix");
		onSave_remove((block *)m_parser->ireport->find(m_data, f, "OPTIONEN", 0, 0),TRUE);
		onSave_remove(alliances(f),TRUE);
		f = f->next;
	}
	return TRUE;
}

BOOL CCR::setUnitAsAnonym(block * u, block * talents, block * items, block *cmds)
{
	if (!u) return FALSE;
	if (!talents) talents = (block *)m_parser->ireport->find(m_data, u, CR_TALENTS, NULL, 0);
	if (!items) items = (block *)m_parser->ireport->find(m_data, u, CR_ITEMS, NULL, 0);
	if (!cmds) cmds = (block *)m_parser->ireport->find(m_data, u, CR_CMDS, NULL, 0);
	if (talents) onSave_remove(talents);
	if (cmds) onSave_remove(cmds);
	
	int nInvisible=0;
	geti(items,CR_RING_INVISIBILITY,nInvisible);
	if (nInvisible==1)
	{
		onSave_remove(u);
		return TRUE;
	}
	BOOL bHasPotions = Potions2Vial(u, items);
	BOOL bHasHerbs = Herbs2Herbbag(u, items);
	BOOL bHasPocket = Silver2Pocket(u, items);
	BOOL bHasHero = onSave_removeEntry(u,CR_HERO); // no hero status
	return talents || bHasHerbs || bHasPocket || bHasHero;
}

BOOL CCR::b_isUnit(const block * bunit)
{
	if (!bunit) return FALSE;
	if (!bunit->type) return FALSE;
	if (!bunit->type->name) return FALSE;
	if (!bunit->ids) return FALSE;
	if (_strcmpi(bunit->type->name,CR_UNIT)) return FALSE;
	return TRUE;
}

BOOL CCR::b_isShip(const block * bunit)
{
	if (!bunit) return FALSE;
	if (!bunit->type) return FALSE;
	if (!bunit->type->name) return FALSE;
	if (!bunit->ids) return FALSE;
	if (_strcmpi(bunit->type->name,CR_SHIP)) return FALSE;
	return TRUE;
}

BOOL CCR::b_isBattle(const block * bBattle)
{
	if (!bBattle) return FALSE;
	if (!bBattle->type) return FALSE;
	if (!bBattle->type->name) return FALSE;
	if (!bBattle->ids) return FALSE;
	if (_strcmpi(bBattle->type->name,CR_BATTLE)) return FALSE;
	return TRUE;
}

BOOL CCR::b_isFaction(const block * bfaction)
{
	if (!bfaction) return FALSE;
	if (!bfaction->type) return FALSE;
	if (!bfaction->type->name) return FALSE;
	if (!bfaction->ids) return FALSE;
	if (_strcmpi(bfaction->type->name,CR_PLAYER)) return FALSE;
	return TRUE;
}

BOOL CCR::b_isRegion(const block * bregion, BOOL *pbAstral)
{
	if (!bregion) 
		return FALSE;
	if (!bregion->type) 
		return FALSE;
	if (!bregion->type->name) 
		return FALSE;
	if (_strcmpi(bregion->type->name,CR_REGION)) 
		return FALSE;
	if (pbAstral)
	{
		*pbAstral=FALSE;
		if (bregion->size==3 && bregion->ids[2]==1)
			*pbAstral=TRUE;
	}
	return TRUE;
}

/////////////////

CString CCR::reg_feature(eRegionFeature n)
{
	return s_sRegionFeatureEntries[n];
}

CString CCR::reg_featureInEnglish(eRegionFeature n)
{
	return CCR::German2English(s_sRegionFeatureEntries[n]);
}

BOOL CCR::reg_i(eRegionFeature n, block *b, int & nb)
{
	return geti(b, s_sRegionFeatureEntries[n], nb);
}

BOOL CCR::reg_s(eRegionFeature n, block *b, CString & s)
{
	return gets(b, s_sRegionFeatureEntries[n], s);
}

// remove name attr for oceans, firewall
int CCR::correctBadRegionNames()
{
	CString sTerrain("");
	CString sRegionName("");
	block * r = regions();
	int n=0;
	while (r)
	{
		if (_strcmpi(CR_REGION,r->type->name)) 
			break;

		gets(r,CR_TERRAIN,sTerrain);
		if (!sTerrain.CompareNoCase(CR_REGIONTYPE_OCEAN) || !sTerrain.CompareNoCase(CR_REGIONTYPE_FIREPROOF_WALL)) 
		{
			if (gets(r,CR_NAME,sRegionName) && !sRegionName.IsEmpty())
			{
				if (onSave_removeEntry(r,CR_NAME))
					n++;
			}

		}

		r=r->next;
	}
	return n;
}

/////////////////

block * CCR::factions(int *p) const 
{ 
	if (!m_bLoaded) return NULL;
	block*b=m_data->blocks;
	/*
	if (p) 	
	{
		block * f=factions();
		while (f)
		{ 
			if (b_isFaction(f)) 
			{
				if (f->ids[0]==*p)
					return f;
			}
			f=f->next;
		}
//		if (f)b=f->parent;
	}*/
	return (block*)m_parser->ireport->find(m_data, b, CR_FACTION, p, (p?1:0));
}

block * CCR::resourceByName(const CString & sWantedType, block  * rs, block * reg)
{
	if (!rs && reg) rs = resources(reg);
	if (!rs) return NULL;
	CString sType("");

	// does not work by find with id
	while (rs)
	{
		gets(rs, CR_RESOURCE_TYPE, sType);
		if (!sType.CompareNoCase(sWantedType))
			break;
		rs = rs->next;
	}		
	return rs;
}

void CCR::fillReport(int nOptions)
{
	m_report.fill(nOptions);
}
