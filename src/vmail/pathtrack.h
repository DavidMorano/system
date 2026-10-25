/* pathtrack HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* path tracking */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PATHTRACK_INCLUDE
#define	PATHTRACK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<dirlist.h>		/* LIBUC */
#include	<hdbstr.h>		/* LIBUC */


#define	PATHTRACK		pathtrack_head
#define	PATHTRACK_MAGIC		0x86529874


struct pathtrack_head {
	DIRLIST		epath ;
	hdbstr		map ;
	uint		magval ;
} ; /* end struct */

typedef	PATHTRACK	pathtrack ;

EXTERNC_begin

extern int pathtrack_start	(pathtrack *) noex ;
extern int pathtrack_finish	(pathtrack *) noex ;
extern int pathtrack_count	(pathtrack *) noex ;
extern int pathtrack_add	(pathtrack *,pid_t) noex ;
extern int pathtrack_poll	(pathtrack *) noex ;

EXTERNC_end


#endif /* PATHTRACK_INCLUDE */



