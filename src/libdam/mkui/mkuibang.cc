/* mkuibang SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a "bang" name from USERINFO data */
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
	mkuibang

	Description:
	This subroutine create a name that combines the nodename
	and the username into a "bangname."

	Synopsis:
	int mkuibang(char *nbuf,int nlen,userinfo *uip) noex

	Arguments:
	nbuf		buffer to receive resulting name
	nlen		length of supplied buffer
	uip		pointer to USERINFO object

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<userinfo.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"mkui.h"


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


/* forward references */


/* local variables */

constexpr bool		f_fullname	= CF_FULLNAME ;
constexpr bool		f_mailname	= CF_MAILNAME ;


/* exported variables */


/* exported subroutines */

int mkuibang(char *rbuf,int rlen,userinfo *uip) noex {
	int		rs = SR_FAULT ;
	if (rbuf && uip) ylikely {
	    rbuf[0] = '\0' ;
	    rs = SR_INVALID ;
	    if (rlen >= 0) ylikely {
	        cchar	*sp = nullptr ;
	        if_constexpr (f_fullname) {
	            if (sp == nullptr) {
	                if (uip->fullname && (uip->fullname[0] != '\0')) {
	                    sp = uip->fullname ;
	                }
	            }
	        } /* end if_constexpr (f_fullname) */
	        if (sp == nullptr) {
	            if (uip->name && (uip->name[0] != '\0')) {
	                sp = uip->name ;
	            }
	        }
	        if_constexpr (f_mailname) {
	            if (sp == nullptr) {
	                if (uip->mailname && (uip->mailname[0] != '\0')) {
	                    sp = uip->mailname ;
	                }
	            }
	        } /* end if_constexpr (f_mailname) */
	        if (sp == nullptr) {
	            if (uip->fullname && (uip->fullname[0] != '\0')) {
	                sp = uip->fullname ;
	            }
	        }
	        {
	            cchar	*nn = uip->nodename ;
	            cchar	*un = uip->username ;
	            rs = SR_NOTFOUND ;
	            if (sp != nullptr) {
	               rs = sncpy6(rbuf,rlen,nn,"!",un," (",sp,")") ;
	            }
	            if ((rs == SR_OVERFLOW) || sp) {
	               rs = sncpy3(rbuf,rlen,nn,"!",un) ;
	            }
	        } /* end block */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (mkuibang) */


