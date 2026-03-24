/* matlocalfs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* determine is the given counted c-string is a local-filesystem name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MATLOCALFS_INCLUDE
#define	MATLOCALFS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern int	matlocalfs(cchar *sp,int sl) noex ;

EXTERNC_end


#endif /* MATLOCALFS_INCLUDE */


