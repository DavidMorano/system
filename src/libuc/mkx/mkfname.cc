/* mkfname SUPPORT */
/* lang=C++20 */

/* make a file name from several component parts */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-12-03, David A­D­ Morano
	This was made specifically for the HDB UUCP modified code.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkfname

	Description:
	This subroutine constructs a filename (a single filename
	component) out the given source strings.  This subroutine
	it totally just a big concatenation of c-strings but with the
	restriction that the destination buffer is only MAXPATHLEN
	long.

	Synopsis:
	int mkfname(char *ofname,cchar *p1,cchar *p2,cchar *p3) noex

	Arguments:
	ofname		buffer for the resulting filename
	p1		first component
	p2		second component
	p3		third component

	Returns:
	<0		error
	>=0		OK, length of resulting filename

	Notes: 
	1. Note that |mkfname1()| is sematically identical to |mkpath1()|
	but uses different code.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstdarg>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"mkfname.h"


/* local defines */


/* imported namespaces */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported subroutines */


/* exported subroutines */

int mkfname1(char *rbuf,cchar *p1) noex {
	return mkfnamex(rbuf,1,p1) ;
}
/* end subroutine (mkfname1) */

int mkfname2(char *rbuf,cchar *p1,cchar *p2) noex {
	return mkfnamex(rbuf,2,p1,p2) ;
}
/* end subroutine (mkfname2) */

int mkfname3(char *rbuf,cchar *p1,cchar *p2,cchar *p3) noex {
	return mkfnamex(rbuf,3,p1,p2,p3) ;
}
/* end subroutine (mkfname3) */

int mkfname4(char *rbuf,cchar *p1,cchar *p2,cchar *p3,cchar *p4) noex {
	return mkfnamex(rbuf,4,p1,p2,p3,p4) ;
}
/* end subroutine (mkfname4) */

int mkfname5(char *rbuf,cc *p1,cc *p2,cc *p3,cc *p4,cc *p5) noex {
	return mkfnamex(rbuf,5,p1,p2,p3,p4,p5) ;
}
/* end subroutine (mkfname5) */

int mkfname6(char *rbuf,cc *p1,cc *p2,cc *p3,cc *p4,cc *p5,cc *p6) noex {
	return mkfnamex(rbuf,6,p1,p2,p3,p4,p5,p6) ;
}
/* end subroutine (mkfname6) */

int mkfnamex(char *rbuf,int na,...) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = maxpathlen) >= 0) {
	    cint	rlen = rs ;
	    va_list	ap ;
	    va_begin(ap,na) ;
	    for (int i = 0 ; (rs >= 0) && (i < na) ; i += 1) {
	        cc	*sp = (cc *) va_arg(ap,cc *) ;
		if (sp) {
	            rs = storebuf_strw(rbuf,rlen,rl,sp,-1) ;
	            rl += rs ;
		}
	    } /* end for */
	    va_end(ap) ;
	} /* end if (maxpathlen) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkfnamex) */


