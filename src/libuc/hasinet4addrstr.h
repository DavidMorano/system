/* hasinet4addrstr HEADER */
/* lang=C20 */

/* test whether the given string contains an INET4 address string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASINET4ADDRSTR_INCLUDE
#define	HASINET4ADDRSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern bool	hasinet4addrstr(cchar *,int) noex ;

static inline bool hasINET4AddrStr(cchar *sp,int sl) noex {
	return hasinet4addrstr(sp,sl) ;
}

EXTERNC_end


#endif /* HASINET4ADDRSTR_INCLUDE */


