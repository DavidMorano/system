/* ucdetach HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* additional UNIX® support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Names:
	uc_detach

  	Description:
	Detach an attached pipe from a file-system file.

*******************************************************************************/

#ifndef	UCDETACH_INCLUDE
#define	UCDETACH_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	uc_detach(cchar *) noex ;

EXTERNC_end


#endif /* UCDETACH_INCLUDE */


