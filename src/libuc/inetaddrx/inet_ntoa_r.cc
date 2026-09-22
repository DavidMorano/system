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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uinet.h>		/* LIBU */
#include	<sbuf.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"inet_ntoa_r.h"

/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *inet_ntoa_r(INADDR in,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rbuf) ylikely {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) ylikely {
	        cint	n = INET4ADDRLEN ;
	        cchar	*ap = charp(&in) ;
	        for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
		    cint	v = mkchar(*ap++) ;
		    if (i > 0) b.chr('.') ;
		    rs = b.deci(v) ;
	        } /* end for */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return (rs >= 0) ? rbuf : nullptr ;
} /* end subroutine (inet_ntoa_r) */


