/* sispan HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character or white-space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SISPAN_INCLUDE
#define	SISPAN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	sispan(cchar *,int,cchar *) noex ;
extern int	sispanalpha(cchar *,int) noex ;
extern int	sispanalnum(cchar *,int) noex ;
extern int	sispandigit(cchar *,int) noex ;
extern int	sispandigex(cchar *,int) noex ;
extern int	sispanoctal(cchar *,int) noex ;
extern int	sispanwhite(cchar *,int) noex ;
extern int	sispanblank(cchar *,int) noex ;

local inline int sispandig(cchar *sp,int sl) noex {
    	return sispandigit(sp,sl) ;
}
local inline int sispanoct(cchar *sp,int sl) noex {
    	return sispanoctal(sp,sl) ;
}
local inline int sispandec(cchar *sp,int sl) noex {
    	return sispandigit(sp,sl) ;
}
local inline int sispanhex(cchar *sp,int sl) noex {
    	return sispandigex(sp,sl) ;
}
local inline int sispanwht(cchar *sp,int sl) noex {
    	return sispanwhite(sp,sl) ;
}
local inline int sispanblk(cchar *sp,int sl) noex {
    	return sispanblank(sp,sl) ;
}
local inline int siskipwhite(cchar *sp,int sl) noex {
    	return sispanwhite(sp,sl) ;
}

EXTERNC_end


#endif /* SISPAN_INCLUDE */


