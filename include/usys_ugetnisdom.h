/* usys_ugetnisdom HEADER */
/* lang=C20 */

/* operating support for retrieving the NIS domain-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This provides a means to get the NIS domain-name from the
	operating system in an OS-independent way by regualr callers.

	Name:
	ugetnisdom

	Synosis:
	int ugetnisdom(char *rbuf,int rlen) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>=0		length of result
	<0		error code (- errno)

*******************************************************************************/

#ifndef	USYSUGETNISDOM_INCLUDE
#define	USYSUGETNISDOM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>


EXTERNC_begin

extern sysret_t ugetnisdom(char *,int) noex ;

EXTERNC_end


#endif /* USYSUGETNISDOM_INCLUDE */


