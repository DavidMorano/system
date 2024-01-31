/* cfb26 HEADER */
/* lang=C20 */

/* convert from base-26 */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFB26_INCLUDE
#define	CFB26_INCLUDE


#include	<envstandards.h>	/* first to configure */
#include	<usysrets.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>		/* <- for |DIGBUFLEN| */


EXTERNC_begin

extern int cfb26i(cchar *,int,int *) noex ;
extern int cfb26l(cchar *,int,long *) noex ;
extern int cfb26ll(cchar *,int,longlong *) noex ;

extern int cfb26ui(cchar *,int,uint *) noex ;
extern int cfb26ul(cchar *,int,ulong *) noex ;
extern int cfb26ull(cchar *,int,ulonglong *) noex ;

EXTERNC_end

#if	__cplusplus

int cfb26(cchar *sp,int sl,int *rp) noex {
	return cfb26i(sp,sl,rp) ;
}
int cfb26(cchar *sp,int sl,long *rp) noex {
	return cfb26l(sp,sl,rp) ;
}
int cfb26(cchar *sp,int sl,longlong *rp) noex {
	return cfb26ll(sp,sl,rp) ;
}

int cfb26(cchar *sp,int sl,uint *rp) noex {
	return cfb26ui(sp,sl,rp) ;
}
int cfb26(cchar *sp,int sl,ulong *rp) noex {
	return cfb26ul(sp,sl,rp) ;
}
int cfb26(cchar *sp,int sl,ulonglong *rp) noex {
	return cfb26ull(sp,sl,rp) ;
}

#endif /* __cplusplus */


#endif /* CFB26_INCLUDE */


