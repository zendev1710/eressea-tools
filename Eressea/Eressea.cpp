#include "config.h"
#include "crnames.h"
#include "Eressea.h"

#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char * sCastleNames[] = {
	EN_FOUNDATION,
	EN_TRADEPOST,
	EN_TOWER,
	EN_STRONGHOLD,
	EN_FORTRESS,
	EN_CITADEL
};

const char * e_castleNameFromCoef(unsigned int nCoef)
{
	int nCoefi=nCoef-1;
	if (nCoefi<0) return "unknown";
	if (nCoefi>5) return "unknown";
	return sCastleNames[nCoefi];
}

unsigned int e_castleBoundFromCoef(unsigned int nCoef)
{
	if (nCoef == 6) return CASTLESIZE_CITADEL;
	if (nCoef == 5) return CASTLESIZE_FORTRESS;
	if (nCoef == 4) return CASTLESIZE_STRONGHOLD;
	if (nCoef == 3) return CASTLESIZE_TOWER;
	if (nCoef == 2) return CASTLESIZE_FOUNDATION;
	return 0;
}

unsigned int e_castleCoef(unsigned int nCastleSize)
{
	unsigned int nCoef = 6;
	if (nCastleSize < CASTLESIZE_CITADEL) nCoef = 5;
	if (nCastleSize < CASTLESIZE_FORTRESS) nCoef = 4;
	if (nCastleSize < CASTLESIZE_STRONGHOLD) nCoef = 3;
	if (nCastleSize < CASTLESIZE_TOWER) nCoef = 2;
	if (nCastleSize < CASTLESIZE_FOUNDATION) nCoef = 1;
	return nCoef;
}

unsigned int e_taxationCoef(unsigned int nCastleSize)
{
	return e_castleCoef(nCastleSize);
}

unsigned int e_orcWageDiff(unsigned int nCastleSize)
{
	unsigned int nDiff = 2;
	if (nCastleSize < CASTLESIZE_FORTRESS) nDiff = 1;
	if (nCastleSize < CASTLESIZE_TOWER) nDiff = 0;
	return nDiff;
}

//TODO: extract english terms
unsigned int e_silverUpkeepBuilding(const char * pTypeEN)
{
	if (!_strcmpi(pTypeEN,"caravanserei")) return 3000;
	if (!_strcmpi(pTypeEN,"academy")) return 1000;
	if (!_strcmpi(pTypeEN,"dam")) return 1000;
	if (!_strcmpi(pTypeEN,"mage tower")) return 1000;
	if (!_strcmpi(pTypeEN,"mine")) return 500;
	if (!_strcmpi(pTypeEN,"smithy")) return 300;
	if (!_strcmpi(pTypeEN,"sawmill")) return 250;
	if (!_strcmpi(pTypeEN,"quarry")) return 250;
	if (!_strcmpi(pTypeEN,"harbour")) return 250;
	if (!_strcmpi(pTypeEN,"stable")) return 150;
	if (!_strcmpi(pTypeEN,"lighthouse")) return 100;
	if (!_strcmpi(pTypeEN,"tunnel")) return 100;
	//if (!strcmpi(pTypeEN,"inn")) return 0; //TODO
	return 0;
}