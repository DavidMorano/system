/* strnul INCLUDE */
/* lang=C++20 */

/* assert a NUL-terminated string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNUL_INCLUDE
#define	STRNUL_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string>
#include	<string_view>
#include	<iostream>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	STRNUL_SHORTLEN	128		/* "short-string optimization" */


struct strnul {
	typedef std::string_view	strview ;
	typedef const char	*charp ;
	cchar		*rp = nullptr ;
	cchar		*sp = nullptr ;
	char		*as = nullptr ;
	int		sl = 0 ;
	char		buf[STRNUL_SHORTLEN+1] ;
	strnul(cchar *ap,int al = -1) noex : sp(ap), sl(al) { 
	    buf[0] = '\0' ;
	} ; /* end ctor */
	strnul(strview &sv) noex {
	    buf[0] = '\0' ;
	    sp = sv.data() ;
	    sl = sv.length() ;
	} ; /* end ctor */
	strnul() noex : strnul(nullptr,0) { } ;
	strnul(const strnul &) = delete ;
	strnul &operator = (const strnul &) = delete ;
	charp operator () (cchar *ap,int al) noex {
	    rp = nullptr ;
	    if (as) {
		delete [] as ;
		as = nullptr ;
	    }
	    sp = ap ;
	    sl = al ;
	    return operator charp () ;
	} ; /* end method */
	charp operator () (strview &sv) noex {
	    cchar	*ap = sv.data() ;
	    cint	al = sv.length() ;
	    return operator () (ap,al) ;
	} ; /* end method */
	operator charp () noex ;
	~strnul() noex {
	    if (as) {
		delete [] as ;
		as = nullptr ;
	    }
	} ; /* end dtor */
} ; /* end struct (strnul) */


#endif /* __cplusplus */
#endif /* STRNUL_INCLUDE */


