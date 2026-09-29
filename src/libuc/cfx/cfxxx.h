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

extern int cfxxxsi	(cchar *,int,int,sint *)	noex ;
extern int cfxxxsl	(cchar *,int,int,slong *)	noex ;
extern int cfxxxsll	(cchar *,int,int,slonglong *)	noex ;

extern int cfxxxui	(cchar *,int,int,uint *)	noex ;
extern int cfxxxul	(cchar *,int,int,ulong *)	noex ;
extern int cfxxxull	(cchar *,int,int,ulonglong *)	noex ;

inline int cfxxxi(cchar *sp,int sl,int b,int *rp)	noex {
	return cfxxxsi(sp,sl,b,rp) ;
} /* end */
inline int cfxxxl(cchar *sp,int sl,int b,long *rp)	noex {
	return cfxxxsl(sp,sl,b,rp) ;
} /* end */
inline int cfxxxll(cchar *sp,int sl,int b,longlong *rp)	noex {
	return cfxxxsll(sp,sl,b,rp) ;
} /* end */

EXTERNC_end

#if	__cplusplus

inline int cfxxx(cchar *sp,int sl,int b,sint *rp)	noex {
	return cfxxxsi(sp,sl,b,rp) ;
} /* end */
inline int cfxxx(cchar *sp,int sl,int b,slong *rp)	noex {
	return cfxxxsl(sp,sl,b,rp) ;
} /* end */
inline int cfxxx(cchar *sp,int sl,int b,slonglong *rp)	noex {
	return cfxxxsll(sp,sl,b,rp) ;
} /* end */

inline int cfxxx(cchar *sp,int sl,int b,uint *rp)	noex {
	return cfxxxui(sp,sl,b,rp) ;
} /* end */
inline int cfxxx(cchar *sp,int sl,int b,ulong *rp)	noex {
	return cfxxxul(sp,sl,b,rp) ;
} /* end */
inline int cfxxx(cchar *sp,int sl,int b,ulonglong *rp)	noex {
	return cfxxxull(sp,sl,b,rp) ;
} /* end */

#endif /* __cplusplus */


#endif /* CFXXX_INCLUDE */


