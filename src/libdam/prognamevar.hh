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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<string_view>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


#define	PROGNAMEVAR_SHORTLEN	63	/* "short-string optimization" */


struct prognamevar {
	typedef std::string_view	strview ;
	cchar		*rp = nullptr ;
	cchar		*sp = nullptr ;
	char		*as = nullptr ;	/* allocated memory */
	int		sl = 0 ;
	char		buf[PROGNAMEVAR_SHORTLEN + 1] ;
	prognamevar(int argc,mainv argv,mainv envv = nullptr) noex {
	    buf[0] = '\0' ;
	    if ((argc > 0) && argv[0]) {
	        if (proc(argv[0]) == false) {
		    procenv(envv) ;
		}
	    }
	} ; /* end ctor */
	prognamevar(cchar *ap,int al = -1) noex {
	    buf[0] = '\0' ;
	    proc(ap,al) ;
	} ; /* end ctor */
	prognamevar(strview &sv) noex {
	    buf[0] = '\0' ;
	    {
	        cchar *cp = sv.data() ;
	        cint cl = (int) sv.length() ;
	        proc(cp,cl) ;
	    }
	} ; /* end ctor */
	prognamevar() noex : prognamevar(nullptr,0) { } ;
	prognamevar(const prognamevar &) = delete ;
	prognamevar &operator = (const prognamevar &) = delete ;
	prognamevar &operator = (const strview &sv) noex {
	    buf[0] = '\0' ;
	    {
	        cchar *cp = sv.data() ;
	        cint cl = (int) sv.length() ;
	        proc(cp,cl) ;
	    }
	    return *this ;
	} ; /* end operator (assignment from |string_view| */
	ccharp operator () (cchar *ap,int al = -1) noex {
	    rp = nullptr ;
	    if (as) {
		delete [] as ;
		as = nullptr ;
	    }
	    proc(ap,al) ;
	    return operator ccharp () ;
	} ; /* end method */
	ccharp operator () (strview &sv) noex {
	    cchar	*ap = sv.data() ;
	    cint	al = (int) sv.length() ;
	    return operator () (ap,al) ;
	} ; /* end method */
	operator ccharp () noex ;
	destruct prognamevar() {
	    if (as) {
		delete [] as ;
		as = nullptr ;
	    }
	} ; /* end dtor */
    private:
	bool proc(cchar *,int = -1) noex ;
	bool procenv(mainv = nullptr) noex ;
} ; /* end struct (prognamevar) */


#endif /* __cplusplus (C++ only) */
#endif /* PROGNAMEVAR_INCLUDE */


