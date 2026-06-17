/* sixbrk HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character or white-space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SIXBRK_INCLUDE
#define	SIXBRK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	siobrk(cchar *,int,cchar *) noex ;
extern int	sirbrk(cchar *,int,cchar *) noex ;

local inline int sibrk(cchar *sp,int sl,cchar *ss) noex {
    	return siobrk(sp,sl,ss) ;
}

EXTERNC_end


#endif /* SIXBRK_INCLUDE */


