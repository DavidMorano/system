/* mknonpath HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a file rooted from the present-working-directory (PWD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKNONPATH_INCLUDE
#define	MKNONPATH_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	mknonpath(char *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* MKNONPATH_INCLUDE */


