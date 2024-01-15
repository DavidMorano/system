/* aiq HEADER */
/* lang=C20 */

/* asynchronous interrupt queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This module was originally written.

	= 1998-07-01, David A­D­ Morano
	This module was enhanced by adding the POSIX thread mutex calls.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	AIQ_INCLUDE
#define	AIQ_INCLUDE


#include	<envstandards.h>
#include	<stddef.h>		/* <- for |ptrdiff_t| */
#include	<pthread.h>
#include	<usystem.h>
#include	<q.h>


#define	AIQ		struct aiq_head
#define	AIQ_ENT		struct aiq_entry
#define	AIQ_MAGIC	0x76925634


struct aiq_entry {
	ptrdiff_t	next ;
	ptrdiff_t	prev ;
} ;

struct aiq_head {
	q		qo ;
	uint		magic ;
} ;

typedef	AIQ		aiq ;
typedef	AIQ_ENT		aiq_ent ;

EXTERNC_begin

extern int aiq_start(AIQ *,int) noex ;
extern int aiq_finish(AIQ *) noex ;
extern int aiq_ins(AIQ *,AIQ_ENT *) noex ;
extern int aiq_inshead(AIQ *,AIQ_ENT *) noex ;
extern int aiq_rem(AIQ *,AIQ_ENT **) noex ;
extern int aiq_remtail(AIQ *,AIQ_ENT **) noex ;
extern int aiq_count(AIQ *) noex ;

EXTERNC_end


#endif /* AIQ_INCLUDE */


