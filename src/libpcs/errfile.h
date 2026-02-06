/* errfile HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* debugging interface to the STDERR output stream */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-06-01, David A­D­ Morano
	This module was created so that a file to store debugging
	or error information could be accessed independent from the
	regular I-O of the program.

*/

/* Copyright © 1994 David A­D­ Morano.  All rights reserved. */

#ifndef	ERRFILE_INCLUDE
#define	ERRFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int erropen(cchar *,cchar *) noex ;
extern int errclose() noex ;
extern int errprintf(cchar *,...) noex ;

EXTERNC_end


#endif /* ERRFILE_INCLUDE */


