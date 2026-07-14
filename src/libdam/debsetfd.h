/* debsetfd HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* set debug file-descriptor */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DEBSETFD_INCLUDE
#define	DEBSETFD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® IDs */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int int debsetfd(int) noex ;

EXTERNC_end


#endif /* DEBSETFD_INCLUDE */


