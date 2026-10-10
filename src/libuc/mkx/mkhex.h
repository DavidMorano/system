/* mkhex HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* make a string of hexadecimal digits */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKHEX_INCLUDE
#define	MKHEX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin
extern int mkhexstr	(char *,int,cvoid *,int) noex ;
EXTERNC_end

#ifdef	__cplusplus
local inline int mkhexstr(char *dp,int dl,cvoid *sp) noex {
    	return mkhexstr(dp,dl,sp,-1) ;
} /* end */
#endif /* __cplusplus */


#endif /* MKHEX_INCLUDE */


