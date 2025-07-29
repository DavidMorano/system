/* vecint HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* vector integer operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	VECINT_INCLUDE
#define	VECINT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


/* object defines */
#define	VECINT_MAGIC		0x73625198
#define	VECINT			struct vecint_head
#define	VECINT_FL		struct vecint_flags
#define	VECINT_CUR		struct vecint_cursor
#define	VECINT_DEFENTS		2
#define	VECINT_TYPE		int

/* options */
#define	VECINT_ODEFAULT		0x0000
#define	VECINT_OREUSE		0x0001		/* reuse empty slots */
#define	VECINT_OCOMPACT		0x0002		/* means NOHOLES */
#define	VECINT_OSWAP		0x0004		/* use swapping */
#define	VECINT_OSTATIONARY	0x0008		/* entries should not move */
#define	VECINT_OCONSERVE	0x0010		/* conserve space */
#define	VECINT_OSORTED		0x0020		/* keep sorted */
#define	VECINT_OORDERED		0x0040		/* keep ordered */


struct vecint_cursor {
	int		i ;
} ;

struct vecint_flags {
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

struct vecint_head {
	VECINT_TYPE	*va ;
	VECINT_FL	fl ;
	uint		magic ;
	int		c ;		/* count of items in list */
	int		i ;		/* highest index */
	int		n ;		/* extent of array */
	int		fi ;		/* free index */
} ;

typedef VECINT_FL	vecint_fl ;
typedef VECINT_CUR	vecint_cur ;

#ifdef	__cplusplus
enum vecintmems {
	vecintmem_count,
	vecintmem_extent,
	vecintmem_delall,
	vecintmem_sort,
	vecintmem_setsorted,
	vecintmem_resize,
	vecintmem_audit,
	vecintmem_finish,
	vecintmem_overlast
} ;
struct vecint_iter {
	VECINT_TYPE	*va = nullptr ;
	int		i = -1 ;
	int		ii = -1 ;
	vecint_iter() = default ;
	vecint_iter(VECINT_TYPE *ov,int oi,int oii) noex : va(ov), i(oi) {
	    ii = oii ;
	} ;
	vecint_iter(const vecint_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	} ;
	vecint_iter &operator = (const vecint_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	    return *this ;
	} ;
	bool operator != (const vecint_iter &) noex ;
	bool operator == (const vecint_iter &) noex ;
	VECINT_TYPE *operator * () noex {
	    VECINT_TYPE	*rp = nullptr ;
	    if (i < ii) rp = (va + i) ;
	    return rp ;
	} ;
	vecint_iter operator + (int) const noex ;
	vecint_iter operator += (int) noex ;
	vecint_iter operator ++ () noex ; /* pre */
	vecint_iter operator ++ (int) noex ; /* post */
	void increment(int = 1) noex ;
} ; /* end struct vecint_iter) */
struct vecint ;
struct vecint_st {
	vecint		*op = nullptr ;
	int		w = -1 ;
	void operator () (vecint *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = 0,int = 0) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (vecint_st) */
struct vecint_co {
	vecint		*op = nullptr ;
	int		w = -1 ;
	void operator () (vecint *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = -1) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (vecint_co) */
struct vecint : vecint_head {
    	vecint_st	start ;
	vecint_co	count ;
	vecint_co	extent ;
	vecint_co	delall ;
	vecint_co	sort ;
	vecint_co	setsorted ;
	vecint_co	resize ;
	vecint_co	audit ;
	vecint_co	finish ;
	vecint() noex {
	    start	(this,0) ;
	    extent	(this,vecintmem_extent) ;
	    count	(this,vecintmem_count) ;
	    delall	(this,vecintmem_delall) ;
	    sort	(this,vecintmem_sort) ;
	    setsorted	(this,vecintmem_setsorted) ;
	    resize	(this,vecintmem_resize) ;
	    audit	(this,vecintmem_audit) ;
	    finish	(this,vecintmem_finish) ;
	    magic = 0 ;
	} ;
	vecint(const vecint &) = delete ;
	vecint &operator = (const vecint &) = delete ;
	int add(VECINT_TYPE) noex ;
	int adduniq(VECINT_TYPE) noex ;
	int insert(int,VECINT_TYPE) noex ;
	int assign(int,VECINT_TYPE) noex ;
	int del(int) noex ;
	int find(VECINT_TYPE) noex ;
	int match(VECINT_TYPE) noex ;
	int getval(int,VECINT_TYPE *) noex ;
	int getvec(VECINT_TYPE **) noex ;
	int mkvec(VECINT_TYPE *) noex ;
	int curbegin(vecint_cur *) noex ;
	int curend(vecint_cur *) noex ;
	int curenum(vecint_cur *,VECINT_TYPE *) noex ;
	vecint_iter begin() noex {
	    vecint_iter		it(va,0,i) ;
	    return it ;
	} ;
	vecint_iter end() noex {
	    vecint_iter		it(va,i,i) ;
	    return it ;
	} ;
	void dtor() noex ;
	destruct vecint() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (vecint) */
#else	/* __cplusplus */
typedef VECINT		vecint ;
#endif /* __cplusplus */

EXTERNC_begin

extern int vecint_start(vecint *,int,int) noex ;
extern int vecint_finish(vecint *) noex ;
extern int vecint_add(vecint *,VECINT_TYPE) noex ;
extern int vecint_addlist(vecint *,const VECINT_TYPE *,int) noex ;
extern int vecint_adduniq(vecint *,VECINT_TYPE) noex ;
extern int vecint_insert(vecint *,int,VECINT_TYPE) noex ;
extern int vecint_assign(vecint *,int,VECINT_TYPE) noex ;
extern int vecint_resize(vecint *,int) noex ;
extern int vecint_del(vecint *,int) noex ;
extern int vecint_count(vecint *) noex ;
extern int vecint_extent(vecint *) noex ;
extern int vecint_sort(vecint *) noex ;
extern int vecint_setsorted(vecint *) noex ;
extern int vecint_find(vecint *,VECINT_TYPE) noex ;
extern int vecint_match(vecint *,VECINT_TYPE) noex ;
extern int vecint_getval(vecint *,int,VECINT_TYPE *) noex ;
extern int vecint_getvec(vecint *,VECINT_TYPE **) noex ;
extern int vecint_mkvec(vecint *,VECINT_TYPE *) noex ;
extern int vecint_curbegin(vecint *,vecint_cur *) noex ;
extern int vecint_curenum(vecint *,vecint_cur *,VECINT_TYPE *) noex ;
extern int vecint_curend(vecint *,vecint_cur *) noex ;
extern int vecint_audit(vecint *) noex ;

EXTERNC_end


#endif /* VECINT_INCLUDE */


