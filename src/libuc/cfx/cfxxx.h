/* cfxxx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert a decimal digit string to its binary integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFXXX_INCLUDE
#define	CFXXX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern int cfxxxsi	(cchar *,int,sint *)		noex ;
extern int cfxxxsl	(cchar *,int,slong *)		noex ;
extern int cfxxxsll	(cchar *,int,slonglong *)	noex ;

extern int cfxxxui	(cchar *,int,uint *)		noex ;
extern int cfxxxul	(cchar *,int,ulong *)		noex ;
extern int cfxxxull	(cchar *,int,ulonglong *)	noex ;

inline int cfxxxi(cchar *sp,int sl,int *rp)		noex {
	return cfxxxsi(sp,sl,rp) ;
} /* end */
inline int cfxxxl(cchar *sp,int sl,long *rp)		noex {
	return cfxxxsl(sp,sl,rp) ;
} /* end */
inline int cfxxxll(cchar *sp,int sl,longlong *rp)		noex {
	return cfxxxsll(sp,sl,rp) ;
} /* end */

EXTERNC_end

#if	__cplusplus

inline int cfxxx(cchar *sp,int sl,sint *rp)		noex {
	return cfxxxsi(sp,sl,rp) ;
} /* end */
inline int cfxxx(cchar *sp,int sl,slong *rp)		noex {
	return cfxxxsl(sp,sl,rp) ;
} /* end */
inline int cfxxx(cchar *sp,int sl,slonglong *rp)		noex {
	return cfxxxsll(sp,sl,rp) ;
} /* end */

inline int cfxxx(cchar *sp,int sl,uint *rp)		noex {
	return cfxxxui(sp,sl,rp) ;
} /* end */
inline int cfxxx(cchar *sp,int sl,ulong *rp)		noex {
	return cfxxxul(sp,sl,rp) ;
} /* end */
inline int cfxxx(cchar *sp,int sl,ulonglong *rp)	noex {
	return cfxxxull(sp,sl,rp) ;
} /* end */

#endif /* __cplusplus */


#endif /* CFXXX_INCLUDE */


