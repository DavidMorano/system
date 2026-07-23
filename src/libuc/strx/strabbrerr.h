/* strabbrerr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-error (strerr) abbreviation retrieval */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRABBRERR_INCLUDE
#define	STRABBRERR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern cchar	*strabbrerr(int) noex ;

EXTERNC_end


#endif /* STRABBRERR_INCLUDE */


