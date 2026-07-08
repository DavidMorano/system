/* offindex HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* offset-index object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OFFINDEX_INCLUDE
#define	OFFINDEX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* POSIX |off_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecobj.h>		/* LIBUC */


#define	OFFINDEX	struct offindex_head
#define	OFFINDEX_FL	struct offindex_flags
#define	OFFINDEX_MAGIC	0x89795142


struct offindex_flags {
	uint		setsorted:1 ;
} ; /* end struct */

struct offindex_head {
	vecobj		*oip ;
	OFFINDEX_FL	fl ;
	uint		magval ;
} ; /* end struct */

typedef	OFFINDEX	offindex ;
typedef	OFFINDEX_FL	offindex_fl ;

EXTERNC_begin

extern int	offindex_start	(offindex *,int) noex ;
extern int	offindex_add	(offindex *,off_t,int) noex ;
extern int	offindex_lookup	(offindex *,off_t) noex ;
extern int	offindex_finish	(offindex *) noex ;

EXTERNC_end


#endif /* OFFINDEX_INCLUDE */


