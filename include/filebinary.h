/* filebinary SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* determine if the named file is a binary file */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	FILEBINARY_INCLUDE
#define	FILEBINARY_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	filebinary(cchar *) noex ;

EXTERNC_end


#endif /* FILEBINARY_INCLUDE */


