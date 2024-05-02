/* usig HEADER */
/* lang=C20 */

/* additional UNIX® limits support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Additional (or supplemental) support for UNIX® limits.

*******************************************************************************/

#ifndef	USIG_INCLUDE
#define	USIG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<signal.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


EXTERNC_begin

extern int u_sigaction(int sn,SIGACTION *nsp,SIGACTION *osp) noex ;
extern int u_sigaltstack(const stack_t *ssp,stack_t *ossp) noex ;
extern int u_sigpending(sigset_t *ssp) noex ;
extern int u_sigprocmask(int how,sigset_t *setp,sigset_t *osetp) noex ;
extern int u_sigsuspend(const sigset_t *ssp) noex ;
extern int u_sigsend(idtype_t idtype,id_t id,int sig) noex ;
extern int u_sigsendset(procset_t *psp,int sig) noex ;
extern int u_sigwait(const sigset_t *ssp,int *rp) noex ;
extern int u_sigmask(int how,sigset_t *setp,sigset_t *osetp) noex ;

EXTERNC_end


#endif /* USIG_INCLUDE */


