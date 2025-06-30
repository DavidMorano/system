/* ucmallocx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-85, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	ucmallocx

	The following subroutines are made available:
	uc_mallocsys
	uc_mallocbuf
	uc_mallocstrw

*******************************************************************************/

#ifndef	UCMALLOCX_INCLUDE
#define	UCMALLOCX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int uc_mallocsys(int,char **) noex ;
extern int uc_mallocbuf(cvoid *,int,cvoid **) noex ;
extern int uc_mallocstrw(cchar *,int,cchar **) noex ;

static inline int uc_mallocstr(cchar *sp,cchar **rpp) noex {
    	return uc_mallocstrw(sp,-1,rpp) ;
}

EXTERNC_end


#endif /* UCMALLOCX_INCLUDE */


