/* recarr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* record-array management */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	RECARR_INCLUDE
#define	RECARR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


#define	RECARR		struct recarr_head
#define	RECARR_FL	struct recarr_flags

/**** options
reuse		= reuse empty slots
compact		= do not allow for holes
swap		= use swapping for empty slot management
stationary	= entries do not move
conserve	= conserve space where possible
sorted		= maintain a sorted list
ordered		= maintain an ordered list
****/

enum recarros {
    recarro_reuse,
    recarro_compact,
    recarro_swap,
    recarro_stationary,
    recarro_conserve,
    recarro_sorted,
    recarro_ordered,
    recarro_overlast
} ; /* end enum (options) */

#ifdef	__cplusplus	/* C++ only! */

struct recarrms {
    inline static cint	reuse		= (1 << recarro_reuse) ;
    inline static cint	compact		= (1 << recarro_compact) ;
    inline static cint	swap		= (1 << recarro_swap) ;
    inline static cint	stationary	= (1 << recarro_stationary) ;
    inline static cint	conserve	= (1 << recarro_conserve) ;
    inline static cint	sorted		= (1 << recarro_sorted) ;
    inline static cint	ordered		= (1 << recarro_ordered) ;
} ; /* end struct (recarrms) */

#endif /* __cplusplus */

#define	RECARR_ODEFAULT		0
#define	RECARR_OREUSE		(1 << recarro_reuse)
#define	RECARR_OCOMPACT		(1 << recarro_compact)
#define	RECARR_OSWAP		(1 << recarro_swap)
#define	RECARR_OSTATIONARY	(1 << recarro_stationary)
#define	RECARR_OCONSERVE	(1 << recarro_conserve)
#define	RECARR_OSORTED		(1 << recarro_sorted)
#define	RECARR_OORDERED		(1 << recarro_ordered)

struct recarr_flags {
	uint		issorted:1 ;
	uint		oreuse:1 ;
	uint		oswap:1 ;
	uint		ostationary:1 ;
	uint		ocompact:1 ;
	uint		osorted:1 ;
	uint		oordered:1 ;
	uint		oconserve:1 ;
} ; /* end struct (recarr_flags) */

struct recarr_head {
	void		**va ;
	int		c ;		/* count of items in list */
	int		i ;		/* highest index */
	int		n ;		/* extent of array */
	int		fi ;		/* free entry index */
	RECARR_FL	fl ;
} ; /* end struct (recarr_head) */

typedef	RECARR		recarr ;
typedef	RECARR_FL	recarr_fl ;

EXTERNC_begin

typedef int (*recarr_cf)(cvoid **,cvoid **) noex ;

extern int recarr_start		(recarr *,int,int) noex ;
extern int recarr_add		(recarr *,cvoid *) noex ;
extern int recarr_count		(recarr *) noex ;
extern int recarr_sort		(recarr *,recarr_cf) noex ;
extern int recarr_setsorted	(recarr *) noex ;
extern int recarr_get		(recarr *,int,cvoid *) noex ; 
extern int recarr_getlast	(recarr *,cvoid *) noex ; 
extern int recarr_ent		(recarr *,cvoid *) noex ;
extern int recarr_search	(recarr *,cvoid *,recarr_cf,void *) noex ;
extern int recarr_del		(recarr *,int) noex ;
extern int recarr_delhand	(recarr *,cvoid *) noex ;
extern int recarr_delall	(recarr *) noex ;
extern int recarr_getvec	(recarr *,void *) noex ;
extern int recarr_extent	(recarr *) noex ;
extern int recarr_audit		(recarr *) noex ;
extern int recarr_finish	(recarr *) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern const recarrms	recarrm ;
#endif /* __cplusplus */


#endif /* RECARR_INCLUDE */


