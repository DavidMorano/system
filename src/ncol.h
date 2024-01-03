/* ncol HEADER */
/* lang=C20 */

/* calculate column values */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	NCOL_INCLUDE
#define	NCOL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int ncolchar(int,int,int) noex ;
extern int ncolstr(int,int,cchar *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* NCOL_INCLUDE */


