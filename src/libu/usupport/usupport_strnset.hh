/* usupport_strnset HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* set a string to a number of characters (exactly - not-NUL-terminated) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSTRNSET_INCLUDE
#define	USUPPORTSTRNSET_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


namespace libu {
    extern char	*strnset(char *dp,int ch,int n) noex ;
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTSTRNSET_INCLUDE */


