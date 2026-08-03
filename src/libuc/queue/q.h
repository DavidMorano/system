/* q HEADER */
/* charset=ISO8859-1 */
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
#include	<stddef.h>		/* CSTD |ptrdiff_t| */
#include	<stdlib.h>		/* CSTD */
#include	<stdint.h>		/* CSTD |intptr_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<plainq.h>		/* LIBUC */


#define	Q		struct q_head
#define	Q_ENT		struct q_entry

#define	Q_TPRIVATE	0		/* q-type: sharing-type - private */
#define	Q_TSHARED	1		/* q-type: sharing-type - shared */


struct q_entry {
	ptrdiff_t	next ;
	ptrdiff_t	prev ;
} ; /* end class (q_entry) */

struct q_head {
	ptm		*mxp ;
	plainq		*pqp ;
} ; /* end class (q_head) */

typedef	Q_ENT		q_ent ;

#ifdef	__cplusplus
enum qmems {
	qmem_start,
	qmem_count,
	qmem_finish,
	qmem_overlast
} ; /* end enum (qmems) */
struct q ;
struct q_co {
        q		*op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (q *p,int m) noex {
            op = p ;
            w = m ;
        } ;
	int operator () (int = 0) noex ;
        operator int () noex {
	    return operator () (0) ;
	} ;
} ; /* end struct (q_co) */
struct q : q_head {
	q_co		start ;
	q_co		count ;
	q_co		finish ;
	constexpr q() noex {
	    start	(this,qmem_start) ;
	    count	(this,qmem_count) ;
	    finish	(this,qmem_finish) ;
	    mxp = nullptr ;
	} ; /* end ctor */
	q(const q &) = delete ;
	q &operator = (const q &) = delete ;
	int	ins	(q_ent *) noex ;
	int	inshead	(q_ent *) noex ;
	int	rem	(q_ent **) noex ;
	int	remtail	(q_ent **) noex ;
	void	dtor() noex ;
	operator int () noex ;
	destruct q() {
	    if (mxp) dtor() ;
	} ; /* end dtor (q) */
} ; /* end class (q) */
#else
typedef Q		q ;
#endif /* __cplusplus */

EXTERNC_begin

extern int q_start	(q *,int) noex ;
extern int q_finish	(q *) noex ;
extern int q_ins	(q *,q_ent *) noex ;
extern int q_inshead	(q *,q_ent *) noex ;
extern int q_rem	(q *,q_ent **) noex ;
extern int q_remtail	(q *,q_ent **) noex ;
extern int q_count	(q *) noex ;

EXTERNC_end


#endif /* Q_INCLUDE */


