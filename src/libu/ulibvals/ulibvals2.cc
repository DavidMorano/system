/* ulibvals2 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* retrieve various Kernel-Library-Values */
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
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<bit>			/* C++STD |endian(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usysdefs.h>		/* LIBU |DIGBASE_MAX| */
#include	<ulogerror.h>		/* LIBU */
#include	<sysconfcmds.h>		/* LIBU |_SC_{xx}| */
#include	<localmisc.h>		/* LIBU |{xxx}BUFLEN| */

#pragma		GCC dependency		"mod/usysconf.ccm"

module ulibvals ;

import usysconf ;			/* |usysconfval(3u)| */

using std::endian ;

#ifndef	SYSUID_MAX
#define	SYSUID_MAX	(500 - 1)	/* from Apple-Darwin? */
#endif

local sig_atomic_t	rscum ;

local constexpr int mkendian() noex {
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
	        ulogerror("ulibvaler",rs,"getval") ;
	    } /* end if (usysconfval) */
	} /* end */
	return rs ;
} /* end subroutine (getval) */

ulibvaler_co::operator int () noex {
    	int		rs = SR_OK ;
	if (val == 0) {
	    int		scmd = -1 ;
	    switch (w) {
	    case ulibvalmem_endianval:
    		rs = mkendian() ;
		val = rs ;
	        break ;
	    case ulibvalmem_pagesz:
	        scmd = _SC_PAGESIZE ;
	        break ;
	    case ulibvalmem_clktck:
	        scmd = _SC_CLK_TCK ;
	        break ;
	    case ulibvalmem_maxpid:
	        scmd = _SC_PID_MAX ;
	        break ;
	    case ulibvalmem_maxarg:
	        scmd = _SC_ARG_MAX ;
	        break ;
	    case ulibvalmem_maxline:
	        scmd = _SC_LINE_MAX ;
	        break ;
	    case ulibvalmem_maxlink:
	        scmd = _SC_LINK_MAX ;
	        break ;
	    case ulibvalmem_maxlogin:
	        scmd = _SC_LOGINNAME_MAX ;
	        break ;
	    case ulibvalmem_maxsymloop:
	        scmd = _SC_SYMLOOP_MAX ;
	        break ;
	    case ulibvalmem_maxsymbol:
	        scmd = _SC_SYMBOL_MAX ;
	        break ;
	    case ulibvalmem_maxgroups:
	        scmd = _SC_NGROUPS_MAX ;
	        break ;
	    case ulibvalmem_maxnamelen:
	        scmd = _SC_NAME_MAX ;
	        break ;
	    case ulibvalmem_maxpathlen:
	        scmd = _SC_PATH_MAX ;
	        break ;
	    case ulibvalmem_maxmsglen:
	        scmd = _SC_MSG_MAX ;
	        break ;
	    case ulibvalmem_maxtzname:
	        scmd = _SC_TZNAME_MAX ;
	        break ;
	    case ulibvalmem_nodenamelen:
	        scmd = _SC_NODENAME_MAX ;
	        break ;
	    case ulibvalmem_usernamelen:
	        scmd = _SC_USERNAME_MAX ;
	        break ;
	    case ulibvalmem_groupnamelen:
    	        scmd = _SC_GROUPNAME_MAX ;
	        break ;
	    case ulibvalmem_projnamelen:
	        scmd = _SC_PROJECTNAME_MAX ;
	        break ;
	    case ulibvalmem_protnamelen:
	        scmd = _SC_PROTNAME_MAX ;
	        break ;
	    case ulibvalmem_hostnamelen:
	        scmd = _SC_HOSTNAME_MAX ;
	        break ;
	    case ulibvalmem_servnamelen:
	        scmd = _SC_SERVNAME_MAX ;
	        break ;
	    case ulibvalmem_maxsysuid:
	    case ulibvalmem_maxtzabbr:
	    case ulibvalmem_maxbase:
	    case ulibvalmem_binbuflen:
	    case ulibvalmem_octbuflen:
	    case ulibvalmem_decbuflen:
	    case ulibvalmem_hexbuflen:
	    case ulibvalmem_digbuflen:
		switch (w) {
		case ulibvalmem_maxsysuid:
		    val = SYSUID_MAX ;
		    break ;
		case ulibvalmem_maxtzabbr:
		    val = TZABBR_MAX ;
		    break ;
	        case ulibvalmem_maxbase:
    		    val = DIGBASE_MAX ;
	            break ;
		case ulibvalmem_binbuflen:
		    val = BINBUFLEN ;	/* for |int256_t| */
		    break ;
		case ulibvalmem_octbuflen:
		    val = OCTBUFLEN ;	/* for |int256_t| */
		    break ;
		case ulibvalmem_decbuflen:
		    val = DECBUFLEN ;	/* for |int256_t| */
		    break ;
		case ulibvalmem_hexbuflen:
		    val = HEXBUFLEN ;	/* for |int256_t| */
		    break ;
		case ulibvalmem_digbuflen:
		    val = DIGBUFLEN ;	/* for |int256_t| */
		    break ;
		default:
		    rs = SR_BUGCHECK ;
		    break ;
		} /* end switch */
	        break ;
	    default:
	        rs = SR_BUGCHECK ;
	        break ;
	    } /* end switch */
	    if ((rs >= 0) && (scmd >= 0) && (val == 0)) {
	        rs = getval(scmd) ;
	        val = rs ;
	    } /* end if (get) */
	} /* end if (needed) */
	return (rs >= 0) ? val : rs ;
} /* end method (ulibvaler_co::operator) */

ulibvaler::operator int () noex {
	return rscum ;
} /* end method (ulibvaler::operator) */


