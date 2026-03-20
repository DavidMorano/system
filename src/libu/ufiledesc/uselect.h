/* uselect HEADER (UNIX® file operations) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	uselect

	Description:
	All of the UNIX® system calls that use (operate on) a
	file-descroptor.

*******************************************************************************/

#ifndef	USELECT_INCLUDE
#define	USELECT_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/select.h>
#include	<clanguage.h>
#include	<usysbase.h>

EXTERNC_begin

extern int u_select	(int,fd_set *,fd_set *,fd_set *) noex ;

EXTERNC_end


#endif /* USELECT_INCLUDE */


