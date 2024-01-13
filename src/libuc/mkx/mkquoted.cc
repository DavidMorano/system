/* mkquoted SUPPORT */
/* lang=C++20 */

/* quote arguments for safe passage through a SHELL */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

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

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>		/* <- for |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<sbuf.h>
#include	<baops.h>
#include	<snwcpy.h>
#include	<ascii.h>
#include	<hasx.h>
#include	<six.h>
#include	<field.h>
#include	<localmisc.h>

#include	"mkquoted.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */

namespace {
    struct quoter {
	char		*qbuf ;
	cchar		*abuf ;
	int		qlen ;
	int		alen ;
	bool		f_white ;
	quoter(char *qp,int ql,cchar *ap,int al) noex {
	    if (al < 0) al = strlen(ap) ;
	    qbuf = qp ;
	    abuf = ap ;
	    qlen = ql ;
	    alen = al ;
	    f_white = haswhite(ap,al) ;
	} ; /* end ctor */
	operator int () noex ;
	int mkq() noex ;
    } ; /* end struct (quoter) */
}


/* forward references */


/* local variables */

constexpr int		termsize = ((UCHAR_MAX+1)/CHAR_BIT) ;

static constexpr cchar	qchars[] = "\"\\$`" ;

static char		qterms[termsize] ;


/* exported subroutines */

int mkquoted(char *qbuf,int qlen,cchar *abuf,int alen) noex {
	int		rs = SR_FAULT ;
	if (qbuf && abuf) {
	    static int	srs = fieldterms(qterms,false,qchars) ;
	    if ((rs = srs) >= 0) {
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
	sbuf		b ;
	int		rs ;
	int		len = 0 ;
	int		al = alen ;
	cchar		*ap = abuf ;
	if ((rs = sbuf_start(&b,qbuf,qlen)) >= 0) {
	    int		i ;
	    if (f_white) {
	        sbuf_char(&b,CH_DQUOTE) ;
	    }
	    while ((i = siterm(ap,al,qterms)) >= 0) {
	        sbuf_strw(&b,ap,i) ;
	        sbuf_char(&b,CH_BSLASH) ;
	        rs = sbuf_char(&b,ap[i]) ;
	        ap += (i+1) ;
	        al -= (i+1) ;
	        if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && (al > 0)) {
	        rs = sbuf_strw(&b,ap,al) ;
	    }
	    if ((rs >= 0) && f_white) {
	        rs = sbuf_char(&b,CH_DQUOTE) ;
	    }
	    len = sbuf_finish(&b) ;
	    if (rs >= 0) rs = len ;
	} /* end if (sbuf) */
	return (rs >= 0) ? len : rs ;
}
/* end method (quoter::mkq) */


