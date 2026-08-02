/* getarch SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the machine architecture string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getarch

	Description:
	Get and return the machine architecture string.

	Synopsis:
	int getarch(char *rbuf,int rlen) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>=0		number of characters returned
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucsysauxinfo.h>	/* LIBUC */
#include	<ucgetx.h>		/* LIBUC |uc_getarch(3uc)| */
#include	<sfx.h>			/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getarch.h"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;			/* |varname(3u)| */

/* local defines */

#ifndef	DEFARCH
#define	DEFARCH		"Armv9"
#endif


/* external subroutines */

extern "C" {
    extern int uc_sysauxinfo(char *,int,int) noex ;
}

/* external variables */


/* local structures */

namespace {
    struct archer ;
    typedef int (archer::*archer_m)() noex ;
    struct archer {
	char		*rbuf ;
	int		rlen ;
	archer(char *b,int l) noex : rbuf(b), rlen(l) { } ;
	operator int () noex ;
	int tryenv() noex ;
	int trylib() noex ;
	int trysys() noex ;
	int trydef() noex ;
    } ; /* end struct (archer) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr archer_m	tries[] = {
	&archer::tryenv,
	&archer::trylib,
	&archer::trysys,
	&archer::trydef
} ; /* end array (tries) */


/* exported variables */


/* exported subroutines */

int getarch(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (rlen >= 0) ylikely {
	        archer	ao(rbuf,rlen) ;
		rs = ao ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (getarch) */


/* local subroutines */

archer::operator int () noex {
	int		rs = SR_OK ;
	for (cauto &m : tries) {
	    rs = (this->*m)() ;
	    if (rs) break ;
	} /* end for */
	return rs ;
} /* end method (archer::operator) */

int archer::tryenv() noex {
	static cchar	*valp = getenver(varname.architecture) ;
	int		rs = SR_OK ;
	if (valp) ylikely {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfshrink(valp,-1,&cp)) > 0) ylikely {
	        rs = snwcpy(rbuf,rlen,cp,cl) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end method (archer::tryenv) */

int archer::trylib() noex {
	return uc_getarch(rbuf,rlen) ;
} /* end method (archer::trylib) */

int archer::trysys() noex {
	cint		cmd = usysauxinforeq_architecture ;
	int		rs ;
	if ((rs = uc_sysauxinfo(rbuf,rlen,cmd)) >= 0) {
	    /* nothing */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} else if (rs == SR_NOSYS) {
	    rs = SR_OK ;
	}
	return rs ;
} /* end method (archer::trysys) */

int archer::trydef() noex {
	return snwcpy(rbuf,rlen,DEFARCH) ;
} /* end method (archer::trydef) */


