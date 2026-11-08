#ifndef _ERESSEA_H
#define _ERESSEA_H

#ifdef __cplusplus
//extern "C" {
#endif

	#define CASTLESIZE_TRADEPOST	(2)
	#define CASTLESIZE_FOUNDATION	(10)
	#define CASTLESIZE_TOWER		(50)
	#define CASTLESIZE_STRONGHOLD	(250)
	#define CASTLESIZE_FORTRESS		(1250)
	#define CASTLESIZE_CITADEL		(6250)

	#define WAGE_MIN_VALUE			(10)
	
	#define EARNMONEY_AMOUNT_BASIC	(20)

	#define EARNMONEY_AMOUNT_ENTERTAINMENT	EARNMONEY_AMOUNT_BASIC
	#define EARNMONEY_AMOUNT_TAX			EARNMONEY_AMOUNT_BASIC

	#define TRADE_NBITEMS_BY_LEVEL	(10)
	
	///////////

	#define COMBATSTATUS_UNKNOWN		(-2)
	#define COMBATSTATUS_AGGRESSIVE		(0)
	#define COMBATSTATUS_FRONT			(1)
	#define COMBATSTATUS_REAR			(2) // BACK
	#define COMBATSTATUS_DEFENSIVE		(3)
	#define COMBATSTATUS_NOT			(4) // PASSIVE
	#define COMBATSTATUS_FLEE			(5) // ESCAPE

	///////////

	unsigned int e_taxationCoef(unsigned int nCastleSize);
	unsigned int e_castleCoef(unsigned int nCastleSize);
	unsigned int e_orcWageDiff(unsigned int nCastleSize);
	unsigned int e_castleBoundFromCoef(unsigned int nCoef);
	unsigned int e_silverUpkeepBuilding(const char * pBuildingTypeNameEN);

	const char * e_castleNameFromCoef(unsigned int nCoef);

#ifdef __cplusplus
// }
#endif

#endif
