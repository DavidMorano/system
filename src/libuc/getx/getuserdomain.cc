/* getuserdomain SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the INET domain-name (c-string) for a specified user-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getuserdomain

	Descriptor:
	This subroutine retrieves the primary INET domain-name
	for a given user.

	Synopsis:
	int getuserdomain(char *rbuf,int rlen,cchar *username) noex

	Arguments:
	rbuf		user supplied buffer to hold result
	rlen		length of user supplied buffer
	username	username to look up

	Returns:
	>=0		length of return organization string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucpwcache.h>		/* LIBUC */
#include	<getax.h>		/* LIBUC */
#include	<getpwx.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<getuserhome.h>		/* LIBUC */
#include	<getnodedomain.h>	/* LIBUC */
#include	<filereadln.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getuserdomain.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;
import uconstants ;			/* |varname(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int getuserdomu(char *,int,ccp) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int getuserdomain(char *rbuf,int rlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (rbuf) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = eos ;
	    if (un) {
		rs = SR_INVALID ;
		if (un[0]) {
		    if (strcmp(un,"-") != 0) {
		        rs = getuserdomu(rbuf,rlen,un) ;
		        len = rs ;
		    } else {
			rs = getsysdomain(rbuf,rlen) ;
			len = rs ;
		    } /* end */
		} /* end if */
	    } else {
		rs = getsysdomain(rbuf,rlen) ;
		len = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getuserdomain) */


/* local subroutines */

local int getuserdomu(char *drbuf,int drlen,ccp un) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	(void) un ;
	if (drbuf) ylikely {
	    if (char *dn ; (rs = lm_hostname(&dn)) >= 0) ylikely {
	        if (drlen < 0) drlen = rs ;
	        if ((rs = getnodedomain(np,dn)) >= 0) ylikely {
	            rs = sncpy(drbuf,drlen,dn) ;
	            len = rs ;
	        } /* end */
	        rs1 = lm_free(dn) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getuserdomainu) */


