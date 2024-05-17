/* logbase HEADER */
/* lang=C20 */

/* Integer-Remaider */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	LOGBASE_INCLUDE
#define	LOGBASE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>


EXTERNC_begin

extern float		logbasef(float,float) noex ;
extern double		logbase(double,double) noex ;
extern long double	logbasel(long double,long double) noex ;

EXTERNC_end

#ifdef	__cplusplus

static inline float		logbase(float b,float v) noex {
	return logbasef(b,v) ;
}
static inline long double	logbase(long double b,long double v) noex {
	return logbasel(b,v) ;
}

#endif /* __cplusplus */


#endif /* LOGBASE_INCLUDE */


