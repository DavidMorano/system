/* getoflags HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get (create) the UNIX® "open" flags from an "open-string" */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETOFLAGS_INCLUDE
#define	GETOFLAGS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<limits.h>		/* |UCHAR_MAX| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<char.h>		/* |CHAR_{xx}(3uc)| */


EXTERNC_begin

extern int	getoflags(cchar *) noex ;

EXTERNC_end


#endif /* GETOFLAGS_INCLUDE */


