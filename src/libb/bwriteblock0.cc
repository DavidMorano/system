/* bwriteblock SUPPORT */
/* lang=C++20 */

/* write a block data from a given file to the current file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This subroutine copies the remainder of the input file to the output
        file.

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


/* external variables */


/* local varaibles */


/* exported variables */


/* exported subroutines */

int bwriteblock(bfile *op,bfile *ifp,int ulen) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = bfile_magic(op,ifp)) > 0) {
	    if (ulen != 0) {
	        int	bsize = (op->pagesize * BFILE_NPAGES) ;
	        char	*buf ;
	        if (ulen >= 0) {
		    if (ulen < bsize) bsize = ulen ;
	        } else {
	            ulen = INT_MAX ;
	        }
	        if ((rs = uc_valloc(bsize,&buf)) >= 0) {
	            int		bl, len ;
	            int		mlen ;
	            while (wlen < ulen) {
		        int	i = 0 ;
	                mlen = min((ulen - wlen),bsize) ;
	                rs = bread(ifp,buf,mlen) ;
	                len = rs ;
	                if (rs <= 0) break ;
	                bl = len ;
	                while ((bl > 0) && ((rs = bwrite(op,(buf+i),bl)) < bl)) {
	                    i += rs ;
	                    bl -= rs ;
	                    if (rs < 0) break ;
	                } /* end while */
	                wlen += len ;
	                if (rs < 0) break ;
	            } /* end while */
	            rs1 = uc_free(buf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (memory-allocation) */
	    } /* end if (not nullfile) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bwriteblock) */


