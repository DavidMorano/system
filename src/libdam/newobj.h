/* newobj HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* new-object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	NEWOBJ_INCLUDE
#define	NEWOBJ_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


/* object defines */
#define	newobj(otype,n)		(otype*) newobjsub((n),szof(otype))

EXTERNC_begin

extern void	*newobjsub(int,int) noex ;

EXTERNC_end


#endif /* NEWOBJ_INCLUDE */


