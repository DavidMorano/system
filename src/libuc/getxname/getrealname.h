/* getrealname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* retrieve the real name of the user from the system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-01, David A­D­ Morano
	This was writen for RightCore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETREALNAME_INCLUDE
#define	GETREALNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	getrealname(char *,int,cchar *) noex ;

EXTERNC_end


#endif /* GETREALNAME_INCLUDE */


