/* sichar HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character or white-space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SICHAR_INCLUDE
#define	SICHAR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	sialpha(cchar *,int) noex ;
extern int	sialnum(cchar *,int) noex ;
extern int	sidigit(cchar *,int) noex ;
extern int	sidigex(cchar *,int) noex ;
extern int	sioctal(cchar *,int) noex ;
extern int	siwhite(cchar *,int) noex ;
extern int	siblank(cchar *,int) noex ;

local inline int sidig(cchar *sp,int sl) noex {
    	return sidigit(sp,sl) ;
}
local inline int sioct(cchar *sp,int sl) noex {
    	return sioctal(sp,sl) ;
}
local inline int sidec(cchar *sp,int sl) noex {
    	return sidigit(sp,sl) ;
}
local inline int sihex(cchar *sp,int sl) noex {
    	return sidigex(sp,sl) ;
}
#ifdef	COMMENT
local inline int siwht(cchar *sp,int sl) noex {
    	return siwhite(sp,sl) ;
}
#endif /* COMMENT */

local inline int siblk(cchar *sp,int sl) noex {
	return siblank(sp,sl) ;
}

EXTERNC_end


#endif /* SICHAR_INCLUDE */


