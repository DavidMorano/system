/* msghdr HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* control-message-header */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-08-01, Dave morano
	File was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	MSGHDR_INCLUDE
#define	MSGHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/socket.h>		/* POSIX® |CONMSGHDR| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	msghdr_sz(MSGHDR *) noex ;

EXTERNC_end


#endif /* MSGHDR_INCLUDE */


