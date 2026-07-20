/* getuserterms HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get a list (returned as vector of c-strings) of the logged in user terms */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-01-10, David A­D­ Morano
	This code was originally written.  It was prompted by
	the failure of other terminal message programs from finding
	the proper controlling terminal.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	GETUSERTERMS_INCLUDE
#define	GETUSERTERMS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC <- results */


EXTERNC_begin

extern int	getuserterms(vecstr *,cchar *) noex ;

EXTERNC_end


#endif /* GETUSERTERMS_INCLUDE */


