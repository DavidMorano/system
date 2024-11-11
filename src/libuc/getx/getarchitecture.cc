/* getarchitecture SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get the machine architecture string */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getarcitecture

	Description:
	Get and return the machine architecture string.

	Synopsis:
	int getarchitecture(char *rbuf,int rlen) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>=0		number of characters returned
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<uvariables.hh>
#include	<ucsysauxinfo.h>	/* |usysauxinforeq_architecture| */
#include	<sfx.h>
#include	<snwcpy.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"getarchitecture.h"


/* local defines */

#ifndef	ARCHITECTURE
#define	ARCHITECTURE	"Armv9"
#endif


/* external subroutines */


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
}


/* forward references */


/* local variables */

constexpr archer_m	mems[] = {
	&archer::tryenv,
	&archer::trylib,
	&archer::trysys,
	&archer::trydef
} ;


/* exported variables */


/* exported subroutines */

int getarchitecture(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (rlen >= 0) {
	        archer	ao(rbuf,rlen) ;
		rs = ao ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getarchitecture) */


/* local subroutines */

archer::operator int () noex {
	int		rs = SR_OK ;
	for (const auto &m : mems) {
	    rs = (this->*m)() ;
	    if (rs != SR_OK) break ;
	} /* end for */
	return rs ;
}
/* end method (archer::operator) */

int archer::tryenv() noex {
	static cchar	*valp = getenv(varname.architecture) ;
	int		rs = SR_OK ;
	if (valp) {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfshrink(valp,-1,&cp)) > 0) {
	        rs = snwcpy(rbuf,rlen,cp,cl) ;
	    }
	}
	return rs ;
}
/* end method (archer::tryenv) */

int archer::trylib() noex {
	return uc_getarchitecture(rbuf,rlen) ;
}
/* end method (archer::trylib) */

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
}
/* end method (archer::trysys) */

int archer::trydef() noex {
	return snwcpy(rbuf,rlen,ARCHITECTURE) ;
}
/* end method (archer::trydef) */


