/*
 *  crparse - a parser for reading Eressea CR files.
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
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "crparse.h"

struct block; //tmp

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void
read_int(parse_info * info, block_t b, char * buffer)
{
  char * tag = buffer;
  char * end;
  int values[10];
  int i = 1;
  while (*tag!=';')
  {
    int k = 0;
    int d;
    if (*tag=='-') {
      ++tag;
      d = -1;
    } else d = 1;
    while (*tag && isdigit(*tag)) {
      k = k*10 + *tag - '0';
      ++tag;
    }
    values[i++] = k * d;
    while (*tag && !isdigit(*tag) && *tag!='-' && *tag!=';') ++tag;
    if (!*tag) {
      if (info->verbose>0) 
		  fprintf(stderr, "parse error: missing ';' in line %d\n", info->line);
      return;
    }
  }
  values[0] = i-1;
  while (*tag && (*tag==';' || isspace(*(unsigned char*)tag))) ++tag;
  if(!*tag) {
    if (info->verbose>0) 
		fprintf(stderr, "parse error: Missing name for attribute in line %d\n", info->line);
    return;
  }
  end = tag+strlen(tag);
  while (isspace(*(const unsigned char*)(end-1))) --end;
  *end = 0;
  if (values[0]==1) {
    if (info->iblock->set_int) info->iblock->set_int(info->bcontext, b, tag, values[1]);
  }
  else {
    if (info->iblock->set_ints) info->iblock->set_ints(info->bcontext, b, tag, values+1, values[0]);
  }
}

void
read_string(parse_info * info, block_t b, char * buffer)
{

  char * value = buffer+1;
  char * tag = value;
  char * end;


  while (*tag && (*tag!='"' || *(tag-1)=='\\')) ++tag;
  if(!*tag) {
    if (info->verbose>0) 
		fprintf(stderr, "error in CR format. line %d: Missing '\"'\n", info->line);
    return;
  }
  *tag++=0;
  while (*tag && (*tag==';' || isspace(*(const unsigned char*)tag))) ++tag;
  if (*tag) {
    end = tag+strlen(tag);
    while (isspace(*(const unsigned char*)(end-1))) --end;
    *end = 0;
    if (info->iblock->set_string) info->iblock->set_string(info->bcontext, b, tag, value);
  }
  else
    if (info->iblock->set_entry) info->iblock->set_entry(info->bcontext, b, value);
}

/*
const char * _to_ansi(const char *utf8text)
{
	char m[1024*32];
	WCHAR w[1024*32];

	strcpy_s(m, 1024*32, utf8text);
	MultiByteToWideChar(CP_UTF8, 0, m, -1, w, sizeof(w) / sizeof(WCHAR));
	WideCharToMultiByte(CP_ACP, 0, w, -1, m, sizeof(m), 0, 0);
	return m;
}

const char * ansi_to_UTF8(const char *ansitext) 
{
	char m[1024*32];
	WCHAR w[1024*32];

	strcpy_s(m, 500, ansitext);
	MultiByteToWideChar(CP_ACP, 0, m, -1, w, sizeof(w) / sizeof(WCHAR));
	WideCharToMultiByte(CP_UTF8, 0, w, -1, m, sizeof(m), 0, 0);
	return m;
}
*/

void cr_parse(parse_info * info, void * infile)
{
  FILE * in = (FILE*)infile;
  char buffer[1024 * 32];
  block_t b = NULL;
  struct block *bb=NULL;;

  info->line = 1;

  while (!feof(in) && fgets(buffer, 1024 * 32, in)) {
	//_to_ansi(buffer);
	if (info->line==1) {
		  char * pts=strstr(buffer,"VERSION");
		  if (pts && pts!=buffer) strcpy_s(buffer,1024*32,pts);
	}
    if (buffer[0]=='"')
	{
		if (b) 
			read_string(info, b, buffer);
	}
	// isdigit pbm due to UTF8
    else if (buffer[0]=='-' || iswdigit(buffer[0]))
	{
		if (b) 
			read_int(info, b, buffer);
	}
    else if (iswalpha(buffer[0])) {
      int ids[10];
      char * name = buffer;
      char * id = buffer;
      int i=1;
      while (*id) {
        while (*id && !isspace(*(const unsigned char*)id)) ++id;
        if (*id) {
          if (i==1) *id++ = 0;
          while (*id && *id != '-' && !isdigit(*id)) ++id;
          if (*id) {
            int k = 0;
            int d = 1;
            if (*id=='-') {
              ++id;
              d = -1;
            }
            while (*id && isdigit(*id)) {
              k = k*10 + *id - '0';
              ++id;
            }
            ids[i++] = d * k;
          }
        }
      }
      ids[0] = i-1;
      {
        block_t last = b;
	    bb=(struct block *)last;
		if (last && info->ireport->add) info->ireport->add(info->bcontext, last);
        if (info->ireport->create) b = info->ireport->create(info->bcontext, name, ids+1, ids[0]);
      }
    }
    info->line++;
  }
  if (b && info->ireport->add) info->ireport->add(info->bcontext, b);
}
