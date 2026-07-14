/* lastlogin HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* retrieve the time of the last login given specified information */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-01-22, David A­D­ Morano
	This subroutine module was adopted for use from some previous
	code that performed the similar sorts of functions.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

#ifndef	LASTLOGIN_INCLUDE
#define	LASTLOGIN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<time.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int lastlogin(cc *,uid_t,time_t *,char *,char *) noex ;

EXTERNC_end


#endif /* LASTLOGIN_INCLUDE */


