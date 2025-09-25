/* mailmsg_loadfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* load the header-lines from a MAILMSG file into the object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mailmsg_loadfile

	Description:
	This subroutine load lines from a file containing a
	mail-message into the MAILMSG object (an dobject for
	abstracting mail-message information).

	Synopsis:
	int mailmsg_loadfile(mailmsg *op,bfile *fp) noex

	Arguments:
	op		pointer to MAILMSG object
	fp		pointer to 'bfile' file handle

	Returns:
	>=0		OK
	<0		error (system-return)

	Note: 
	At first we skip empty lines until we find a non-empty line;
	afterwards we do not ignore empty lines.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<bfile.h>
#include	<rmx.h>
#include	<localmisc.h>

#include	"mailmsg.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsg_loadfile(mailmsg *op,bfile *fp) noex {
	int		rs ;
	int		rs1 ;
	int		tlen = 0 ; /* return-value */
	if ((rs = mailmsg_magic(op,fp)) >= 0) {
	    if ((rs = getbufsize(getbufsize_ml)) >= 0) {
		cint	llen = (rs * MAILMSG_MF) ;
		if (char *lbuf ; (rs = libmem.mall((llen + 1),&lbuf)) >= 0) {
	    	    int		ln = 0 ;
	    	    for (cc *lp = lbuf ; (rs = fp->readln(lbuf,llen)) > 0 ; ) {
	        	tlen += rs ;
	        	if (cint ll = rmeol(lbuf,rs) ; (ll > 0) || (ln > 0)) {
	            	    ln += 1 ;
	            	    rs = mailmsg_loadline(op,lp,ll) ;
	        	}
	        	if (rs <= 0) break ;
	    	    } /* end for */
	    	    rs1 = libmem.free(lbuf) ;
	    	    if (rs >= 0) rs = rs1 ;
		} /* end if (memory-allocation) */
	    } /* end if (getbufsize) */
	} /* end if (magic) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (mailmsg_loadfile) */


