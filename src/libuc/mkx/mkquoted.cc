/* mkquoted SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* quote arguments for safe passage through a SHELL */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

	= 20238-12-08, David A­D­ Morano
	I moved the character terminator block ('qterms' below)
	into file-scope static storage. This way, it only gets
	initialized once rather than each time this subroutine is
	called.  I should have done tbis from the begining.  Duh!

*/

/* Copyright © 1998,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkquoted

	Description:
	This subroutine examines a string supplied by the caller
	and produces an output string with any necessary quotes
	applied such that interpretation by a POSIX-conforming SHELL
	will result in the original string.

	Synopsis:
	int mkquoted(char *qbuf,int qlen,cchar *abuf,int alen) noex

	Arguments:
	qbuf		result buffer pointer
	qlen		result buffer length
	abuf		shell argument to be quoted
	alen		length of shell argument to be quoted

	Returns:
	>=0		length of used destination buffer from conversion
	<0		destination buffer was not big enough or other problem

	Reminder:
	Zero-length arguments are both allowed and common!  So to
	any maintainers out there: do not check for a non-zero
	positive source string to be quoted.  It could be and often
	is, zero-length!  And no, I did not get caught out with
	this.  Rather I am antisipating a problem if I had accidentally
	messed up with this.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sbuf.h>
#include	<snwcpy.h>
#include	<ascii.h>
#include	<six.h>			/* |siterm(3uc)| */
#include	<field.h>
#include	<fieldterms.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"mkquoted.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct quoter {
	char		*qbuf ;
	cchar		*abuf ;
	int		qlen ;
	int		alen ;
	bool		f_white ;
	quoter(char *qp,int ql,cchar *ap,int al) noex {
	    if (al < 0) al = lenstr(ap) ;
	    qbuf = qp ;
	    abuf = ap ;
	    qlen = ql ;
	    alen = al ;
	    f_white = haswhite(ap,al) ;
	} ; /* end ctor */
	operator int () noex ;
	int mkq() noex ;
    } ; /* end struct (quoter) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr int		termsize = ((UCHAR_MAX+1)/CHAR_BIT) ;

constexpr cchar		qchars[] = "\"\\$`" ;

static char		qterms[termsize] ;


/* exported variables */


/* exported subroutines */

int mkquoted(char *qbuf,int qlen,cchar *abuf,int alen) noex {
	int		rs = SR_FAULT ;
	if (qbuf && abuf) ylikely {
	    static cint		srs = fieldterms(qterms,false,qchars) ;
	    if ((rs = srs) >= 0) ylikely {
	        quoter	q(qbuf,qlen,abuf,alen) ;
	        rs = q ;
	    } /* end if (fieldterms) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkquoted) */


/* local subroutines */

quoter::operator int () noex {
	int		rs ;
	if (f_white || (siterm(abuf,alen,qterms) >= 0)) {
	    rs = mkq() ;
	} else {
	    rs = snwcpy(qbuf,qlen,abuf,alen) ;
	}
	return rs ;
}
/* end method (quoter::operator) */

int quoter::mkq() noex {
	int		rs ;
	int		len = 0 ;
	int		al = alen ;
	cchar		*ap = abuf ;
	if (sbuf b ; (rs = b.start(qbuf,qlen)) >= 0) ylikely {
	    if (f_white) {
	        b.chr(CH_DQUOTE) ;
	    }
	    for (int i ; (i = siterm(ap,al,qterms)) >= 0 ; ) {
	        b.strw(ap,i) ;
	        b.chr(CH_BSLASH) ;
	        rs = b.chr(ap[i]) ;
	        ap += (i+1) ;
	        al -= (i+1) ;
	        if (rs < 0) break ;
	    } /* end for */
	    if ((rs >= 0) && (al > 0)) {
	        rs = b.strw(ap,al) ;
	    }
	    if ((rs >= 0) && f_white) {
	        rs = b.chr(CH_DQUOTE) ;
	    }
	    len = b.finish ;
	    if (rs >= 0) rs = len ;
	} /* end if (sbuf) */
	return (rs >= 0) ? len : rs ;
}
/* end method (quoter::mkq) */


