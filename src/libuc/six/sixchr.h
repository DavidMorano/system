/* sixchr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character or white-space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SIXCHR_INCLUDE
#define	SIXCHR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	siochr(cchar *,int,int) noex ;
extern int	sirchr(cchar *,int,int) noex ;

local inline int sichr(cchar *sp,int sl,int sch) noex {
	return siochr(sp,sl,sch) ;
}

EXTERNC_end

#ifdef	__cplusplus
inline int sichr(cchar *sp,int sch) noex {
	return siochr(sp,-1,sch) ;
}
#endif /* __cplusplus */


#endif /* SIXCHR_INCLUDE */


