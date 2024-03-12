/* mkfnamesuf SUPPORT */
/* lang=C++20 */

/* make a file name from parts (one base and some suffixes) */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-12-03, David A­D­ Morano
	This was made specifically for the HDB UUCP modified code.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkfnamesuf

	Description:
	This subroutine constructs a filename (a single filename
	component) out of several possible strings.  The first
	string is a base name, the other possible strings are suffix
	components.

	Synopsis:
	int mkfnamesuf(char *ofname,int n,cc *p1,...) noex

	Arguments:
	ofname		buffer for the resulting filename
	n		number of suffixes
	p1		base file-name component
	...		one or more possible strings

	Returns:
	>=0		OK, length of resulting filename
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstdarg>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"mkfnamesuf.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int mkfnamesuf1(char *ofname,cc *p1,cc *s1) noex {
	return mkfnamesufx(ofname,1,p1,s1) ;
}
/* end subroutine (mkfnamesuf1) */

int mkfnamesuf2(char *ofname,cc *p1,cc *s1,cc *s2) noex {
	return mkfnamesufx(ofname,2,p1,s1,s2) ;
}
/* end subroutine (mkfnamesuf2) */

int mkfnamesuf3(char *ofname,cc *p1,cc *s1,cc *s2,cc *s3) noex {
	return mkfnamesufx(ofname,3,p1,s1,s2,s3) ;
}
/* end subroutine (mkfnamesuf3) */

int mkfnamesuf4(char *ofname,cc *p1,cc *s1,cc *s2,cc *s3,cc *s4) noex {
	return mkfnamesufx(ofname,4,p1,s1,s2,s3,s4) ;
}
/* end subroutine (mkfnamesuf4) */

int mkfnamesuf5(char *ofname,cc *p1,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5) noex {
	return mkfnamesufx(ofname,5,p1,s1,s2,s3,s4,s5) ;
}
/* end subroutine (mkfnamesuf5) */

int mkfnamesufx(char *rbuf,int n,cc *p1,...) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = maxpathlen) >= 0) {
	    cint	rlen = rs ;
	    if (rs >= 0) {
	        rs = storebuf_strw(rbuf,rlen,rl,p1,-1) ;
	        rl += rs ;
	    }
	    if ((rs >= 0) && (n > 0)) {
	        rs = storebuf_char(rbuf,rlen,rl,'.') ;
	        rl += rs ;
	    }
	    if (rs >= 0) {
	        va_list		ap ;
	        va_begin(ap,p1) ;
	        for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	            cc		*sp = (cc *) va_arg(ap,cc *) ;
		    if (sp) {
	                rs = storebuf_strw(rbuf,rlen,rl,sp,-1) ;
	                rl += rs ;
		    }
	        } /* end for */
	        va_end(ap) ;
	    } /* end if (ok) */
	} /* end if (maxpathlen) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkfnamesufx) */


