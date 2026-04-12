/* usupport_ctdec HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTCTDEC_INCLUDE
#define	USUPPORTCTDEC_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* |nullptr_t| */
#include	<stdlib.h>
#include	<stdint.h>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<stdintx.h>


namespace libu {
    extern int ctdecui	(char *,int,uint)		noex ;
    extern int ctdecul	(char *,int,ulong)		noex ;
    extern int ctdecull	(char *,int,ulonglong)		noex ;
    extern int ctdec_unknown	(char *,int)		noex ;
    template<typename T> inline int ctdec(char *rp,int rl,T uv) noex {
	(void) uv ;
	return ctdec_unknown(rp,rl) ;
    }
    template<> inline int ctdec(char *dp,int dl,uint uv)	noex {
	return ctdecui(dp,dl,uv) ;
    }
    template<> inline int ctdec(char *dp,int dl,ulong uv)	noex {
	return ctdecul(dp,dl,uv) ;
    }
    template<> inline int ctdec(char *dp,int dl,ulonglong uv)	noex {
	return ctdecull(dp,dl,uv) ;
    }
    template<> inline int ctdec(char *dp,int dl,int sv)		noex {
	uint		uv = uint(sv) ;
	return ctdecui(dp,dl,uv) ;
    }
    template<> inline int ctdec(char *dp,int dl,long sv)	noex {
	ulong 		uv = ulong(sv) ;
	return ctdecul(dp,dl,uv) ;
    }
    template<> inline int ctdec(char *dp,int dl,longlong sv)	noex {
	ulonglong	uv = ulonglong(sv) ;
	return ctdecull(dp,dl,uv) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTCTDEC_INCLUDE */


