/* argutils MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Argument-Vector utilities */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

	= 2020-04-23, David A­D­ Morano
	I converted this code to be a C++ module.


*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	argsz
    	argloader

	Description:
	These are utility subroutines for managing argument-vector
	objects.

	Synopsis:
	pair<int,int> argsz(cchar *sp,int chx) noex

	Arguments:
	sp		pointer to data containing the argument-string
	chx		field sepærator chacter

	Returns:
	-		C++ |pair| containing: 
				na	- number of arguments
				sz	- size of congolmerate

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<utility>		/* C++STD |pair(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBUC */
#include	<libdebug.h>		/* LIBU LIBDEBUG */

#pragma		GCC dependency		"mod/libutil.ccm"

module argutils ;

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif


/* imported namespaces */

using std::pair ;			/* type */
using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

#ifdef	COMMENT
namespace {
    struct argsizer {
	int	na ;
	int	sz ;
	argsizer(cchar *sp,int chx) noex {
    	    cnullptr	np{} ;
	    for (cchar *tp ; (tp = strchr(sp,chx)) != np ; ) {
	        na += 1 ;
	        sz += intconv((tp - sp) + 1) ;
	        sp = (tp + 1) ;
	    } /* end for */
	    if (sp[0] != '\0') {
	        na += 1 ;
	        sz += (lenstr(sp) + 1) ;
	    } /* end if */
	    sz += ((na + 1) * szof(cchar **)) ;
	} ; /* end ctor */
    } ; /* end struct (argsizer) */
} /* end namespace */
#endif /* COMMENT */

argloader::argloader(char *a,cchar *sp,int na,int chx) noex {
	    cnullptr	np{} ;
	    cint	avsz = (na + 1) * szof(cchar **) ;
	    char	*bp = a ;
	    int		c = 1 ;
	    av = ccharpp(a) ;
	    bp += avsz ;
	    for (cchar *tp ; (tp = strchr(sp,chx)) != np ; ) {
		cint tl = intconv(tp - sp) ;
	        av[c++] = bp ;
		bp = (strwcpy(bp,sp,tl) + 1) ;
		sp = (tp + 1) ;
	    } /* end for */
	    if (sp[0] != '\0') {
	        av[c++] = sp ;
	        bp = (strwcpy(bp,sp,-1) + 1) ;
	    } /* end if */
	    av[c] = nullptr ;
} /* end ctor (argloader) */

pair<int,int> argsz(cchar *sp,int chx) noex {
    	cnullptr	np{} ;
    	pair<int,int>	rv{} ;
	int	sz = 0 ; /* return-value */
	int	na = 0 ;
	for (cchar *tp ; (tp = strchr(sp,chx)) != np ; ) {
	    na += 1 ;
	    sz += intconv((tp - sp) + 1) ;
	    sp = (tp + 1) ;
	} /* end for */
	if (sp[0] != '\0') {
	    na += 1 ;
	    sz += (lenstr(sp) + 1) ;
	} /* end if */
	rv.first = na ;
	rv.second = ((na + 1) * szof(cchar **)) ;
	return rv ;
} /* end subroutine (argsz) */


