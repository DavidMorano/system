/* conmsghdr HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* control-message-header */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-08-01, Dave morano
	File was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	CONMSGHDR_INCLUDE
#define	CONMSGHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/socket.h>		/* |CONMSGHDR| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#ifndef	CONMSGHDR_TYPEDEF
#define	CONMSGHDR_TYPEDEF

typedef CONMSGHDR	conmsghdr ;

#endif /* CONMSGHDR_TYPEDEF */

EXTERNC_begin

extern int	conmsghdr_passed(CONMSGHDR *) noex ;

EXTERNC_end


#endif /* CONMSGHDR_INCLUDE */


