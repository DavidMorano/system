/* cfa26 HEADER */
/* lang=C20 */

/* convert from Alpha (a base-26 numeric representation) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	CFA26_INCLUDE
#define	CFA26_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<usysrets.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>


EXTERNC_begin

extern int cfa26i(cchar *,int,int *) noex ;
extern int cfa26l(cchar *,int,long *) noex ;
extern int cfa26ll(cchar *,int,longlong *) noex ;

extern int cfa26ui(cchar *,int,uint *) noex ;
extern int cfa26ul(cchar *,int,ulong *) noex ;
extern int cfa26ull(cchar *,int,ulonglong *) noex ;

EXTERNC_end

#if	__cplusplus

int cfa26(cchar *sp,int sl,int *rp) noex {
	return cfa26i(sp,sl,rp) ;
}
int cfa26(cchar *sp,int sl,long *rp) noex {
	return cfa26l(sp,sl,rp) ;
}
int cfa26(cchar *sp,int sl,longlong *rp) noex {
	return cfa26ll(sp,sl,rp) ;
}

int cfa26(cchar *sp,int sl,uint *rp) noex {
	return cfa26ui(sp,sl,rp) ;
}
int cfa26(cchar *sp,int sl,ulong *rp) noex {
	return cfa26ul(sp,sl,rp) ;
}
int cfa26(cchar *sp,int sl,ulonglong *rp) noex {
	return cfa26ull(sp,sl,rp) ;
}

#endif /* __cplusplus */


#endif /* CFA26_INCLUDE */


