/* six HEADER */
/* lang=C20 */

/* string-index operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	SIX_INCLUDE
#define	SIX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int siochr(cchar *,int,int) noex ;
extern int sirchr(cchar *,int,int) noex ;
extern int sialpha(cchar *,int) noex ;
extern int sidigit(cchar *,int) noex ;
extern int sialnum(cchar *,int) noex ;
extern int sibasename(cchar *,int) noex ;
extern int sibreak(cchar *,int,cchar *) noex ;
extern int sibrk(cchar *,int,cchar *) noex ;
extern int sicasesub(cchar *,int,cchar *) noex ;
extern int sihyphen(cchar *,int) noex ;
extern int siskipwhite(cchar *,int) noex ;
extern int sispan(cchar *,int,cchar *) noex ;
extern int sisub(cchar *,int,cchar *) noex ;
extern int sirec(cchar *,int,int) noex ;
extern int sicite(cchar *,int,cchar *,int) noex ;
extern int siterm(cchar *,int,cchar *) noex ;

static inline int sichr(cchar *sp,int sl,int sch) noex {
	return siochr(sp,sl,sch) ;
}

EXTERNC_end


#endif /* SIX_INCLUDE */


