/* usupport_geusmtime HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTGETUSMTIME_INCLUDE
#define	USUPPORTGETUSMTIME_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>

#include	<usupport_prime.h>


struct usys_mtime {
	operator mtime_t () noex {
	    return umtime() ;
	} ;
	static mtime_t operator () () noex {
	    return umtime() ;
	} ;
} ; /* end struct (usys_mtime) */

extern usys_mtime	getusmtime ;


#endif /* __cplusplus */
#endif /* USUPPORTGETUSMTIME_INCLUDE */


