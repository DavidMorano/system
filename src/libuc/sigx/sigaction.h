/* sigaction HEADER */
/* encoding=ISO8859-1 */
/* lang=C99 */

/* signal event object methods */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-04-04, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	sigaction_load

	Description:
	Methods for the signal-event object.

*******************************************************************************/

#ifndef	SIGACTION_INCLUDE
#define	SIGACTION_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<signal.h>
#include	<usystem.h>


#ifndef	TYPEDEF_SIGINFOHAND
#define	TYPEDEF_SIGINFOHAND
EXTERNC_begin
    typedef void (*siginfohand_f)(int,siginfo_t *,void *) noex ;
EXTERNC_end
#endif

EXTERNC_begin

extern int sigaction_load(SIGACTION *,sigset_t *,int,siginfohand_f) noex ;

EXTERNC_end


#endif /* SIGACTION_INCLUDE */


