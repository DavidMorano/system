/* securefile HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* check if a given file name is SUID and owned by our effective UID */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SECUREFILE_INCLUDE
#define	SECUREFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® IDs */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int securefile(cchar *,uid_t,gid_t) noex ;

EXTERNC_end


#endif /* SECUREFILE_INCLUDE */


