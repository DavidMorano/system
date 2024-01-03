/* piq HEADER */
/* lang=C20 */

/* pointer interlocked queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PIQ_INCLUDE
#define	PIQ_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<usystem.h>
#include	<ptm.h>
#include	<pq.h>
#include	<localmisc.h>


#define	PIQ		struct piq_head
#define	PIQ_ENT		struct piq_ent
#define	PIQ_MAGIC	0x9635230


struct piq_ent {
	PIQ_ENT		*next ;
	PIQ_ENT		*prev ;
} ;

struct piq_head {
	ptm		m ;
	pq		frees ;
	uint		magic ;
} ;

typedef	PIQ		piq ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int piq_start(PIQ *) noex ;
extern int piq_ins(PIQ *,void *) noex ;
extern int piq_rem(PIQ *,void *) noex ;
extern int piq_count(PIQ *) noex ;
extern int piq_audit(PIQ *) noex ;
extern int piq_finish(PIQ *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* PIQ_INCLUDE */


