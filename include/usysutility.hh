/* usysutility HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* LIBU system utility subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 2023-10-08, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USYSUTILITY_INCLUDE
#define	USYSUTILITY_INCLUDE
#ifdef	__cplusplus /* C++ only) */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>		/* |va_list(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


namespace libu {
    typedef double	dbl ;
    extern int	uloadavgd(double *,int) noex ;
    extern int	snprintf(char *,int,cchar *,...) noex ;
    extern int	snvprintf(char *,int,cchar *,va_list) noex ;
    extern int	snloadavgd(char *,int,int,double *,int) noex ;
    extern int	ctdecf(char *,int,int,double) noex ;
#ifdef	COMMENT
    static inline int snloadavg(char *rb,int rl,int pr,dbl *ap,int al) noex {
	return snloadavgd(rb,rl,pr,ap,al) ;
    }
#endif /* COMMENT */
} /* end namespace (libu) */


#endif /* __cplusplus (C++ only) */
#endif /* USYSUTILITY_INCLUDE */


