/* getuserhome HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the best home directory for a given username */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETUSERHOME_INCLUDE
#define	GETUSERHOME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	getuserhome(char *,int,cchar *) noex ;

EXTERNC_end


#endif /* GETUSERHOME_INCLUDE */


