/* cfbin HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert a digit c-string to its integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFBIN_INCLUDE
#define	CFBIN_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern int cfbinsi	(cchar *,int,sint *)		noex ;
extern int cfbinsl	(cchar *,int,slong *)		noex ;
extern int cfbinsll	(cchar *,int,slonglong *)	noex ;

extern int cfbinui	(cchar *,int,uint *)		noex ;
extern int cfbinul	(cchar *,int,ulong *)		noex ;
extern int cfbinull	(cchar *,int,ulonglong *)	noex ;

local inline int cfbini(cchar *sp,int sl,int *rp)		noex {
	return cfbinsi(sp,sl,rp) ;
} /* end */
local inline int cfbinl(cchar *sp,int sl,long *rp)		noex {
	return cfbinsl(sp,sl,rp) ;
} /* end */
local inline int cfbinll(cchar *sp,int sl,longlong *rp)		noex {
	return cfbinsll(sp,sl,rp) ;
} /* end */

EXTERNC_end

#if	__cplusplus

inline int cfbin(cchar *sp,int sl,int *rp)		noex {
	return cfbini(sp,sl,rp) ;
} /* end */
inline int cfbin(cchar *sp,int sl,long *rp)		noex {
	return cfbinl(sp,sl,rp) ;
} /* end */
inline int cfbin(cchar *sp,int sl,longlong *rp)		noex {
	return cfbinll(sp,sl,rp) ;
} /* end */

inline int cfbin(cchar *sp,int sl,uint *rp)		noex {
	return cfbinui(sp,sl,rp) ;
} /* end */
inline int cfbin(cchar *sp,int sl,ulong *rp)		noex {
	return cfbinul(sp,sl,rp) ;
} /* end */
inline int cfbin(cchar *sp,int sl,ulonglong *rp)	noex {
	return cfbinull(sp,sl,rp) ;
} /* end */

#endif /* __cplusplus */


#endif /* CFBIN_INCLUDE */


