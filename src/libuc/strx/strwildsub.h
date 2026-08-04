/* strwildsub HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a substring within a larger string, but w/ wild-card characters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRWILDSUB_INCLUDE
#define	STRWILDSUB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern char	*strwildsub(cchar *,cchar *) noex ;

EXTERNC_end


#endif /* STRWILDSUB_INCLUDE */


