/* cfoct HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert a decimal digit string to its binary integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFOCT_INCLUDE
#define	CFOCT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern int cfoctsi	(cchar *,int,sint *)		noex ;
extern int cfoctsl	(cchar *,int,slong *)		noex ;
extern int cfoctsll	(cchar *,int,slonglong *)	noex ;

extern int cfoctui	(cchar *,int,uint *)		noex ;
extern int cfoctul	(cchar *,int,ulong *)		noex ;
extern int cfoctull	(cchar *,int,ulonglong *)	noex ;

local inline int cfocti(cchar *sp,int sl,int *rp)		noex {
	return cfoctsi(sp,sl,rp) ;
} /* end */
local inline int cfoctl(cchar *sp,int sl,long *rp)		noex {
	return cfoctsl(sp,sl,rp) ;
} /* end */
local inline int cfoctll(cchar *sp,int sl,longlong *rp)		noex {
	return cfoctsll(sp,sl,rp) ;
} /* end */

EXTERNC_end

#if	__cplusplus

inline int cfoct(cchar *sp,int sl,int *rp)		noex {
	return cfocti(sp,sl,rp) ;
} /* end */
inline int cfoct(cchar *sp,int sl,long *rp)		noex {
	return cfoctl(sp,sl,rp) ;
} /* end */
inline int cfoct(cchar *sp,int sl,longlong *rp)		noex {
	return cfoctll(sp,sl,rp) ;
} /* end */

inline int cfoct(cchar *sp,int sl,uint *rp)		noex {
	return cfoctui(sp,sl,rp) ;
} /* end */
inline int cfoct(cchar *sp,int sl,ulong *rp)		noex {
	return cfoctul(sp,sl,rp) ;
} /* end */
inline int cfoct(cchar *sp,int sl,ulonglong *rp)	noex {
	return cfoctull(sp,sl,rp) ;
} /* end */

#endif /* __cplusplus */


#endif /* CFOCT_INCLUDE */


