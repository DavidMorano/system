/* findxfile HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* find an eXecutable file given a name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	FINDXFILE_INCLUDE
#define	FINDXFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ids.h>


EXTERNC_begin

extern int	findxfile(ids *,char *,cchar *) noex ;

EXTERNC_end


#endif /* FINDXFILE_INCLUDE */


