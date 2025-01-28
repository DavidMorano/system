/* usysutility HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* utility subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 2023-10-08, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USYSUTILITY_INCLUDE
#define	USYSUTILITY_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdarg>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


namespace libu {
    extern int	uloadavgd(double *,int) noex ;
    extern int	snuprintf(char *,int,cchar *,...) noex ;
    extern int	snuloadavgd(char *,int,int,double *,int) noex ;
    extern int	ctdecf(char *,int,int,double) noex ;
    static inline int snuloadavg(char *rb,int rl,int pr,
	    double *ap,int al) noex {
	return snuloadavgd(rb,rl,pr,ap,al) ;
    }
}


#endif /* __cplusplus */
#endif /* USYSUTILITY_INCLUDE */


