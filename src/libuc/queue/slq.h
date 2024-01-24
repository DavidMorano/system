/* slq HEADER */
/* lang=C20 */

/* Circular-Pointer-Queue */
/* regular (no-frills) pointer queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was modeled after assembly.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SLQ_INCLUDE
#define	SLQ_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<clanguage.h>


#define	SLQ		struct slq_head
#define	SLQ_ENT		struct slq_ent


struct slq_ent {
	SLQ_ENT		*next ;
} ;

struct slq_head {
	SLQ_ENT		*head ;
	SLQ_ENT		*tail ;
} ;

typedef SLQ		slq ;
typedef SLQ_ENT		slq_ent ;

EXTERNC_begin

extern int slq_start(slq *) noex ;
extern int slq_ins(slq *,slq_ent *) noex ;
extern int slq_insgroup(slq *,void *,int,int) noex ;
extern int slq_rem(slq *,slq_ent **) noex ;
extern int slq_gettail(slq *,slq_ent **) noex ;
extern int slq_audit(slq *) noex ;
extern int slq_finish(slq *) noex ;

EXTERNC_end


#endif /* SLQ_INCLUDE */


