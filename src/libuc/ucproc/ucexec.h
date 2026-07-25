/* ucexec HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* execute a process (swap out existing program for another) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCEXEC_INCLUDE
#define	UCEXEC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	uc_execve	(cchar *,con mainv,con mainv) noex ;
extern int	uc_isaexecve	(cchar *,con mainv,con mainv) noex ;

EXTERNC_end


#endif /* UCEXEC_INCLUDE */


