/* mtime HEADER */
/* lang=C20 */

/* Millisecond Time */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MTIME_INCLUDE
#define	MTIME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<stdint.h>


#ifndef	TYPEDEF_MTIME
#define	TYPEDEF_MTIME
typedef int64_t		mtime_t ;
#endif

EXTERNC_begin

extern mtime_t	mtime(void) noex ;

EXTERNC_end


#endif /* MTIME_INCLUDE */


