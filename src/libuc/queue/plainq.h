/* plainq HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* plain self-relative double-linked queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PLAINQ_INCLUDE
#define	PLAINQ_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	PLAINQ		struct plainq_head
#define	PLAINQ_CUR	struct plainq_cursor
#define	PLAINQ_ENT	struct plainq_entry
#define	PLAINQ_MAGIC	0x76925634


struct plainq_cursor {
	PLAINQ_ENT	*entp ;
} ; /* end struct (plainq_cursor) */

struct plainq_entry {
	ptrdiff_t	next ;
	ptrdiff_t	prev ;
} ; /* end struct (plainq_entry) */

struct plainq_head {
	ptrdiff_t	head ;
	ptrdiff_t	tail ;
	uint		magval ;
	int		cnt ;
} ; /* end struct (plainq_head) */

typedef PLAINQ_CUR	plainq_cur ;
typedef PLAINQ_ENT	plainq_ent ;

#ifdef	__cplusplus
enum plainqmems {
	plainqmem_start,
	plainqmem_count,
	plainqmem_audit,
	plainqmem_finish,
	plainqmem_overlast
} ; /* end enum (plainqmems) */
struct plainq ;
struct plainq_co {
        plainq		*op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (plainq *p,int m) noex {
            op = p ;
            w = m ;
        } ;
        operator int () noex ;
	int operator () () noex {
	    return operator int () ;
	} ;
} ; /* end struct (plainq_co) */
struct plainq : plainq_head {
	plainq_co	start ;
	plainq_co	count ;
	plainq_co	audit ;
	plainq_co	finish ;
	constexpr plainq() noex {
	    start	(this,plainqmem_start) ;
	    count	(this,plainqmem_count) ;
	    audit	(this,plainqmem_audit) ;
	    finish	(this,plainqmem_finish) ;
	    magval = 0 ;
	} ; /* end ctor */
	plainq(const plainq &) = delete ;
	plainq &operator = (const plainq &) = delete ;
	int ins		(plainq_ent *) noex ;
	int insgroup	(plainq_ent *,int,int) noex ;
	int inshead	(plainq_ent *) noex ;
	int gethead	(plainq_ent **) noex ;
	int gettail	(plainq_ent **) noex ;
	int rem		(plainq_ent **) noex ;
	int remtail	(plainq_ent **) noex ;
	int unlink	(plainq_ent *) noex ;
	int curbegin	(plainq_cur *) noex ;
	int curend	(plainq_cur *) noex ;
	int curenum	(plainq_cur *,plainq_ent **) noex ;
	void	dtor() noex ;
	operator int () noex ;
	destruct plainq() {
	    if (magval) dtor() ;
	} ; /* end dtor (plainq) */
} ; /* end class (plainq) */
#else
typedef PLAINQ		plainq ;
#endif /* __cplusplus */

EXTERNC_begin

extern int plainq_start		(plainq *) noex ;
extern int plainq_finish	(plainq *) noex ;
extern int plainq_ins		(plainq *,plainq_ent *) noex ;
extern int plainq_insgroup	(plainq *,plainq_ent *,int,int) noex ;
extern int plainq_inshead	(plainq *,plainq_ent *) noex ;
extern int plainq_gethead	(plainq *,plainq_ent **) noex ;
extern int plainq_gettail	(plainq *,plainq_ent **) noex ;
extern int plainq_rem		(plainq *,plainq_ent **) noex ;
extern int plainq_remtail	(plainq *,plainq_ent **) noex ;
extern int plainq_unlink	(plainq *,plainq_ent *) noex ;
extern int plainq_count		(plainq *) noex ;
extern int plainq_audit		(plainq *) noex ;

EXTERNC_end


#endif /* PLAINQ_INCLUDE */


