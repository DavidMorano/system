/* cachetime HEADER */
/* lang=C20 */

/* cache-time manager */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-09-10, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

#ifndef	CACHETIME_INCLUDE
#define	CACHETIME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<hdb.h>
#include	<ptm.h>
#include	<localmisc.h>


#define	CACHETIME_MAGIC		0x79854123
#define	CACHETIME		struct cachetime_head
#define	CACHETIME_CUR		struct cachetime_cursor
#define	CACHETIME_STATS		struct cachetime_s


struct cachetime_s {
	uint		req, hit, miss ;
} ;

struct cachetime_cursor {
	hdb_cur		cur ;
} ;

struct cachetime_e {
	cchar		*name ;
	time_t		mtime ;
} ;

struct cachetime_head {
	hdbB		db ;
	ptmM		m ;
	uint		magic ;
	uint		c_req ;
	uint		c_hit ;
	uint		c_miss ;
} ;

typedef CACHETIME		cachetime ;
typedef CACHETIME_CUR		cachetime_cur ;
typedef CACHETIME_STATS		cachetime_stats ;

EXTERNC_begin

extern int cachetime_start(CACHETIME *) noex ;
extern int cachetime_lookup(CACHETIME *,cchar *,int,time_t *) noex ;
extern int cachetime_curbegin(CACHETIME *,CACHETIME_CUR *) noex ;
extern int cachetime_curend(CACHETIME *,CACHETIME_CUR *) noex ;
extern int cachetime_enum(CACHETIME *,CACHETIME_CUR *,char *,int,
		time_t *) noex ;
extern int cachetime_stats(CACHETIME *,CACHETIME_STATS *) noex ;
extern int cachetime_finish(CACHETIME *) noex ;

EXTERNC_end


#endif /* CACHETIME_INCLUDE */


