/* mailmsg_loadfd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* load a mail-message from a file-descriptor (FD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mailmsg_loadfd

	Description:
	This subroutine loads a mail-message (into the MAILMSG
	object) from a source that consists of a file-descriptor
	(FD).

	Note: 
	At first we skip empty lines until we find a non-empty
	line; afterwards we do not ignore empty lines.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<filer.h>
#include	<rmx.h>
#include	<localmisc.h>

#include	"mailmsg.h"


/* local defines */

#define	MM		mailmsg


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int mailmsg_read(MM *op,int,off_t,char *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsg_loadfd(MM *op,int mfd,off_t fbo) noex {
	int		rs ;
	int		rs1 ;
	int		tlen = 0 ; /* return-value */
	if ((rs = mailmsg_magic(op)) >= 0) {
	    if ((rs = getbufsize(getbufsize_ml)) >= 0) {
		cint	llen = (rs * MAILMSG_MF) ;
		if (char *lbuf ; (rs = libmem.mall((llen + 1),&lbuf)) >= 0) {
		    {
		        rs = mailmsg_read(op,mfd,fbo,lbuf,llen) ;
		        tlen = rs ;
		    }
	    	    rs1 = libmem.free(lbuf) ;
	    	    if (rs >= 0) rs = rs1 ;
		} /* end if (memory-allocation) */
	    } /* end if (getbufsize) */
	} /* end if (magic) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (mailmsg_loadfd) */


/* local subroutines */

local int mailmsg_read(MM *op,int mfd,off_t fbo,char *lbuf,int llen) noex {
	cint		bsz = 2048 ;
    	int		rs ;
	int		rs1 ;
	int		tlen = 0 ; /* return-value */
	cchar		*lp = lbuf ;
	if (filer b ; (rs = b.start(mfd,fbo,bsz,0)) >= 0) {
	    int	ln = 0 ;
	    while ((rs = b.readln(lbuf,llen,-1)) > 0) {
	        tlen += rs ;
	        if (cint ll = rmeol(lbuf,rs) ; (ll > 0) || (ln > 0)) {
	            ln += 1 ;
	            rs = mailmsg_loadline(op,lp,ll) ;
	        }
	        if (rs <= 0) break ;
	    } /* end while (reading lines) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? tlen : rs ;
} /* end subroutine (mailmsg_read) */


