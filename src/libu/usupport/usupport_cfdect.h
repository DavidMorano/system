/* usupport_cfdect HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Convert-From-Decimal-Time */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTCFDECT_INCLUDE
#define	USUPPORTCFDECT_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<stdintx.h>


namespace libu {
    extern int cfdecti(cchar *,int,int *) noex ;
    inline int cfdect(cchar *sp,int sl,int *rp = nullptr) noex {
	return cfdecti(sp,sl,rp) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTCFDECT_INCLUDE */


