/* haspath INCLUDE */
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
	haspath{x}

	Description:
	These subroutines check if a specified c-string has any of
	some characteristic we are looking for.

	Synopsis:
	bool haspath{x}(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		c-string does not have all digits
	true		c-string has all digits in it

*******************************************************************************/

#ifndef	HASPATH_INCLUDE
#define	HASPATH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern bool haspathuser		(cchar *,int) noex ;
extern bool haspathvar		(cchar *,int) noex ;
extern bool haspathmulti	(cchar *,int) noex ;
extern bool haspathprefix	(cchar *,int) noex ;
local inline bool haspathexp(cchar *sp,int sl) noex {
	return haspathprefix	(sp,sl) ;
} /* end subroutine (haspathexp) */

EXTERNC_end


#endif /* HASPATH_INCLUDE */


