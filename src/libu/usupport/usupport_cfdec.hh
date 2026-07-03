/* usupport_cfdec HEADER */
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

#ifndef	USUPPORTCFDEC_INCLUDE
#define	USUPPORTCFDEC_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD |abs(3c)| */
#include	<cstdint>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


namespace libu {
    extern int cfdec(cchar *,int,int *)		noex ;
    extern int cfdec(cchar *,int,long *)	noex ;
    extern int cfdec(cchar *,int,longlong *)	noex ;
    extern int cfdec(cchar *,int,uint *)	noex ;
    extern int cfdec(cchar *,int,ulong *)	noex ;
    extern int cfdec(cchar *,int,ulonglong *)	noex ;
    inline int cfdeci(cchar *sp,int sl,int *vp = nullptr) noex {
    	int		rs ;
	if (int v ; (rs = libu::cfdec(sp,sl,&v)) >= 0) {
	    cint va = abs(v) ;
	    rs = va ;
	    if (vp) *vp = v ;
	} /* end if */
	return rs ;
    } /* end subroutine (cfdeci) */
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTCFDEC_INCLUDE */


