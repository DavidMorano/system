/* progchecklock HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* check if we still own the lock file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PROGCHECKLOCK_INCLUDE
#define	PROGCHECKLOCK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<proginfo.h>


EXTERNC_begin

extern int progchecklock(proginfo *,bfile *,
		cc *,cc *,cc *,pid_t) noex ;

EXTERNC_end


#endif /* PROGCHECKLOCK_INCLUDE */


