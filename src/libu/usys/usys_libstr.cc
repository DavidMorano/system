/* usys_libstr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>		/* |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>

#include	"usys_libstr.h"

/* STRNLEN begin */
#if	(!defined(SYSHAS_STRNLEN)) || (SYSHAS_STRNLEN == 0)

#ifndef	SUBROUTINE_STRNLEN
#define	SUBROUTINE_STRNLEN

size_t strnlen(cchar *s,size_t nsz) noex {
	int		rsz = 0 ;
	if (s) {
	    for (rsz = 0 ; (rsz < nsz) && *s ; rsz += 1) {
	        s += 1 ;
	    }
	} /* end if (non-null) */
	return rsz ;
}
/* end subroutine (strnlen) */

#endif /* SUBROUTINE_STRNLEN */

#endif /* (!defined(SYSHAS_STRNLEN)) || (SYSHAS_STRNLEN == 0) */
/* STRNLEN end */

/* STRLCPY begin */
#if	(!defined(SYSHAS_STRLCPY)) || (SYSHAS_STRLCPY == 0)

#ifndef	SUBROUTINE_STRLCPY
#define	SUBROUTINE_STRLCPY

size_t strlcpy(char *dst,cchar *src,size_t msz) noex {
	size_t		rsz = 0 ;
	if (dst && src) {
	    if (msz) {
	        for (rsz = 0 ; (rsz < (msz - 1)) && *src ; msz += 1) {
	            dst[rsz] = *src++ ;
	        }
	    }
	    dst[rsz] = '\0' ;
	    if (*src) rsz += strlen(src) ;
	} /* end if (non-null) */
	return rsz ;
}
/* end subroutine (strlcpy) */

#endif /* SUBROUTINE_STRLCPY */

#endif /* (!defined(SYSHAS_STRLCPY)) || (SYSHAS_STRLCPY == 0) */
/* STRLCPY end */


