/* mesg HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* manage terminal messages */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

#ifndef	MESG_INCLUDE
#define	MESG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® |mode_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	mesg(int) noex ;

EXTERNC_end


#endif /* MESG_INCLUDE */


