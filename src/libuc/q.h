/* q INCLUDE */
/* lang=C20 */

/* self-relative double-linked queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	Q_INCLUDE
#define	Q_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stddef.h>		/* <- for |ptrdiff_t| */
#include	<stdint.h>		/* <- for |intptr_t| */
#include	<usystem.h>
#include	<plainq.h>
#include	<ptm.h>


#define	Q		struct q_head
#define	Q_ENT		struct q_entry

#define	Q_TPRIVATE	0		/* q-type: private to a process */
#define	Q_TSHARED	1		/* q-type: shared among processes */


struct q_entry {
	ptrdiff_t	next ;
	ptrdiff_t	prev ;
} ;

struct q_head {
	plainq		pq ;
	ptm		m ;
} ;

typedef	Q		q ;
typedef	Q_ENT		q_ent ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int q_start(Q *,int) noex ;
extern int q_finish(Q *) noex ;
extern int q_ins(Q *,Q_ENT *) noex ;
extern int q_inshead(Q *,Q_ENT *) noex ;
extern int q_rem(Q *,Q_ENT **) noex ;
extern int q_remtail(Q *,Q_ENT **) noex ;
extern int q_count(Q *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* Q_INCLUDE */


