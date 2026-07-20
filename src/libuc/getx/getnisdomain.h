/* getnisdomain HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* retrieve the system NIS domain-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

#ifndef	GETNISDOMAIN_INCLUDE
#define	GETNISDOMAIN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	getnisdomain(char *,int) noex ;

EXTERNC_end


#endif /* GETNISDOMAIN_INCLUDE */


