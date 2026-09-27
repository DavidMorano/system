/* cfdecf HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert a decimal digit string to its binary floating value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFDECF_INCLUDE
#define	CFDECF_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern int	cfdecf	(cchar *,int,float *)		noex ;
extern int	cfdecd	(cchar *,int,double *)		noex ;
extern int	cfdecld	(cchar *,int,longdouble *)	noex ;

/* historical (from about 1995 or earlier) */
local inline int cfdouble(cchar *sp,int sl,double *rp)	noex {
    	return cfdecd(sp,sl,rp) ;
} /* end */

EXTERNC_end

#ifdef	__cplusplus
inline int	cfdec	(cchar *sp,int sl,float *rp)		noex {
    	return cfdecf(sp,sl,rp) ;
} /* end */
inline int	cfdec	(cchar *sp,int sl,double *rp)		noex {
    	return cfdecd(sp,sl,rp) ;
} /* end */
inline int	cfdec	(cchar *sp,int sl,longdouble *rp)	noex {
    	return cfdecld(sp,sl,rp) ;
} /* end */
#endif /* __cplusplus */


#endif /* CFDECF_INCLUDE */


