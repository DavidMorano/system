/* mallocxx INCLUDE */
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

#ifndef	MALLOCXX_INCLUDE
#define	MALLOCXX_INCLUDE


#include	<envstandards.h>
#include	<usystem.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int malloc_ma(char **) noex ;
extern int malloc_mn(char **) noex ;
extern int malloc_mp(char **) noex ;
extern int malloc_ml(char **) noex ;
extern int malloc_nn(char **) noex ;
extern int malloc_hn(char **) noex ;
extern int malloc_un(char **) noex ;
extern int malloc_gn(char **) noex ;
extern int malloc_pn(char **) noex ;
extern int malloc_pw(char **) noex ;
extern int malloc_sp(char **) noex ;
extern int malloc_ua(char **) noex ;
extern int malloc_gr(char **) noex ;
extern int malloc_pj(char **) noex ;
extern int malloc_pr(char **) noex ;
extern int malloc_nw(char **) noex ;
extern int malloc_ho(char **) noex ;
extern int malloc_sv(char **) noex ;
extern int malloc_zn(char **) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* MALLOCXX_INCLUDE */


