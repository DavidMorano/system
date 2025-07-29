/* mkfname SUPPORT */
/* charset=ISO8859-1 */
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
	component) with the given source strings.  This subroutine
	it totally just a big concatenation of c-strings but with the
	restriction that the destination buffer is only MAXPATHLEN
	long.  So this is very similar to |sncpyx(3uc)| with the
	length of the destination string given as MAXPATHLEN.

	Synopsis:
	int mkfname(char *ofname,cchar *p1,cchar *p2,cchar *p3) noex

	Arguments:
	ofname		buffer for the resulting filename
	p1		first component
	p2		second component
	p3		third component

	Returns:
	>=0		OK, length of resulting filename
	<0		error (system-return)

	Notes: 
	1. Note that |mkfname1()| is sematically identical to |mkpath1()|
	but uses different code.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>		/* |va_list(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"mkfname.h"


/* local defines */


/* imported namespaces */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


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
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf) ylikely {
	    if ((rs = maxpathlen) >= 0) ylikely {
	        storebuf	sb(rbuf,rs) ;
	        va_begin(ap,na) ;
	        for (int i = 0 ; (rs >= 0) && (i < na) ; i += 1) {
	            cc	*sp = (cc *) va_arg(ap,cc *) ;
		    if (sp) {
	                rs = sb.str(sp) ;
		    }
	        } /* end for */
	        rl = sb.idx ;
	        va_end(ap) ;
	    } /* end if (maxpathlen) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkfnamex) */


