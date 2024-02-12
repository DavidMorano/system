/* bcopyfile SUPPORT */
/* lang=C++20 */

/* copy a file to another file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine copies the remainder of the input file to
	the output file.

	Synospsis:
	int bcopyfile(bfile *ifp,bfile *ofp,char *ubuf,int ulen) noex

	Arguments:
	+ ifp		input file pointer to copy from
	+ ofp		output file pointer to copy to
	+ buf		buffer to use for the operation
	+ len		length of supplied buffer

	Returns:
	>=0		length of data copied or error return
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<usystem.h>
#include	<sysval.hh>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* exported variables */


/* local variables */

static sysval		pagesize(sysval_ps) ;


/* exported variables */


/* exported subroutines */

int bcopyfile(bfile *ifp,bfile *ofp,char *ubuf,int ulen) noex {
	int		rs ;
	int		rs1 ;
	int		tlen = 0 ;
	if ((rs = bmagic(ifp,ofp,ubuf)) >= 0) {
	    int		blen = ulen ;
	    char	*bbuf = ubuf ;
	    bool	falloc = false ;
	    if ((ubuf == nullptr) || (ulen < 0)) {
	        if ((rs = malloc_ps(&bbuf)) >= 0) {
		    falloc = true ;
	            ulen = rs ;
		}
	    }
	    if (rs >= 0) {
		auto	bw = bwrite ;
	        int	i{} ;
	        int	bl ;
	        while ((rs = bread(ifp,bbuf,blen)) > 0) {
	            cint	len = rs ;
	            i = 0 ;
	            bl = len ;
	            while ((bl > 0) && ((rs = bw(ofp,(bbuf + i),bl)) < bl)) {
	                i += rs ;
	                bl -= rs ;
	                if (rs < 0) break ;
	            } /* end while */
	            tlen += len ;
	            if (rs < 0) break ;
	        } /* end while */
	    } /* end if (ok) */
	    if (falloc && bbuf) {
	        rs1 = uc_free(bbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (bcopyfile) */


