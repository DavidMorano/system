/* uctruncate HEADER */
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
	uc_truncate

  	Description:
	Additional (or supplemental) support for UNIX® limits.

	Synopsis:
	int uc_truncate		(cchar *fn,off_t len) noex

	Arguments:
	fn		file-name
	len		file offset

*******************************************************************************/

#ifndef	UCTRUNCATE_INCLUDE
#define	UCTRUNCATE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	uc_truncate(cchar *,off_t) noex ;

EXTERNC_end


#endif /* UCTRUNCATE_INCLUDE */


