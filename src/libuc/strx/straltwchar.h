/* straltwchar HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find and return am alternative c-string for a wide-character */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-01-10, David A-D- Morano
	This was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRALTWCHAR_INCLUDE
#define	STRALTWCHAR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern cchar	*straltwchar(int) noex ;

EXTERNC_end


#endif /* STRALTWCHAR_INCLUDE */


