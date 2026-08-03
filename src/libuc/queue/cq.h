/* cq HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Container-Queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-17, David A­D­ Morano
	Oh what a cheap Q!  I do not know why I am doing this!

	= 2017-11-21, David A­D­ Morano
	Added new method |cq_unlink()|.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	CQ_INCLUDE
#define	CQ_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vechand.h>		/* LIBUC */


#define	CQ		struct cq_head
#define	CQ_CUR		struct cq_cursor
#define	CQ_MAGIC	0x65748392
#define	CQ_DEFENTS	10


struct cq_head {
	vechand		*qp ;
	uint		magval ;
} ; /* end struct (cq_head) */

struct cq_cursor {
	int		i ;
} ; /* end struct (cq_cursor) */

typedef CQ_CUR		cq_cur ;

#ifdef	__cplusplus
enum cqmems {
	cqmem_start,
	cqmem_count,
	cqmem_finish,
	cqmem_overlast
} ; /* end enum (cqmems) */
struct cq ;
struct cq_co {
        cq		*op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (cq *p,int m) noex {
            op = p ;
            w = m ;
        } ;
        operator int () noex ;
	int operator () () noex {
	    return operator int () ;
	} ;
} ; /* end struct (cq_co) */
struct cq : cq_head {
	cq_co		start ;
	cq_co		count ;
	cq_co		finish ;
	constexpr cq() noex {
	    start	(this,cqmem_start) ;
	    count	(this,cqmem_count) ;
	    finish	(this,cqmem_finish) ;
	    magval = 0 ;
	} ; /* end ctor */
	cq(const cq &) = delete ;
	cq &operator = (const cq &) = delete ;
	int ins		(void *) noex ;
	int rem		(void *) noex ;
	int unlink	(void *) noex ;
	int curbegin	(cq_cur *) noex ;
	int curend	(cq_cur *) noex ;
	int curenum	(cq_cur *,void *) noex ;
	void	dtor() noex ;
	operator int () noex ;
	destruct cq() {
	    if (magval) dtor() ;
	} ; /* end dtor (cq) */
} ; /* end class (cq) */
#else
typedef CQ		cq ;
#endif /* __cplusplus */

EXTERNC_begin

extern int cq_start	(cq *) noex ;
extern int cq_finish	(cq *) noex ;
extern int cq_ins	(cq *,void *) noex ;
extern int cq_rem	(cq *,void *) noex ;
extern int cq_unlink	(cq *,void *) noex ;
extern int cq_count	(cq *) noex ;
extern int cq_curbegin	(cq *,cq_cur *) noex ;
extern int cq_curend	(cq *,cq_cur *) noex ;
extern int cq_curenum	(cq *,cq_cur *,void *) noex ;

EXTERNC_end


#endif /* CQ_INCLUDE */


