/* utmpsizes HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UTMP entry (field buffer) sizes */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-13, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	utmpsizes

	Description:
	This struct (type) provides UTMP entry types (by name).

*******************************************************************************/

#ifndef	UTMPSIZES_INCLUDE
#define	UTMPSIZES_INCLUDE
#ifdef	__cplusplus	/* C++ only! */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


struct utmpsizes {
    static cint	id ;
    static cint	user ;
    static cint	line ;
    static cint	host ;
} ; /* end struct (utmpsizes) */

extern const utmpsizes	utmpsize ;


#endif /* __cplusplus (C++ only) */
#endif /* UTMPSIZES_INCLUDE */


