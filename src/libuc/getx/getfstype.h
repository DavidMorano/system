/* getfstype HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the type of the filesystem for an FD */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-09-27, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	GETFSTYPE_INCLUE
#define	GETFSTYPE_INCLUE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	getfstype(char *,int,int) noex ;

EXTERNC_end


#endif /* GETFSTYPE_INCLUDE */


