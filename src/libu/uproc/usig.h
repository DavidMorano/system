/* usig HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® process signal mangement */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	usig

	Description:
	This module provides UNIX® process signal mangement.

*******************************************************************************/

#ifndef	USIG_INCLUDE
#define	USIG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int u_alarm		(int) noex ;
extern int u_kill		(pid_t,int) noex ;
extern int u_killpg		(pid_t,int) noex ;
extern int u_raise		(int) noex ;
extern int u_sigaction		(int,con SIGACTION *,mut SIGACTION *) noex ;
extern int u_sigaltstack	(const stack_t *,stack_t *) noex ;
extern int u_sigpending		(mut sigset_t *) noex ;
extern int u_sigprocmask	(int,con sigset_t *,sigset_t *) noex ;
extern int u_sigsuspend		(con sigset_t *) noex ;
extern int u_sigsend		(idtype_t,id_t,int) noex ;
extern int u_sigsendset		(procset_t *,int) noex ;
extern int u_sigmask		(int,con sigset_t *,mut sigset_t *) noex ;
extern int u_sigwait		(con sigset_t *,mut int *) noex ;

EXTERNC_end

#ifdef	__cplusplus
inline int u_sigaction		(int sig,con SIGACTION *sap) noex {
    	return u_sigaction(sig,sap,nullptr) ;
} /* end subroutine */
inline int u_sigwait		(con sigset_t *ssp) noex {
    	return u_sigwait(ssp,nullptr) ;
} /* end subroutine */
#endif /* __cplusplus */


#endif /* USIG_INCLUDE */


