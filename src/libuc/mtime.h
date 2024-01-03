/* mtime HEADER */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MTIME_INCLUDE
#define	MTIME_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<stdint.h>

typedef int64_t		mtime_t ;

#ifdef	__cplusplus
extern "C" {
#endif

extern mtime_t	mtime(void) ;

#ifdef	__cplusplus
}
#endif


#endif /* MTIME_INCLUDE */


