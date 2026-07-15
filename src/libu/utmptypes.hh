/* utmptypes HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UTMP entry types */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-13, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	utmptypes

	Description:
	This struct (type) provides UTMP entry types (by name).

*******************************************************************************/

#ifndef	UTMPTYPES_INCLUDE
#define	UTMPTYPES_INCLUDE
#ifdef	__cplusplus	/* C++ only! */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


struct utmptypes {
    static cshort	empty ;
    static cshort	runlevel ;
    static cshort	boottime ;
    static cshort	timeold ;
    static cshort	timenew ;
    static cshort	procinit ;
    static cshort	proclogin ;
    static cshort	procuser ;
    static cshort	procdead ;
    static cshort	account ;
    static cshort	signature ;
    static cshort	timeshut ;
} ; /* end struct (utmptypes) */

extern const utmptypes	utmptype ;


#endif /* __cplusplus (C++ only) */
#endif /* UTMPTYPES_INCLUDE */


