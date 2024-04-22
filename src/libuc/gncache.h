/* gncache HEADER */
/* lang=C20 */

/* group-name cache */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

#ifndef	GNCACHE_INCLUDE
#define	GNCACHE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<vechand.h>
#include	<cq.h>
#include	<localmisc.h>		/* |GROUPNAME| */


#define	GNCACHE			struct gncache_head
#define	GNCACHE_ENT		struct gncache_entry
#define	GNCACHE_ST		struct gncache_stats

#define	GNCACHE_MAGIC		0x98643162
#define	GNCACHE_DEFENT		10
#define	GNCACHE_DEFMAX		20	/* default maximum entries */
#define	GNCACHE_DEFTTL		600	/* default time-to-live */
#define	GNCACHE_MAXFREE		4


struct gncache_e {
	gid_t		gid ;
	char		groupname[GROUPNAMELEN + 1] ;
} ;

struct gncache_s {
	uint		nentries ;
	uint		total ;			/* access */
	uint		refreshes ;
	uint		phits, pmisses ;	/* positive */
	uint		nhits, nmisses ;	/* negative */
} ;

struct gncache_head {
	cq		recsfree ;
	vechand		recs ;
	time_t		ti_check ;
	GNCACHE_ST	s ;
	uint		magic ;
	int		ttl ;		/* time-to-live in seconds */
	int		nmax ;		/* maximum entries */
} ;

typedef	GNCACHE		gncache ;
typedef	GNCACHE_ST	gncache_st ;
typedef	GNCACHE_ENT	gncache_ent ;

EXTERNC_begin

extern int gncache_start(gncache *,int,int) noex ;
extern int gncache_add(gncache *,gid_t,char *) noex ;
extern int gncache_lookname(gncache *,gncache_ent *,char *) noex ;
extern int gncache_lookgid(gncache *,gncache_ent *,gid_t) noex ;
extern int gncache_check(gncache *,time_t) noex ;
extern int gncache_stats(gncache *,gncache_st *) noex ;
extern int gncache_finish(gncache *) noex ;

EXTERNC_end


#endif /* GNCACHE_INCLUDE */


