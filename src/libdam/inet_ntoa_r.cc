/* inet_ntoa_r SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* convert network-format INET address to base-256 d.d.d.d representation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-26, David A­D­ Morano
        I wrote this to get a reentrant subroutine for this function. Maybe
        someday POSIX© will make this subroutine instead but until, you can use
        this! It's dirty, but POSIX© should have already done it. What planet do
        those POSIX© guys live on?

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	inet_ntoa_r

	Description:
	This is a reentrant version of the subroutine 'inet_ntoa(3xnet)'.

	Synopsis:
	char *inet_ntoa_r(INADDR in,char rbuf,int rlen) noex

	Arguments:
	in		INET4 address to convert
	rbuf		buffer to hold result
	rlen		length of buffer to hold result

	Returns:
	-		pointer to NUL-terminated result buffer

	Notes:
	Q. What is a |struct in_addr|?
	A. Good question.  We had to do some thinking (and examination)
	to figure out what that is.  Obviously it holds an INET4
	address.  But what order are the bytes in? Answer: the bytes
	are in order such that if the type was read out as a string,
	the first character of the string would be the high-order
	byte of the corresponding INET4 address.  We use this exact
	strategy below to convert a |struct in_addr| to a string
	representation.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<uinet.h>
#include	<sbuf.h>
#include	<mkchar.h>
#include	<localmisc.h>


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
	if (rbuf) {
	    sbuf	b ;
	    if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
	        cint	n = INET4ADDRLEN ;
	        cchar	*ap = charp(&in) ;
	        for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
		    cint	v = mkchar(*ap++) ;
		    if (i > 0) sbuf_chr(&b,'.') ;
		    rs = sbuf_deci(&b,v) ;
	        } /* end for */
	        rs1 = sbuf_finish(&b) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return (rs >= 0) ? rbuf : nullptr ;
}
/* end subroutine (inet_ntoa_r) */


