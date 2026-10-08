/* usys_libstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	*c-string manipulation subroutines*

  	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

	Notes:
	1. I provide somme subroutines that are not yet in some standard
	C-language libraries on some operating systems.  I provide
	at least (to be standardized, maybe) |strlcpy(3c)| and
	|strnlen(3c)|.  
	2. An additional little (funny) note here: I wrote the
	|strnlen()| subroutine myself long ago.  That is: I invented
	the API of that myself some decades before some standards
	committee decided to made the very same subroutine.  My
	original version took an 'int' as a size, while the
	standardized version is going to take a 'size_t' type as
	the size.
	3. These subroutines below (at least |strlcpy(3c)| and 
	|strnlen(3c)| are also located in other libraries that I have
	written.  Some of these other libraries are 'libac' and 
	'libcx'.  The subroutine implementations might be different
	in each library.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstring>		/* CSTD |strlen(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

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
	    } /* end for */
	} /* end if (non-null) */
	return rsz ;
} /* end subroutine (strnlen) */

#endif /* SUBROUTINE_STRNLEN */

#endif /* (!defined(SYSHAS_STRNLEN)) || (SYSHAS_STRNLEN == 0) */
/* STRNLEN end */

/* STRLCPY begin */
#if	(!defined(SYSHAS_STRLCPY)) || (SYSHAS_STRLCPY == 0)

#ifndef	SUBROUTINE_STRLCPY
#define	SUBROUTINE_STRLCPY

size_t strlcpy(char *dst,cchar *src,size_t msz) noex {
	size_t		rsz = 0 ;
	if (dst && src) ylikely {
	    if (msz) ylikely {
	        for (rsz = 0 ; (rsz < (msz - 1)) && *src ; msz += 1) {
	            dst[rsz] = *src++ ;
	        } /* end for */
	    } /* end */
	    dst[rsz] = '\0' ;
	    if (*src) rsz += strlen(src) ;
	} /* end if (non-null) */
	return rsz ;
} /* end subroutine (strlcpy) */

#endif /* SUBROUTINE_STRLCPY */

#endif /* (!defined(SYSHAS_STRLCPY)) || (SYSHAS_STRLCPY == 0) */
/* STRLCPY end */


