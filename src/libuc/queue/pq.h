/* pq HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* regular (no-frills) pointer queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PQ_INCLUDE
#define	PQ_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	PQ		struct pq_head
#define	PQ_ENT		struct pq_entry
#define	PQ_CUR		struct pq_cursor


struct pq_cursor {
	PQ_ENT		*entp ;
} ; /* end struct (pq_cursor) */

struct pq_entry {
	PQ_ENT		*next ;
	PQ_ENT		*prev ;
} ; /* end struct (pq_entry) */

struct pq_head {
	PQ_ENT		*head ;
	PQ_ENT		*tail ;
	int		cnt ;
} ; /* end struct (pq_head) */

typedef PQ_ENT		pq_ent ;
typedef PQ_CUR		pq_cur ;

#ifdef	__cplusplus
enum pqmems {
	pqmem_start,
	pqmem_count,
	pqmem_audit,
	pqmem_finish,
	pqmem_overlast
} ; /* end enum (pqmems) */
struct pq ;
struct pq_co {
        pq		*op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (pq *p,int m) noex {
            op = p ;
            w = m ;
        } ;
        operator int () noex ;
	int operator () () noex {
	    return operator int () ;
	} ;
} ; /* end struct (pq_co) */
struct pq : pq_head {
	pq_co		start ;
	pq_co		count ;
	pq_co		audit ;
	pq_co		finish ;
	constexpr pq() noex {
	    start	(this,pqmem_start) ;
	    count	(this,pqmem_count) ;
	    audit	(this,pqmem_audit) ;
	    finish	(this,pqmem_finish) ;
	    head = nullptr ;
	} ; /* end ctor */
	pq(const pq &) = delete ;
	pq &operator = (const pq &) = delete ;
	int ins		(pq_ent *) noex ;
	int insgroup	(pq_ent *,int,int) noex ;
	int gethead	(pq_ent **) noex ;
	int gettail	(pq_ent **) noex ;
	int rem		(pq_ent **) noex ;
	int remtail	(pq_ent **) noex ;
	int unlink	(pq_ent *) noex ;
	int curbegin	(pq_cur *) noex ;
	int curend	(pq_cur *) noex ;
	int curenum	(pq_cur *,pq_ent **) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct pq() {
	    if (head) dtor() ;
	} ; /* end dtor (pq) */
} ; /* end class (pq) */
#else
typedef PQ		pq ;
#endif /* __cplusplus */

EXTERNC_begin

extern int pq_start	(pq *) noex ;
extern int pq_ins	(pq *,pq_ent *) noex ;
extern int pq_insgroup	(pq *,pq_ent *,int,int) noex ;
extern int pq_gethead	(pq *,pq_ent **) noex ;
extern int pq_gettail	(pq *,pq_ent **) noex ;
extern int pq_rem	(pq *,pq_ent **) noex ;
extern int pq_remtail	(pq *,pq_ent **) noex ;
extern int pq_unlink	(pq *,pq_ent *) noex ;
extern int pq_curbegin	(pq *,pq_cur *) noex ;
extern int pq_curend	(pq *,pq_cur *) noex ;
extern int pq_curenum	(pq *,pq_cur *,pq_ent **) noex ;
extern int pq_count	(pq *) noex ;
extern int pq_audit	(pq *) noex ;
extern int pq_finish	(pq *) noex ;

EXTERNC_end


#endif /* PQ_INCLUDE */


