/* getnisdomain SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the NIS domain name for the current host */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getnisdomain

	Description:
	Get the NIS domain name for the local host.

	Synopsis:
	int getnisdomain(char *dbuf,int dlen) noex

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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucgetx.h>		/* LIBUC |uc_getnisdomain(3uc)| */
#include	<filereadln.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getnisdomain.h"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;			/* |varname(3u)| */

/* local defines */

#define	NISFN		"/etc/defaultdomain"


/* imported namespaces */

using libu::ugetnisdom ;		/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct nisfind {
	char		*rbuf ;
	int		rlen ;
	nisfind(char *b,int l) noex : rbuf(b), rlen(l) { } ;
	operator int () noex ;
	int tryenv() noex ;
	int tryget() noex ;
	int tryfile() noex ;
    } ; /* end struct (nisfind) */
} /* end namespace */

typedef int (nisfind::*nisfind_m)() noex ;


/* forward references */

local int	nisfile(char *,int,cchar *) noex ;


/* local variables */

constexpr nisfind_m	tries[] = {
	&nisfind::tryenv,
	&nisfind::tryget,
	&nisfind::tryfile
} ; /* end array (tries) */


/* exported variables */


/* exported subroutines */

int getnisdomain(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (rlen >= 0) ylikely {
	 	nisfind	 nis(rbuf,rlen) ;
		rs = nis ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (getnisdomain) */


/* local subroutines */

nisfind::operator int () noex {
	int		rs = SR_OK ;
	for (cauto &m : tries) {
	    rs = (this->*m)() ;
	    if (rs) break ;
	} /* end for */
	return rs ;
} /* end method (nisfind::operator) */

int nisfind::tryenv() noex {
	static cchar	*val = getenver(varname.nisdomain) ;
	int		rs = SR_OK ;
	if (val && val[0]) {
	    rs = sncpy(rbuf,rlen,val) ;
	} /* end block (environment variable) */
	return rs ;
} /* end method (nisfind::tryenv) */

int nisfind::tryget() noex {
	return uc_getnisdomain(rbuf,rlen) ;
} /* end method (nisfind::tryget) */

int nisfind::tryfile() noex {
	return nisfile(rbuf,rlen,NISFN) ;
} /* end method (nisfind::tryfile) */

local int nisfile(char *rbuf,int rlen,cchar *fname) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = filereadln(fname,rbuf,rlen)) >= 0) {
	    len = rs ;
	} else if (isNotAccess(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? len : rs ;
} /* end subroutine (nisfile) */


