/* emaentry_getbestaddr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* walk a directory tree */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	EMAENTRYGETBESTADDR_INCLUDE
#define	EMAENTRYGETBESTADDR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ema.h>			/* LIBUC */


EXTERNC_begin

extern int emaentry_getbestaddr(ema_ent *,cchar **) noex ;

EXTERNC_end


#endif /* EMAENTRYGETBESTADDR_INCLUDE */


