/* fileliner HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* calculate the number of lines in a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FILELINER_INCLUDE
#define	FILELINER_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	fileliner(cchar *,int) noex ;

EXTERNC_end


#endif /* FILELINER_INCLUDE */


