/* readignore HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* read data while ignoring it */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	READIGNORE_INCLUDE
#define	READIGNORE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® |off_t| */
#include	<sys/param.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int readignore(int,int) noex ;

EXTERNC_end


#endif /* READIGNORE_INCLUDE */


