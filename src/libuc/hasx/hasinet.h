/* hasinet INCLUDE */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does a counted c-string some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	hasinet{x}

	Description:
	These subroutines check if a specified c-string has the
	characteristic that we are looking for.

	Synopsis:
	bool hasnet4addrstr(cchar *sp,int sl) noex
	bool hasnet6addrstr(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	true		true (empty)
	false		false (not empty)

*******************************************************************************/

#ifndef	HASINET_INCLUDE
#define	HASINET_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern bool hasinet4addrstr(cchar *,int) noex ;
extern bool hasinet6addrstr(cchar *,int) noex ;

local inline bool hasINET4addrstr(cchar *sp,int sl) noex {
    	return hasinet4addrstr(sp,sl) ;
}
local inline bool hasINET6addrstr(cchar *sp,int sl) noex {
    	return hasinet6addrstr(sp,sl) ;
}

EXTERNC_end


#endif /* HASINET_INCLUDE */


