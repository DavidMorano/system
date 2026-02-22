/* veclong HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* vector long-integer operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	VECLONG_INCLUDE
#define	VECLONG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


/* object defines */
#define	VECLONG_MAGIC		0x73625199
#define	VECLONG_DEFENTS		2
#define	VECLONG			struct veclong_head
#define	VECLONG_FL		struct veclong_flags
#define	VECLONG_CUR		struct veclong_cursor
#define	VECLONG_TYPE		long
#define	VECLONG_MIN		LONG_MIN
#define	VECLONG_MAX		LONG_MAX

/**** options
reuse		= reuse empty slots
compact		= do not allow for holes
swap		= use swapping for empty slot management
stationary	= entries do not move
conserve	= conserve space where possible
sorted		= maintain a sorted list
ordered		= maintain an ordered list
****/

enum veclongos {
    veclongo_reuse,
    veclongo_compact,
    veclongo_swap,
    veclongo_stationary,
    veclongo_conserve,
    veclongo_sorted,
    veclongo_ordered,
    veclongo_overlast
} ; /* end enum (options) */

#ifdef	__cplusplus	/* C++ only! */

struct veclongms {
    constexpr static int	reuse		= (1 << veclongo_reuse) ;
    constexpr static int	compact		= (1 << veclongo_compact) ;
    constexpr static int	swap		= (1 << veclongo_swap) ;
    constexpr static int	stationary	= (1 << veclongo_stationary) ;
    constexpr static int	conserve	= (1 << veclongo_conserve) ;
    constexpr static int	sorted		= (1 << veclongo_sorted) ;
    constexpr static int	ordered		= (1 << veclongo_ordered) ;
} ; /* end struct (veclongms) */

#endif /* __cplusplus */

#define	VECLONG_ODEFAULT	0
#define	VECLONG_OREUSE		(1 << veclongo_reuse)
#define	VECLONG_OCOMPACT	(1 << veclongo_compact)
#define	VECLONG_OSWAP		(1 << veclongo_swap)
#define	VECLONG_OSTATIONARY	(1 << veclongo_stationary)
#define	VECLONG_OCONSERVE	(1 << veclongo_conserve)
#define	VECLONG_OSORTED		(1 << veclongo_sorted)
#define	VECLONG_OORDERED	(1 << veclongo_ordered)

struct veclong_cursor {
	int		i ;
} ; /* end struct (veclong_cursor) */

struct veclong_flags {
	uint		issorted:1 ;
	uint		oreuse:1 ;
	uint		onoholes:1 ;
	uint		oswap:1 ;
	uint		ostationary:1 ;
	uint		ocompact:1 ;
	uint		osorted:1 ;
	uint		oordered:1 ;
	uint		oconserve:1 ;
} ; /* end struct (veclong_flags) */

struct veclong_head {
	VECLONG_TYPE	*va ;
	VECLONG_FL	fl ;
	uint		magic ;
	int		c ;		/* count of items in list */
	int		i ;		/* highest index */
	int		n ;		/* extent of array */
	int		fi ;		/* free index */
} ; /* end struct (veclong_head) */

typedef VECLONG_FL	veclong_fl ;
typedef VECLONG_CUR	veclong_cur ;

