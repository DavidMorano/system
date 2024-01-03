/* malloclog HEADER */
/* lang=C20 */

/* malloc logging */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MALLOCLOG_INCLUDE
#define	MALLOCLOG_INCLUDE


#include	<envstandards.h>
#include	<usystem.h>
#include	<localinfo.h>


#ifndef	TYPEDEF_CVOID
#define	TYPEDEF_CVOID
typedef const xvoid	cvoid ;
#endif


#if	(! defined(MALLOCLOG_MASTER)) || (MALLOCLOG_MASTER == 0)

#ifdef	__cplusplus
extern "C" {
#endif

extern void	malloclog_alloc(cvoid *,int,const char *) noex ;
extern void	malloclog_free(cvoid *,const char *) noex ;
extern void	malloclog_realloc(cvoid *,cvoid *,int,const char *) noex ;
extern void	malloclog_mark() noex ;
extern void	malloclog_dump() noex ;
extern void	malloclog_clear() noex ;
extern int	malloclog_printf(const char [],...) noex ;

#ifdef	__cplusplus
}
#endif

#endif /* MALLOCLOG_MASTER */


#endif /* MALLOCLOG_INCLUDE */


