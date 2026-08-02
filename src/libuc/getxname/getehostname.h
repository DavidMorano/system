/* gethehostname HEADER (Get-Effective-Hostname) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get a HOSTENT for a given INET name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETEHOSTNAME_INCLUDE
#define	GETEHOSTNAME_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	getehostname(char *,cchar *) noex ;

EXTERNC_end


#endif /* GETEHOSTNAME_INCLUDE */


