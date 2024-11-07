/* ncol HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* calculate column values */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	NCOL_INCLUDE
#define	NCOL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int ncolchar(int,int,int) noex ;
extern int ncolstr(int,int,cchar *,int) noex ;
extern int getcols(int,int,int,cchar *,int) noex ;

static inline int charcols(int ntab,int ccol,int ch) noex {
	return ncolchar(ntab,ccol,ch) ;
}

EXTERNC_end


#endif /* NCOL_INCLUDE */


