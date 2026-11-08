/*
 *  crdata - data structures for Eressea CR files.
 *  Copyright (C) 2000 Enno Rehling
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef _CR_DATA_H
#define _CR_DATA_H

#include "crnames.h"
#include "crparse.h"

#define TMAXHASH 1023
#define BMAXHASH 1023
#define KEYMASK 0xFFFF
#define KEYSIZE 16

struct blocktype;

typedef struct property {
  struct property * nexthash;
  unsigned int hashkey : KEYSIZE;
  const char * name;
} property;

typedef enum { E_NONE, E_STRING, E_MESSAGE, E_INT, E_INTS } e_type;

typedef struct entry {
  struct entry * next;
  struct property * tag;
  union {
    void * vp;
    char * cp;
    int * ip;
    int i;
  } data;
  e_type type;
} entry;

typedef struct block {
  struct block * parent;
  struct block * next;
  struct block * children;
  struct block * nexttype; /* next object of same type with the same unique parent */
  struct block * nexthash; /* hashtable to find a block based on its type and ids */

  int turn;
  struct blocktype * type;
  struct entry * entries;
  int * ids;
  size_t size;
} block;

/** the context for one report is stored in a crdata structure
 * it contains the hierarchy, data blocks, parser definition
 * as well as additional datastructures (hashes, typedefinitions)
 * that the user should not have to worry about.
 * */

typedef struct crdata {
  /* the root of the block tree: */
  struct block * blocks;

  /* the last block we've parsed: */
  struct block * current;

  /* the block containing the version info: */
  struct blocktype * version;

  /* info used during the parsing: */
  int turnstack[16];
  int tstack;
  parse_info * parser; /* required in here to get the current lineno */

  /* hashtables: */
  block * blockhash[BMAXHASH];
  property * taghash[TMAXHASH];
  struct blocktype * blocktypes;

  void *parent;

} crdata;

extern void cr_writeblock_echeck_min(crdata * data, FILE * out, block *b);
extern void cr_writeblock_echeck(crdata * data, FILE * out, block *b);
extern void cr_writeblock_std(crdata * data, FILE * out, block *b);

extern void (*cr_write)(crdata * data, FILE * out, block *b);

extern crdata * crdata_init(FILE * hierarchy,void* parent=NULL);
extern void crdata_destroy(struct crdata *);
extern const block_interface crdata_iblock;

extern const report_interface crdata_ireport_std;
extern const report_interface crdata_ireport_echeck;
extern const report_interface crdata_ireport_save;

/* return values for crdata_iblock::get functions */
#define CR_SUCCESS 0
#define CR_NOENTRY -1
#define CR_ILLEGALTYPE -2

extern int verbose;

extern int block_move(block *u, block * bnr);
extern int block_remove(block *u);
extern int block_removeBrothers(block *u);
extern int block_removeChildren(block *u);
extern int block_entry_remove(context_t context, block_t bt, const char *pTagname);

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#endif
