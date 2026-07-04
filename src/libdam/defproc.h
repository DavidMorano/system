/* defproc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* process a "def" (define) file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DEFPROC_INCLUDE
#define	DEFPROC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */
#include	<expcook.h>		/* LIBUC */


EXTERNC_begin

extern int defproc(vecstr *,con mainv envv,expcook *,cchar *) noex ;

EXTERNC_end


#endif /* DEFPROC_INCLUDE */


