/* shellunder HEADER */
/* lang=C20 */

/* parse a c-string into the shell-under information components */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SHELLUNDER_INCLUDE
#define	SHELLUNDER_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#define	SHELLUNDER	struct shellunder


struct shellunder {
	cchar		*progename ;	/* child program exec-name */
	pid_t		pid ;		/* parent (shell) PID */
} ;

EXTERNC_begin

extern int	shellunder(SHELLUNDER *,cchar *) noex ;

EXTERNC_end


#endif /* SHELLUNDER_INCLUDE */


