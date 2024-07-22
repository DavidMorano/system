/* pcsnsmgr HEADER */
/* lang=C20 */

/* PCS Name-Server Cache */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSNSMGR_INCLUDE
#define	PCSNSMGR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>


#define	PCSNSMGR_STATS		struct pcsnsmgr_stats

#define	PCSNSMGR_MAX		120		/* max cache entries */
#define	PCSNSMGR_TTL		(20*60)		/* entry time-out in seconds */
#define	PCSNSMGR_TOLOCK		5


struct pcsnsmgr_stats {
	uint		max ;
	uint		ttl ;
	uint		nent ;
	uint		acc ;
	uint		phit, nhit ;
	uint		pmis, nmis ;
} ;

EXTERNC_begin

extern int pcsnsmgr_init() noex ;
extern int pcsnsmgr_set(cchar *,int,cchar *,int,int) noex ;
extern int pcsnsmgr_get(char *,int,cchar *,int) noex ;
extern int pcsnsmgr_invalidate(cchar *,int) noex ;
extern int pcsnsmgr_stats(PCSNSMGR_STATS *) noex ;
extern int pcsnsmgr_fini() noex ;

EXTERNC_end


#endif /* PCSNSMGR_INCLUDE */


