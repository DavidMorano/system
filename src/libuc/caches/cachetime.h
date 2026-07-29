/* cachetime HEADER */
/* charset=ISO8859-1 */
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
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<hdb.h>			/* LIBUC */


#define	CACHETIME		struct cachetime_head
#define	CACHETIME_CUR		struct cachetime_cursor
#define	CACHETIME_ENT		struct cachetime_entry
#define	CACHETIME_ST		struct cachetime_statistics
#define	CACHETIME_MAGIC		0x40096056
#define	CACHETIME_NENTS		400


struct cachetime_statistics {
	uint		req ;
	uint		hit ;
	uint		miss ;
} ; /* end struct (cachetime_statistics) */

struct cachetime_cursor {
	hdb_cur		*hcp ;
} ; /* end struct (cachetime_cursor) */

struct cachetime_entry {
	cchar		*name ;
	time_t		mtime ;
} ; /* end struct (cachetime_entry) */

struct cachetime_head {
	hdb		*dbp ;
	ptm		*mxp ;
	uint		magval ;
	uint		c_req ;
	uint		c_hit ;
	uint		c_miss ;
} ; /* end struct (cachetime_head) */

typedef CACHETIME		cachetime ;
typedef CACHETIME_CUR		cachetime_cur ;
typedef CACHETIME_ENT		cachetime_ent ;
typedef CACHETIME_ST		cachetime_st ;

EXTERNC_begin

extern int cachetime_start(cachetime *) noex ;
extern int cachetime_lookup(cachetime *,cchar *,int,time_t *) noex ;
extern int cachetime_curbegin(cachetime *,cachetime_cur *) noex ;
extern int cachetime_curend(cachetime *,cachetime_cur *) noex ;
extern int cachetime_curenum(cachetime *,cachetime_cur *,char *,int,
		time_t *) noex ;
extern int cachetime_stats(cachetime *,cachetime_st *) noex ;
extern int cachetime_finish(cachetime *) noex ;

EXTERNC_end


#endif /* CACHETIME_INCLUDE */


