/* usupport_siext HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* determine if file-name (just a string) has an approved file-extension */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPOETSIEXT_INCLUDE
#define	USUPPOETSIEXT_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


namespace libu {
    extern int	siext(cchar *,int) noex ;
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPOETSIEXT_INCLUDE */


