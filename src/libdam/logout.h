/* logout HEADER (Log Out) */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* log out of the system (out of the UTMP database) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LOGOUT_INCLUDE
#define	LOGOUT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® |pid_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int logout(pid_t) noex ;

EXTERNC_end


#endif /* LOGOUT_INCLUDE */


