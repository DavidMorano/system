/* usupport_cfdec HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTCFEC_INCLUDE
#define	USUPPORTCFEC_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |abs(3c)| */
#include	<cstdint>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<stdintx.h>


namespace libu {
    extern int cfdec(cchar *,int,int *) noex ;
    extern int cfdec(cchar *,int,long *) noex ;
    extern int cfdec(cchar *,int,longlong *) noex ;
    extern int cfdec(cchar *,int,uint *) noex ;
    extern int cfdec(cchar *,int,ulong *) noex ;
    extern int cfdec(cchar *,int,ulonglong *) noex ;
    inline int cfdeci(cchar *sp,int sl,int *vp = nullptr) noex {
    	int		rs ;
	if (int v ; (rs = cfdec(sp,sl,&v)) >= 0) {
	    cint va = abs(v) ;
	    rs = va ;
	    if (vp) *vp = v ;
	}
	return rs ;
    } /* end subroutine (cfdeci) */
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTCFEC_INCLUDE */


