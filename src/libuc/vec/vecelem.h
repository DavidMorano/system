/* vecelem HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* vector-element */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VECELEM_INCLUDE
#define	VECELEM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |caddr_t| */
#include	<clanguage.h>
#include	<usysbase.h>


/* object defines */
#define	VECELEM_MAGIC		0x73625198
#define	VECELEM			struct vecelem_head
#define	VECELEM_FL		struct vecelem_flags
#define	VECELEM_DEFENTS		2

/**** options
reuse		= reuse empty slots
compact		= do not allow for holes
swap		= use swapping for empty slot management
stationary	= entries do not move
conserve	= conserve space where possible
sorted		= maintain a sorted list
ordered		= maintain an ordered list
****/

enum vecelemos {
    vecelemo_reuse,
    vecelemo_compact,
    vecelemo_swap,
    vecelemo_stationary,
    vecelemo_conserve,
    vecelemo_sorted,
    vecelemo_ordered,
    vecelemo_overlast
} ; /* end enum (vecelemos) */

#ifdef	__cplusplus	/* C++ only! */

struct vecelemms {
    constexpr static int	reuse		= (1 << vecelemo_reuse) ;
    constexpr static int	compact		= (1 << vecelemo_compact) ;
    constexpr static int	swap		= (1 << vecelemo_swap) ;
    constexpr static int	stationary	= (1 << vecelemo_stationary) ;
    constexpr static int	conserve	= (1 << vecelemo_conserve) ;
    constexpr static int	sorted		= (1 << vecelemo_sorted) ;
    constexpr static int	ordered		= (1 << vecelemo_ordered) ;
} ; /* end struct (vecelemms) */

#endif /* __cplusplus */

/* options */
#define	VECELEM_ODEFAULT	0
#define	VECELEM_OREUSE		(1 << vecelemo_reuse)
#define	VECELEM_OCOMPACT	(1 << vecelemo_compact)
#define	VECELEM_OSWAP		(1 << vecelemo_swap)
#define	VECELEM_OSTATIONARY	(1 << vecelemo_stationary)
#define	VECELEM_OCONSERVE	(1 << vecelemo_conserve)
#define	VECELEM_OSORTED		(1 << vecelemo_sorted)
#define	VECELEM_OORDERED	(1 << vecelemo_ordered)

struct vecelem_flags {
	uint		issorted:1 ;
	uint		oreuse:1 ;
	uint		onoholes:1 ;
	uint		oswap:1 ;
	uint		ostationary:1 ;
	uint		ocompact:1 ;
	uint		osorted:1 ;
	uint		oordered:1 ;
	uint		oconserve:1 ;
} ; /* end struct (vecelem_flags) */

struct vecelem_head {
	voidp		va ;		/* value-aray */
	VECELEM_FL	fl ;
	uint		magic ;
	int		c ;		/* count of items in list */
	int		i ;		/* highest index */
	int		n ;		/* extent of array */
	int		fi ;		/* free index */
	int		esz ;		/* element size */
} ; /* end struct (vecelem_head) */

EXTERNC_begin
typedef int (*vecelem_vcmp)(cvoid **,cvoid **) noex ;
EXTERNC_end

#ifdef	__cplusplus
enum vecelemmems {
	vecelemmem_count,
	vecelemmem_delall,
	vecelemmem_extent,
	vecelemmem_audit,
	vecelemmem_finish,
	vecelemmem_overlast
} ;
struct vecelem ;
struct vecelem_co {
	vecelem		*op = nullptr ;
	int		w = -1 ;
	void operator () (vecelem *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (vecelem_co) */
struct vecelem : vecelem_head {
	vecelem_co	count ;
	vecelem_co	delall ;
	vecelem_co	extent ;
	vecelem_co	audit ;
	vecelem_co	finish ;
	vecelem() noex {
	    count	(this,vecelemmem_count) ;
	    delall	(this,vecelemmem_delall) ;
	    extent	(this,vecelemmem_extent) ;
	    audit	(this,vecelemmem_audit) ;
	    finish	(this,vecelemmem_finish) ;
	    va = nullptr ;
	} ; /* end tcor */
	vecelem(const vecelem &) = delete ;
	vecelem &operator = (const vecelem &) = delete ;
	int start	(int = 0,int = 0,int = 0) noex ;
	int add		(cvoid *) noex ;
	int addlist	(cvoid *,int) noex ;
	int adduniq	(cvoid *) noex ;
	int sort	(int,vecelem_vcmp = nullptr) noex ;
	int get		(int,void *) noex ;
	int getval	(int,void *) noex ;
	int getvec	(void **) noex ;
	int search	(cvoid *,vecelem_vcmp,void **) noex ;
	int del		(int = -1) noex ;
	int sort	(vecelem_vcmp) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct vecelem() {
	    if (va) dtor() ;
	} ;
} ; /* end struct (vecelem) */
#else	/* __cplusplus */
typedef VECELEM		vecelem ;
#endif /* __cplusplus */

typedef VECELEM_FL	vecelem_fl ;

EXTERNC_begin

extern int vecelem_start	(vecelem *,int,int,int) noex ;
extern int vecelem_finish	(vecelem *) noex ;
extern int vecelem_add		(vecelem *,cvoid *) noex ;
extern int vecelem_addlist	(vecelem *,cvoid *,int) noex ;
extern int vecelem_adduniq	(vecelem *,cvoid *) noex ;
extern int vecelem_sort		(vecelem *,vecelem_vcmp) noex ;
extern int vecelem_setsorted	(vecelem *) noex ;
extern int vecelem_search	(vecelem *,vecelem_vcmp) noex ;
extern int vecelem_get		(vecelem *,int,void *) noex ;	/* pointer */
extern int vecelem_getval	(vecelem *,int,void *) noex ;	/* value */
extern int vecelem_getvec	(vecelem *,void **) noex ;
extern int vecelem_count	(vecelem *) noex ;
extern int vecelem_extent	(vecelem *) noex ;
extern int vecelem_del		(vecelem *,int) noex ;
extern int vecelem_delall	(vecelem *) noex ;
extern int vecelem_audit	(vecelem *) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern const vecelemms	vecelemm ;
#endif /* __cplusplus */


#endif /* VECELEM_INCLUDE */


