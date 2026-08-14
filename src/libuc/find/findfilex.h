/* findfilex HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find an eXecutable file given a name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FINDFILEX_INCLUDE
#define	FINDFILEX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ids.h>			/* LIBUC */


EXTERNC_begin

extern int	findfilex(ids *,char *,cchar *) noex ;

EXTERNC_end


#endif /* FINDFILEX_INCLUDE */


