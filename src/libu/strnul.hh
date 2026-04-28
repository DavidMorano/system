/* strnul HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* assert a NUL-terminated string given a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNUL_INCLUDE
#define	STRNUL_INCLUDE
#ifdef	__cplusplus /* C++ only! */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<string_view>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


#define	STRNUL_SHORTLEN		127	/* "short-string optimization" */


class strnul {
	typedef std::string_view	strview ;
	cchar		*rp = nullptr ;
	char		*as = nullptr ;	/* allocated memory */
	char		buf[STRNUL_SHORTLEN + 1] ;
	void clear() noex ;
	void alloc(cchar *,int) noex ;
	ccharp proc(cchar *,int = -1) noex ;
	void dtor() noex ;
    public:
	bool		fok = true ;
	strnul(cchar *,int = -1) noex ;
	strnul(const strview &sv) noex ;
	strnul() noex : strnul(nullptr,0) { } ;
	strnul(const strnul &) = delete ;
	strnul &operator = (const strnul &) = delete ;
	strnul &operator = (cchar *) noex ;
	strnul &operator = (const strview &) noex ;
	ccharp operator () (cchar *,int = -1) noex ;
	ccharp operator () (const strview &) noex ;
	operator ccharp () const noex {
	    return rp ;
	} ;
	operator bool () const noex {
	    return fok ;
	} ;
	destruct strnul() {
	    if (as) dtor() ;
	} ; /* end dtor */
} ; /* end struct (strnul) */


#endif /* __cplusplus (C++ only) */
#endif /* STRNUL_INCLUDE */


