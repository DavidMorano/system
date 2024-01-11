/* vecitem HEADER */
/* lang=C++20 */

/* vector�item operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-13, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1999 David A�D� Morano.  All rights reserved. */

#ifndef	VECITEM_INCLUDE
#define	VECITEM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


/* object defines */

#define	VECITEM			struct vecitem_head
#define	VECITEM_FL		struct vecitem_flags
#define	VECITEM_CUR		struct vecitem_cursor
#define	VECITEM_DEFENTS		10

/* options */

#define	VECITEM_ODEFAULT	0x0000
#define	VECITEM_OREUSE		0x0001
#define	VECITEM_OCOMPACT	0x0002		/* means NOHOLES */
#define	VECITEM_OSWAP		0x0004
#define	VECITEM_OSTATIONARY	0x0008
#define	VECITEM_OCONSERVE	0x0010
#define	VECITEM_OSORTED		0x0020
#define	VECITEM_OORDERED	0x0040


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
	VECITEM_FL	f ;
	int		c ;		/* count of items in list */
	int		i ;		/* highest index */
	int		n ;		/* extent of array */
	int		fi ;		/* free index */
} ;

struct vecitem_cursor  {
	int		i, c ;
} ;

typedef VECITEM		vecitem ;
typedef VECITEM_CUR	vecitem_cur ;

EXTERNC_begin

typedef int (*vecitem_cmpf)(cvoid **,cvoid **) noex ;

extern int vecitem_start(vecitem *,int,int) noex ;
extern int vecitem_finish(vecitem *) noex ;
extern int vecitem_add(vecitem *,void *,int) noex ;
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


