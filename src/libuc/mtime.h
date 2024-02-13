/* mtime HEADER */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MTIME_INCLUDE
#define	MTIME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<stdint.h>
#include	<utypedefs.h>
#include	<clanguage.h>


typedef int64_t		mtime_t ;

EXTERNC_begin

extern mtime_t	mtime(void) ;

EXTERNC_end


#endif /* MTIME_INCLUDE */


