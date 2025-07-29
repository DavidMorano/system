/* mkuix SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* try to divine the best real name from a USERINFO object */
/* version %I% last-modified %G% */

#define	CF_FULLNAME	0		/* use full-name */
#define	CF_MAILNAME	1		/* use mail-name */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkuiname

	Description:
	We try to divine the best name that we can for the current
	user.  We do this simply by looking through some options
	contained in the USERINFO object.

	Synopsis:
	int mkuiname(char *rbuf,int rlen,usernfo *uip) noex

	Arguments:
	rbuf		supplied result buffer
	rlen		supplied result buffer length
	uip		pointer to USERINFO object

	Returns:
	>=0		length of resulting name
	<0		error (system-return)


	Name:
	mkuibang

	Description:
	This subroutine create a name that combines the nodename
	and the username into a "bangname."

	Synopsis:
	int mkuibang(char *nbuf,int nlen,USERINFO *uip) noex

	Arguments:
	nbuf		buffer to receive resulting name
	nlen		length of supplied buffer
	uip		pointer to USERINFO object

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<userinfo.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"mkuix.h"


/* local defines */

#ifndef	CF_FULLNAME
#define	CF_FULLNAME	0		/* use full-name */
#endif

#ifndef	CF_MAILNAME
#define	CF_MAILNAME	1		/* use mail-name */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* local variables */

constexpr bool	f_fullname = CF_FULLNAME ;
constexpr bool	f_mailname  = CF_MAILNAME ;


/* exported variables */


/* exported subroutines */

int mkuiname(char *rbuf,int rlen,userinfo *uip) noex {
	int		rs = SR_OK ;
	if (rbuf && uip) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (rlen >= 0) ylikely {
		cchar	*ns = nullptr ;
		rs = SR_NOTFOUND ;
	        if (uip->fullname && (uip->fullname[0] != '\0')) {
	            ns = uip->fullname ;
	        } else if (uip->name && (uip->name[0] != '\0')) {
	            ns = uip->name ;
	        } else if (uip->mailname && (uip->mailname[0] != '\0')) {
	            ns = uip->mailname ;
	        } else if (uip->username && (uip->username[0] != '\0')) {
	            ns = uip->username ;
	        }
	        if (ns) ylikely {
	            rs = sncpy(rbuf,rlen,ns) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkuiname) */

int mkuibang(char *rbuf,int rlen,userinfo *uip) noex {
	int		rs = SR_FAULT ;
	if (rbuf && uip) ylikely {
	    rbuf[0] = '\0' ;
	    rs = SR_INVALID ;
	    if (rlen >= 0) ylikely {
	        cchar	*ns = nullptr ;
	        if_constexpr (f_fullname) {
	            if (ns == nullptr) {
	                if (uip->fullname && (uip->fullname[0] != '\0')) {
	                    ns = uip->fullname ;
	                }
	            }
	        } /* end if_constexpr (f_fullname) */
	        if (ns == nullptr) {
	            if (uip->name && (uip->name[0] != '\0')) {
	                ns = uip->name ;
	            }
	        }
	        if_constexpr (f_mailname) {
	            if (ns == nullptr) {
	                if (uip->mailname && (uip->mailname[0] != '\0')) {
	                    ns = uip->mailname ;
	                }
	            }
	        } /* end if_constexpr (f_mailname) */
	        if (ns == nullptr) {
	            if (uip->fullname && (uip->fullname[0] != '\0')) {
	                ns = uip->fullname ;
	            }
	        }
	        {
	            cchar	*nn = uip->nodename ;
	            cchar	*un = uip->username ;
	            rs = SR_NOTFOUND ;
	            if (ns) ylikely {
	                rs = sncpy(rbuf,rlen,nn,"!",un," (",ns,")") ;
	                if (rs == SR_OVERFLOW) {
	                   rs = sncpy(rbuf,rlen,nn,"!",un) ;
	                }
		    }
	        } /* end block */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkuibang) */


