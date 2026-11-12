/* stringto SUPPORT (String-To) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert a C++ string of digits to an integer (of various sizes) */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

  	Description:
	These subroutines convert a C++ string of digits to an
	integer (of various sizes).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<stdexcept>		/* C++STD */
#include	<string>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<stox.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"stringto.hh"

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


using std::string ;			/* type */

template<typename T>
using stox = T (*)(con string &,size_t *,int) ;

template<typename T>
int stringtox(stox<T> cvt,con string &s,int b,T *rp) noex {
    	int		rs = SR_OK ;
	try {
	    T res = cvt(s,nullptr,b) ;
	    *rp = res ;
	} catch (std::invalid_argument &) {
	    rs = SR_INVALID ;
	} catch (std::domain_error &) {
	    rs = SR_NOTSUP ;
	} catch (std::out_of_range &) {
	    rs = SR_RANGE ;
	} catch (...) {
	    rs = SR_BADE ;		/* <- "invalid exchange" */
	} /* end try-catch */
	return rs ;
} /* end subroutine (stringtox) */


/* exported variables */


/* exported subroutines */

int stringtosi	(con string &s,int b,sint *rp)		noex {
    	return stringtox(stosi,s,b,rp) ;
} /* end subroutine (stringtosi) */

int stringtosl	(con string &s,int b,slong *rp)		noex {
    	return stringtox(stosl,s,b,rp) ;
} /* end subroutine (stringtosl) */

int stringtosll	(con string &s,int b,slonglong *rp)	noex {
    	return stringtox(stosll,s,b,rp) ;
} /* end subroutine (stringtosll) */

int stringtoui	(con string &s,int b,uint *rp)		noex {
    	return stringtox(stoui,s,b,rp) ;
} /* end subroutine (stringtoui) */

int stringtoul	(con string &s,int b,ulong *rp)		noex {
    	return stringtox(stoul,s,b,rp) ;
} /* end subroutine (stringtoul) */

int stringtoull	(con string &s,int b,ulonglong *rp)	noex {
    	return stringtox(stoull,s,b,rp) ;
} /* end subroutine (stringtoull) */


