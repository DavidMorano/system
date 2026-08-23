/* ucclock HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UCLOCK object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCLOCK_INCLUDE
#define	UCLOCK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<time.h>		/* POSIX® clock defines */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int uc_clockset	(clockid_t,const TIMESPEC *) noex ;
extern int uc_clockres	(clockid_t,TIMESPEC *) noex ;
extern int uc_clockget	(clockid_t,TIMESPEC *) noex ;

local inline int uc_clockreal(TIMESPEC *tsp) noex {
    const clockid_t	cid = CLOCK_REALTIME ;
    return uc_clockget(cid,tsp) ;
} /* end subroutine (uc_clockreal) */

EXTERNC_end


#endif /* UCLOCK_INCLUDE */


