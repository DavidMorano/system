/* stox SUPPORT (String-To-Integer) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* C++ |string| to integer conversion */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	stox

	Description:
	These subroutines convert the C++ |string| type to integers
	(of various sizes).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<stdexcept>		/* C++STD */
#include	<string>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"stox.hh"


/* local defines */


/* imported namespaces */

using std::string ;			/* type */
using std::exception ;			/* type */
using std::invalid_argument ;		/* type */
using std::domain_error ;		/* type */
using std::out_of_range ;		/* type */
using std::bad_alloc ;			/* type */
using std::system_error ;		/* type */


/* local typedefs */

using ext_inval		= std::invalid_argument ;	/* INVALID */
using ext_nosup		= std::domain_error ;		/* NOSUP */
using ext_range		= std::out_of_range ;		/* RANGE */
using ext_nomem		= std::bad_alloc ;		/* NOMEM */
using ext_nosys		= std::system_error ;		/* NOSYS */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local std::exception *findexc(errno_t) noex ;


/* local variables */

cchar			str_inval[]	= "inbalid-argument" ;
cchar			str_nosup[]	= "domain-error" ;
cchar			str_range[]	= "out-of-range" ;

static ext_inval	ex_inv(str_inval) ;
static ext_nosup	ex_nos(str_nosup) ;
static ext_range	ex_ran(str_range) ;
static ext_nomem	ex_mem ;	/* string argument not allowed */
static ext_nosys	ex_sys ;	/* string argument not allowed */


/* exported variables */


/* exported subroutines */

sint		stosi	(con string &s,size_t *rpos,int b) {
    	return std::stoi(s,rpos,b) ;
} /* end subroutine (stosi) */

slong		stosl	(con string &s,size_t *rpos,int b) {
    	return std::stol(s,rpos,b) ;
} /* end subroutine (stosl) */

slonglong	stosll	(con string &s,size_t *rpos,int b) {
	std::exception	*exp = nullptr ;
    	slonglong	res = 0 ;
    	ccharp		str = s.c_str() ;
	char		*endp{} ;
	errno = 0 ;
	res = strtoxsll(str,&endp,b) ;
	if (rpos) *rpos = size_t(endp - str) ;
	if (errno_t ec = errno) {
	    exp = findexc(ec) ;
	} /* end if */
	if (exp) {
	    throw *exp ;
	} /* end */
	return res ;
} /* end subroutine (stosll) */

uint		stoui	(con string &s,size_t *rpos,int b) {
	std::exception	*exp = nullptr ;
    	uint		res = 0 ;
    	ccharp		str = s.c_str() ;
	char		*endp{} ;
	errno = 0 ;
	res = strtoxui(str,&endp,b) ;
	if (rpos) *rpos = size_t(endp - str) ;
	if (errno_t ec = errno) {
	    exp = findexc(ec) ;
	} /* end if */
	if (exp) {
	    throw *exp ;
	} /* end */
	return res ;
} /* end subroutine (stoui) */

ulong		stoul	(con string &s,size_t *rpos,int b) {
    	return std::stoul(s,rpos,b) ;
} /* end subroutine (stoul) */

ulonglong	stoull	(con string &s,size_t *rpos,int b) {
	std::exception	*exp = nullptr ;
    	ulonglong	res = 0 ;
    	ccharp		str = s.c_str() ;
	char		*endp{} ;
	errno = 0 ;
	res = strtoxull(str,&endp,b) ;
	if (rpos) *rpos = size_t(endp - str) ;
	if (errno_t ec = errno) {
	    exp = findexc(ec) ;
	} /* end if */
	if (exp) {
	    throw *exp ;
	} /* end */
	return res ;
} /* end subroutine (stoull) */


/* local subroutines */

local std::exception *findexc(errno_t ec) noex {
	std::exception	*exp = nullptr ;
	switch (ec) {
	case EINVAL:
	    exp = &ex_inv ;
	    break ;
	case ENOTSUP:
	    exp = &ex_nos ;
	    break ;
	case ERANGE:
	    exp = &ex_ran ;
	    break ;
	default:
	    exp = &ex_sys ;
	    break ;
	} /* end switch */
	return exp ;
} /* end subroutine (findexc) */


