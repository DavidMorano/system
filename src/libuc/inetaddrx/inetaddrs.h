/* inetaddrs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* object to manipulate INET adresses */
/* version %I% last-modified %G% */

/* revision history:

	= 1998-02-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	INETADDRS_INCLUDE
#define	INETADDRS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<netinet/in.h>		/* POSIX */
#include	<netdb.h>		/* POSIX */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


enum inetaddrs {
    	inetaddr_bin,
    	inetaddr_str,
    	inetaddr_dot,
    	inetaddr_overlast
} ; /* end enum (inetaddrs) */


#endif /* INETADDRS_INCLUDE */


