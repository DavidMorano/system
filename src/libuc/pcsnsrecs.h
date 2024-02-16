/* pcsnsrecs HEADER */
/* lang=C20 */

/* PCS Name-Server Records */
/* version %I% last-modified %G% */


/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSNSRECS_INCLUDE
#define	PCSNSRECS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<pq.h>
#include	<localmisc.h>		/* extra types */


#define	PCSNSRECS_MAGIC		0x98643168
#define	PCSNSRECS		struct pcsnsrecs_head
#define	PCSNSRECS_ST		struct pcsnsrecs_s

#define	PCSNSRECS_DEFENTS	10	/* default entries */
#define	PCSNSRECS_DEFMAX	20	/* default maximum entries */
#define	PCSNSRECS_DEFTTL	(10*60)	/* default time-to-live */


struct pcsnsrecs_s {
	uint		nentries ;		/* number of current entries */
	uint		total ;			/* accesses */
	uint		refreshes ;		/* refreshes */
	uint		phits, pmisses ;	/* positive */
	uint		nhits, nmisses ;	/* negative */
} ;

struct pcsnsrecs_head {
	void		*recs ;		/* linear array (of recs) */
	PCSNSRECS_ST	s ;
	pq		lru ;		/* least-recently-used */
	time_t		ti_check ;
	uint		wcount ;	/* write-count */
	uint		magic ;
	int		ttl ;		/* time-to-live */
	int		max ;		/* maximum entries */
} ;

typedef PCSNSRECS	pcsnsrecs ;

EXTERNC_begin

extern int pcsnsrecs_start(PCSNSRECS *,int,int) noex ;
extern int pcsnsrecs_store(PCSNSRECS *,cchar *,int,cchar *,int,int) noex ;
extern int pcsnsrecs_lookup(PCSNSRECS *,char *,int,cchar *,int) noex ;
extern int pcsnsrecs_invalidate(PCSNSRECS *,cchar *,int) noex ;
extern int pcsnsrecs_check(PCSNSRECS *,time_t) noex ;
extern int pcsnsrecs_stats(PCSNSRECS *,PCSNSRECS_ST *) noex ;
extern int pcsnsrecs_finish(PCSNSRECS *) noex ;

EXTERNC_end


#endif /* PCSNSRECS_INCLUDE */


