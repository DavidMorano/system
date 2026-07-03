/* usupport_strncpyxc HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSTRNCPYXC_INCLUDE
#define	USUPPORTSTRNCPYXC_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


namespace libu {
    extern char	* strncpybc (char *,cchar *,int) noex ;
    extern char	* strncpylc (char *,cchar *,int) noex ;
    extern char	* strncpyuc (char *,cchar *,int) noex ;
    extern char	* strncpyfc (char *,cchar *,int) noex ;
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTSTRNCPYXC_INCLUDE */


