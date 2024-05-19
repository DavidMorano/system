/* gskew HEADER */
/* lang=C20 */


/* revision history:

	= 2001-03-22, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	GSKEW_INCLUDE
#define	GSKEW_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


#define	GSKEW_MAGIC		0x45678429
#define	GSKEW			struct gskew_head
#define	GSKEW_STATS		struct gskew_stats

/* more important defines */
#define	GSKEW_COUNTBITS	2	/* counter bits */


struct gskew_stats {
	uint	tlen ;
	uint	bits ;
	uint	lu ;
	uint	use_bim ;
	uint	use_eskew ;
	uint	update_meta ;
	uint	updateup_meta ;
	uint	update_all, update_bim, update_eskew ;
} ;

struct gskew_banks {
	uint	bim : 2 ;
	uint	g0 : 2 ;
	uint	g1 : 2 ;
	uint	meta : 2 ;
} ;

struct gskew_head {
	struct gskew_stats	s ;
	struct gskew_banks	*table ;
	uint			magic ;
	uint			bhistory ;	/* global branch history */
	uint			tlen ;
	int			n ;		/* 'n' from the papers ! :-) */
	uint			nhist ;		/* history bits */
	uint			tmask, hmask ;
} ;

EXTERNC_begin

extern int	gskew_init(GSKEW *,int,int,int,int) ;
extern int	gskew_lookup(GSKEW *,uint) ;
extern int	gskew_confidence(GSKEW *,uint) ;
extern int	gskew_update(GSKEW *,uint,int) ;
extern int	gskew_zerostats(GSKEW *) ;
extern int	gskew_stats(GSKEW *,GSKEW_STATS *) ;
extern int	gskew_free(GSKEW *) ;

EXTERNC_end


#endif /* GSKEW_INCLUDE */


