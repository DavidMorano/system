/* ucprochave HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* is a proceess (given by a PID) in the system? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCPROCHAVE_INCLUDE
#define	UCPROCHAVE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	uc_prochave(pid_t) noex ;

local inline int isproc(pid_t pid) noex {
	return uc_prochave(pid) ;
}

EXTERNC_end


#endif /* UCPROCHAVE_INCLUDE */


