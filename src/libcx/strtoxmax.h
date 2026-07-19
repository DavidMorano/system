/* strtoxmax HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* C-language c-string support */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-09-07, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

#ifndef	STRTOXMAX_INCLUDE
#define	STRTOXMAX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<inttypes.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern intmax_t	strtoimax(cchar *,char **,int) ;
extern intmax_t	strtointmax(cchar *,char **,int) ;

extern uintmax_t strtoumax(cchar *,char **,int) ;
extern uintmax_t strtouintmax(cchar *,char **,int) ;

EXTERNC_end


#endif /* STRTOXMAX_INCLUDE */


