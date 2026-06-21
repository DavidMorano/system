/* prognamevar HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* assert a NUL-terminated string given a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PROGNAMEVAR_INCLUDE
#define	PROGNAMEVAR_INCLUDE
#ifdef	__cplusplus /* C++ only! */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<string_view>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#define	PROGNAMEVAR_SHORTLEN	63	/* "short-string optimization" */


struct prognamevar {
	typedef std::string_view	strview ;
	cchar		*rp = nullptr ;
	cchar		*sp = nullptr ;
	char		*as = nullptr ;	/* allocated memory */
	int		sl = 0 ;
	char		buf[PROGNAMEVAR_SHORTLEN + 1] ;
	prognamevar(int argc,con mainv argv,con mainv envv = nullptr) noex ;
	prognamevar(cchar *ap,int al = -1) noex ;
	prognamevar(strview &sv) noex ;
	prognamevar() noex : prognamevar(nullptr,0) { } ;
	prognamevar(con prognamevar &) = delete ;
	prognamevar &operator = (con prognamevar &) = delete ;
	prognamevar &operator = (con strview &sv) noex ;
	ccharp operator () (cchar *ap,int al = -1) noex ;
	ccharp operator () (con strview &sv) noex ;
	operator ccharp () noex ;
	void dtor() noex ;
	destruct prognamevar() {
	    if (as) dtor() ;
	} ; /* end dtor */
    private:
	bool proc(cchar *,int = -1) noex ;
	bool procenv(con mainv = nullptr) noex ;
} ; /* end struct (prognamevar) */


#endif /* __cplusplus (C++ only) */
#endif /* PROGNAMEVAR_INCLUDE */


