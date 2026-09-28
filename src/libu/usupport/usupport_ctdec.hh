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
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<stdint.h>		/* CSTD */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<stdintx.h>


namespace libu {
    extern int ctdecsi	(char *,int,sint)		noex ;
    extern int ctdecsl	(char *,int,slong)		noex ;
    extern int ctdecsll	(char *,int,slonglong)		noex ;
    extern int ctdecui	(char *,int,uint)		noex ;
    extern int ctdecul	(char *,int,ulong)		noex ;
    extern int ctdecull	(char *,int,ulonglong)		noex ;
    extern int ctdec_unknown	(char *,int)		noex ;
    template<typename T> inline int ctdec(char *rp,int rl,T uv) noex {
	(void) uv ;
	return libu::ctdec_unknown(rp,rl) ;
    } /* end */
    template<> inline int ctdec(char *dp,int dl,sint sv)	noex {
	return libu::ctdecsi(dp,dl,sv) ;
    } /* end */
    template<> inline int ctdec(char *dp,int dl,slong sv)	noex {
	return libu::ctdecsl(dp,dl,sv) ;
    } /* end */
    template<> inline int ctdec(char *dp,int dl,slonglong sv)	noex {
	return libu::ctdecsll(dp,dl,sv) ;
    } /* end */
    template<> inline int ctdec(char *dp,int dl,uint uv)	noex {
	return libu::ctdecui(dp,dl,uv) ;
    } /* end */
    template<> inline int ctdec(char *dp,int dl,ulong uv)	noex {
	return libu::ctdecul(dp,dl,uv) ;
    } /* end */
    template<> inline int ctdec(char *dp,int dl,ulonglong uv)	noex {
	return libu::ctdecull(dp,dl,uv) ;
    } /* end */
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTCTDEC_INCLUDE */


