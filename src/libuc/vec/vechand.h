/* vechand HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* vector list structure (Vector Handle) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VECHAND_INCLUDE
#define	VECHAND_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


/* object defines */
#define	VECHAND		struct vechand_head
#define	VECHAND_FL	struct vechand_flags
#define	VECHAND_DEFENTS	10
/* options */
#define	VECHAND_ODEFAULT	0
#define	VECHAND_OREUSE		(1 << 0)	/* reuse empty slots */
#define	VECHAND_OCOMPACT	(1 << 1)	/* means NOHOLES */
#define	VECHAND_OSWAP		(1 << 2)	/* use swapping */
#define	VECHAND_OSTATIONARY	(1 << 3)	/* entries should not move */
#define	VECHAND_OCONSERVE	(1 << 4)	/* conserve space */
#define	VECHAND_OSORTED		(1 << 5)	/* keep sorted */
#define	VECHAND_OORDERED	(1 << 6)	/* keep ordered */


struct vechand_flags {
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

struct vechand_head {
	void		**va ;
	int		c ;		/* count of items in list */
	int		i ;		/* highest index */
	int		n ;		/* extent of array */
	int		fi ;		/* free entry index */
	VECHAND_FL	f ;
} ; /* end struct (vechand_head) */

EXTERNC_begin

typedef int (*vechand_vcmp)(cvoid **,cvoid **) noex ;
typedef int (*vechand_f)(cvoid **,cvoid **) noex ;

EXTERNC_end

#ifdef	__cplusplus
enum vechandmems {
	vechandmem_count,
	vechandmem_setsorted,
	vechandmem_issorted,
	vechandmem_delall,
	vechandmem_extent,
	vechandmem_audit,
	vechandmem_finish,
	vechandmem_overlast
} ;
struct vechand ;
struct vechand_co {
	vechand		*op = nullptr ;
	int		w = -1 ;
	void operator () (vechand *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (vechand_co) */
struct vechand : vechand_head {
	vechand_co	count ;
	vechand_co	setsorted ;
	vechand_co	issorted ;
	vechand_co	delall ;
	vechand_co	extent ;
	vechand_co	audit ;
	vechand_co	finish ;
	vechand() noex {
	    count(this,vechandmem_count) ;
	    setsorted(this,vechandmem_setsorted) ;
	    issorted(this,vechandmem_issorted) ;
	    delall(this,vechandmem_delall) ;
	    extent(this,vechandmem_extent) ;
	    audit(this,vechandmem_audit) ;
	    finish(this,vechandmem_finish) ;
	} ;
	vechand(const vechand &) = delete ;
	vechand &operator = (const vechand &) = delete ;
	int start(int = 0,int = 0) noex ;
	int add(cvoid *) noex ;
	int sort(int,vechand_f = nullptr) noex ;
	int get(int,void **) noex ;
	int getlast(int,void **) noex ;
	int getvec(void *) noex ;
	int search(cvoid *,vechand_f,void **) noex ;
	int del(int = -1) noex ;
	int delhand(cvoid *) noex ;
	int sort(vechand_f) noex ;
	void dtor() noex ;
	~vechand() noex {
	    dtor() ;
	} ;
} ; /* end struct (vechand) */
#else	/* __cplusplus */
typedef VECHAND		vechand ;
#endif /* __cplusplus */

typedef	VECHAND_FL	vechand_fl ;

EXTERNC_begin

extern int vechand_start(vechand *,int,int) noex ;
extern int vechand_add(vechand *,cvoid *) noex ;
extern int vechand_count(vechand *) noex ;
extern int vechand_sort(vechand *,vechand_vcmp) noex ;
extern int vechand_setsorted(vechand *) noex ;
extern int vechand_get(vechand *,int,void **) noex ; 
extern int vechand_getlast(vechand *,void **) noex ; 
extern int vechand_search(vechand *,cvoid *,vechand_vcmp,void **) noex ;
extern int vechand_ent(vechand *,cvoid *) noex ;
extern int vechand_issorted(vechand *) noex ;
extern int vechand_del(vechand *,int) noex ;
extern int vechand_delhand(vechand *,cvoid *) noex ;
extern int vechand_delall(vechand *) noex ;
extern int vechand_getvec(vechand *,void *) noex ;
extern int vechand_extent(vechand *) noex ;
extern int vechand_audit(vechand *) noex ;
extern int vechand_finish(vechand *) noex ;

EXTERNC_end


#endif /* VECHAND_INCLUDE */


