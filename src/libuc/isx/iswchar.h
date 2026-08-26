/* iswchar HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* test a wide-character for a property */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ISWCHAR_INCLUDE
#define	ISWCHAR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stddef.h>		/* CSTD |whcar_t| */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern bool	iswcwhite(wchar_t) noex ;

EXTERNC_end


#endif /* ISWCHAR_INCLUDE */


