/* usupport_ctdec HEADER */
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
    extern int ctdeci	(char *,int,int)		noex ;
    extern int ctdecl	(char *,int,long)		noex ;
    extern int ctdecll	(char *,int,longlong)		noex ;
    extern int ctdecui	(char *,int,uint)		noex ;
    extern int ctdecul	(char *,int,ulong)		noex ;
    extern int ctdecull	(char *,int,ulonglong)		noex ;
    extern int ctdec_unknown	(char *,int)		noex ;
    template<typename T> inline int ctdec(char *rp,int rl,T uv) noex {
	(void) uv ;
	return ctdec_unknown(rp,rl) ;
    }
    template<> inline int ctdec(char *dp,int dl,int sv)		noex {
	return ctdeci(dp,dl,sv) ;
    }
    template<> inline int ctdec(char *dp,int dl,long sv)	noex {
	return ctdecl(dp,dl,sv) ;
    }
    template<> inline int ctdec(char *dp,int dl,longlong sv)	noex {
	return ctdecll(dp,dl,sv) ;
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
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTCTDEC_INCLUDE */


