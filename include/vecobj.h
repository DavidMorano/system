/* vecobj HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* vector-object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VECOBJ_INCLUDE
#define	VECOBJ_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<lookaside.h>


/* object defines */
#define	VECOBJ			struct vecobj_head
#define	VECOBJ_FL		struct vecobj_flags
#define	VECOBJ_CUR		struct vecobj_cursor
#define	VECOBJ_DEFENTS		10

/**** options
reuse		= reuse empty slots
compact		= do not allow for holes
swap		= use swapping for empty slot management
stationary	= entries do not move
conserve	= conserve space where possible
sorted		= maintain a sorted list
ordered		= maintain an ordered list
****/

enum vecobjos {
    vecobjo_reuse,
    vecobjo_compact,
    vecobjo_swap,
    vecobjo_stationary,
    vecobjo_conserve,
    vecobjo_sorted,
    vecobjo_ordered,
    vecobjo_overlast
} ;

#ifdef	__cplusplus	/* C++ only! */

struct vecobjms {
    static int	reuse ;
    static int	compact ;
    static int	swap ;
    static int	stationary ;
    static int	conserve ;
    static int	sorted ;
    static int	ordered ;
} ;

#endif /* __cplusplus */

#define	VECOBJ_ODEFAULT		0
#define	VECOBJ_OREUSE		(1 << vecobjo_reuse)
#define	VECOBJ_OCOMPACT		(1 << vecobjo_compact)
#define	VECOBJ_OSWAP		(1 << vecobjo_swap)
#define	VECOBJ_OSTATIONARY	(1 << vecobjo_stationary)
#define	VECOBJ_OCONSERVE	(1 << vecobjo_conserve)
#define	VECOBJ_OSORTED		(1 << vecobjo_sorted)
#define	VECOBJ_OORDERED		(1 << vecobjo_ordered)

struct vecobj_flags {
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

struct vecobj_head {
	void		**va ;
	lookaside	*lap ;
	VECOBJ_FL	fl ;
	int		c ;		/* count of items in list */
	int		i ;		/* highest index */
	int		n ;		/* extent of array */
	int		fi ;		/* free index */
	int		esz ;		/* object size */
} ;

struct vecobj_cursor {
	int		i, c ;
} ;

typedef VECOBJ_CUR	vecobj_cur ;

EXTERNC_begin

typedef int (*vecobj_vcf)(cvoid **,cvoid **) noex ;

EXTERNC_end

#ifdef	__cplusplus
enum vecobjmems {
	vecobjmem_count,
	vecobjmem_delall,
	vecobjmem_audit,
	vecobjmem_finish,
	vecobjmem_overlast
} ;
struct vecobj ;
struct vecobj_co {
	vecobj		*op = nullptr ;
	int		w = -1 ;
	void operator () (vecobj *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (vecobj_co) */
struct vecobj : vecobj_head {
	vecobj_co	count ;
	vecobj_co	delall ;
	vecobj_co	audit ;
	vecobj_co	finish ;
	vecobj() noex {
	    count	(this,vecobjmem_count) ;
	    delall	(this,vecobjmem_delall) ;
	    audit	(this,vecobjmem_audit) ;
	    finish	(this,vecobjmem_finish) ;
	    va = nullptr ;
	} ;
	vecobj(const vecobj &) = delete ;
	vecobj &operator = (const vecobj &) = delete ;
	int start(int,int = 0,int = 0) noex ;
	int add(cvoid *) noex ;
	int adduniq(cvoid *) noex ;
	int store(cvoid *,void **) noex ;
	int find(cvoid *) noex ;
	int get(int,void **) noex ;
	int getvec(void ***) noex ;
	int del(int = -1) noex ;
	int search(cvoid *,vecobj_vcf,void **) noex ;
	int sort(vecobj_vcf) noex ;
	int curbegin(vecobj_cur *) noex ;
	int curfetch(cvoid *,vecobj_cur *,vecobj_vcf,void **) noex ;
	int curend(vecobj_cur *) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct vecobj() {
	    if (va) dtor() ;
	} ;
} ; /* end struct (vecobj) */
#else	/* __cplusplus */
typedef VECOBJ		vecobj ;
#endif /* __cplusplus */

EXTERNC_begin

typedef int (*vecobj_vcf)(cvoid **,cvoid **) noex ;

extern int vecobj_start(vecobj *,int,int,int) noex ;
extern int vecobj_add(vecobj *,cvoid *) noex ;
extern int vecobj_adduniq(vecobj *,cvoid *) noex ;
extern int vecobj_store(vecobj *,cvoid *,void **) noex ;
extern int vecobj_inorder(vecobj *,cvoid *,vecobj_vcf,int) noex ;
extern int vecobj_del(vecobj *,int) noex ;
extern int vecobj_delall(vecobj *) noex ;
extern int vecobj_count(vecobj *) noex ;
extern int vecobj_sort(vecobj *,vecobj_vcf) noex ;
extern int vecobj_setsorted(vecobj *) noex ;
extern int vecobj_find(vecobj *,cvoid *) noex ;
extern int vecobj_curbegin(vecobj *,vecobj_cur *) noex ;
extern int vecobj_curfetch(vecobj *,cvoid *,vecobj_cur *,
		vecobj_vcf,void **) noex ;
extern int vecobj_curend(vecobj *,vecobj_cur *) noex ;
extern int vecobj_search(vecobj *,cvoid *,vecobj_vcf,void **) noex ;
extern int vecobj_get(vecobj *,int,void **) noex ;
extern int vecobj_getvec(vecobj *,void ***) noex ;
extern int vecobj_audit(vecobj *) noex ;
extern int vecobj_finish(vecobj *) noex ;
extern int vecobj_addnew(vecobj *,void **) noex ;

EXTERNC_end

#ifdef	__cplusplus

extern const vecobjms	vecobjm ;

#endif /* __cplusplus */


#endif /* VECOBJ_INCLUDE */


