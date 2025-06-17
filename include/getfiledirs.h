/* getfiledirs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find all directories that contain a specified file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETFILEDIRS_INCLUDE
#define	GETFILEDIRS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>


EXTERNC_begin

extern int	getfiledirs(cc *,cc *,cc *,vecstr *) noex ;

EXTERNC_end


#endif /* GETFILEDIRS_INCLUDE */


