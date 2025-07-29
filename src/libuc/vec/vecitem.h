/* vecitem HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vector¯item operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	VECITEM_INCLUDE
#define	VECITEM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


/* object defines */
#define	VECITEM			struct vecitem_head
#define	VECITEM_FL		struct vecitem_flags
#define	VECITEM_CUR		struct vecitem_cursor
#define	VECITEM_DEFENTS		10
/* options */
#define	VECITEM_ODEFAULT	0
#define	VECITEM_OREUSE		(1 << 0)
#define	VECITEM_OCOMPACT	(1 << 1)	/* means NOHOLES */
#define	VECITEM_OSWAP		(1 << 2)
#define	VECITEM_OSTATIONARY	(1 << 3)
#define	VECITEM_OCONSERVE	(1 << 4)
#define	VECITEM_OSORTED		(1 << 5)
#define	VECITEM_OORDERED	(1 << 6)


struct vecitem_flags {
	uint		issorted:1 ;
	uint		oreuse:1 ;
	uint		onoholes:1 ;
	uint		oswap:1 ;
	uint		ostationary:1 ;
	uint		ocompact:1 ;
	uint		osorted:1 ;
	uint		oordered:1 ;
	uint		oconserve:1 ;
} ;

struct vecitem_head {
	void		**va ;
	VECITEM_FL	fl ;
	int		c ;		/* count of items in list */
	int		i ;		/* highest index */
	int		n ;		/* extent of array */
	int		fi ;		/* free index */
} ;

struct vecitem_cursor  {
	int		i ;
	int		c ;
} ;

typedef VECITEM_FL	vecitem_fl ;
typedef VECITEM_CUR	vecitem_cur ;

EXTERNC_begin

typedef int (*vecitem_cmpf)(cvoid **,cvoid **) noex ;
typedef int (*vecitem_f)(cvoid **,cvoid **) noex ;

EXTERNC_end

#ifdef	__cplusplus
enum vecitemmems {
	vecitemmem_del,
	vecitemmem_count,
	vecitemmem_audit,
	vecitemmem_finish,
	vecitemmem_overlast
} ;
struct vecitem_iter {
	void		**va = nullptr ;
	int		i = -1 ;
	int		ii = -1 ;
	vecitem_iter() = default ;
	vecitem_iter(void **ov,int oi,int oii) noex : va(ov), i(oi) {
	    ii = oii ;
	} ;
	vecitem_iter(const vecitem_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	} ;
	vecitem_iter &operator = (const vecitem_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	    return *this ;
	} ;
	bool operator != (const vecitem_iter &) noex ;
	bool operator == (const vecitem_iter &) noex ;
	void *operator * () noex {
	    void	*rp = nullptr ;
	    if (i < ii) rp = va[i] ;
	    return rp ;
	} ;
	vecitem_iter operator + (int) const noex ;
	vecitem_iter operator += (int) noex ;
	vecitem_iter operator ++ () noex ; /* pre */
	vecitem_iter operator ++ (int) noex ; /* post */
	void increment(int = 1) noex ;
} ; /* end struct vecitem_iter) */
struct vecitem ;
struct vecitem_co {
	vecitem		*op = nullptr ;
	int		w = -1 ;
	void operator () (vecitem *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = -1) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (vecitem_co) */
struct vecitem : vecitem_head {
	vecitem_co	del ;
	vecitem_co	count ;
	vecitem_co	audit ;
	vecitem_co	finish ;
	vecitem() noex {
	    del		(this,vecitemmem_del) ;
	    count	(this,vecitemmem_count) ;
	    audit	(this,vecitemmem_audit) ;
	    finish	(this,vecitemmem_finish) ;
	    va = nullptr ;
	} ;
	vecitem(const vecitem &) = delete ;
	vecitem &operator = (const vecitem &) = delete ;
	int start(int = 0,int = 0) noex ;
	int add(cvoid *,int) noex ;
	int get(int,void *) noex ;
	int getvec(void **) noex ;
	int find(cvoid *,int) noex ;
	int fetch(cvoid *,vecitem_cur *,vecitem_cmpf,void *) noex ;
	int search(cvoid *,vecitem_cmpf,void *) noex ;
	int sort(vecitem_cmpf) noex ;
	operator int () noex ;
	vecitem_iter begin() noex {
	    vecitem_iter		it(va,0,i) ;
	    return it ;
	} ;
	vecitem_iter end() noex {
	    vecitem_iter		it(va,i,i) ;
	    return it ;
	} ;
	void dtor() noex ;
	destruct vecitem() {
	    if (va) dtor() ;
	} ;
} ; /* end struct (vecitem) */
#else	/* __cplusplus */
typedef VECITEM		vecitem ;
#endif /* __cplusplus */

EXTERNC_begin

typedef int (*vecitem_cmpf)(cvoid **,cvoid **) noex ;
typedef int (*vecitem_f)(cvoid **,cvoid **) noex ;

extern int vecitem_start(vecitem *,int,int) noex ;
extern int vecitem_finish(vecitem *) noex ;
extern int vecitem_add(vecitem *,cvoid *,int) noex ;
extern int vecitem_del(vecitem *,int) noex ;
extern int vecitem_count(vecitem *) noex ;
extern int vecitem_sort(vecitem *,vecitem_cmpf) noex ;
extern int vecitem_find(vecitem *,cvoid *,int) noex ;
extern int vecitem_fetch(vecitem *,cvoid *,vecitem_cur *,
		vecitem_cmpf,void *) noex ;
extern int vecitem_search(vecitem *,cvoid *,vecitem_cmpf,void *) noex ;
extern int vecitem_get(vecitem *,int,void *) noex ;
extern int vecitem_getvec(vecitem *,void **) noex ;
extern int vecitem_audit(vecitem *) noex ;
extern int vecitem_curbegin(vecitem *,vecitem_cur *) noex ;
extern int vecitem_curend(vecitem *,vecitem_cur *) noex ;

EXTERNC_end


#endif /* VECITEM_INCLUDE */


