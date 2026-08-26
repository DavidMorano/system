/* ciq HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* circular (really doubly linked) interlocked queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-27, David A­D­ Morano
	This module was adapted from the PPI/LPPI OS code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CIQ_INCLUDE
#define	CIQ_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<pq.h>			/* LIBUC */


#define	CIQ		struct ciq_head
#define	CIQ_MAGIC	0x9635230


struct ciq_head {
	ptm		*mxp ;
	pq		*fifop ;
	pq		*freep ;
	uint		magval ;
} ; /* end struct (ciq_head) */

#ifdef	__cplusplus
enum ciqmems {
	ciqmem_start,
	ciqmem_count,
	ciqmem_audit,
	ciqmem_finish,
	ciqmem_overlast
} ; /* end enum (ciqmems) */
struct ciq ;
struct ciq_co {
        ciq		*op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (ciq *p,int m) noex {
            op = p ;
            w = m ;
        } ; /* end */
        operator int () noex ;
	int operator () () noex {
	    return operator int () ;
	} ; /* end */
} ; /* end struct (ciq_co) */
struct ciq : ciq_head {
	ciq_co		start ;
	ciq_co		count ;
	ciq_co		finish ;
	constexpr ciq() noex {
	    start	(this,ciqmem_start) ;
	    count	(this,ciqmem_count) ;
	    audit	(this,ciqmem_audit) ;
	    finish	(this,ciqmem_finish) ;
	    magval = 0 ;
	} ; /* end ctor */
	ciq(const ciq &) = delete ;
	ciq &operator = (const ciq &) = delete ;
	int	ins	(void *) noex ;
	int	rem	(void *) noex ;
	int	gettail	(void *) noex ;
	int	remtail	(void *) noex ;
	int	rement	(void *) noex ;
	void	dtor() noex ;
	operator int () noex ;
	destruct ciq() {
	    if (magval) dtor() ;
	} ; /* end dtor (ciq) */
} ; /* end class (ciq) */
#else
typedef CIQ		ciq ;
#endif /* __cplusplus */

EXTERNC_begin

extern int ciq_start	(ciq *) noex ;
extern int ciq_ins	(ciq *,void *) noex ;
extern int ciq_rem	(ciq *,void *) noex ;
extern int ciq_gettail	(ciq *,void *) noex ;
extern int ciq_remtail	(ciq *,void *) noex ;
extern int ciq_rement	(ciq *,void *) noex ;
extern int ciq_count	(ciq *) noex ;
extern int ciq_audit	(ciq *) noex ;
extern int ciq_finish	(ciq *) noex ;

EXTERNC_end


#endif /* CIQ_INCLUDE */


