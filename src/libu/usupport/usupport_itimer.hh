/* usupport_itimer HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTITIMER_INCLUDE
#define	USUPPORTITIMER_INCLUDE
#ifdef	__cplusplus /* (C++ only) */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


namespace libu {
    extern int uitimer_get(int,mut ITIMERVAL *) noex ;
    extern int uitimer_set(int,CITIMERVAL *,mut ITIMERVAL * = nullptr) noex ;
} /* end namespace (libu) */


#endif /* __cplusplus (C++ only) */
#endif /* USUPPORTITIMER_INCLUDE */


