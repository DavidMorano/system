/* inet_ntoa_r SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert network-format INET address to base-256 d.d.d.d representation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-26, David A­D­ Morano
	I wrote this to get a reentrant subroutine for this function.
	Maybe someday POSIX© will make this subroutine instead but
	until, you can use this!  It is dirty, but POSIX© should have
	already done it. What planet do those POSIX© guys live on?

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	inet_ntoa_r

	Description:
	This is a reentrant version of the subroutine |inet_ntoa(3xnet)|.

	Synopsis:
	char *inet_ntoa_r(INADDR in,char rbuf,int rlen) noex

	Arguments:
	in		INET4 address to convert
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	-		pointer to NUL-terminated result buffer

	Notes:
	Q. What is a |struct in_addr|?
	A. Good question.  We had to do some thinking (and examination)
	to figure out what that is.  Obviously it holds an INET4
	address.  But what order are the bytes in?  Answer: the bytes
	are in an order such that if the type was read out as a string,
	the first character of the string would be the high-order
	byte of the corresponding INET4 address.  We use this exact
	strategy below to convert a |struct in_addr| to a string
	representation.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<string>		/* C++STD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<uinet.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"inet_ntoa_r.h"

/* local defines */


/* imported namespaces */

using std::string ;			/* type */
using std::to_string ;			/* subroutine */
using libu::strnwcpy ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */

cint		ndots	= 3 ;		/* number of dots in result */
cint		nzeros	= 4 ;		/* number of zeros in result */


/* exported variables */


/* exported subroutines */

char *inet_ntoa_r(INADDR in,char *rbuf,int rlen) noex {
	char		*rp = nullptr ;
	if (rbuf) ylikely {
	    rbuf[0] = '0' ;
	    if (rlen >= (ndots + nzeros)) {
	        cint	n = INET4ADDRLEN ;
	        string	b ; 
	        cchar	*ap = charp(&in) ;
		bool	fbad = false ;
	        for (int i = 0 ; i < n ; i += 1) {
		    cint v = mkchar(*ap++) ;
		    try {
		        if (i > 0) b += '.' ;
		        b += to_string(v) ;
		    } catch (...) {
			fbad = true ;
			break ;
		    }
	        } /* end for */
	        if (! fbad) {
	            if (cint cl = intconv(b.size()) > 0) {
		        cchar *cp = b.c_str() ;
		        rp = strnwcpy(rbuf,rlen,cp,cl) ;
	            }
	        } /* end if (! fbad) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rp ;
} /* end subroutine (inet_ntoa_r) */


