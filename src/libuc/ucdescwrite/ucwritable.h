/* ucwritable HEADER */
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
	uc_writable
	uc_writablewait

  	Description:
	Additional (or supplemental) support for UNIX® limits.

	Synopsis:

	Arguments:
	sfd		srouce file-descriptor
	dfd		destination file-descriptor
	sfname		source file-name
	dfname		destination file-name
	len		number of byte to copy

*******************************************************************************/

#ifndef	UCWRITABLE_INCLUDE
#define	UCWRITABLE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	uc_writedesc(int,int,int) noex ;

EXTERNC_end


#endif /* UCWRITABLE_INCLUDE */


