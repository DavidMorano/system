/* procxpath HEADER (Log Out) */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* Shared-Memory-Allocation management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PROCXPATH_INCLUDE
#define	PROCXPATH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® |pid_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */


EXTERNC_begin

extern int procxpath(vecstr *,cchar *) noex ;

EXTERNC_end


#endif /* PROCXPATH_INCLUDE */


