/* comsatmsg SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create and parse COMSAT messages */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	comsatmsg

	Description:
	This module contains the code to make and parse Comsat
	messages.  A Comsat message looks like:
		<user>@<offset>[:<file>]

	Where:
		<user>		is the user who received the message
		<offset>	is the file-offset to this message
		<file>		associated filename (If present)

	Example:
		dam@251:bobby


	Description:
	This subroutine is used to create or to parse a Comsat message.
		
	Synopsis:
	int comsatmsg_start(comsatmsg(comsatmsg *op) noex
	int comsatmsg_rd(comsatmsg(comsatmsg *op,char *rdbuf,int rdlen) noex
	int comsatmsg_wr(comsatmsg(comsatmsg *op,cchar *wrbuf,int wrlen) noex
	int comsatmsg_finish(comsatmsg(comastmsg *op) noex
		
	Arguments:
	op		COMSATMSG object pointer
	rdbuf		read-buffer pointer
	webuf		write-buffer pointer
	rdlen		read-buffer length
	wrlen		write-buffer length

	Returns:
	>0		lrngth of result when creating
	>=0		for parsing
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<sbuf.h>
#include	<strn.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<cfdec.h>
#include	<char.h>
#include	<localmisc.h>

#include	"comsatmsg.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		usernamelen ;
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int comsatmsg_start(comsatmsg *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    static cint		rsv = var ;
	    if ((rs = rsv) >= 0) {
	        cint	sz = (var.usernamelen + var.maxpathlen + 2) ;
	        if (char *cp ; (rs = libmem.mall(sz,&cp)) >= 0) {
		    op->a = cp ;
		    op->username = (cp) ;
		    op->fname = (cp + (var.usernamelen + 1)) ;
	        } /* end if (memory-allocation) */
	    } /* end if (vars) */
	} /* end if (non-null) */
	return rs ;
}

int comsatmsg_finish(comsatmsg *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->a) {
		rs1 = libmem.free(op->a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}

int comsatmsg_wr(comsatmsg *op,cchar *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	if (op && mbuf) {
	    cint	ulen = var.usernamelen ;
	    char	*ubuf = op->username ;
	    int		sl = mlen ;
	    cchar	*sp = mbuf ;
	    rs = SR_OK ;
	    op->offset = 0 ;
	    op->username[0] = '\0' ;
	    op->fname[0] = '\0' ;
	    while ((sl > 0) && CHAR_ISWHITE(sp[sl - 1])) {
	        sl -= 1 ;
	    }
	    if (cc *tp ; (tp = strnchr(sp,sl,'@')) != nullptr) {
		int	ll = intconv(tp - sp) ;
		int	cl ;
		cchar	*cp{} ;
	        if ((rs = sncpy1w(ubuf,ulen,sp,ll)) >= 0) {
	            sl -= intconv((tp + 1) - mbuf) ;
	            sp = (tp + 1) ;
	            cp = sp ;
	            cl = sl ;
	            if ((tp = strnchr(sp,sl,':')) != nullptr) {
			ll = intconv((sp+sl)-(tp+1)) ;
	                char	*fbuf = op->fname ;
	                cl = intconv(tp - sp) ;
	                rs = mkpath1w(fbuf,(tp+1),ll) ;
	            }
	            if (rs >= 0) {
	                if (ulong ulv ; (rs = cfdec(cp,cl,&ulv)) >= 0) {
	                    op->offset = ulv ;
			}
	            }
	        } /* end if */
	    } else {
	        rs = SR_BADMSG ;
	    }
	} /* end if (non-null) */
	return rs ;
}

int comsatmsg_rd(comsatmsg *op,char *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && mbuf) {
	    if (sbuf sb ; (rs = sb.start(mbuf,mlen)) >= 0) {
	        ulong	ulv = op->offset ;
	        sb.str(op->username) ;
	        sb.chr('@') ;
	        sb.dec(ulv) ;
	        if (op->fname[0] != '\0') {
	            sb.chr(':') ;
	            sb.str(op->fname) ;
	        } /* end if */
	        len = sb.finish ;
	        if (rs >= 0) rs = len ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (comsatmsg_rd) */


/* private subroutines */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_un)) >= 0) {
	    usernamelen = rs ;
	    if ((rs = getbufsize(getbufsize_mp)) >= 0) {
		maxpathlen = rs ;
	    }
	}
	return rs ;
}
/* end method (vars::operator) */

int comsatmsg::rd(char *rdbuf,int rdlen) noex {
	return comsatmsg_rd(this,rdbuf,rdlen) ;
}

int comsatmsg::wr(cchar *sp,int sl) noex {
	return comsatmsg_wr(this,sp,sl) ;
}

void comsatmsg::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("comsatmsg",rs,"fini-finish") ;
	}
}

comsatmsg_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case comsatmsgmem_start:
	        rs = comsatmsg_start(op) ;
	        break ;
	    case comsatmsgmem_finish:
	        rs = comsatmsg_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (comsatmsg_co::operator) */


