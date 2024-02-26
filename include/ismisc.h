/* ismisc HEADER */
/* lang=C20 */

/* convert a single character to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ISMISC_INCLUDE
#define	ISMISC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern bool	isprime(uint) noex ;
extern bool	isdotdir(cchar *) noex ;

static inline bool isDotDir(cchar *dp) noex {
	return isdotdir(dp) ;
}

EXTERNC_end


#endif /* ISMISC_INCLUDE */


