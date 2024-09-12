/* upwcache INCLUDE */
/* lang=C20 */

/* special process-wide cache for the password SYSDB database */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

#ifndef	UPWCACHE_INCLUDE
#define	UPWCACHE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<pwd.h>
#include	<usystem.h>
#include	<pq.h>


#define	UPWCACHE_MAGIC		0x98643168
#define	UPWCACHE		struct upwcache_head
#define	UPWCACHE_STATS		struct upwcache_s

#define	UPWCACHE_DEFENTS	10	/* default entries */
#define	UPWCACHE_DEFMAX		20	/* default maximum entries */
#define	UPWCACHE_DEFTTL		(10*60)	/* default time-to-live */


struct upwcache_s {
	uint		nentries ;		/* number of current entries */
	uint		total ;			/* accesses */
	uint		refreshes ;		/* refreshes */
	uint		phits, pmisses ;	/* positive */
	uint		nhits, nmisses ;	/* negative */
} ;

struct upwcache_head {
	uint		magic ;
	void		*recs ;		/* linear array (of recs) */
	UPWCACHE_STATS	s ;
	pq		lru ;		/* least-recently-used */
	time_t		ti_check ;
	uint		wcount ;	/* write-count */
	int		ttl ;		/* time-to-live */
	int		max ;		/* maximum entries */
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int upwcache_start(UPWCACHE *,int,int) noex ;
extern int upwcache_lookup(UPWCACHE *,PASSWD *,char *,int,cchar *) noex ;
extern int upwcache_uid(UPWCACHE *,PASSWD *,char *,int,uid_t) noex ;
extern int upwcache_invalidate(UPWCACHE *,cchar *) noex ;
extern int upwcache_check(UPWCACHE *,time_t) noex ;
extern int upwcache_stats(UPWCACHE *,UPWCACHE_STATS *) noex ;
extern int upwcache_finish(UPWCACHE *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UPWCACHE_INCLUDE */


