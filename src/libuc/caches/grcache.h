/* grcache HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* GROUP cache */
/* version %I% last-modified %G% */


/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

#ifndef	GRCACHE_INCLUDE
#define	GRCACHE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® system types */
#include	<time.h>		/* POSIX® |time_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vechand.h>		/* LIBUC */
#include	<cq.h>			/* LIBUC */


#define	GRCACHE			struct grcache_head
#define	GRCACHE_ST		struct grcache_stats
#define	GRCACHE_MAGIC		0x98643162
#define	GRCACHE_DEFENTS		10
#define	GRCACHE_DEFMAX		20	/* default maximum entries */
#define	GRCACHE_DEFTTL		600	/* default time-to-live */
#define	GRCACHE_MAXFREE		4


struct grcache_stats {
	uint		nentries ;		/* number of current entries */
	uint		total ;			/* accesses */
	uint		refreshes ;		/* refreshes */
	uint		phits, pmisses ;	/* positive */
	uint		nhits, nmisses ;	/* negative */
} ; /* end struct (gncache_stats) */

struct grcache_head {
	cq		*flp ;		/* free-list-pointer */
	vechand		*rlp ;		/* record-list-pointer */
	time_t		ti_check ;
	GRCACHE_ST	s ;
	uint		magval ;
	uint		wcount ;
	int		ttl ;		/* time-to-live */
	int		nmax ;		/* maximum entries */
} ; /* end struct (gncache_head) */

typedef	GRCACHE		grcache ;
typedef	GRCACHE_ST	grcache_st ;

EXTERNC_begin

extern int grcache_start(grcache *,int,int) noex ;
extern int grcache_lookname(grcache *,ucentgr *,char *,int,cchar *) noex ;
extern int grcache_lookgid(grcache *,ucentgr *,char *,int,gid_t) noex ;
extern int grcache_check(grcache *,time_t) noex ;
extern int grcache_getstats(grcache *,grcache_st *) noex ;
extern int grcache_finish(grcache *) noex ;

EXTERNC_end


#endif /* GRCACHE_INCLUDE */


