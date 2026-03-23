/* raqhand HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Random-Access Queue Handler */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RAQHAND_INCLUDE
#define	RAQHAND_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


#define	RAQHAND		struct raqhand_head
#define	RAQHAND_FL	struct raqhand_flags
#define	RAQHAND_DEFENTS	10

/**** options
reuse		= reuse empty slots
compact		= do not allow for holes
swap		= use swapping for empty slot management
stationary	= entries do not move
conserve	= conserve space where possible
sorted		= maintain a sorted list
ordered		= maintain an ordered list
****/

enum raqhandos {
    raqhando_reuse,
    raqhando_compact,
    raqhando_swap,
    raqhando_stationary,
    raqhando_conserve,
    raqhando_sorted,
    raqhando_ordered,
    raqhando_overlast
} ; /* end enum (raqhandos) */

#ifdef	__cplusplus	/* C++ only! */

struct raqhandms {
    inline static cint	reuse		= (1 << raqhando_reuse) ;
    inline static cint	compact		= (1 << raqhando_compact) ;
    inline static cint	swap		= (1 << raqhando_swap) ;
    inline static cint	stationary	= (1 << raqhando_stationary) ;
    inline static cint	conserve	= (1 << raqhando_conserve) ;
    inline static cint	sorted		= (1 << raqhando_sorted) ;
    inline static cint	ordered		= (1 << raqhando_ordered) ;
} ; /* end struct (raqhandms) */

#endif /* __cplusplus */

#define	RAQHAND_ODEFAULT	0
#define	RAQHAND_OREUSE		(1 << raqhando_reuse)
#define	RAQHAND_OCOMPACT	(1 << raqhando_compact)
#define	RAQHAND_OSWAP		(1 << raqhando_swap)
#define	RAQHAND_OSTATIONARY	(1 << raqhando_stationary)
#define	RAQHAND_OCONSERVE	(1 << raqhando_conserve)
#define	RAQHAND_OSORTED		(1 << raqhando_sorted)
#define	RAQHAND_OORDERED	(1 << raqhando_ordered)

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
} ; /* end struct (raqhand_flags) */

struct raqhand_head {
	cvoid		**va ;
	RAQHAND_FL	fl ;
	int		c ;		/* count of items in list */
	int		n ;		/* extent of array */
	int		hi ;		/* head */
	int		ti ;		/* tail */
} ; /* end struct (raqhand_head) */

typedef RAQHAND		raqhand ;
typedef RAQHAND_FL	raqhand_fl ;

EXTERNC_begin

extern int raqhand_start	(raqhand *,int,int) noex ;
extern int raqhand_ins		(raqhand *,cvoid *) noex ;
extern int raqhand_rem		(raqhand *,void **) noex ;
extern int raqhand_del		(raqhand *,int) noex ;
extern int raqhand_delall	(raqhand *) noex ;
extern int raqhand_count	(raqhand *) noex ;
extern int raqhand_acc		(raqhand *,int,void **) noex ; 
extern int raqhand_acclast	(raqhand *,void **) noex ; 
extern int raqhand_get		(raqhand *,int,void **) noex ; 
extern int raqhand_ent		(raqhand *,cvoid *) noex ;
extern int raqhand_finish	(raqhand *) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern const raqhandms	raqhandm ;
#endif /* __cplusplus */


#endif /* RAQHAND_INCLUDE */


