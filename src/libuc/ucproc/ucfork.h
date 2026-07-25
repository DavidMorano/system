/* ucfork HEADER (LIBUC Terminal-Control) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* LIBUC Teerminal-Control functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	ucfork

	Names:
	ucfork_init
	ucfork_fini
	uc_fork
	uc_forklockbegin
	uc_forklockend

	Description:
	UNIX
	RO operations realted to |fork(2)|.

  	Synopsis:
	int ucfork_init() noex
	int ucfork_fini() noex
	int uc_fork() noex
	int uc_forklockbegin(int to) noex
	int uc_forklockend() noex

	Arguments:
	to		time-out

	Returns:
	>0		OK (or child PID)
	==0		OK and we are the parent
	<0		no child and error (system-return)

*******************************************************************************/

#ifndef	UCFORK_INCLUDE
#define	UCFORK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int ucfork_init() noex ;
extern int ucfork_fini() noex ;
extern int uc_fork() noex ;
extern int uc_forklockbegin(int) noex ;
extern int uc_forklockend() noex ;

EXTERNC_end


#endif /* UCFORK_INCLUDE */


