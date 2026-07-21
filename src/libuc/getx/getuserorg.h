/* getuserorg HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the best organization string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETUSERORG_INCLUDE
#define	GETUSERORG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	getuserorg(char *,int,cchar *) noex ;
extern int	gethomeorg(char *,int,cchar *) noex ;

EXTERNC_end


#endif /* GETUSERORG_INCLUDE */


