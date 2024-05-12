/* ucsysauxinfo SUPPORT */
/* lang=C++20 */

/* UNIX® system-auxillary-information */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ucsysauxinfo

	Description:
	We (try to) retrieve auxillary informaton from the operating
	system.

	Synopsis:
	int uc_sysauxinfo(char *rbuf,int rlen,int req) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	req		system-auxillary-value to request

	Returns:
	-		length of returned string

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/utsname.h>
#include	<cstring>
#include	<usystem.h>
#include	<timewatch.hh>
#include	<sigblocker.h>
#include	<ptm.h>
#include	<localmisc.h>

#include	"usysauxinfo.h"


/* local defines */

#if	defined(SYSHAS_SYSAUXINFO) && (SYSHAS_SYSAUXINFO > 0)
#define	F_SYSAUXINFO		1
#else
#define	F_SYSAUXINFO		0
#endif /* defined(SYSHAS_SYSAUXINFO) && (SYSHAS_SYSAUXINFO > 0) */


/* imported namespaces */

using usys::usysauxinfo ;		/* subroutines */


/* local typedefs */


/* external subroutines */


/* local structures */

namespace {
    struct ucsysauxinfo ;
    typedef int (ucsysauxinfo::*mem_f)(char *,int,int) noex ;
    struct ucsysauxinfo {
	mem_f		m ;
	ucsysauxinfo() noex { } ;
	int get(char *,int,int) noex ;
	int operator () (char *,int,int) noex ;
    } ; /* end struct (ucsysauxinfo) */
}


/* forward references */

extern "C" {
    int		uc_sysauxinfo(char *,int,int) noex ;
}


/* local variables */

constexpr bool		f_sysauxinfo = F_SYSAUXINFO ;


/* exported variables */


/* exported subroutines */

int uc_sysauxinfo(char *rbuf,int rlen,int req) noex {
	ucsysauxinfo	aio ;
	aio.m = &ucsysauxinfo::get ;
	return aio(rbuf,rlen,req) ;
}


/* local subroutines */

int ucsysauxinfo::operator () (char *rbuf,int rlen,int req) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if ((rlen > 0) && (req >= 0)) {
	        int	to_again = utimeout[uto_again] ;
	        int	to_nomem = utimeout[uto_nomem] ;
	        bool	f_exit = false ;
	        repeat {
	            if ((rs = (this->*m)(rbuf,rlen,req)) < 0) {
	                switch (rs) {
	                case SR_AGAIN:
		            if (to_again-- > 0) {
	                        msleep(1000) ;
	 	            } else {
		                f_exit = true ;
		            }
		            break ;
	                case SR_NOMEM:
		            if (to_nomem-- > 0) {
	                        msleep(1000) ;
	 	            } else {
		                f_exit = true ;
		            }
		            break ;
	                case SR_INTR:
	                    break ;
	                default:
		            f_exit = true ;
		            break ;
	                } /* end switch */
	            } /* end if (error) */
	        } until ((rs >= 0) || f_exit) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucsysauxinfo::operator) */

int ucsysauxinfo::get(char *rbuf,int rlen,int req) noex {
	int		rs = SR_NOSYS ;
	errno = 0 ;
	if constexpr (f_sysauxinfo) {
	    if ((rs = usysauxinfo(rbuf,rlen,req)) < 0) {
	        rs = (-rs) ;
	    } else if (rs == 0) {
		if (errno) {
		    rs = (-errno) ;
		} else {
		    rs = SR_IO ;
		}
	    }
	} else {
 	    rbuf[0] = '\0' ;
	} /* end if constexpr (selection) */
	return rs ;
}
/* end subroutine (ucsysauxinfo::get) */


