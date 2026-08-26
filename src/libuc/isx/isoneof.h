/* isoneof HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* test is an integer is one of a group of integers */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ISONEOF_INCLUDE
#define	ISONEOF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern bool isoneof	(cint *,int) noex ;
extern bool isonebad	(cint *,int) noex ;

local inline bool isOneOf(cint *rsa,int rs) noex {
	return isoneof(rsa,rs) ;
} /* end */
local inline bool isOneBad(cint *rsa,int rs) noex {
	return isonebad(rsa,rs) ;
} /* end */

EXTERNC_end


#endif /* ISONEOF_INCLUDE */


