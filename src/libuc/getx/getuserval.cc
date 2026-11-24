/* getuserval SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get an attribute (a c-string) for a specified user-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getuserval

	Descriptor:
	This subroutine retrieves an attribute for the given
	user and the given attribute key-name.

	Synopsis:
	int getuserval(char *rbuf,int rlen,ccp atp,cchar *un) noex

	Arguments:
	rbuf		user supplied buffer to hold result
	rlen		length of user supplied buffer
	atp		pointer to attribute key-name c-string
	un		username to look up

	Returns:
	>=0		length of return organization string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<userattrdb.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getuserval.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;
import uconstants ;			/* |varname(3u)| */

/* local defines */


/* external subroutines */

using libuc::libmem ;			/* variable */


/* external variables */


/* local structures */

namespace {
    struct submgr {
	ccharp		attr ;
	charp		ubuf ;
	charp		rbuf ;
	int		rlen ;
	int		ulen ;
	bool		falloc ;
	submgr(char *rb,int rl,ccp ap) noex : rbuf(rb), rlen(rl) {
	    attr = ap ;
	    ubuf = nullptr ;
	    ulen = 0 ;
	    falloc = false ;
	} ; /* end ctor */
	int operator ()	(ccharp) noex ;
	int userbeg	(ccharp) noex ;
	int userend	() noex ;
	int getdb	() noex ;
    } ; /* end struct (submgr) */
} /* end namespace */


/* forward references */

local bool	isempty(char) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int getuserval(char *rbuf,int rlen,cchar *atp,cchar *un) noex {
	int		rs = SR_FAULT ;
	if (rbuf && atp) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = eos ;
	    if ((rlen > 0) && atp[0]) {
		submgr so(rbuf,rlen,atp) ;
		rs = so(un) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (getuserval) */


/* local subroutines */

int submgr::operator () (ccharp un) noex {
    	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if ((rs = userbeg(un)) >= 0) {
	    {
		rs = getdb() ;
		len = rs ;
	    }
	    rs1 = userend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ckuser) */
    	return (rs >= 0) ? len : rs ;
} /* end method (submgr::operator) */

int submgr::userbeg(ccharp un) noex {
    	cnullptr	np{} ;
    	int		rs = SR_OK ;
	if ((un == np) || isempty(un[0])) {
	    if ((rs = libmem.un(&ubuf)) >= 0) {
		falloc = true ;
		ulen = rs ;
		rs = getusername(ubuf,ulen,-1) ;
		if (rs < 0) {
		    libmem.free(ubuf) ;
		    ubuf = nullptr ;
		    falloc = false ;
		} /* end if (error) */
	    } /* end if (memory-acquire) */
	} else {
	    ubuf = cast_const<charp>(un) ;
	} /* end if (allocation needed) */
	return rs ;
} /* end method (submgr::userbeg) */

int submgr::userend() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (falloc && ubuf) {
	    rs1 = libmem.free(ubuf) ;
	    if (rs >= 0) rs = rs1 ;
	    ubuf = nullptr ;
	    ulen = 0 ;
	    falloc = false ;
	} /* end if (allocation) */;
	return rs ;
} /* end method (submgr::userend) */

int submgr::getdb() noex {
    	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (userattrdb db ; (rs = db.open(ubuf)) >= 0) {
	    {
		rs = db.look(rbuf,rlen,attr) ;
		len = rs ;
	    }
	    rs1 = db.close ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (userattrdb) */
	return (rs >= 0) ? len : rs ;
} /* end method (submgr::getdb) */

local bool isempty(char c) noex {
    	return ((c == '\0') || (c == '-')) ;
} /* end subroutine (isempty) */


