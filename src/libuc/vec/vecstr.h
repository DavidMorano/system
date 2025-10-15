/* vecstr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* vector-string container (Vector-String) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-13, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	vecstr

	Description:
	This object implements a vector of c-strings.  This are
	C-styled strings (of course, and as it should be).

*******************************************************************************/

#ifndef	VECSTR_INCLUDE
#define	VECSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


/* object defines */
#define	VECSTR			struct vecstr_head
#define	VECSTR_FL		struct vecstr_flags
#define	VECSTR_DEFENTS		5

/**** options
reuse		= reuse empty slots
compact		= do not allow for holes
swap		= use swapping for empty slot management
stationary	= entries do not move
conserve	= conserve space where possible
sorted		= maintain a sorted list
ordered		= maintain an ordered list
****/

enum vecstros {
    vecstro_reuse,
    vecstro_compact,
    vecstro_swap,
    vecstro_stationary,
    vecstro_conserve,
    vecstro_sorted,
    vecstro_ordered,
    vecstro_overlast
} ;

#ifdef	__cplusplus	/* C++ only! */

struct vecstrms {
    static int	reuse ;
    static int	compact ;
    static int	swap ;
    static int	stationary ;
    static int	conserve ;
    static int	sorted ;
    static int	ordered ;
} ;

#endif /* __cplusplus */

#define	VECSTR_ODEFAULT		0
#define	VECSTR_OREUSE		(1 << vecstro_reuse)
#define	VECSTR_OCOMPACT		(1 << vecstro_compact)
#define	VECSTR_OSWAP		(1 << vecstro_swap)
#define	VECSTR_OSTATIONARY	(1 << vecstro_stationary)
#define	VECSTR_OCONSERVE	(1 << vecstro_conserve)
#define	VECSTR_OSORTED		(1 << vecstro_sorted)
#define	VECSTR_OORDERED		(1 << vecstro_ordered)

struct vecstr_flags {
	uint		issorted:1 ;
	uint		oreuse:1 ;
	uint		onoholes:1 ;
	uint		oswap:1 ;
	uint		ostationary:1 ;
	uint		ocompact:1 ;
	uint		osorted:1 ;
	uint		oordered:1 ;
	uint		oconserve:1 ;
	uint		stsize:1 ;		/* not option (just flag) */
} ; /* end struct (vecstr_flags) */

struct vecstr_head {
	cchar		**va ;
	VECSTR_FL	fl ;
	int		c ;		/* count of items in list */
	int		i ;		/* overlast index */
	int		n ;		/* current extent of array */
	int		fi ;		/* free index */
	int		stsize ;	/* total string-table length */
} ; /* end struct (vecstr_head) */

EXTERNC_begin

typedef int (*vecstr_vcmp)(cchar **,cchar **) noex ;
typedef int (*vecstr_f)(cchar **,cchar **) noex ;

EXTERNC_end

