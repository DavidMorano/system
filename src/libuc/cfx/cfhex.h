/* cfhex HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert a decimal digit string to its binary integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFHEX_INCLUDE
#define	CFHEX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern int cfhexsi	(cchar *,int,sint *)		noex ;
extern int cfhexsl	(cchar *,int,slong *)		noex ;
extern int cfhexsll	(cchar *,int,slonglong *)	noex ;

extern int cfhexui	(cchar *,int,uint *)		noex ;
extern int cfhexul	(cchar *,int,ulong *)		noex ;
extern int cfhexull	(cchar *,int,ulonglong *)	noex ;

local inline int cfhexi(cchar *sp,int sl,int *rp)		noex {
	return cfhexsi(sp,sl,rp) ;
} /* end */
local inline int cfhexl(cchar *sp,int sl,long *rp)		noex {
	return cfhexsl(sp,sl,rp) ;
} /* end */
local inline int cfhexll(cchar *sp,int sl,longlong *rp)		noex {
	return cfhexsll(sp,sl,rp) ;
} /* end */

EXTERNC_end

#if	__cplusplus

inline int cfhex(cchar *sp,int sl,int *rp)		noex {
	return cfhexi(sp,sl,rp) ;
} /* end */
inline int cfhex(cchar *sp,int sl,long *rp)		noex {
	return cfhexl(sp,sl,rp) ;
} /* end */
inline int cfhex(cchar *sp,int sl,longlong *rp)		noex {
	return cfhexll(sp,sl,rp) ;
} /* end */

inline int cfhex(cchar *sp,int sl,uint *rp)		noex {
	return cfhexui(sp,sl,rp) ;
} /* end */
inline int cfhex(cchar *sp,int sl,ulong *rp)		noex {
	return cfhexul(sp,sl,rp) ;
} /* end */
inline int cfhex(cchar *sp,int sl,ulonglong *rp)	noex {
	return cfhexull(sp,sl,rp) ;
} /* end */

#endif /* __cplusplus */


#endif /* CFHEX_INCLUDE */


