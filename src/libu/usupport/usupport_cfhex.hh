/* usupport_cfhex HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTCFHEX_INCLUDE
#define	USUPPORTCFHEX_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |abs(3c)| */
#include	<cstdint>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<stdintx.h>


namespace libu {
    extern int cfhex(cchar *,int,int *)		noex ;
    extern int cfhex(cchar *,int,long *)	noex ;
    extern int cfhex(cchar *,int,longlong *)	noex ;
    extern int cfhex(cchar *,int,uint *)	noex ;
    extern int cfhex(cchar *,int,ulong *)	noex ;
    extern int cfhex(cchar *,int,ulonglong *)	noex ;
    inline int cfhexi(cchar *sp,int sl,int *vp = nullptr) noex {
    	int		rs ;
	if (int v ; (rs = cfhex(sp,sl,&v)) >= 0) {
	    cint va = abs(v) ;
	    rs = va ;
	    if (vp) *vp = v ;
	}
	return rs ;
    } /* end subroutine (cfhexi) */
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTCFHEX_INCLUDE */


