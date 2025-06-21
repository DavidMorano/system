/* umemalloc HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* memory allocation facility (for library use) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UMEMALLOC_INCLUDE
#define	UMEMALLOC_INCLUDE
#ifdef	__cplusplus /* C++ only! */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


namespace libu {
    extern int umemallocstrw(cchar *,int,cchar **) noex ;
    extern int umemalloc(int,void *) noex ;
    extern int umemvalloc(int,void *) noex ;
    extern int umemcalloc(int,int,void *) noex ;
    extern int umemrealloc(void *,int,void *) noex ;
    extern int umemfree(void *) noex ;
    extern int umemrsfree(int,void *) noex ;
}


#endif /* __cplusplus (C++ only) */
#endif /* UMEMALLOC_INCLUDE */


