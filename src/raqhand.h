/* raqhand INCLUDE */
/* lang=C20 */

/* Random-Access Queue Handler */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RAQHAND_INCLUDE
#define	RAQHAND_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	RAQHAND_DEFENTS	10
#define	RAQHAND		struct raqhand_head
#define	RAQHAND_FL	struct raqhand_flags


struct raqhand_flags {
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

struct raqhand_head {
	cvoid		**va ;
	int		c ;		/* count of items in list */
	int		n ;		/* extent of array */
	int		hi ;		/* head */
	int		ti ;		/* tail */
	RAQHAND_FL	f ;
} ;

typedef struct raqhand_head	raqhand ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int raqhand_start(raqhand *,int,int) noex ;
extern int raqhand_ins(raqhand *,cvoid *) noex ;
extern int raqhand_rem(raqhand *,void **) noex ;
extern int raqhand_del(raqhand *,int) noex ;
extern int raqhand_delall(raqhand *) noex ;
extern int raqhand_count(raqhand *) noex ;
extern int raqhand_acc(raqhand *,int,void **) noex ; 
extern int raqhand_acclast(raqhand *,void **) noex ; 
extern int raqhand_get(raqhand *,int,void **) noex ; 
extern int raqhand_ent(raqhand *,cvoid *) noex ;
extern int raqhand_finish(raqhand *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* RAQHAND_INCLUDE */


