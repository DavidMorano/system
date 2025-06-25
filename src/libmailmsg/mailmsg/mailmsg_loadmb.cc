/* mailmsg_loadmb SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* load a mail-message from a mailbox object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mailmsg_loadmb

	Description:
	This subroutine loads a mail-message (into the mailmsg
	object) from a source that consists of a mailbox object.
	The mailbox object also reads the associated mail-box where
	the mail-msg is located, so it can provide access to that
	same mail-box. It does so through a read-interface that is
	a small subset of the full mailbox interface.

        Note: 
	At first we skip empty lines until we find a non-empty line;
        afterwards we do not ignore empty lines.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mailbox.h>
#include	<rmx.h>
#include	<localmisc.h>

#include	"mailmsg.h"


/* local defines */

#define	MAILMSG_BSIZE	0		/* let it figure out what is best */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsg_loadmb(mailmsg *op,mailbox *mbp,off_t fbo) noex {
	int		rs ;
	int		rs1 ;
	int		tlen = 0 ;
	if ((rs = mailmsg_magic(op)) >= 0) {
	    if ((rs = getbufsize(getbufsize_ml)) >= 0) {
	        cint	llen = (rs * MAILMSG_MF) ;
	        if (char *lbuf{} ; (rs = uc_malloc((llen+1),&lbuf)) >= 0) {
	            mailbox_read	cur ;
	            cint		bsz = MAILMSG_BSIZE ;
	            if ((rs = mailbox_readbegin(mbp,&cur,fbo,bsz)) >= 0) {
	                int	line = 0 ;
	    	        cchar	*lp = lbuf ;
	                while ((rs = mailbox_readln(mbp,&cur,lbuf,llen)) > 0) {
			    cint	ll = rmeol(lbuf,rs) ;
	                    tlen += rs ;
	                    if ((ll > 0) || (line > 0)) {
			        line += 1 ;
	                        rs = mailmsg_loadline(op,lp,ll) ;
	                    }
	                    if (rs <= 0) break ;
	                } /* end while (reading lines) */
	                rs1 = mailbox_readend(mbp,&cur) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (mailbox) */
	            rs1 = uc_free(lbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
 	    } /* end if (getbufsize) */
	} /* end if (magic) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (mailmsg_loadmb) */