#ifdef	__cplusplus
enum vecstrmems {
    	vecstrmem_addcspath,
	vecstrmem_count,
	vecstrmem_delall,
	vecstrmem_strsize,
	vecstrmem_recsize,
	vecstrmem_cksize,
	vecstrmem_audit,
	vecstrmem_finish,
	vecstrmem_overlast
} ;
struct vecstr_iter {
	cchar		**va = nullptr ;
	int		i = -1 ;
	int		ii = -1 ;
	vecstr_iter() = default ;
	vecstr_iter(cchar **ov,int oi,int oii) noex : va(ov), i(oi) {
	    ii = oii ;
	} ;
	vecstr_iter(const vecstr_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	} ;
	vecstr_iter &operator = (const vecstr_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	    return *this ;
	} ;
	bool operator != (const vecstr_iter &) noex ;
	bool operator == (const vecstr_iter &) noex ;
	cchar *operator * () noex {
	    cchar	*rp = nullptr ;
	    if (i < ii) rp = va[i] ;
	    return rp ;
	} ;
	vecstr_iter operator + (int) const noex ;
	vecstr_iter &operator += (int) noex ;
	vecstr_iter operator ++ () noex ; /* pre */
	vecstr_iter operator ++ (int) noex ; /* post */
    private:
	void increment(int = 1) noex ;
} ; /* end struct vecstr_iter) */
struct vecstr ;
struct vecstr_st {
	vecstr		*op = nullptr ;
	void operator () (vecstr *p) noex {
	    op = p ;
	} ;
	int operator () (int = 0,int = 0) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (vecstr_st) */
struct vecstr_so {
	vecstr		*op = nullptr ;
	void init(vecstr *p) noex {
	    op = p ;
	} ;
	int operator () (vecstr_f = nullptr) noex ;
	operator int () noex {
	    return operator () (nullptr) ;
	} ;
} ; /* end struct (vecstr_so) */
struct vecstr_co {
	vecstr		*op = nullptr ;
	int		w = -1 ;
	void operator () (vecstr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (vecstr_co) */
struct vecstr : vecstr_head {
	vecstr_st	start ;
	vecstr_so	sort ;
	vecstr_co	addcspath ;
	vecstr_co	count ;
	vecstr_co	delall ;
	vecstr_co	strsize ;
	vecstr_co	recsize ;
	vecstr_co	cksize ;
	vecstr_co	audit ;
	vecstr_co	finish ;
	vecstr() noex {
	    start(this) ;
	    addcspath(this,vecstrmem_addcspath) ;
	    count(this,vecstrmem_count) ;
	    delall(this,vecstrmem_delall) ;
	    strsize(this,vecstrmem_strsize) ;
	    recsize(this,vecstrmem_recsize) ;
	    cksize(this,vecstrmem_cksize) ;
	    audit(this,vecstrmem_audit) ;
	    finish(this,vecstrmem_finish) ;
	    sort.init(this) ;
	    va = nullptr ;
	} ; /* end ctor */
	vecstr(const vecstr &) = delete ;
	vecstr &operator = (const vecstr &) = delete ;
	int add(cchar *,int = -1) noex ;
	int adduniq(cchar *,int = -1) noex ;
	int addsyms(cchar *,mainv) noex ;
	int addpath(cchar *,int = -1) noex ;
	int insert(int,cchar *,int = -1) noex ;
	int store(cchar *,int,cchar **) noex ;
	int already(cchar *,int) noex ;
	int get(int,cchar **) noex ;
	int getlast(cchar **) noex ;
	int getvec(mainv *) noex ;
	int envadd(cchar *,cchar *,int = -1) noex ;
	int envset(cchar *,cchar *,int = -1) noex ;
	int envfile(cchar *) noex ;
	int find(cchar *) noex ;
	int findn(cchar *,int = -1) noex ;
	int search(cchar *,vecstr_f,cchar ** = nullptr) noex ;
	int finder(cchar *,vecstr_f,cchar ** = nullptr) noex ;
	int del(int = -1) noex ;
	operator int () noex ;
	vecstr_iter begin() noex {
	    vecstr_iter		it(va,0,i) ;
	    return it ;
	} ;
	vecstr_iter end() noex {
	    vecstr_iter		it(va,i,i) ;
	    return it ;
	} ;
	void dtor() noex ;
	destruct vecstr() {
	    if (va) dtor() ;
	} ;
} ; /* end struct (vecstr) */
#else	/* __cplusplus */
typedef VECSTR		vecstr ;
#endif /* __cplusplus */

typedef	VECSTR_FL	vecstr_fl ;

EXTERNC_begin

extern int vecstr_start		(vecstr *,int,int) noex ;
extern int vecstr_add		(vecstr *,cchar *,int) noex ;
extern int vecstr_adduniq	(vecstr *,cchar *,int) noex ;
extern int vecstr_addkeyval	(vecstr *,cchar *,int,cchar *,int) noex ;
extern int vecstr_insert	(vecstr *,int,cchar *,int) noex ;
extern int vecstr_store		(vecstr *,cchar *,int,cchar **) noex ;
extern int vecstr_already	(vecstr *,cchar *,int) noex ;
extern int vecstr_get		(vecstr *,int,cchar **) noex ;
extern int vecstr_getlast	(vecstr *,cchar **) noex ;
extern int vecstr_del		(vecstr *,int) noex ;
extern int vecstr_delall	(vecstr *) noex ;
extern int vecstr_count		(vecstr *) noex ;
extern int vecstr_sort		(vecstr *,vecstr_vcmp) noex ;
extern int vecstr_search	(vecstr *,cchar *,vecstr_vcmp,cchar **) noex ;
extern int vecstr_searchl  (vecstr *,cchar *,int,vecstr_vcmp,cchar **) noex ;
extern int vecstr_find		(vecstr *,cchar *) noex ;
extern int vecstr_findn		(vecstr *,cchar *,int) noex ;
extern int vecstr_finder	(vecstr *,cchar *,vecstr_vcmp,cchar **) noex ;
extern int vecstr_findaddr	(vecstr *,cchar *) noex ;
extern int vecstr_getvec	(vecstr *,mainv *) noex ;
extern int vecstr_strsize	(vecstr *) noex ;
extern int vecstr_strmk		(vecstr *,char *,int) noex ;
extern int vecstr_recsize	(vecstr *) noex ;
extern int vecstr_cksize	(vecstr *) noex ;
extern int vecstr_recmk		(vecstr *,int *,int) noex ;
extern int vecstr_recmkstr	(vecstr *,int *,int,char *,int) noex ;
extern int vecstr_avmkstr	(vecstr *,cchar **,int,char *,int) noex ;
extern int vecstr_audit		(vecstr *) noex ;
extern int vecstr_finish	(vecstr *) noex ;
/* extras (so-called) */
extern int vecstr_addpathclean	(vecstr *,cchar *,int) noex ;
extern int vecstr_addpath	(vecstr *,cchar *,int) noex ;
extern int vecstr_addcspath	(vecstr *) noex ;
extern int vecstr_adds		(vecstr *,cchar *,int) noex ;
extern int vecstr_addsyms	(vecstr *,cchar *,mainv) noex ;
extern int vecstr_addsubdirs	(vecstr *,cchar *) noex ;
extern int vecstr_adduniqs	(vecstr *,cchar *,int) noex ;
extern int vecstr_envadd	(vecstr *,cchar *,cchar *,int) noex ;
extern int vecstr_envadds	(vecstr *,cchar *,int) noex ;
extern int vecstr_envset	(vecstr *,cchar *,cchar *,int) noex ;
extern int vecstr_envget	(vecstr *,cchar *,cchar **) noex ;
extern int vecstr_envfile	(vecstr *,cchar *) noex ;
extern int vecstr_foilcmp	(vecstr *,vecstr *) noex ;
extern int vecstr_loadfile	(vecstr *,int,cchar *) noex ;
extern int vecstr_loadgrusers	(vecstr *,gid_t) noex ;
extern int vecstr_srvargs	(vecstr *,cchar *) noex ;
extern int vecstr_svcargs	(vecstr *,int *,cchar *) noex ;

static inline int vecstr_loaddirs(vecstr *op,cchar *newsdname) noex {
	return vecstr_addsubdirs(op,newsdname) ;
}

EXTERNC_end

#ifdef	__cplusplus

extern const vecstrms	vecstrm ;

#endif /* __cplusplus */


#endif /* VECSTR_INCLUDE */


