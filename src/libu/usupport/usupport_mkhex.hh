/* usupport_mkhex HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* make a string of hexadecimal digits */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTMKHEX_INCLUDE
#define	USUPPORTMKHEX_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


namespace libu {
    extern int mkhexstr	(char *,int,cvoid *,int = -1) noex ;
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTMKHEX_INCLUDE */


