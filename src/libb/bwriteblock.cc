/* bwriteblock SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* write a block of data from a given file to the current file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bwriteblock

	Description:
	This subroutine copies a block (with a given length) of
	datæ from another file to the output file.

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
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"bfile.h"


/* local defines */

#define	BFILE_NPAGES	4


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


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
    } ; /* end struct (writer_rd) */
    struct writer {
	writer_rd	rd ;
	bfile		*op ;
	bfile		*ifp ;
	char		*tbuf ;		/* temporary buffer pointer */
	int		tlen ;		/* temporary buffer length */
	int		ulen ;
	writer(bfile *p,bfile *ip,int ul) noex : op(p), ifp(ip), ulen(ul) { 
	    rd(this) ;
	} ; /* end ctor */
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
	} ; /* end method (ird) */
    } ; /* end struct (writer) */
} /* end namespace */


/* forward references */


/* local varaibles */


/* exported variables */


/* exported subroutines */

int bwriteblock(bfile *op,bfile *ifp,int ulen) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = bfile_magic(op,ifp)) > 0) {
	    if ((rs = bfile_ckwr(op)) >= 0) {
	        if (ulen != 0) {
		    if (writer wo(op,ifp,ulen) ; (rs = wo) >= 0) {
		        wlen = rs ;
		    }
	        } /* end if (not nullfile) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (bwriteblock) */


/* local subroutines */

writer::operator int () noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = abegin()) >= 0) {
	    while ((rs = rd) > 0) {
		rs = bwrite(op,tbuf,rs) ;
		ulen -= rs ;
		wlen += rs ;
		if (rs < 0) break ;
	    } /* end while */
	    rs1 = aend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (allocation) */
	return (rs >= 0) ? wlen : rs ;
} /* end method (writer::operator) */

int writer::abegin() noex {
	if (tlen = (op->pagesz * BFILE_NPAGES) ; ulen >= 0) {
	    if (ulen < tlen) tlen = ulen ;
	} else {
	    ulen = INT_MAX ;
	}
	return mem.vall(tlen,&tbuf) ;
} /* end method (writer::abegin) */

int writer::aend() noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (tbuf) {
	    rs1 = mem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    tbuf = nullptr ;
	    tlen = 0 ;
	} /* end if (memory-release) */
	return rs ;
} /* end method (writer::aend) */

writer_rd::operator int () noex {
	return wrp->ird() ;
} /* end method */


