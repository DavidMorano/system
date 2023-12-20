/* strnul INCLUDE */
/* lang=C20 */

/* assert a NUL-terminated string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNUL_INCLUDE
#define	STRNUL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	STRNUL_SHORTLEN	128		/* "short-string optimization" */


struct strnul {
	typedef const char	*charp ;
	cchar		*sp = nullptr ;
	char		*as = nullptr ;
	int		sl = 0 ;
	char		buf[STRNUL_SHORTLEN+1] ;
	strnul(cchar *ap,int al = -1) noex : sp(ap), sl(al) { 
	    buf[0] = '\0' ;
	} ; /* end ctor */
	strnul() noex : strnul(nullptr,0) { } ;
	strnul(const strnul &) = delete ;
	strnul &operator = (const strnul &) = delete ;
	void operator () (cchar *ap,int al) noex {
	    if (as) {
		delete [] as ;
		as = nullptr ;
	    }
	    sp = ap ;
	    sl = al ;
	} ; /* end method */
	operator charp () noex ;
	~strnul() noex {
	    if (as) {
		delete [] as ;
		as = nullptr ;
	    }
	} ; /* end dtor */
} ; /* end struct (strnul) */


#endif /* STRNUL_INCLUDE */


