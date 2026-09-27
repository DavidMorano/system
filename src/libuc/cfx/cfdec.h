/* cfdec HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert a decimal digit string to its binary integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFDEC_INCLUDE
#define	CFDEC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */

#include	<cfdect.h>
#include	<cfdecmf.h>
#include	<cfdecf.h>


EXTERNC_begin

extern int cfdecsi	(cchar *,int,sint *)		noex ;
extern int cfdecsl	(cchar *,int,slong *)		noex ;
extern int cfdecsll	(cchar *,int,slonglong *)	noex ;

extern int cfdecui	(cchar *,int,uint *)		noex ;
extern int cfdecul	(cchar *,int,ulong *)		noex ;
extern int cfdecull	(cchar *,int,ulonglong *)	noex ;

local  inline int cfdeci(cchar *sp,int sl,int *rp)		noex {
	return cfdecsi(sp,sl,rp) ;
} /* end */
local  inline int cfdecl(cchar *sp,int sl,long *rp)		noex {
	return cfdecsl(sp,sl,rp) ;
} /* end */
local  inline int cfdecll(cchar *sp,int sl,longlong *rp)	noex {
	return cfdecsll(sp,sl,rp) ;
} /* end */

EXTERNC_end

#if	__cplusplus

inline int cfdec(cchar *sp,int sl,int *rp)		noex {
	return cfdeci(sp,sl,rp) ;
} /* end */
inline int cfdec(cchar *sp,int sl,long *rp)		noex {
	return cfdecl(sp,sl,rp) ;
} /* end */
inline int cfdec(cchar *sp,int sl,longlong *rp)		noex {
	return cfdecll(sp,sl,rp) ;
} /* end */

inline int cfdec(cchar *sp,int sl,uint *rp)		noex {
	return cfdecui(sp,sl,rp) ;
} /* end */
inline int cfdec(cchar *sp,int sl,ulong *rp)		noex {
	return cfdecul(sp,sl,rp) ;
} /* end */
inline int cfdec(cchar *sp,int sl,ulonglong *rp)	noex {
	return cfdecull(sp,sl,rp) ;
} /* end */

#endif /* __cplusplus */


#endif /* CFDEC_INCLUDE */


