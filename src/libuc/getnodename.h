/* getnodename HEADER */
/* lang=C20 */

/* get program root directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETNODENAME_INCLUDE
#define	GETNODENAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int getnodename(char *,int) noex ;
extern int getnodedomain(char *,char *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GETNODENAME_INCLUDE */


