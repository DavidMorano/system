/* ucgetnetname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get the Net-name of the current user */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	ucgetnetname

	Description:
	This subroutine retrieves (we will go with that word) the
	(so-called) net-name of the current user.

	Net names look like:
		unix.<uid>@<nisdomain>

	an example of which is:
		unix.201@rightcore.com

	Synopsis:
	int uc_getnetname(char *nbuf,int nlen) noex

	Arguments:
	nbuf		result buffer pointer
	nlen		result buffer length

	Rerurns:
	>=0		length (in bytes) of result
	<0		error (return-system)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD placement-new */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucgetnetname.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	PROCNAME	"keyserv"	/* required process */


/* local namespaces */

using libu::sncpy ;			/* subroutine */
using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int uc_procpid(cchar *,uid_t) noex ;
} /* end extern (C) */

extern "C" {
    extern int	getnetname(char *) noex ;	/* UNIX® ONC interface */
} /* end extern (C) */


/* external variables */


/* local structures */

namespace {
    struct vars {
    	int	netnamelen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */


/* local variables */

static vars	var ;


/* exported variables */


/* exported subroutines */

int uc_getnetname(char *nbuf,int nlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (nbuf) ylikely {
	    nbuf[0] = '\0' ;
	    if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
		cint rlen = rs ;
		if (char *rbuf ; (rs = libmem.mall((rlen+1),&rbuf)) >= 0) {
	            const uid_t	uid = 0 ; /* root user */
	            if ((rs = uc_procpid(PROCNAME,uid)) > 0) ylikely {
	                if (getnetname(rbuf) > 0) {
			    rs = sncpy(nbuf,nlen,rbuf) ;
	                    len = rs ;
	                } else {
	                    rs = SR_NOTFOUND ;
	                }
	            } else if (rs == 0) {
		        rs = SR_BUSY ; /* was previously SR_UNAVAIL */
	            }
		    rs1 = libmem.free(rbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (vars) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (uc_getnetname) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = usys::getnetnamelen()) >= 0) ylikely {
	    netnamelen = rs ;
	}
    	return rs ;
} /* end method (vars::operator) */


