/* libmallocxx INCLUDE */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-06-17, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module provides some subroutines to serve as short-cuts to
	allocating system buffers (that is: buffers that are used by
	the UNIX® system with their particular buffer-sizes).


*******************************************************************************/

#ifndef	LIBMALLOCXX_INCLUDE
#define	LIBMALLOCXX_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int libmalloc_ma(char **) noex ;
extern int libmalloc_mn(char **) noex ;
extern int libmalloc_mp(char **) noex ;
extern int libmalloc_ml(char **) noex ;
extern int libmalloc_nn(char **) noex ;
extern int libmalloc_hn(char **) noex ;
extern int libmalloc_un(char **) noex ;
extern int libmalloc_gn(char **) noex ;
extern int libmalloc_pn(char **) noex ;
extern int libmalloc_pw(char **) noex ;
extern int libmalloc_sp(char **) noex ;
extern int libmalloc_ua(char **) noex ;
extern int libmalloc_gr(char **) noex ;
extern int libmalloc_pj(char **) noex ;
extern int libmalloc_pr(char **) noex ;
extern int libmalloc_nw(char **) noex ;
extern int libmalloc_ho(char **) noex ;
extern int libmalloc_sv(char **) noex ;
extern int libmalloc_zn(char **) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* LIBMALLOCXX_INCLUDE */


