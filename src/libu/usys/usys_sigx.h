/* usys_sigx HEADER */
/* lang=C20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSSIGX_INCLUDE
#define	USYSSIGX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>

/* SIGX begin */
#if	(!defined(SYSHAS_SIGQUEUE)) || (SYSHAS_SIGQUEUE == 0)


#ifndef	SUBROUTINE_SIGQUEUE
#define	SUBROUTINE_SIGQUEUE
EXTERNC_begin

extern int sigqueue(pid_t,int,const SIGVAL) noex ;

EXTERNC_end
#endif /* SUBROUTINE_SIGUEUE */


#ifndef	SUBROUTINE_SIGSEND
#define	SUBROUTINE_SIGSEND
EXTERNC_begin

extern int sigsend(idtype_t,id_t,int) noex ;

EXTERNC_end
#endif /* SUBROUTINE_SIGSEND */

#ifndef	SUBROUTINE_SIGSENDSET
#define	SUBROUTINE_SIGSENDSET
EXTERNC_begin

extern int sigsendset(procset_t *,int) noex ;

EXTERNC_end
#endif /* SUBROUTINE_SIGSENDSET */


#endif /* (!defined(SYSHAS_SIGX)) || (SYSHAS_SIGX == 0) */
/* SIGX end */


#endif /* USYSSIGX_INCLUDE */


