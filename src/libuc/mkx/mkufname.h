/* mkufname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a file rooted from the present-working-directory (PWD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKUFNAME_INCLUDE
#define	MKUFNAME_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	mkufname(char *,cchar *,cchar *) noex ;

EXTERNC_end


#endif /* MKUFNAME_INCLUDE */


