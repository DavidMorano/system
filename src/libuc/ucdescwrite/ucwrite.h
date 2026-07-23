/* ucwrite HEADER */
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
	uc_writedesc
	uc_write
	uc_writefile

  	Description:
	Additional (or supplemental) support for UNIX® limits.

	Synopsis:
	extern int	uc_writedesc(int,int,int) noex
	extern int	uc_write(int,cvoid *,int) noex
	extern int	uc_writefile(int,cchar *) noex

	Arguments:
	sfd		srouce file-descriptor
	dfd		destination file-descriptor
	sfname		source file-name
	dfname		destination file-name
	len		number of byte to copy

*******************************************************************************/

#ifndef	UCWRITE_INCLUDE
#define	UCWRITE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD |nullptr_t| */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */

EXTERNC_begin

extern int	uc_writeto	(int,cvoid *,int,int) noex ;
extern int	uc_write	(int,cvoid *,int) noex ;
extern int	uc_writen	(int,cvoid *,int) noex ;

EXTERNC_end


#endif /* UCWRITE_INCLUDE */


