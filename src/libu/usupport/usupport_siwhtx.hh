/* usupport_siwhtx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character or white-space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSIWHTX_INCLUDE
#define	USUPPORTSIWHTX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


namespace libu {
    extern int	siwhtchr	(cchar *,int,int) noex ;
    extern int	siwhtbrk	(cchar *,int,cchar *) noex ;
} /* end namespace (libu) */


#endif /* USUPPORTSIWHTX_INCLUDE */


