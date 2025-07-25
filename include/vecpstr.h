/* vecpstr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* vector-packed-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	vecpstr

	Description:
	This object implements a vector of (packed) c-strings.  This
	are C-styled strings (of course, and as it should be).

*******************************************************************************/

#ifndef	VECPSTR_INCLUDE
#define	VECPSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vechand.h>


/* object defines */
#define	VECPSTR_MAGIC		0x88776215
#define	VECPSTR			struct vecpstr_head
#define	VECPSTR_FL		struct vecpstr_flags
#define	VECPSTR_CH		struct vecpstr_chunk
#define	VECPSTR_DEFENTS		10

/**** options
reuse		= reuse empty slots
compact		= do not allow for holes
swap		= use swapping for empty slot management
stationary	= entries do not move
conserve	= conserve space where possible
sorted		= maintain a sorted list
ordered		= maintain an ordered list
****/

enum vecpstros {
    vecpstro_reuse,
    vecpstro_compact,
    vecpstro_swap,
    vecpstro_stationary,
    vecpstro_conserve,
    vecpstro_sorted,
    vecpstro_ordered,
    vecpstro_overlast
} ; /* end enum (vecpstros) */

#ifdef	__cplusplus	/* C++ only! */

struct vecpstrms {
    static int	reuse ;
    static int	compact ;
    static int	swap ;
    static int	stationary ;
    static int	conserve ;
    static int	sorted ;
    static int	ordered ;
} ; /* end struct (vecpstrms) */

#endif /* __cplusplus */

#define	VECPSTR_ODEFAULT	0
#define	VECPSTR_OREUSE		(1 << vecpstro_reuse)
#define	VECPSTR_OCOMPACT	(1 << vecpstro_compact)
#define	VECPSTR_OSWAP		(1 << vecpstro_swap)
#define	VECPSTR_OSTATIONARY	(1 << vecpstro_stationary)
#define	VECPSTR_OCONSERVE	(1 << vecpstro_conserve)
#define	VECPSTR_OSORTED		(1 << vecpstro_sorted)
#define	VECPSTR_OORDERED	(1 << vecpstro_ordered)

struct vecpstr_flags {
	uint		issorted:1 ;
	uint		oreuse:1 ;
	uint		onoholes:1 ;
	uint		oswap:1 ;
	uint		ostationary:1 ;
	uint		ocompact:1 ;
	uint		osorted:1 ;
	uint		oordered:1 ;
	uint		oconserve:1 ;
	uint		stsize:1 ;
} ; /* end struct (vecpstr_flags) */

struct vecpstr_chunk {
	char		*tab ;		/* table pointer */
	int		tabsize ;	/* tab-bytes allocated extent */
	int		tablen ;	/* tab-bytes amount used */
	int		count ;		/* number of items */
} ; /* end struct (vecpstr_chunk) */

struct vecpstr_head {
	cchar		**va ;
	vechand		*clp ;		/* chunk-list-pointer */
	VECPSTR_CH	*chp ;		/* chunk (current) pointer */
	VECPSTR_FL	fl ;
	uint		magic ;
	int		chsize ;
	int		addnum ;	/* suggested add-number */
	int		cnt ;		/* total item count */
	int		idx ;		/* index */
	int		ext ;		/* extent */
	int		fidx ;		/* first-index */
	int		stsize ;	/* string table size */
} ; /* end struct (vecpstr_head) */

EXTERNC_begin

typedef int (*vecpstr_vcmp)(cchar **,cchar **) noex ;
typedef int (*vecpstr_f)(cchar **,cchar **) noex ;

EXTERNC_end

