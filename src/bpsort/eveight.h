/* eveight HEADER */
/* lang=C20 */


/* revision history:

	= 2002-05-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	EVEIGHT_INCLUDE
#define	EVEIGHT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


#define	EVEIGHT_MAGIC		0x45678429
#define	EVEIGHT			struct eveight_head
#define	EVEIGHT_STATS		struct eveight_stats

/* more important defines */
#define	EVEIGHT_COUNTBITS	2	/* counter bits */


struct eveight_stats {
	uint		tlen ;
	uint		bits ;
} ;

struct eveight_banks {
	uint		bim : 2 ;
	uint		g0 : 2 ;
	uint		g1 : 2 ;
	uint		meta : 2 ;
} ;

struct eveight_head {
	struct eveight_stats	s ;
	struct eveight_banks	*table ;
	uint			magic ;
	uint			bhistory ;	/* global branch history */
	uint			tlen ;
	uint			tmask ;
} ;

EXTERNC_begin

extern int	eveight_init(EVEIGHT *,int,int,int,int) ;
extern int	eveight_lookup(EVEIGHT *,uint) ;
extern int	eveight_confidence(EVEIGHT *,uint) ;
extern int	eveight_update(EVEIGHT *,uint,int) ;
extern int	eveight_zerostats(EVEIGHT *) ;
extern int	eveight_stats(EVEIGHT *,EVEIGHT_STATS *) ;
extern int	eveight_free(EVEIGHT *) ;

EXTERNC_end


#endif /* EVEIGHT_INCLUDE */


