/* ucfiledesc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* additional UNIX® support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Names:
	uc_seeko
	uc_fcntl

  	Description:
	Additional (or supplemental) support for UNIX® limits.

*******************************************************************************/

#ifndef	UCFILEDESC_INCLUDE
#define	UCFILEDESC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* types? */
#include	<fcntl.h>		/* types? */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	uc_seeko(int,off_t,int,off_t *) noex ;
extern int	uc_fcntl(int,int,...) noex ;

EXTERNC_end


#endif /* UCFILEDESC_INCLUDE */


