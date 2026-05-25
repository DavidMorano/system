/* utmptypes HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

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
    static cint	empty ;
    static cint	runlevel ;
    static cint	boottime ;
    static cint	timeold ;
    static cint	timenew ;
    static cint	procinit ;
    static cint	proclogin ;
    static cint	procuser ;
    static cint	procdead ;
    static cint	account ;
    static cint	signature ;
    static cint	timeshut ;
} ; /* end struct (utmptypes) */

extern const utmptypes	utmptype ;


#endif /* __cplusplus (C++ only) */
#endif /* UTMPTYPES_INCLUDE */


