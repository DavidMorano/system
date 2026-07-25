/* ucstrtof HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* floating string-to-value functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCSTRTOD_INCLUDE
#define	UCSTRTOD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern int	uc_strtof(cchar *,cchar **,float *)		noex ;
extern int	uc_strtod(cchar *,cchar **,double *)		noex ;
extern int	uc_strtold(cchar *,cchar **,longdouble *)	noex ;

EXTERNC_end

#ifdef	__cplusplus

inline int uc_strto(cchar *sp,cchar **endpp,float *rp)		noex {
	return uc_strtof(sp,endpp,rp) ;
}
inline int uc_strto(cchar *sp,cchar **endpp,double *rp)		noex {
	return uc_strtod(sp,endpp,rp) ;
}
inline int uc_strto(cchar *sp,cchar **endpp,longdouble *rp)	noex {
	return uc_strtold(sp,endpp,rp) ;
}

#endif /* __cplusplus */


#endif /* UCSTRTOD_INCLUDE */


