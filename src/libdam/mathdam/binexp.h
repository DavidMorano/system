/* binexp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* calculate an exponetion yieling a double floating result */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BINEXP_INCLUDE
#define	BINEXP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern float		binexpf(float,int) noex ;
extern double		binexp(double,int) noex ;
extern long double	binexpl(long double,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

inline float		binexp(float x,int n) noex {
    	return binexpf(x,n) ;
}
inline long double	binexp(long double x,int n) noex {
    	return binexpl(x,n) ;
}


#endif /* __cplusplus */


#endif /* BINEXP_INCLUDE */


