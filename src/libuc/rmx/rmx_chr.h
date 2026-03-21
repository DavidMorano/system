/* rmx_chr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* remove characters from counted c-string before or after a given character */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RMXCHR_INCLUDE
#define	RMXCHR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern int	rmochr(cchar *,int,int) noex ;
extern int	rmrchr(cchar *,int,int) noex ;

local inline int rmchr(cchar *sp,int sl,int sch) noex {
	return rmochr(sp,sl,sch) ;
}
local inline int rmdot(cchar *sp,int sl) noex {
	return rmrchr(sp,sl,'.') ;
}

EXTERNC_end

#ifdef	__cplusplus

local inline int rmdot(cchar *sp) noex {
	return rmrchr(sp,-1,'.') ;
}

#endif /* __cplusplus */


#endif /* RMXCHR_INCLUDE */


