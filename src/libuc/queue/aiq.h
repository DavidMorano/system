/* aiq HEADER */
/* charset=ISO8859-1 */
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


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<pthread.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<q.h>			/* LIBUC */


#define	AIQ		struct aiq_head
#define	AIQ_ENT		struct aiq_entry
#define	AIQ_MAGIC	0x76925634


struct aiq_entry {
	ptrdiff_t	next ;
	ptrdiff_t	prev ;
} ; /* end struct (aiq_entry) */

struct aiq_head {
	q		*qp ;
	uint		magval ;
} ; /* end struct (aiq_head) */

typedef	AIQ_ENT		aiq_ent ;

#ifdef	__cplusplus
enum aiqmems {
	aiqmem_start,
	aiqmem_count,
	aiqmem_finish,
	aiqmem_overlast
} ; /* end enum (aiqmems) */
struct aiq ;
struct aiq_co {
        aiq		*op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (aiq *p,int m) noex {
            op = p ;
            w = m ;
        } ;
	int operator () (int = 0) noex ;
        operator int () noex {
	    return operator () (0) ;
	} ;
} ; /* end struct (aiq_co) */
struct aiq : aiq_head {
	aiq_co		start ;
	aiq_co		count ;
	aiq_co		finish ;
	constexpr aiq() noex {
	    start	(this,aiqmem_start) ;
	    count	(this,aiqmem_count) ;
	    finish	(this,aiqmem_finish) ;
	    magval = 0 ;
	} ; /* end ctor */
	aiq(const aiq &) = delete ;
	aiq &operator = (const aiq &) = delete ;
	int ins		(aiq_ent *) noex ;
	int inshead	(aiq_ent *) noex ;
	int rem		(aiq_ent **) noex ;
	int remtail	(aiq_ent **) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct aiq() {
	    if (magval) dtor() ;
	} ; /* end dtor (aiq) */
} ; /* end class (aiq) */
#else
typedef AIQ		aiq ;
#endif /* __cplusplus */

EXTERNC_begin

extern int aiq_start	(aiq *,int) noex ;
extern int aiq_finish	(aiq *) noex ;
extern int aiq_ins	(aiq *,aiq_ent *) noex ;
extern int aiq_inshead	(aiq *,aiq_ent *) noex ;
extern int aiq_rem	(aiq *,aiq_ent **) noex ;
extern int aiq_remtail	(aiq *,aiq_ent **) noex ;
extern int aiq_count	(aiq *) noex ;

EXTERNC_end


#endif /* AIQ_INCLUDE */


