/* listenusd HEADER */
/* lang=C20 */

/* Listen-on-UNIX-Socket-Datagram */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-03-04, David A.D. Morano
	This was written from scratch.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	listenusd

	Description:
	Listen on a UNIX® datagram socket.

	Synopsis:

	Arguments:

	Returns:
	-		result

*******************************************************************************/

#ifndef	LISTENUSD_INCLUDE
#define	LISTENUSD_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* system types |mode_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>


EXTERNC_begin

extern int	listenusd(cchar *,mode_t,int) noex ;

EXTERNC_end


#endif /* LISTENUSD_INCLUDE */


