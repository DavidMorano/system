/* ucmallox INCLUDE */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-85, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	The following subroutines are made available:

	uc_mallocsys
	uc_mallocbuf
	uc_mallocstrw

*******************************************************************************/

#ifndef	UCMALLOX_INCLUDE
#define	UCMALLOX_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

int uc_mallocsys(int,char **) noex ;
int uc_mallocbuf(cvoid *,int,cvoid **) noex ;
int uc_mallocstrw(cchar *,int,cchar **) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCMALLOX_INCLUDE */


