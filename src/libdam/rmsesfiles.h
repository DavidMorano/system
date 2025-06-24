/* rmsesfiles HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* remove session files and session directories (as appropriate) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright (c) 1998 David A-D- Morano.  All rights reserved. */

#ifndef	RMSESFILES_INCLUDE
#define	RMSESFILES_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	rmsesfiles(cchar *) noex ;

EXTERNC_end


#endif /* RMSESFILES_INCLUDE */


