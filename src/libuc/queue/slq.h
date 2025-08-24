/* slq HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Single-List-Queue - regular (no-frills) single list pointer queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was modeled after assembly.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SLQ_INCLUDE
#define	SLQ_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	SLQ		struct slq_head
#define	SLQ_ENT		struct slq_ent


struct slq_ent {
	SLQ_ENT		*next ;
} ;

struct slq_head {
	SLQ_ENT		*head ;
	SLQ_ENT		*tail ;
} ;

typedef SLQ_ENT		slq_ent ;

#ifdef	__cplusplus
enum slqmems {
    	slqmem_start,
	slqmem_audit,
	slqmem_finish,
	slqmem_overlast
} ;
struct slq ;
struct slq_co {
	slq		*op = nullptr ;
	int		w = -1 ;
	void operator () (slq *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (slq_co) */
struct slq : slq_head {
	slq_co		start ;
	slq_co		audit ;
	slq_co		finish ;
	slq() noex {
	    start	(this,slqmem_start) ;
	    audit	(this,slqmem_audit) ;
	    finish	(this,slqmem_finish) ;
	    head = nullptr ;
	} ;
	slq(const slq &) = delete ;
	slq &operator = (const slq &) = delete ;
	int ins(slq_ent *) noex ;
	int insgroup(slq_ent *,int,int) noex ;
	int gethead(slq_ent **) noex ;
	int gettail(slq_ent **) noex ;
	int rem(slq_ent **) noex ;
	void dtor() noex ;
	destruct slq() {
	    if (head) dtor() ;
	} ;
} ; /* end struct (slq) */
#else	/* __cplusplus */
typedef SLIST		slq ;
#endif /* __cplusplus */


EXTERNC_begin

extern int slq_start(slq *) noex ;
extern int slq_ins(slq *,slq_ent *) noex ;
extern int slq_insgroup(slq *,void *,int,int) noex ;
extern int slq_rem(slq *,slq_ent **) noex ;
extern int slq_gethead(slq *,slq_ent **) noex ;
extern int slq_gettail(slq *,slq_ent **) noex ;
extern int slq_audit(slq *) noex ;
extern int slq_finish(slq *) noex ;

EXTERNC_end


#endif /* SLQ_INCLUDE */


