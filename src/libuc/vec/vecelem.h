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
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


/* object defines */
#define	VECELEM_MAGIC		0x73625198
#define	VECELEM			struct vecelem_head
#define	VECELEM_FL		struct vecelem_flags
#define	VECELEM_DEFENTS		2
/* options */
#define	VECELEM_ODEFAULT	0
#define	VECELEM_OREUSE		(1 << 0)	/* reuse empty slots */
#define	VECELEM_OCOMPACT	(1 << 1)	/* means NOHOLES */
#define	VECELEM_OSWAP		(1 << 2)	/* use swapping */
#define	VECELEM_OSTATIONARY	(1 << 3)	/* entries should not move */
#define	VECELEM_OCONSERVE	(1 << 4)	/* conserve space */
#define	VECELEM_OSORTED		(1 << 5)	/* keep sorted */
#define	VECELEM_OORDERED	(1 << 6)	/* keep ordered */


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
} ;

struct vecelem_head {
	voidpp		va ;
	VECELEM_FL	fl ;
	uint		magic ;
	int		c ;		/* count of items in list */
	int		i ;		/* highest index */
	int		n ;		/* extent of array */
	int		fi ;		/* free index */
	int		esz ;		/* element size */
} ;

typedef VECELEM		vecelem ;
typedef VECELEM_FL	vecelem_fl ;

EXTERNC_begin

typedef int (*vecelem_vcmp)(cvoid **,cvoid **) noex ;

extern int vecelem_start(vecelem *,int,int,int) noex ;
extern int vecelem_finish(vecelem *) noex ;
extern int vecelem_add(vecelem *,cvoid *) noex ;
extern int vecelem_adduniq(vecelem *,cvoid *) noex ;
extern int vecelem_count(vecelem *) noex ;
extern int vecelem_sort(vecelem *,vecelem_vcmp) noex ;
extern int vecelem_setsorted(vecelem *) noex ;
extern int vecelem_search(vecelem *,vecelem_vcmp) noex ;
extern int vecelem_get(vecelem *,int,void *) noex ;
extern int vecelem_getval(vecelem *,int,void *) noex ;
extern int vecelem_getvec(vecelem *,void *) noex ;
extern int vecelem_audit(vecelem *) noex ;

EXTERNC_end


#endif /* VECELEM_INCLUDE */


