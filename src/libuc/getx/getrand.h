/* getrand HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get random data from the UNIX® kernel */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETRAND_INCLUDE
#define	GETRAND_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<stdintx.h>


EXTERNC_begin

extern int	getrand		(void *,int)	noex ;
extern int	getrandi	(int *)		noex ;
extern int	getrandl	(long *)	noex ;
extern int	getrandll	(longlong *)	noex ;

EXTERNC_end

#ifdef	__cplusplus

local inline int getrand(uint *p,int n = 1)	noex {
	cint	sz = szof(uint) ;
	return getrand(p,(sz * n)) ;
}
local inline int getrand(ulong *p,int n = 1)	noex {
	cint	sz = szof(ulong) ;
	return getrand(p,(sz * n)) ;
}
local inline int getrand(longlong *p,int n = 1)	noex {
	cint	sz = szof(ulonglong) ;
	return getrand(p,(sz * n)) ;
}

#endif /* __cplusplus */


#endif /* GETRAND_INCLUDE */


