/* bwriteblock SUPPORT */
/* lang=C++20 */

/* write a block data from a given file to the current file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bwriteblock

	Description:
	This subroutine copies the remainder of the input file to
	the output file.

	Synospsis:
	int bwriteblock(bfile *op,bfile *ifp,int ulen) noex

	Arguments:
	op		output file pointer to copy to
	ifp		input file pointer to copy from
	ulen		length of supplied buffer

	Returns:
	>=0		length of data copied or error return
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */

#define	BFILE_NPAGES	4


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	bfile_write(bfile *,cvoid *,int) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct writer ;
    struct writer_rd {
	writer		*wrp = nullptr ;
	void operator () (writer *p) noex {
	    wrp = p ;
	} ;
	operator int () noex ;
    } ;
    struct writer {
	writer_rd	rd ;
	bfile		*op ;
	bfile		*ifp ;
	char		*tbuf ;		/* temporary buffer pointer */
	int		tlen ;		/* temporary buffer length */
	int		ulen ;
	writer(bfile *p,bfile *ip,int ul) noex : op(p), ifp(ip), ulen(ul) { 
	    rd(this) ;
	} ;
	operator int () noex ;
	int abegin() noex ;
	int aend() noex ;
	int ird() noex {
	    cint	rl = min(ulen,tlen) ;
	    int		rs = SR_OK ;
	    if (rl) {
		rs = bread(ifp,tbuf,rl) ;
	    }
	    return rs ;
	} ;
    } ;
}


/* forward references */


/* local varaibles */


/* exported variables */


/* exported subroutines */

int bwriteblock(bfile *op,bfile *ifp,int ulen) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = bfile_magic(op,ifp)) > 0) {
	    if ((rs = bfile_wr(op)) >= 0) {
	        if (ulen != 0) {
		    writer	wo(op,ifp,ulen) ;
		    rs = wo ;
		    wlen = rs ;
	        } /* end if (not nullfile) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bwriteblock) */


/* local subroutines */

writer::operator int () noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = abegin()) >= 0) {
	    while ((rs = rd) > 0) {
		rs = bfile_write(op,tbuf,rs) ;
		ulen -= rs ;
		wlen += rs ;
		if (rs < 0) break ;
	    } /* end while */
	    rs1 = aend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (allocation) */
	return (rs >= 0) ? wlen : rs ;
}
/* end method (writer::operator) */

int writer::abegin() noex {
	tlen = (op->pagesize * BFILE_NPAGES) ;
	if (ulen >= 0) {
	    if (ulen < tlen) tlen = ulen ;
	} else {
	    ulen = INT_MAX ;
	}
	return uc_valloc(tlen,&tbuf) ;
}
/* end method (writer::abegin) */

int writer::aend() noex {
	int		rs = SR_BUGCHECK ;
	if (tbuf) {
	    rs = uc_free(tbuf) ;
	    tbuf = nullptr ;
	    tlen = 0 ;
	}
	return rs ;
}
/* end method (writer::aend) */

writer_rd::operator int () noex {
	return wrp->ird() ;
}


