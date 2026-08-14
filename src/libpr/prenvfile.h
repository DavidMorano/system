/* prenvfile HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* object to handle parameter files */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-05, David A­D­ Morano
	This code was started for Levo related work.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PRENVFILE_INCLUDE
#define	PRENVFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */


EXTERNC_begin

extern int	prenvfile(cchar *,vecstr *,cchar *) noex ;

EXTERNC_end


#endif /* PRENVFILE_INCLUDE */


