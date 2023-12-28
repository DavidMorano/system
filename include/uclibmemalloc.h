/* uclibmemalloc INCLUDE */
/* lang=C20 */

/* memory allocation facility (for library use) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCLIBMEMALLOC_INCLUDE
#define	UCLIBMEMALLOC_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

int uc_libmallocstrw(cchar *,int,cchar **) noex ;
int uc_libmallocsys(int,char **) noex ;
int uc_libmalloc(int,void *) noex ;
int uc_libvalloc(int,void *) noex ;
int uc_libcalloc(int,int,void *) noex ;
int uc_librealloc(const void *,int,void *) noex ;
int uc_libfree(cvoid *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCLIBMEMALLOC_INCLUDE */


