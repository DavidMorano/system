/* yags HEADER */
/* lang=C20 */


/* revision history:

	= 2001-03-22, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	YAGS_INCLUDE
#define	YAGS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>

#include	"bpload.h"


#define	YAGS_MAGIC	0x29456781
#define	YAGS		struct yags_head
#define	YAGS_STATS	struct yags_stats

/* more important defines */
#define	YAGS_TAGBITS		8	/* number of tag bits */
#define	YAGS_COUNTBITS		2	/* counter bits */

#define	YAGS_TAGMASK		((1 << YAGS_TAGBITS) - 1)


struct yags_stats {
	uint			cpht ;		/* choice length */
	uint			dpht ;		/* cache length */
	uint			bits ;		/* total bits */
} ;

struct yags_pht {
	uint	counter : 2 ;
} ;

struct yags_cache {
	uint	tag0 : (YAGS_TAGBITS + 1) ;	/* IAoff + 1 history */
	uint	tag1 : (YAGS_TAGBITS + 1) ;	/* IAoff + 1 history */
	uint	counter0 : 2 ;
	uint	counter1 : 2 ;
	uint	lru : 1 ;			/* least recently used */
} ;

struct yags_head {
	struct yags_stats	s ;
	struct yags_pht		*choice ;
	struct yags_cache	*taken, *nottaken ;
	uint			magic ;
	uint			bhistory ;	/* global branch history */
	uint			chlen ;		/* choice length */
	uint			calen ;		/* cache length */
} ;

EXTERNC_begin

extern int	yags_init(YAGS *,int,int) ;
extern int	yags_lookup(YAGS *,uint) ;
extern int	yags_update(YAGS *,uint,int) ;
extern int	yags_zerostats(YAGS *) ;
extern int	yags_stats(YAGS *,YAGS_STATS *) ;
extern int	yags_free(YAGS *) ;

EXTERNC_end


#endif /* YAGS_INCLUDE */


