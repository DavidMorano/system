/* cfdect HEADER (Convert-From-Decimal-Time) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Convert-From-Decimal-Time */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFDECT_INCLUDE
#define	CFDECT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern int cfdecti	(cchar *,int,int *)			noex ;

EXTERNC_end

#ifdef	__cplusplus

local inline int cfdect(cchar *sp,int sl,int *rp = nullptr)	noex {
	return cfdecti(sp,sl,rp) ;
} /* end subroutine */

#else /* __cplusplus */

local inline int cfdect(cchar *sp,int sl,int *rp)		noex {
	return cfdecti(sp,sl,rp) ;
} /* end subroutine */

#endif /* __cplusplus */


#endif /* CFDECT_INCLUDE */


