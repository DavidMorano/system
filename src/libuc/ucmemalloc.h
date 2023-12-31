/* ucbmemalloc HEADER */
/* lang=C20 */

/* memory allocation facility */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	UCMEMALLOC_INCLUDE
#define	UCMEMALLOC_INCLUDE


#include	<envstandards.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	UCMEMALLOC_STATS	struct ucmemalloc_stats


struct ucmemalloc_stats {
	ulong		err_underflow ;
	ulong		err_overflow ;
	ulong		err_noalloc ;
	ulong		err_nofree ;
	ulong		num_allocs ;
	ulong		num_malloc ;
	ulong		num_valloc ;
	ulong		num_calloc ;
	ulong		num_realloc ;
	ulong		num_frees ;
	ulong		num_outmax ;
	ulong		out_size ;
	ulong		out_sizemax ;
	int		err_rs ;
} ; /* end structure (ucmemalloc_stats) */

#ifdef	__cplusplus
extern "C" {
#endif

extern int uc_malloc(int,void *) noex ;
extern int uc_calloc(int,int,void *) noex ;
extern int uc_valloc(int,void *) noex ;
extern int uc_valloc(int,void *) noex ;
extern int uc_realloc(cvoid *,int,void *) noex ;
extern int uc_free(cvoid *vp) noex ;
extern int uc_mallset(int) noex ;
extern int uc_mallcount(ulong *) noex ;
extern int uc_mallout(ulong *) noex ;
extern int uc_mallstats(UCMEMALLOC_STATS *) noex ;
extern int uc_mallpresent(cvoid *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCMEMALLOC_INCLUDE */


