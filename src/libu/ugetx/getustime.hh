/* getustime HEADER */
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

#ifndef	GETUSTIME_INCLUDE
#define	GETUSTIME_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#ifndef	OBJECT_GETUSTIME
#define	OBJECT_GETUSTIME

namespace libu {
    struct ustimemgr {
	operator time_t () noex {
	    return time(nullptr) ;
	} ;
	static time_t operator () () noex {
	    return time(nullptr) ;
	} ;
    } ; /* end struct (ustimemgr) */
} /* end namespace (libu) */

extern libu::ustimemgr	getustime ;

#endif /* OBJECT_GETUSTIME */


#endif /* __cplusplus */
#endif /* GETUSTIME_INCLUDE */


