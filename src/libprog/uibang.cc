/* uibang SUPPORT */
/* lang=C++20 */

/* make a "bang" name from USERINFO data */
/* version %I% last-modified %G% */

#define	CF_FULLNAME	0		/* use full-name */
#define	CF_MAILNAME	1		/* use mail-name */

/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uibang

	Description:
	This subroutine create a name that combines the nodename
	and the username into a "bangname."

	Synopsis:
	int uibang(char *nbuf,int nlen,USERINFO *uip) noex

	Arguments:
	nbuf		buffer to receive resulting name
	nlen		length of supplied buffer
	uip		pointer to USERINFO object

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<varnames.hh>
#include	<userinfo.h>
#include	<sncpyx.h>
#include	<localmisc.h>


/* local defines */

#ifndef	CF_FULLNAME
#define	CF_FULLNAME	0		/* use full-name */
#endif

#ifndef	CF_MAILNAME
#define	CF_MAILNAME	1		/* use mail-name */
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* local variables */

constexpr bool	f_fullname = CF_FULLNAME ;
constexpr bool	f_mailname  = CF_MAILNAME ;


/* exported variables */


/* exported subroutines */

int uibang(USERINFO *uip,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf && uip) {
	    rbuf[0] = '\0' ;
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
	        cchar	*np = nullptr ;
	        if constexpr (f_fullname) {
	            if (np == NULL) {
	                if (uip->fullname && (uip->fullname[0] != '\0')) {
	                    np = uip->fullname ;
	                }
	            }
	        } /* end if-constexpr (f_fullname) */
	        if (np == NULL) {
	            if (uip->name && (uip->name[0] != '\0')) {
	                np = uip->name ;
	            }
	        }
	        if constexpr (f_mailname) {
	            if (np == NULL) {
	                if (uip->mailname && (uip->mailname[0] != '\0')) {
	                    np = uip->mailname ;
	                }
	            }
	        } /* end if-constexpr (f_mailname) */
	        if (np == NULL) {
	            if (uip->fullname && (uip->fullname[0] != '\0')) {
	                np = uip->fullname ;
	            }
	        }
	        {
	            cchar	*nn = uip->nodename ;
	            cchar	*un = uip->username ;
	            rs = SR_NOTFOUND ;
	            if (np != NULL) {
	               rs = sncpy6(rbuf,rlen,nn,"!",un," (",np,")") ;
	            }
	            if ((rs == SR_OVERFLOW) || np) {
	               rs = sncpy3(rbuf,rlen,nn,"!",un) ;
	            }
	        } /* end block */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uibang) */


