/* usupport_cthex HEADER */
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

#ifndef	USUPPORTCTHEX_INCLUDE
#define	USUPPORTCTHEX_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<stdint.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


namespace libu {
    extern int cthexsi	(char *,int,sint)		noex ;
    extern int cthexsl	(char *,int,slong)		noex ;
    extern int cthexsll	(char *,int,slonglong)		noex ;
    extern int cthexui	(char *,int,uint)		noex ;
    extern int cthexul	(char *,int,ulong)		noex ;
    extern int cthexull	(char *,int,ulonglong)		noex ;
    extern int cthex_unknown	(char *,int)		noex ;
    template<typename T> inline int cthex(char *rp,int rl,T uv) noex {
	(void) uv ;
	return libu::cthex_unknown(rp,rl) ;
    } /* end */
    template<> inline int cthex(char *dp,int dl,sint sv)	noex {
	return libu::cthexsi(dp,dl,sv) ;
    } /* end */
    template<> inline int cthex(char *dp,int dl,slong sv)	noex {
	return libu::cthexsl(dp,dl,sv) ;
    } /* end */
    template<> inline int cthex(char *dp,int dl,slonglong sv)	noex {
	return libu::cthexsll(dp,dl,sv) ;
    } /* end */
    template<> inline int cthex(char *dp,int dl,uint uv)	noex {
	return libu::cthexui(dp,dl,uv) ;
    } /* end */
    template<> inline int cthex(char *dp,int dl,ulong uv)	noex {
	return libu::cthexul(dp,dl,uv) ;
    } /* end */
    template<> inline int cthex(char *dp,int dl,ulonglong uv)	noex {
	return libu::cthexull(dp,dl,uv) ;
    } /* end */
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTCTHEX_INCLUDE */


