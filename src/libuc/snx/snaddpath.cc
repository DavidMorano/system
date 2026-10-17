/* snaddpath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* append a slash character ('/') to a given counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snaddpath

	Description:
	These subroutines are used to concatenate a path (as a
	c-string) or a counted path (as a counted c-string) to an
	existing counted path (as a counted c-string).

	Synopsis:
	int snaddpath(char *rbuf,int rlen,int rl,cchar *sp,int sl) noex
	int snaddpath(char *rbuf,int rlen,int rl,cchar *sp) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	rl		result buffer index
	sp		source c-string pointer
	sl		source c-string length

	Returns:
	>=0		the amount of additional string added to
			the existing c-string buffer
	<0		error (system-return)

	See-also:
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"snaddpath.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct adder {
	char	*rbuf ;
	int	rlen ;
	int	rl ;
	adder(charp ªrb,int ªrl,int ªl) noex {
	    rbuf	= ªrb ;
	    rlen	= ªrl ;
	    rl		= ªl ;
	} ; /* end ctor */
	int operator ()	(cchar *,int) noex ;
	int aslash	() noex ;
	int astr	(cchar *,int) noex ;
    } ; /* end struct (adder) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snaddpath(char *pbuf,int plen,int pl,cchar *sp) noex {
	return snaddpath(pbuf,plen,pl,sp,-1) ;
} /* end subroutine (snaddpath) */

int snaddpath(char *pbuf,int plen,int pl,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (pbuf && sp) ylikely {
	    rs = SR_INVALID ;
	    if ((plen >= 0) && (pl >= 0)) ylikely {
		adder ao(pbuf,plen,pl) ;
		rs = ao(sp,sl) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (snaddpath) */


/* local subroutines */

int adder::operator () (cchar *sp,int sl) noex {
    	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = aslash()) >= 0) {
	    len += rs ;
	    rs = astr(sp,sl) ;
	    len += rs ;
	    rbuf[rl] = eos ;
	} /* end if (aslash) */
	return (rs >= 0) ? len : rs ;
} /* end method (adder::operator) */

int adder::aslash() noex {
	int		rs = SR_OK ;
    	int		len = 0 ;
	if ((rl > 0) && (rbuf[rl - 1] != '/')) {
	    rs = SR_OVERFLOW ;
	    if (rl < rlen) {
		rs = SR_OK ;
	        rbuf[rl++] = '/' ;
	        len += 1 ;
	    } /* end if (good) */
	} /* end if (needed) */
	return (rs >= 0) ? len : rs ;
} /* end method (adder::operator) */

int adder::astr(cchar *sp,int sl) noex {
	int		rs = SR_OVERFLOW ;
	ccharp		ep = (rbuf + rlen) ;
	charp		bp = (rbuf + rl) ;
	if (sl >= 0) {
	    while ((bp < ep) && sl && *sp) {
		*bp++ = *sp++ ;
		sl -= 1 ;
	    } /* end while */
	    if ((sl == 0) || (*sp == eos)) {
		rs = intconv(bp - (rbuf + rl)) ;
		rl += rs ;
	    } /* end if (good) */
	} else {
	    while ((bp < ep) && *sp) {
		*bp++ = *sp++ ;
	    } /* end while */
	    if (*sp == eos) {
		rs = intconv(bp - (rbuf + rl)) ;
		rl += rs ;
	    } /* end if (good) */
	} /* end if */
	return rs ;
} /* end method (adder::operator) */


