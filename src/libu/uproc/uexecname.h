/* uexecname HEADER */
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
	u_execve

	Description:
	All of the UNIX® system calls that use (operate on) a
	UNIX® process.

*******************************************************************************/

#ifndef	UEXECNAME_INCLUDE
#define	UEXECNAME_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int u_execname(char *,int) noex ;

EXTERNC_end


#endif /* UEXECNAME_INCLUDE */