#ifdef	__cplusplus
enum veclongmems {
	veclongmem_count,
	veclongmem_extent,
	veclongmem_delall,
	veclongmem_sort,
	veclongmem_setsorted,
	veclongmem_resize,
	veclongmem_audit,
	veclongmem_finish,
	veclongmem_overlast
} ;
struct veclong_iter {
	VECLONG_TYPE	*va = nullptr ;
	int		i = -1 ;
	int		ii = -1 ;
	veclong_iter() = default ;
	veclong_iter(VECLONG_TYPE *ov,int oi,int oii) noex : va(ov), i(oi) {
	    ii = oii ;
	} ;
	veclong_iter(const veclong_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	} ;
	veclong_iter &operator = (const veclong_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	    return *this ;
	} ;
	bool operator != (const veclong_iter &) noex ;
	bool operator == (const veclong_iter &) noex ;
	VECLONG_TYPE *operator * () noex {
	    VECLONG_TYPE	*rp = nullptr ;
	    if (i < ii) rp = (va + i) ;
	    return rp ;
	} ;
	veclong_iter operator + (int) const noex ;
	veclong_iter operator += (int) noex ;
	veclong_iter operator ++ () noex ; /* pre */
	veclong_iter operator ++ (int) noex ; /* post */
	void increment(int = 1) noex ;
} ; /* end struct veclong_iter) */
struct veclong ;
struct veclong_st {
	veclong		*op = nullptr ;
	int		w = -1 ;
	void operator () (veclong *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = 0,int = 0) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (veclong_st) */
struct veclong_co {
	veclong		*op = nullptr ;
	int		w = -1 ;
	void operator () (veclong *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = -1) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (veclong_co) */
struct veclong : veclong_head {
    	veclong_st	start ;
	veclong_co	count ;
	veclong_co	extent ;
	veclong_co	delall ;
	veclong_co	sort ;
	veclong_co	setsorted ;
	veclong_co	resize ;
	veclong_co	audit ;
	veclong_co	finish ;
	veclong() noex {
	    start	(this,0) ;
	    extent	(this,veclongmem_extent) ;
	    count	(this,veclongmem_count) ;
	    delall	(this,veclongmem_delall) ;
	    sort	(this,veclongmem_sort) ;
	    setsorted	(this,veclongmem_setsorted) ;
	    resize	(this,veclongmem_resize) ;
	    audit	(this,veclongmem_audit) ;
	    finish	(this,veclongmem_finish) ;
	    magic = 0 ;
	} ; /* end ctor */
	veclong(const veclong &) = delete ;
	veclong &operator = (const veclong &) = delete ;
	int add		(VECLONG_TYPE) noex ;
	int addlist	(const VECLONG_TYPE *,int) noex ;
	int adduniq	(VECLONG_TYPE) noex ;
	int insert	(int,VECLONG_TYPE) noex ;
	int assign	(int,VECLONG_TYPE) noex ;
	int del		(int) noex ;
	int find	(VECLONG_TYPE) noex ;
	int match	(VECLONG_TYPE) noex ;
	int getval	(int,VECLONG_TYPE *) noex ;
	int getvec	(VECLONG_TYPE **) noex ;
	int mkvec	(VECLONG_TYPE *) noex ;
	int curbegin	(veclong_cur *) noex ;
	int curend	(veclong_cur *) noex ;
	int curenum	(veclong_cur *,VECLONG_TYPE *) noex ;
	veclong_iter begin() noex {
	    veclong_iter		it(va,0,i) ;
	    return it ;
	} ;
	veclong_iter end() noex {
	    veclong_iter		it(va,i,i) ;
	    return it ;
	} ;
	void dtor() noex ;
	destruct veclong() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (veclong) */
#else	/* __cplusplus */
typedef VECLONG		veclong ;
#endif /* __cplusplus */

EXTERNC_begin

extern int veclong_start	(veclong *,int,int) noex ;
extern int veclong_finish	(veclong *) noex ;
extern int veclong_add		(veclong *,VECLONG_TYPE) noex ;
extern int veclong_addlist	(veclong *,const VECLONG_TYPE *,int) noex ;
extern int veclong_adduniq	(veclong *,VECLONG_TYPE) noex ;
extern int veclong_insert	(veclong *,int,VECLONG_TYPE) noex ;
extern int veclong_assign	(veclong *,int,VECLONG_TYPE) noex ;
extern int veclong_resize	(veclong *,int) noex ;
extern int veclong_del		(veclong *,int) noex ;
extern int veclong_count	(veclong *) noex ;
extern int veclong_sort		(veclong *) noex ;
extern int veclong_setsorted	(veclong *) noex ;
extern int veclong_find		(veclong *,VECLONG_TYPE) noex ;
extern int veclong_match	(veclong *,VECLONG_TYPE) noex ;
extern int veclong_search	(veclong *,VECLONG_TYPE) noex ;
extern int veclong_getval	(veclong *,int,VECLONG_TYPE *) noex ;
extern int veclong_getvec	(veclong *,VECLONG_TYPE **) noex ;
extern int veclong_mkvec	(veclong *,VECLONG_TYPE *) noex ;
extern int veclong_curbegin	(veclong *,veclong_cur *) noex ;
extern int veclong_curenum	(veclong *,veclong_cur *,VECLONG_TYPE *) noex ;
extern int veclong_curend	(veclong *,veclong_cur *) noex ;
extern int veclong_audit	(veclong *) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern const veclongms	veclongm ;
#endif /* __cplusplus */


#endif /* VECLONG_INCLUDE */


