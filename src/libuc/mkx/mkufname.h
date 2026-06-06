/* mkufname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a "user" file-name (the name is quite misleading) */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-04-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

#ifndef	MKUFNAME_INCLUDE
#define	MKUFNAME_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	mkufname(char *,cchar *,cchar *) noex ;

EXTERNC_end


#endif /* MKUFNAME_INCLUDE */


