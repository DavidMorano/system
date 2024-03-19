/* rmdirs HEADER */
/* lang=C20 */

/* make a series of possibly hierarchical directories */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RMDIRS_INCLUDE
#define	RMDIRS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int rmdirs(cchar *) noex ;
extern int removes(cchar *) noex ;
extern int rmdirfiles(cchar *,cchar *,int) noex ;

EXTERNC_end


#endif /* RMDIRS_INCLUDE */


