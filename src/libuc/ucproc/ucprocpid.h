/* ucprocpid HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* is a proceess (given by a PID) in the system? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCPROCPID_INCLUDE
#define	UCPROCPID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	uc_procpid(cchar *,uid_t) noex ;

EXTERNC_end


#endif /* UCPROCPID_INCLUDE */


