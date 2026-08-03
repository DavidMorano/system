/* piq HEADER */
/* charset=ISO8859-1 */
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
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<pq.h>			/* LIBUC */


#define	PIQ		struct piq_head
#define	PIQ_ENT		struct piq_entry
#define	PIQ_MAGIC	0x9635230


struct piq_entry {
	PIQ_ENT		*next ;
	PIQ_ENT		*prev ;
} ; /* end struct (piq_entry) */

struct piq_head {
	ptm		*mxp ;
	pq		*qlp ;
	uint		magval ;
} ; /* end struct (piq_head) */

#ifdef	__cplusplus
enum piqmems {
	piqmem_start,
	piqmem_count,
	piqmem_audit,
	piqmem_finish,
	piqmem_overlast
} ; /* end enum (piqmems) */
struct piq ;
struct piq_co {
        piq		*op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (piq *p,int m) noex {
            op = p ;
            w = m ;
        } ;
        operator int () noex ;
	int operator () () noex {
	    return operator int () ;
	} ;
} ; /* end struct (piq_co) */
struct piq : piq_head {
	piq_co		start ;
	piq_co		count ;
	piq_co		audit ;
	piq_co		finish ;
	constexpr piq() noex {
	    start	(this,piqmem_start) ;
	    count	(this,piqmem_count) ;
	    audit	(this,piqmem_audit) ;
	    finish	(this,piqmem_finish) ;
	    magval = 0 ;
	} ; /* end ctor */
	piq(const piq &) = delete ;
	piq &operator = (const piq &) = delete ;
	int ins		(void *) noex ;
	int rem		(void *) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct piq() {
	    if (magval) dtor() ;
	} ; /* end dtor (piq) */
} ; /* end class (piq) */
#else
typedef PIQ		piq ;
#endif /* __cplusplus */

typedef PIQ_ENT		piq_ent ;

EXTERNC_begin

extern int piq_start	(piq *) noex ;
extern int piq_ins	(piq *,void *) noex ;
extern int piq_rem	(piq *,void *) noex ;
extern int piq_count	(piq *) noex ;
extern int piq_audit	(piq *) noex ;
extern int piq_finish	(piq *) noex ;

EXTERNC_end


#endif /* PIQ_INCLUDE */


