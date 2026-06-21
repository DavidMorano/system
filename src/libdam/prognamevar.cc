/* prognamevar SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* assert a NUL-terminated string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	prognamevar

	Descriptor:
	This object module (prognamevar) provides support for creating
	NUL-terminated strings when only a counted string is
	available.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |strwcpy(3u)| */
#include	<ulogerror.h>		/* LIBU */
#include	<shellunder.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"prognamevar.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"

import libutil ;			/* |lenstr(3u)| */
import ureserve ;			/* |sfbasename(3u)| */

/* local defines */


/* imported namespaces */

using libu::sirchr ;			/* subroutine */
using libu::strwcpy ;			/* subroutine */


/* local typedefs */

typedef const mainv	cmv ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

prognamevar::prognamevar(int argc,cmv argv,cmv envv) noex {
	buf[0] = '\0' ;
	if ((argc > 0) && argv[0]) {
	    if (proc(argv[0]) == false) {
		procenv(envv) ;
	    }
	} else {
	    procenv(envv) ;
	}
} /* end ctor (prognamevar) */

prognamevar::prognamevar(cchar *ap,int al) noex {
	buf[0] = '\0' ;
	proc(ap,al) ;
} /* end ctor (prognamevar) */

prognamevar::prognamevar(strview &sv) noex {
	buf[0] = '\0' ;
	{
	    cchar *cp = sv.data() ;
	    cint cl = (int) sv.length() ;
	    proc(cp,cl) ;
	}
} /* end ctor (prognamevar) */

prognamevar &prognamevar::operator = (const strview &sv) noex {
	buf[0] = '\0' ;
	{
	    cchar *cp = sv.data() ;
	    cint cl = (int) sv.length() ;
	    proc(cp,cl) ;
	}
	return *this ;
} /* end method (prognamevar::operator) */

ccharp prognamevar::operator () (cchar *ap,int al) noex {
	rp = nullptr ;
	if (as) {
	    delete [] as ;
	    as = nullptr ;
	} /* end if (memory-release) */
	proc(ap,al) ;
	return operator ccharp () ;
} /* end method (prognamevar::operator) */

ccharp prognamevar::operator () (const strview &sv) noex {
	cchar	*ap = sv.data() ;
	cint	al = (int) sv.length() ;
	return operator () (ap,al) ;
} /* end method (prognamevar::operator) */

prognamevar::operator ccharp () noex {
    	cnullptr	np{} ;
	cnothrow	nt{} ;
	if (rp == nullptr) {
	    rp = sp ;
	    if ((sl >= 0) && (sp[sl] != '\0')) {
	        if (sl > STRNUL_SHORTLEN) {
		    if ((as = new(nt) char[sl + 1]) != np) {
			strwcpy(as,sp,sl) ;
			rp = as ;
		    } else { /* (memory-acquire) */
			cint rsnomem = SR_NOMEM ;
			ulogerror("prognamevar",rsnomem,"mem-alloc failure") ;
			rp = "«mem-alloc-failure»" ;
		    }
	        } else {
		    strwcpy(buf,sp,sl) ;
		    rp = buf ;
	        }
	    } /* end if (possibly required) */
	} /* end if (need calculation) */
	return rp ;
} /* end method (prognamevar::operator) */

void prognamevar::dtor() noex {
	if (as) {
	    delete [] as ;
	    as = nullptr ;
	} /* end if (memory-release) */
} /* end method (prognamevar::dtor) */

bool prognamevar::proc(cchar *pp,int pl) noex {
    	bool		f = false ;
        if ((sl = sfbasename(pp,pl,&sp)) > 0) {
            if (cint si = sirchr(sp,sl,'.') ; si > 0) {
	        sl = si ;
		f = true ;
	    } else if (si < 0) {
		sl = pl ;
		f = true ;
	    }
        } /* end if (sfbasename) */
	return f ;
} /* end method (prognamevar::proc) */

bool prognamevar::procenv(con mainv envv) noex {
    	bool		f = false ;
	sp = buf ;
	if (envv) {
	    if (cchar *valp = getourenv(envv,"_") ; valp) {
		int rs{} ;
		if (shellunder_dat d ; (rs = shellunder_load(&d,valp)) >= 0) {
		    f = proc(d.execname) ;
		} else if (rs < 0) {
		    ulogerror("prognamevar",rs,"shellunder") ;
		}
	    } /* end if (getourenv) */
	} /* end if (non-null) */
	return f ;
} /* end method (prognamevar::procenv) */


