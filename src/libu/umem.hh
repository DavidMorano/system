/* umem HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* memory allocation facility (for library use) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UMEM_INCLUDE
#define	UMEM_INCLUDE
#ifdef	__cplusplus /* C++ only! */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


namespace libu {
    struct umems {
        int mallocstrw(cchar *,int,cchar **) noex ;
        int malloc(int,void *) noex ;
        int valloc(int,void *) noex ;
        int calloc(int,int,void *) noex ;
        int ralloc(void *,int,void *) noex ;
        int free(void *) noex ;
        int rsfree(int,void *) noex ;
    } ; /* end struct (umems) */
    extern umems	umem ;
} /* end namespace (libu) */


#endif /* __cplusplus (C++ only) */
#endif /* UMEM_INCLUDE */


