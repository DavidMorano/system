/* nisdomainname SUPPORT */
/* lang=C++20 */

/* get the NIS domain name for the current host */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	nisdomainname

	Description:
	Get the NIS domain name for the local host.

	Synopsis:
	int nisdomainname(char *dbuf,int dlen) noex

	Arguments:
	dbuf		result buffer pointer
	dlen		retsul buffer length

	Returns:
	>=0		length of NIS domain name
	<0		error (system-return)

	Implementation:

	We try to find the NIS domain in the following order:
	1. envionment variable 'NISDOMAIN'
	2. from the kernel (using |ugetnisdom(2usys)|)
	3. from reading the file '/etc/defaultdomain'

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<usystem.h>
#include	<filereadln.h>
#include	<sncpyx.h>
#include	<hasx.h>		/* |hascontent(3uc)| */
#include	<isnot.h>
#include	<localmisc.h>

#include	"nisdomainname.h"


/* local defines */

#define	VARNISDOMAIN	"NISDOMAIN"

#define	NISDOMAINNAME	"/etc/defaultdomain"


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct nisfind {
	char		*rbuf  ;
	int		rlen ;
	nisfind(char *b,int l) noex : rbuf(b), rlen(l) { } ;
	operator int () noex ;
	int tryenv() noex ;
	int tryget() noex ;
	int tryfile() noex ;
    } ; /* end struct (nisfind) */
}

typedef int (nisfind::*nisfind_m)() noex ;


/* forward references */

static int	nisfile(char *,int,cchar *) noex ;


/* local variables */

constexpr nisfind_m	mems[] = {
	&nisfind::tryenv,
	&nisfind::tryget,
	&nisfind::tryfile
} ;

constexpr cchar		vn[] = VARNISDOMAIN ;


/* exported variables */


/* exported subroutines */

int nisdomainname(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (rlen >= 0) {
	 	nisfind		no(rbuf,rlen) ;
		rs = no ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (nisdomainname) */


/* local subroutines */

nisfind::operator int () noex {
	int		rs = SR_OK ;
	for (auto m : mems) {
	    rs = (this->*m)() ;
	    if (rs != 0) break ;
	} /* end for */
	return rs ;
}
/* end method (nisfind::operator) */

int nisfind::tryenv() noex {
	static cchar	*val = getenv(vn) ;
	int		rs = SR_OK ;
	if (val && val[0]) {
	    rs = sncpy1(rbuf,rlen,val) ;
	} /* end block (environment variable) */
	return rs ;
}
/* end method (nisfind::tryenv) */

int nisfind::tryget() noex {
	return ugetnisdom(rbuf,rlen) ;
}
/* end method (nisfind::tryget) */

int nisfind::tryfile() noex {
	return nisfile(rbuf,rlen,NISDOMAINNAME) ;
}
/* end method (nisfind::tryget) */


/* local subroutines */

static int nisfile(char *rbuf,int rlen,cchar *fname) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = filereadln(fname,rbuf,rlen)) >= 0) {
	    len = rs ;
	} else if (isNotAccess(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (nisfile) */


