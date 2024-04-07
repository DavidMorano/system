/* uopen HEADER */
/* lang=C20 */

/* additional UNIX® limits support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Additional (or supplemental) support for UNIX® limits.

*******************************************************************************/

#ifndef	UOPEN_INCLUDE
#define	UOPEN_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


/* extra system flags for 'openXXX(2)' and friends */
#ifndef	OM_SPECIAL
#define	OM_SPECIAL	0xFF000000
#endif

#ifndef	O_SETSID
#define	O_SETSID	(1<<24)
#endif
#ifndef	O_CLOEXEC
#define	O_CLOEXEC	(1<<25)
#endif
#ifndef	O_NETWORK
#define	O_NETWORK	(1<<26)
#endif
#ifndef	O_MINMODE
#define	O_MINMODE	(1<<27)
#endif

/* extra system flags for |uc_lockfile(3uc)| */
#ifndef	F_UNLOCK
#define	F_UNLOCK	F_ULOCK
#endif
#ifndef	F_WLOCK	
#define	F_WLOCK		F_LOCK
#endif
#ifndef	F_TWLOCK
#define	F_TWLOCK	F_TLOCK
#endif
#ifndef	F_WTEST
#define	F_WTEST		F_TEST
#endif
#ifndef	F_RLOCK
#define	F_RLOCK		10		/* new! (watch UNIX® for changes) */
#endif
#ifndef	F_TRLOCK
#define	F_TRLOCK	11		/* new! (watch UNIX® for changes) */
#endif
#ifndef	F_RTEST
#define	F_RTEST		12		/* new! (watch UNIX® for changes) */
#endif


EXTERNC_begin

extern int u_open(cchar *,int,mode_t) noex ;
extern int u_openat(int,cchar *,int,mode_t) noex ;
extern int u_socket(int,int,int) noex ;
extern int u_socketpair(int,int,int,int *) noex ;
extern int u_pipe(int *) noex ;

EXTERNC_end


#endif /* UOPEN_INCLUDE */


