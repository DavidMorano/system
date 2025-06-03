/* umemalloc HEADER */
/* encoding=ISO8859-1 */
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
    extern int umallocstrw(cchar *,int,cchar **) noex ;
    extern int umalloc(int,void *) noex ;
    extern int uvalloc(int,void *) noex ;
    extern int ucalloc(int,int,void *) noex ;
    extern int urealloc(void *,int,void *) noex ;
    extern int ufree(void *) noex ;
    extern int rsufree(int,void *) noex ;
}


#endif /* __cplusplus (C++ only) */
#endif /* UMEMALLOC_INCLUDE */


