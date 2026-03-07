/* hasnot HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* does a counted c-string some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASNOT_INCLUDE
#define	HASNOT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern bool hasnotdots(cchar *,int) noex ;
extern bool hasnotempty(cchar *,int) noex ;

local inline bool hasNotDots(cchar *sp,int sl) noex {
	return hasnotdots(sp,sl) ;
}
local inline bool hasNotEmpty(cchar *sp,int sl) noex {
	return hasnotempty(sp,sl) ;
}

EXTERNC_end


#endif /* HASNOT_INCLUDE */


