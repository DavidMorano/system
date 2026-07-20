/* getnfile HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* retrive the maximum number of possible file-descriptors */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETNFILE_INCLUDE
#define	GETNFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	getnfile(int) noex ;

EXTERNC_end


#endif /* GETNFILE_INCLUDE */


