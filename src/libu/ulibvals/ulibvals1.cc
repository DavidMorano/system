/* ulibvals1 MODULE */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* retrieve various Ukernel-Library-Values */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	ulibvals

	Description:
	This module contains a structure (ULIBVALS) that itself
	contains various system related integer values.

	Synopsis:
	import ulibvals
	ulibval.{x}

	Returns:
	>=0		requested value
	<0		error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<bit>			/* |endian(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>		/* |MAXBASE| */
#include	<ulogerror.h>
#include	<sysconfcmds.h>		/* |_SC_{xx}| */
#include	<localmisc.h>		/* |{xxx}BUFLEN| */

#pragma		GCC dependency	"mod/usysconf.ccm"

module ulibvals ;

import usysconf ;			/* |usysconfval(3u)| */

using std::endian ;

static int	rscum ;

static constexpr int mkendian() noex {
    	using enum	endian ;	/* get the values */
    	int	n ;
	if_constexpr (native == little) {
	    n = 0 ;
	} else if_constexpr (native == big) {
	    n = 1 ;
	} else {
	    n = 2 ;
	} /* end if_constexpr */
	return n ;
} /* end subroutine (mkendian) */

int getval(int cmd) noex {
    	int		rs ;
	if ((rs = rscum) >= 0) {
	    if ((rs = usysconfval(cmd)) < 0) {
	        rscum = rs ;
	        ulogerror("ulibvals",rs,"getval") ;
	    }
	}
	return rs ;
} /* end subroutine (getval) */

const int	ulibvals::endianval	= mkendian() ;
const int	ulibvals::pagesz	= getval(_SC_PAGESIZE) ;
const int	ulibvals::clktck	= getval(_SC_CLK_TCK) ;
const int	ulibvals::maxbase	= MAXBASE ; /* the standard value */
const int	ulibvals::maxpid	= getval(_SC_PID_MAX) ;
const int	ulibvals::maxarg	= getval(_SC_ARG_MAX) ;
const int	ulibvals::maxline	= getval(_SC_LINE_MAX) ;
const int	ulibvals::maxlogin	= getval(_SC_LOGIN_NAME_MAX) ;
const int	ulibvals::maxsymbol	= getval(_SC_SYMBOL_MAX) ;
const int	ulibvals::maxgroups	= getval(_SC_NGROUPS_MAX) ;
const int	ulibvals::maxnamelen	= getval(_SC_NAME_MAX) ;
const int	ulibvals::maxpathlen	= getval(_SC_PATH_MAX) ;
const int	ulibvals::maxmsglen	= getval(_SC_MSG_MAX) ;
const int	ulibvals::usernamelen	= getval(_SC_USERNAME_MAX) ;
const int	ulibvals::groupnamelen	= getval(_SC_GROUPNAME_MAX) ;
const int	ulibvals::projnamelen	= 
				getval(_SC_PROJECTNAME_MAX) ;
const int	ulibvals::nodenamelen	= getval(_SC_NODENAME_MAX) ;
const int	ulibvals::hostnamelen	= getval(_SC_HOSTNAME_MAX) ;
const int	ulibvals::binbuflen	= BINBUFLEN ;	/* for |int256_t| */
const int	ulibvals::octbuflen	= OCTBUFLEN ;	/* for |int256_t| */
const int	ulibvals::decbuflen	= DECBUFLEN ;	/* for |int256_t| */
const int	ulibvals::hexbuflen	= HEXBUFLEN ;	/* for |int256_t| */
const int	ulibvals::digbuflen	= DIGBUFLEN ;	/* for |int256_t| */

ulibvals::operator int () noex {
	return rscum ;
}

const ulibvals	ulibval ;


