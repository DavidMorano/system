/* umask HEADER */
/* lang=C20 */

/* UNIX® information (a cache for 'uname(2)' and sisters) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UINFO_INCLUDE
#define	UINFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int umask_init() noex ;
extern int umask_fini() noex ;

extern int umaskset(mode_t) noex ;
extern int umaskget() noex ;

static inline int getmask() noex {
	return umaskget() ;
}
static inline int setumask(mode_t m) noex {
	return umaskset(m) ;
}

#ifdef	__cplusplus
}
#endif


#endif /* UINFO_INCLUDE */


