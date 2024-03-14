/* veclong HEADER */
/* lang=C20 */

/* vector long-integer operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-14, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1999 David A�D� Morano.  All rights reserved. */

#ifndef	VECLONG_INCLUDE
#define	VECLONG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


/* object defines */
#define	VECLONG_MAGIC		0x73625199
#define	VECLONG_DEFENTS		2
#define	VECLONG			struct veclong_head
#define	VECLONG_FL		struct veclong_flags
#define	VECLONG_CUR		struct veclong_cursor
#define	VECLONG_TYPE		long

/* options */
#define	VECLONG_ODEFAULT	0x0000
#define	VECLONG_OREUSE		0x0001		/* reuse empty slots */
#define	VECLONG_OCOMPACT	0x0002		/* means NOHOLES */
#define	VECLONG_OSWAP		0x0004		/* use swapping */
#define	VECLONG_OSTATIONARY	0x0008		/* entries should not move */
#define	VECLONG_OCONSERVE	0x0010		/* conserve space */
#define	VECLONG_OSORTED		0x0020		/* keep sorted */
#define	VECLONG_OORDERED	0x0040		/* keep ordered */


struct veclong_cursor {
	int		i ;
} ;

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
} ;

struct veclong_head {
	VECLONG_TYPE	*va ;
	VECLONG_FL	f ;
	uint		magic ;
	int		c ;		/* count of items in list */
	int		i ;		/* highest index */
	int		n ;		/* extent of array */
	int		fi ;		/* free index */
} ;

typedef VECLONG		veclong ;
typedef VECLONG_CUR	veclong_cur ;

EXTERNC_begin

extern int veclong_start(veclong *,int,int) noex ;
extern int veclong_finish(veclong *) noex ;
extern int veclong_add(veclong *,VECLONG_TYPE) noex ;
extern int veclong_addlist(veclong *,const VECLONG_TYPE *,int) noex ;
extern int veclong_adduniq(veclong *,VECLONG_TYPE) noex ;
extern int veclong_insert(veclong *,VECLONG_TYPE) noex ;
extern int veclong_assign(veclong *,int,VECLONG_TYPE) noex ;
extern int veclong_resize(veclong *,int) noex ;
extern int veclong_del(veclong *,int) noex ;
extern int veclong_count(veclong *) noex ;
extern int veclong_sort(veclong *) noex ;
extern int veclong_setsorted(veclong *) noex ;
extern int veclong_find(veclong *,VECLONG_TYPE) noex ;
extern int veclong_match(veclong *,VECLONG_TYPE) noex ;
extern int veclong_search(veclong *,VECLONG_TYPE) noex ;
extern int veclong_getval(veclong *,int,VECLONG_TYPE *) noex ;
extern int veclong_getvec(veclong *,VECLONG_TYPE **) noex ;
extern int veclong_mkvec(veclong *,VECLONG_TYPE *) noex ;
extern int veclong_curbegin(veclong *,veclong_cur *) noex ;
extern int veclong_enum(veclong *,veclong_cur *,VECLONG_TYPE *) noex ;
extern int veclong_curend(veclong *,veclong_cur *) noex ;
extern int veclong_audit(veclong *) noex ;

EXTERNC_end


#endif /* VECLONG_INCLUDE */


