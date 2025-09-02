/* ucsigx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	UCSIGX_INCLUDE
#define	UCSIGX_INCLUDE

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>


EXTERNC_begin

extern int uc_sigdefault(int) noex ;
extern int uc_sigignore(int) noex ;
extern int uc_sighold(int) noex ;
extern int uc_sigrelease(int) noex ;
extern int uc_sigpause(int) noex ;
extern int uc_sigqueue(pid_t,int,CSIGVAL) noex ;
extern int uc_sigtimedwait(const sigset_t *,siginfo_t *,CTIMESPEC *) noex ;
extern int uc_sigwaitinfoto(const sigset_t *,siginfo_t *,CTIMESPEC *) noex ;
extern int uc_sigwaitinfo(const sigset_t *,siginfo_t *) noex ;

EXTERNC_end


#endif /* UCSIGX_INCLUDE */


