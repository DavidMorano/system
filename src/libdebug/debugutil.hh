/* debugutil HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* debug utilities */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DEBUGUTIL_INCLUDE
#define	DEBUGUTIL_INCLUDE
#ifdef	__cplusplus /* (C++ only) */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


namespace libdebug {
    extern int	checkbasebounds(cchar *,int,void *) noex ;
}


#endif /* __cplusplus (C++ only) */
#endif /* DEBUGUTIL_INCLUDE */