#ifdef	__cplusplus
enum vecpstrmems {
	vecpstrmem_addcspath,
	vecpstrmem_count,
	vecpstrmem_delall,
	vecpstrmem_strsize,
	vecpstrmem_recsize,
	vecpstrmem_cksize,
	vecpstrmem_audit,
	vecpstrmem_finish,
	vecpstrmem_overlast
} ; /* end enum (vecpstrmems) */
struct vecpstr_iter {
	cchar		**va = nullptr ;
	int		i = -1 ;
	int		ii = -1 ;
	vecpstr_iter() = default ;
	vecpstr_iter(cchar **ov,int oi,int oii) noex : va(ov), i(oi) {
	    ii = oii ;
	} ;
	vecpstr_iter(const vecpstr_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	} ;
	vecpstr_iter &operator = (const vecpstr_iter &oit) noex {
	    if (this != &oit) {
		va = oit.va ;
		i = oit.i ;
		ii = oit.ii ;
	    }
	    return *this ;
	} ;
	bool operator != (const vecpstr_iter &) noex ;
	bool operator == (const vecpstr_iter &) noex ;
	cchar *operator * () noex {
	    cchar	*rp = nullptr ;
	    if (i < ii) rp = va[i] ;
	    return rp ;
	} ;
	vecpstr_iter operator + (int) const noex ;
	vecpstr_iter &operator += (int) noex ;
	vecpstr_iter operator ++ () noex ; /* pre */
	vecpstr_iter operator ++ (int) noex ; /* post */
    private:
	void increment(int = 1) noex ;
} ; /* end struct vecpstr_iter) */
struct vecpstr ;
struct vecpstr_st {
	vecpstr		*op = nullptr ;
	void operator () (vecpstr *p) noex {
	    op = p ;
	} ;
	int operator () (int = 0,int = 0,int = 0) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (vecpstr_st) */
struct vecpstr_co {
	vecpstr		*op = nullptr ;
	int		w = -1 ;
	void operator () (vecpstr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (vecpstr_co) */
struct vecpstr : vecpstr_head {
	vecpstr_st	start ;
	vecpstr_co	addcspath ;
	vecpstr_co	count ;
	vecpstr_co	delall ;
	vecpstr_co	strsize ;
	vecpstr_co	recsize ;
	vecpstr_co	cksize ;
	vecpstr_co	audit ;
	vecpstr_co	finish ;
	vecpstr() noex {
	    start	(this) ;
	    addcspath	(this,vecpstrmem_addcspath) ;
	    count	(this,vecpstrmem_count) ;
	    delall	(this,vecpstrmem_delall) ;
	    strsize	(this,vecpstrmem_strsize) ;
	    recsize	(this,vecpstrmem_recsize) ;
	    recsize	(this,vecpstrmem_recsize) ;
	    audit	(this,vecpstrmem_audit) ;
	    finish	(this,vecpstrmem_finish) ;
	    magic = 0 ;
	} ; /* end ctor */
	vecpstr(const vecpstr &) = delete ;
	vecpstr &operator = (const vecpstr &) = delete ;
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
	int search(cchar *,vecpstr_f,cchar ** = nullptr) noex ;
	int finder(cchar *,vecpstr_f,cchar ** = nullptr) noex ;
	int del(int = -1) noex ;
	int sort(vecpstr_f = nullptr) noex ;
	operator int () noex ;
	vecpstr_iter begin() noex {
	    vecpstr_iter		it(va,0,idx) ;
	    return it ;
	} ;
	vecpstr_iter end() noex {
	    vecpstr_iter		it(va,idx,idx) ;
	    return it ;
	} ;
	void dtor() noex ;
	destruct vecpstr() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (vecpstr) */
#else	/* __cplusplus */
typedef VECPSTR		vecpstr ;
#endif /* __cplusplus */

typedef VECPSTR_FL	vecpstr_fl ;
typedef VECPSTR_CH	vecpstr_ch ;

EXTERNC_begin

extern int vecpstr_start(vecpstr *,int,int,int) noex ;
extern int vecpstr_finish(vecpstr *) noex ;
extern int vecpstr_add(vecpstr *,cchar *,int) noex ;
extern int vecpstr_adduniq(vecpstr *,cchar *,int) noex ;
extern int vecpstr_addkeyval(vecpstr *,cchar *,int,cchar *,int) noex ;
extern int vecpstr_insert(vecpstr *,int,cchar *,int) noex ;
extern int vecpstr_store(vecpstr *,cchar *,int,cchar **) noex ;
extern int vecpstr_already(vecpstr *,cchar *,int) noex ;
extern int vecpstr_get(vecpstr *,int,cchar **) noex ;
extern int vecpstr_getlast(vecpstr *,cchar **) noex ;
extern int vecpstr_del(vecpstr *,int) noex ;
extern int vecpstr_delall(vecpstr *) noex ;
extern int vecpstr_count(vecpstr *) noex ;
extern int vecpstr_sort(vecpstr *,vecpstr_vcmp) noex ;
extern int vecpstr_search(vecpstr *,cchar *,vecpstr_vcmp,cchar **) noex ;
extern int vecpstr_find(vecpstr *,cchar *) noex ;
extern int vecpstr_findn(vecpstr *,cchar *,int) noex ;
extern int vecpstr_finder(vecpstr *,cchar *,vecpstr_vcmp,cchar **) noex ;
extern int vecpstr_findaddr(vecpstr *,cchar *) noex ;
extern int vecpstr_getsize(vecpstr *) noex ;
extern int vecpstr_strsize(vecpstr *) noex ;
extern int vecpstr_strmk(vecpstr *,char *,int) noex ;
extern int vecpstr_recsize(vecpstr *) noex ;
extern int vecpstr_cksize(vecpstr *) noex ;
extern int vecpstr_recmk(vecpstr *,int *,int) noex ;
extern int vecpstr_recmkstr(vecpstr *,int *,int,char *,int) noex ;
extern int vecpstr_avmkstr(vecpstr *,cchar **,int,char *,int) noex ;
extern int vecpstr_indlen(vecpstr *) noex ;
extern int vecpstr_indsize(vecpstr *) noex ;
extern int vecpstr_indmk(vecpstr *,int (*)[3],int,int) noex ;
extern int vecpstr_audit(vecpstr *) noex ;
extern int vecpstr_getvec(vecpstr *,mainv *) noex ;
extern int vecpstr_finish(vecpstr *) noex ;
/* extras (so-called) */
extern int vecpstr_addpathclean(vecpstr *,cchar *,int) noex ;
extern int vecpstr_addpath(vecpstr *,cchar *,int) noex ;
extern int vecpstr_addcspath(vecpstr *) noex ;
extern int vecpstr_adds(vecpstr *,cchar *,int) noex ;
extern int vecpstr_addsyms(vecpstr *,cchar *,mainv) noex ;
extern int vecpstr_addsubdirs(vecpstr *,cchar *) noex ;
extern int vecpstr_adduniqs(vecpstr *,cchar *,int) noex ;
extern int vecpstr_envadd(vecpstr *,cchar *,cchar *,int) noex ;
extern int vecpstr_envadds(vecpstr *,cchar *,int) noex ;
extern int vecpstr_envset(vecpstr *,cchar *,cchar *,int) noex ;
extern int vecpstr_envget(vecpstr *,cchar *,cchar **) noex ;
extern int vecpstr_envfile(vecpstr *,cchar *) noex ;
extern int vecpstr_foilcmp(vecpstr *,vecpstr *) noex ;
extern int vecpstr_loadfile(vecpstr *,int,cchar *) noex ;
extern int vecpstr_loadgrusers(vecpstr *,gid_t) noex ;
extern int vecpstr_srvargs(vecpstr *,cchar *) noex ;
extern int vecpstr_svcargs(vecpstr *,int *,cchar *) noex ;

static inline int vecpstr_loaddirs(vecpstr *op,cchar *newsdname) noex {
	return vecpstr_addsubdirs(op,newsdname) ;
}

EXTERNC_end

#ifdef	__cplusplus

extern const vecpstrms	vecpstrm ;

#endif /* __cplusplus */


#endif /* VECPSTR_INCLUDE */


