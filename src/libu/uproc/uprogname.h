/* uprogname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	u_progname

	Description:
	Try to retrieve the (so-called) "program" name of the current
	process.  Note that (on-purpose) not every process will
	have this sort of name.  That is because it is derived from
	the zero-th c-string given in the process ARGV c-string
	vector at program invocation.  That c-string is allowed to
	be be missing.  But that would be a rather special and
	unusual case.

*******************************************************************************/

#ifndef	UPROGNAME_INCLUDE
#define	UPROGNAME_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin
extern int u_progname(char *,int) noex ;
EXTERNC_end


#endif /* UPROGNAME_INCLUDE */


