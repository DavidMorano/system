/* uclibmem HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* memory allocation facility (for library use) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCLIBMEM_INCLUDE
#define	UCLIBMEM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int uc_libmemallocstrw(cchar *,int,cchar **) noex ;
extern int uc_libmemallocitem(cvoid *,int,voidpp) noex ;
extern int uc_libmemalloc(int,void *) noex ;
extern int uc_libmemvalloc(int,void *) noex ;
extern int uc_libmemcalloc(int,int,void *) noex ;
extern int uc_libmemrealloc(void *,int,void *) noex ;
extern int uc_libmemfree(void *) noex ;
extern int uc_libmemrsfree(int,void *) noex ;

EXTERNC_end

#ifdef	__cplusplus

namespace libuc {
    struct libmems {
	int mallocstrw(cchar *,int,cchar **) noex ;
	int mallocitem(cvoid *,int,voidpp) noex ;
	int malloc(int,void *) noex ;
	int valloc(int,void *) noex ;
	int calloc(int,int,void *) noex ;
	int ralloc(void *,int,void *) noex ;
	int free(void *) noex ;
	int rsfree(int,void *) noex ;
    } ; /* end struct (libmems) */
    extern libmems	libmem ;
} /* end namespace (libuc) */

#endif /* __cplusplus */


#endif /* UCLIBMEM_INCLUDE */


